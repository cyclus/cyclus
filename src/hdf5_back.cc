#include "hdf5_back.h"

#include <cmath>
#include <string.h>

#include "blob.h"

namespace cyclus {

Hdf5Back::Hdf5Back(std::string path) : path_(path) {
  H5open();
  hasher_.Clear();
  if (boost::filesystem::exists(path_))
    file_ = H5Fopen(path_.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  else  
    file_ = H5Fcreate(path_.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  opened_types_.clear();
  vldatasets_.clear();
  vldts_.clear();
  vlkeys_.clear();

  uuid_type_ = H5Tcopy(H5T_C_S1);
  H5Tset_size(uuid_type_, CYCLUS_UUID_SIZE);
  H5Tset_strpad(uuid_type_, H5T_STR_NULLPAD);
  opened_types_.insert(uuid_type_);

  hsize_t sha1_len = CYCLUS_SHA1_NINT;  // 160 bits == 32 bits / int  * 5 ints
  sha1_type_ = H5Tarray_create2(H5T_NATIVE_UINT, 1, &sha1_len);
  opened_types_.insert(sha1_type_);

  vlstr_type_ = H5Tcopy(H5T_C_S1);
  H5Tset_size(vlstr_type_, H5T_VARIABLE);
  opened_types_.insert(vlstr_type_);
  vldts_[VL_STRING] = vlstr_type_;

  blob_type_ = vlstr_type_;
  vldts_[BLOB] = blob_type_;
}

Hdf5Back::~Hdf5Back() {
  // cleanup HDF5
  Flush();
  H5Fclose(file_);
  std::set<hid_t>::iterator t;
  for (t = opened_types_.begin(); t != opened_types_.end(); ++t) 
    H5Tclose(*t);
  std::map<std::string, hid_t>::iterator vldsit;
  for (vldsit = vldatasets_.begin(); vldsit != vldatasets_.end(); ++vldsit)
    H5Dclose(vldsit->second);

  // cleanup memory
  std::map<std::string, size_t*>::iterator it;
  std::map<std::string, DbTypes*>::iterator dbtit;
  for (it = col_offsets_.begin(); it != col_offsets_.end(); ++it) {
    delete[](it->second);
  }
  for (it = col_sizes_.begin(); it != col_sizes_.end(); ++it) {
    delete[](it->second);
  }
  for (dbtit = schemas_.begin(); dbtit != schemas_.end(); ++dbtit) {
    delete[](dbtit->second);
  }
};

void Hdf5Back::Notify(DatumList data) {
  std::map<std::string, DatumList> groups;
  for (DatumList::iterator it = data.begin(); it != data.end(); ++it) {
    std::string name = (*it)->title();
    if (schema_sizes_.count(name) == 0) {
      if (H5Lexists(file_, name.c_str(), H5P_DEFAULT)) {
        LoadTableTypes(name, (*it)->vals().size());
      } else {
        //Datum* d = *it;
        //CreateTable(d);
        CreateTable(*it);
      }
    }
    groups[name].push_back(*it);
  }

  std::map<std::string, DatumList>::iterator it;
  for (it = groups.begin(); it != groups.end(); ++it) {
    WriteGroup(it->second);
  }
}

template <>
std::string Hdf5Back::VLRead<std::string, VL_STRING>(const char* rawkey) { 
  using std::string;
  // key is used as offset
  Digest key;
  memcpy(key.val, rawkey, CYCLUS_SHA1_SIZE);
  const std::vector<hsize_t> idx = key.cast<hsize_t>();
  hid_t dset = VLDataset(VL_STRING, false);
  hid_t dspace = H5Dget_space(dset);
  hid_t mspace = H5Screate_simple(CYCLUS_SHA1_NINT, vlchunk_, NULL);
  herr_t status = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, (const hsize_t*) &idx[0], 
                                      NULL, vlchunk_, NULL);
  if (status < 0)
    throw IOError("could not select hyperslab of string value array for reading "
                  "in the database '" + path_ + "'.");
  char** buf = new char* [sizeof(char *)];
  status = H5Dread(dset, vldts_[VL_STRING], mspace, dspace, H5P_DEFAULT, buf);
  if (status < 0)
    throw IOError("failed to read in variable length string data "
                  "in database '" + path_ + "'.");
  string val;
  if (buf[0] != NULL)
    val = string(buf[0]);
  status = H5Dvlen_reclaim(vldts_[VL_STRING], mspace, H5P_DEFAULT, buf);
  if (status < 0)
    throw IOError("failed to reclaim variable length string data space in "
                  "database '" + path_ + "'.");
  delete[] buf;
  H5Sclose(mspace);
  H5Sclose(dspace);
  return val;
}

template <>
Blob Hdf5Back::VLRead<Blob, BLOB>(const char* rawkey) { 
  // key is used as offset
  Digest key;
  memcpy(key.val, rawkey, CYCLUS_SHA1_SIZE);
  const std::vector<hsize_t> idx = key.cast<hsize_t>();
  hid_t dset = VLDataset(BLOB, false);
  hid_t dspace = H5Dget_space(dset);
  hid_t mspace = H5Screate_simple(CYCLUS_SHA1_NINT, vlchunk_, NULL);
  herr_t status = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, (const hsize_t*) &idx[0], 
                                      NULL, vlchunk_, NULL);
  if (status < 0)
    throw IOError("could not select hyperslab of Blob value array for reading "
                  "in the database '" + path_ + "'.");
  char** buf = new char* [sizeof(char *)];
  status = H5Dread(dset, vldts_[BLOB], mspace, dspace, H5P_DEFAULT, buf);
  if (status < 0)
    throw IOError("failed to read in Blob data in database '" + path_ + "'.");
  Blob val = Blob(buf[0]);
  status = H5Dvlen_reclaim(vldts_[BLOB], mspace, H5P_DEFAULT, buf);
  if (status < 0)
    throw IOError("failed to reclaim Blob data space in database "
                  "'" + path_ + "'.");
  delete[] buf;
  H5Sclose(mspace);
  H5Sclose(dspace);
  return val;
}

QueryResult Hdf5Back::Query(std::string table, std::vector<Cond>* conds) {
  using std::string;
  using std::vector;
  using std::set;
  using std::list;
  using std::pair;
  using std::map;
  if (!H5Lexists(file_, table.c_str(), H5P_DEFAULT))
    throw IOError("table '" + table + "' does not exist in '" + path_ + "'.");
  int i;
  int j;
  int jlen;
  herr_t status = 0;
  hid_t tb_set = H5Dopen2(file_, table.c_str(), H5P_DEFAULT);
  hid_t tb_space = H5Dget_space(tb_set);
  hid_t tb_plist = H5Dget_create_plist(tb_set);
  hid_t tb_type = H5Dget_type(tb_set);
  size_t tb_typesize = H5Tget_size(tb_type);
  int tb_length = H5Sget_simple_extent_npoints(tb_space);
  hsize_t tb_chunksize;
  H5Pget_chunk(tb_plist, 1, &tb_chunksize);
  unsigned int nchunks = (tb_length/tb_chunksize) + (tb_length%tb_chunksize == 0?0:1);

  // set up field-conditions map
  std::map<std::string, std::vector<Cond*> > field_conds = std::map<std::string, 
                                                           std::vector<Cond*> >();
  if (conds != NULL) {
    Cond* cond;
    for (i = 0; i < conds->size(); ++i) {
      cond = &((*conds)[i]);
      if (field_conds.count(cond->field) == 0)
        field_conds[cond->field] = std::vector<Cond*>();
      field_conds[cond->field].push_back(cond);
    }
  }

  // read in data
  QueryResult qr = GetTableInfo(table, tb_set, tb_type);
  int nfields = qr.fields.size();
  for (i = 0; i < nfields; ++i) {
    if (field_conds.count(qr.fields[i]) == 0) {
      field_conds[qr.fields[i]] = std::vector<Cond*>();
    }
  }
  for (unsigned int n = 0; n < nchunks; ++n) {
    // This loop is meant to be OpenMP-izable
    hid_t field_type;
    hsize_t start = n * tb_chunksize;
    hsize_t count = (tb_length-start)<tb_chunksize ? tb_length - start : tb_chunksize;
    char* buf = new char [tb_typesize * count];
    hid_t memspace = H5Screate_simple(1, &count, NULL);
    status = H5Sselect_hyperslab(tb_space, H5S_SELECT_SET, &start, NULL, &count, NULL);
    status = H5Dread(tb_set, tb_type, memspace, tb_space, H5P_DEFAULT, buf);
    int offset = 0;
    bool is_row_selected;
    for (i = 0; i < count; ++i) {
      offset = i * tb_typesize;
      is_row_selected = true;
      QueryRow row = QueryRow(nfields);
      for (j = 0; j < nfields; ++j) {
        switch (qr.types[j]) {
          case BOOL: {
            bool x = *reinterpret_cast<bool*>(buf + offset);
            is_row_selected = CmpConds<bool>(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case INT: {
            int x = *reinterpret_cast<int*>(buf + offset);
            is_row_selected = CmpConds<int>(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case FLOAT: {
            float x = *reinterpret_cast<float*>(buf + offset);
            is_row_selected = CmpConds<float>(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case DOUBLE: {
            double x = *reinterpret_cast<double*>(buf + offset);
            is_row_selected = CmpConds<double>(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case STRING: {
            std::string x = std::string(buf + offset, col_sizes_[table][j]);
            size_t nullpos = x.find('\0');
            if (nullpos != std::string::npos)
              x.resize(nullpos);
            is_row_selected = CmpConds<std::string>(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_STRING: {
            std::string x = VLRead<std::string, VL_STRING>(buf + offset);
            is_row_selected = CmpConds<std::string>(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case BLOB: {
            Blob x = VLRead<Blob, BLOB>(buf + offset);
            is_row_selected = CmpConds<Blob>(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case UUID: {
            boost::uuids::uuid x;
            memcpy(&x, buf + offset, 16);
            is_row_selected = CmpConds<boost::uuids::uuid>(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VECTOR_INT: {
            std::vector<int> x = std::vector<int>(col_sizes_[table][j] / sizeof(int));
            memcpy(&x[0], buf + offset, col_sizes_[table][j]);
            is_row_selected = CmpConds<std::vector<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_VECTOR_INT: {
            std::vector<int> x = VLRead<std::vector<int>, VL_VECTOR_INT>(buf + offset);
            is_row_selected = CmpConds<std::vector<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VECTOR_STRING: {
            hid_t field_type = H5Tget_member_type(tb_type, j);
            size_t nullpos;
            hsize_t fieldlen;
            H5Tget_array_dims2(field_type, &fieldlen);
            unsigned int strlen = col_sizes_[table][j] / fieldlen;
            vector<string> x = vector<string>(fieldlen);
            for (unsigned int k = 0; k < fieldlen; ++k) {
              x[k] = string(buf + offset + strlen*k, strlen);
              nullpos = x[k].find('\0');
              if (nullpos != std::string::npos)
                x[k].resize(nullpos);
            }
            is_row_selected = CmpConds<vector<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            H5Tclose(field_type);
            break;
          }
          case VECTOR_VL_STRING: {
            jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
            vector<string> x = vector<string>(jlen);
            for (unsigned int k = 0; k < jlen; ++k) {
              x[k] = VLRead<std::string, VL_STRING>(buf + offset + CYCLUS_SHA1_SIZE*k);
            }
            is_row_selected = CmpConds<vector<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_VECTOR_STRING: {
            vector<string> x = VLRead<vector<string>, VL_VECTOR_STRING>(buf + offset);
            is_row_selected = CmpConds<vector<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_VECTOR_VL_STRING: {
            vector<string> x = VLRead<vector<string>, VL_VECTOR_VL_STRING>(buf + offset);
            is_row_selected = CmpConds<vector<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case SET_INT: {
            jlen = col_sizes_[table][j] / sizeof(int);
            int* xraw = reinterpret_cast<int*>(buf + offset);
            std::set<int> x = std::set<int>(xraw, xraw+jlen);
            is_row_selected = CmpConds<std::set<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_SET_INT: {
            std::set<int> x = VLRead<std::set<int>, VL_SET_INT>(buf + offset);
            is_row_selected = CmpConds<std::set<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case SET_STRING: {
            hid_t field_type = H5Tget_member_type(tb_type, j);
            size_t nullpos;
            hsize_t fieldlen;
            H5Tget_array_dims2(field_type, &fieldlen);
            unsigned int strlen = col_sizes_[table][j] / fieldlen;
            set<string> x;
            for (unsigned int k = 0; k < fieldlen; ++k) {
              string s = string(buf + offset + strlen*k, strlen);
              nullpos = s.find('\0');
              if (nullpos != std::string::npos)
                s.resize(nullpos);
              x.insert(s);
            }
            is_row_selected = CmpConds<set<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            H5Tclose(field_type);
            break;
          }
          case SET_VL_STRING: {
            jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
            set<string> x;
            for (unsigned int k = 0; k < jlen; ++k) {
              x.insert(VLRead<string, VL_STRING>(buf + offset + CYCLUS_SHA1_SIZE*k));
            }
            is_row_selected = CmpConds<set<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_SET_STRING: {
            set<string> x = VLRead<set<string>, VL_SET_STRING>(buf + offset);
            is_row_selected = CmpConds<set<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_SET_VL_STRING: {
            set<string> x = VLRead<set<string>, VL_SET_VL_STRING>(buf + offset);
            is_row_selected = CmpConds<set<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case LIST_INT: {
            jlen = col_sizes_[table][j] / sizeof(int);
            int* xraw = reinterpret_cast<int*>(buf + offset);
            std::list<int> x = std::list<int>(xraw, xraw+jlen);
            is_row_selected = CmpConds<std::list<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_LIST_INT: {
            std::list<int> x = VLRead<std::list<int>, VL_LIST_INT>(buf + offset);
            is_row_selected = CmpConds<std::list<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case LIST_STRING: {
            hid_t field_type = H5Tget_member_type(tb_type, j);
            size_t nullpos;
            hsize_t fieldlen;
            H5Tget_array_dims2(field_type, &fieldlen);
            unsigned int strlen = col_sizes_[table][j] / fieldlen;
            list<string> x;
            for (unsigned int k = 0; k < fieldlen; ++k) {
              string s = string(buf + offset + strlen*k, strlen);
              nullpos = s.find('\0');
              if (nullpos != std::string::npos)
                s.resize(nullpos);
              x.push_back(s);
            }
            is_row_selected = CmpConds<list<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            H5Tclose(field_type);
            break;
          }
          case LIST_VL_STRING: {
            jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
            list<string> x;
            for (unsigned int k = 0; k < jlen; ++k) {
              x.push_back(VLRead<string, VL_STRING>(buf + offset + CYCLUS_SHA1_SIZE*k));
            }
            is_row_selected = CmpConds<list<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_LIST_STRING: {
            list<string> x = VLRead<list<string>, VL_LIST_STRING>(buf + offset);
            is_row_selected = CmpConds<list<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_LIST_VL_STRING: {
            list<string> x = VLRead<list<string>, VL_LIST_VL_STRING>(buf + offset);
            is_row_selected = CmpConds<list<string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case PAIR_INT_INT: {
            pair<int, int> x = std::make_pair(*reinterpret_cast<int*>(buf + offset), 
                                              *reinterpret_cast<int*>(buf + offset + sizeof(int)));
            is_row_selected = CmpConds<pair<int, int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case MAP_INT_INT: {
            map<int, int> x = map<int, int>();
            jlen = col_sizes_[table][j] / (2*sizeof(int));
            for (unsigned int k = 0; k < jlen; ++k) {
              x[*reinterpret_cast<int*>(buf + offset + 2*sizeof(int)*k)] = \
                *reinterpret_cast<int*>(buf + offset + 2*sizeof(int)*k + sizeof(int));
            }
            is_row_selected = CmpConds<map<int, int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_MAP_INT_INT: {
            map<int, int> x = VLRead<map<int, int>, VL_MAP_INT_INT>(buf + offset);
            is_row_selected = CmpConds<map<int, int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case MAP_INT_DOUBLE: {
            map<int, double> x = map<int, double>();
            size_t itemsize = sizeof(int) + sizeof(double);
            jlen = col_sizes_[table][j] / itemsize;
            for (unsigned int k = 0; k < jlen; ++k) {
              x[*reinterpret_cast<int*>(buf + offset + itemsize*k)] = \
                *reinterpret_cast<double*>(buf + offset + itemsize*k + sizeof(int));
            }
            is_row_selected = CmpConds<map<int, double> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_MAP_INT_DOUBLE: {
            map<int, double> x = VLRead<map<int, double>, VL_MAP_INT_DOUBLE>(buf + offset);
            is_row_selected = CmpConds<map<int, double> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case MAP_INT_STRING: {
            hid_t field_type = H5Tget_member_type(tb_type, j);
            size_t nullpos;
            hsize_t fieldlen;
            H5Tget_array_dims2(field_type, &fieldlen);
            unsigned int itemsize = col_sizes_[table][j] / fieldlen;
            unsigned int strlen = itemsize - sizeof(int);
            map<int, string> x;
            for (unsigned int k = 0; k < fieldlen; ++k) {
              string s = string(buf + offset + itemsize*k + sizeof(int), strlen);
              nullpos = s.find('\0');
              if (nullpos != std::string::npos)
                s.resize(nullpos);
              x[*reinterpret_cast<int*>(buf + offset + itemsize*k)] = s;
            }
            is_row_selected = CmpConds<map<int, string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            H5Tclose(field_type);
            break;
          }
          case MAP_INT_VL_STRING: {
            unsigned int itemsize = sizeof(int) + CYCLUS_SHA1_SIZE;
            jlen = col_sizes_[table][j] / itemsize;
            map<int, string> x;
            for (unsigned int k = 0; k < jlen; ++k) {
              x[*reinterpret_cast<int*>(buf + offset + itemsize*k)] = \
                VLRead<string, VL_STRING>(buf + offset + itemsize*k + sizeof(int));
            }
            is_row_selected = CmpConds<map<int, string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_MAP_INT_STRING: {
            map<int, string> x = VLRead<map<int, string>, VL_MAP_INT_STRING>(buf + offset);
            is_row_selected = CmpConds<map<int, string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_MAP_INT_VL_STRING: {
            map<int, string> x = VLRead<map<int, string>, VL_MAP_INT_VL_STRING>(buf + offset);
            is_row_selected = CmpConds<map<int, string> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case MAP_STRING_INT: {
            hid_t field_type = H5Tget_member_type(tb_type, j);
            size_t nullpos;
            hsize_t fieldlen;
            H5Tget_array_dims2(field_type, &fieldlen);
            unsigned int itemsize = col_sizes_[table][j] / fieldlen;
            unsigned int strlen = itemsize - sizeof(int);
            map<string, int> x;
            for (unsigned int k = 0; k < fieldlen; ++k) {
              string s = string(buf + offset + itemsize*k, strlen);
              nullpos = s.find('\0');
              if (nullpos != std::string::npos)
                s.resize(nullpos);
              x[s] = *reinterpret_cast<int*>(buf + offset + itemsize*k + strlen);
            }
            is_row_selected = CmpConds<map<string, int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            H5Tclose(field_type);
            break;
          }
          case VL_MAP_STRING_INT: {
            map<string, int> x = VLRead<map<string, int>, VL_MAP_STRING_INT>(buf + offset);
            is_row_selected = CmpConds<map<string, int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case MAP_VL_STRING_INT: {
            unsigned int itemsize = sizeof(int) + CYCLUS_SHA1_SIZE;
            jlen = col_sizes_[table][j] / itemsize;
            map<string, int> x;
            for (unsigned int k = 0; k < jlen; ++k) {
              x[VLRead<string, VL_STRING>(buf + offset + itemsize*k)] = \
                *reinterpret_cast<int*>(buf + offset + itemsize*k + CYCLUS_SHA1_SIZE);
            }
            is_row_selected = CmpConds<map<string, int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_MAP_VL_STRING_INT: {
            map<string, int> x = VLRead<map<string, int>, VL_MAP_VL_STRING_INT>(buf + offset);
            is_row_selected = CmpConds<map<string, int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case MAP_STRING_DOUBLE: {
            hid_t field_type = H5Tget_member_type(tb_type, j);
            size_t nullpos;
            hsize_t fieldlen;
            H5Tget_array_dims2(field_type, &fieldlen);
            unsigned int itemsize = col_sizes_[table][j] / fieldlen;
            unsigned int strlen = itemsize - sizeof(double);
            map<string, double> x;
            for (unsigned int k = 0; k < fieldlen; ++k) {
              string s = string(buf + offset + itemsize*k, strlen);
              nullpos = s.find('\0');
              if (nullpos != std::string::npos)
                s.resize(nullpos);
              x[s] = *reinterpret_cast<double*>(buf + offset + itemsize*k + strlen);
            }
            is_row_selected = CmpConds<map<string, double> >(&x, 
                                                      &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            H5Tclose(field_type);
            break;
          }
          case VL_MAP_STRING_DOUBLE: {
            map<string, double> x = \
              VLRead<map<string, double>, VL_MAP_STRING_DOUBLE>(buf + offset);
            is_row_selected = CmpConds<map<string, double> >(&x,
                                                      &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case MAP_STRING_STRING: {
            hid_t field_type = H5Tget_member_type(tb_type, j);
            hid_t item_type = H5Tget_super(field_type);
            hid_t key_type = H5Tget_member_type(item_type, 0);
            size_t nullpos;
            hsize_t fieldlen;
            H5Tget_array_dims2(field_type, &fieldlen);
            unsigned int itemsize = col_sizes_[table][j] / fieldlen;
            unsigned int keylen = H5Tget_size(key_type);
            unsigned int vallen = itemsize - keylen;
            map<string, string> x;
            for (unsigned int k = 0; k < fieldlen; ++k) {
              string key = string(buf + offset + itemsize*k, keylen);
              nullpos = key.find('\0');
              if (nullpos != std::string::npos)
                key.resize(nullpos);
              string val = string(buf + offset + itemsize*k + keylen, vallen);
              nullpos = val.find('\0');
              if (nullpos != std::string::npos)
                val.resize(nullpos);
              x[key] = val;
            }
            is_row_selected = CmpConds<map<string, string> >(&x, 
                                                      &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            H5Tclose(key_type);
            H5Tclose(item_type);
            H5Tclose(field_type);
            break;
          }
          case VL_MAP_STRING_STRING: {
            map<string, string> x = \
              VLRead<map<string, string>, VL_MAP_STRING_STRING>(buf + offset);
            is_row_selected = CmpConds<map<string, string> >(&x,
                                                      &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case MAP_STRING_VL_STRING: {
            hid_t field_type = H5Tget_member_type(tb_type, j);
            size_t nullpos;
            hsize_t fieldlen;
            H5Tget_array_dims2(field_type, &fieldlen);
            unsigned int itemsize = col_sizes_[table][j] / fieldlen;
            unsigned int keylen = itemsize - CYCLUS_SHA1_SIZE;
            map<string, string> x;
            for (unsigned int k = 0; k < fieldlen; ++k) {
              string key = string(buf + offset + itemsize*k, keylen);
              nullpos = key.find('\0');
              if (nullpos != std::string::npos)
                key.resize(nullpos);
              x[key] = VLRead<string, VL_STRING>(buf + offset + itemsize*k + keylen);
            }
            is_row_selected = CmpConds<map<string, string> >(&x,
                                                      &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            H5Tclose(field_type);
            break;
          }
          case VL_MAP_STRING_VL_STRING: {
            map<string, string> x = \
              VLRead<map<string, string>, VL_MAP_STRING_VL_STRING>(buf + offset);
            is_row_selected = CmpConds<map<string, string> >(&x,
                                                      &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case MAP_VL_STRING_DOUBLE: {
            unsigned int itemsize = sizeof(double) + CYCLUS_SHA1_SIZE;
            jlen = col_sizes_[table][j] / itemsize;
            map<string, double> x;
            for (unsigned int k = 0; k < jlen; ++k) {
              x[VLRead<string, VL_STRING>(buf + offset + itemsize*k)] = \
                *reinterpret_cast<double*>(buf + offset + itemsize*k + CYCLUS_SHA1_SIZE);
            }
            is_row_selected = CmpConds<map<string, double> >(&x,
                                                      &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          case VL_MAP_VL_STRING_DOUBLE: {
            map<string, double> x = \
              VLRead<map<string, double>, VL_MAP_VL_STRING_DOUBLE>(buf + offset);
            is_row_selected = CmpConds<map<string, double> >(&x,
                                                      &(field_conds[qr.fields[j]]));
            if (is_row_selected)
              row[j] = x;
            break;
          }
          default: {
            throw IOError("querying column '" + qr.fields[j] + "' in table '" + \
                          table + "' failed due to unsupported data type.");
            break;
          }
        }
        if (!is_row_selected)
          break;
        offset += col_sizes_[table][j];
      }
      if (is_row_selected) {
        qr.rows.push_back(row);
      }
    }
    delete[] buf;
    H5Sclose(memspace);
  }

  // close and return
  H5Tclose(tb_type);
  H5Pclose(tb_plist);
  H5Sclose(tb_space);
  H5Dclose(tb_set);
  return qr;
}

QueryResult Hdf5Back::GetTableInfo(std::string title, hid_t dset, hid_t dt) {
  int i;
  char * colname;
  hsize_t ncols = H5Tget_nmembers(dt);
  std::string fieldname;
  std::string fieldtype;
  //LoadTableTypes(title, dset, ncols);
  DbTypes* dbtypes = schemas_[title];

  QueryResult qr;
  for (i = 0; i < ncols; ++i) {
    colname = H5Tget_member_name(dt, i);
    fieldname = std::string(colname);
    free(colname);
    qr.fields.push_back(fieldname);
    qr.types.push_back(dbtypes[i]);
  }
  return qr;
}

void Hdf5Back::LoadTableTypes(std::string title, hsize_t ncols) {
  if (schemas_.count(title) > 0)
    return;
  hid_t dset = H5Dopen2(file_, title.c_str(), H5P_DEFAULT);
  LoadTableTypes(title, dset, ncols);
  H5Dclose(dset);
}

void Hdf5Back::LoadTableTypes(std::string title, hid_t dset, hsize_t ncols) {
  if (schemas_.count(title) > 0)
    return;

  int i;
  hid_t subt;
  hid_t t = H5Dget_type(dset);
  schema_sizes_[title] = H5Tget_size(t);
  size_t* offsets = new size_t[ncols];
  size_t* sizes = new size_t[ncols];
  for (i = 0; i < ncols; ++i) {
    offsets[i] = H5Tget_member_offset(t, i);
    subt = H5Tget_member_type(t, i);
    sizes[i] = H5Tget_size(subt);
    H5Tclose(subt);
  }
  H5Tclose(t);
  col_offsets_[title] = offsets;
  col_sizes_[title] = sizes;

  // get types from db
  int dbt[ncols];
  hid_t dbtypes_attr = H5Aopen(dset, "cyclus_dbtypes", H5P_DEFAULT);
  hid_t dbtypes_type = H5Aget_type(dbtypes_attr);
  H5Aread(dbtypes_attr, dbtypes_type, dbt);
  H5Tclose(dbtypes_type);
  H5Aclose(dbtypes_attr);

  // store types
  DbTypes* dbtypes = new DbTypes[ncols];
  for (i = 0; i < ncols; ++i)
    dbtypes[i] = static_cast<DbTypes>(dbt[i]);
  schemas_[title] = dbtypes;
}

hid_t Hdf5Back::CreateFLStrType(int n) {
  hid_t str_type = H5Tcopy(H5T_C_S1);
  H5Tset_size(str_type, n);
  H5Tset_strpad(str_type, H5T_STR_NULLPAD);
  opened_types_.insert(str_type);
  return str_type;
}

std::string Hdf5Back::Name() {
  return path_;
}

void Hdf5Back::CreateTable(Datum* d) {
  using std::set;
  using std::string;
  using std::vector;
  using std::list;
  using std::pair;
  using std::list;
  using std::map;
  Datum::Vals vals = d->vals();
  hsize_t nvals = vals.size();
  Datum::Shape shape;
  Datum::Shapes shapes = d->shapes();

  herr_t status;
  size_t dst_size = 0;
  size_t* dst_offset = new size_t[nvals];
  size_t* dst_sizes = new size_t[nvals];
  hid_t field_types[nvals];
  DbTypes* dbtypes = new DbTypes[nvals];
  const char* field_names[nvals];
  for (int i = 0; i < nvals; ++i) {
    dst_offset[i] = dst_size;
    field_names[i] = vals[i].first;
    const std::type_info& valtype = vals[i].second.type();
    if (valtype == typeid(bool)) {
      dbtypes[i] = BOOL;
      field_types[i] = H5T_NATIVE_CHAR;
      dst_sizes[i] = sizeof(char);
    } else if (valtype == typeid(int)) {
      dbtypes[i] = INT;
      field_types[i] = H5T_NATIVE_INT;
      dst_sizes[i] = sizeof(int);
    } else if (valtype == typeid(float)) {
      dbtypes[i] = FLOAT;
      field_types[i] = H5T_NATIVE_FLOAT;
      dst_sizes[i] = sizeof(float);
    } else if (valtype == typeid(double)) {
      dbtypes[i] = DOUBLE;
      field_types[i] = H5T_NATIVE_DOUBLE;
      dst_sizes[i] = sizeof(double);
    } else if (valtype == typeid(std::string)) {
      shape = shapes[i];
      if (shape.empty() || shape[0] < 1) {
        dbtypes[i] = VL_STRING;
        field_types[i] = sha1_type_;
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = STRING;
        field_types[i] = H5Tcopy(H5T_C_S1);
        H5Tset_size(field_types[i], shape[0]);
        H5Tset_strpad(field_types[i], H5T_STR_NULLPAD);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = sizeof(char) * shape[0];
      }
    } else if (valtype == typeid(Blob)) {
      dbtypes[i] = BLOB;
      field_types[i] = sha1_type_;
      dst_sizes[i] = CYCLUS_SHA1_SIZE;
    } else if (valtype == typeid(boost::uuids::uuid)) {
      dbtypes[i] = UUID;
      field_types[i] = uuid_type_;
      dst_sizes[i] = CYCLUS_UUID_SIZE;
    } else if (valtype == typeid(std::vector<int>)) {
      shape = shapes[i];
      if (shape.empty() || shape[0] < 1) {
        dbtypes[i] = VL_VECTOR_INT;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_VECTOR_INT) == 0) {
          vldts_[VL_VECTOR_INT] = H5Tvlen_create(H5T_NATIVE_INT);
          opened_types_.insert(vldts_[VL_VECTOR_INT]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = VECTOR_INT;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(H5T_NATIVE_INT, 1, &shape0);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = sizeof(int) * shape[0];
      }
    } else if (valtype == typeid(std::vector<std::string>)) {
      shape = shapes[i];
      if (shape.empty() || (shape[0] < 1 && shape[1] < 1)) {
        dbtypes[i] = VL_VECTOR_VL_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_VECTOR_VL_STRING) == 0) {
          vldts_[VL_VECTOR_VL_STRING] = H5Tvlen_create(sha1_type_);
          opened_types_.insert(vldts_[VL_VECTOR_VL_STRING]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] < 1 && shape[1] >= 1) {
        dbtypes[i] = VL_VECTOR_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_VECTOR_STRING) == 0) {
          vldts_[VL_VECTOR_STRING] = H5Tvlen_create(sha1_type_);
          opened_types_.insert(vldts_[VL_VECTOR_STRING]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] >= 1 && shape[1] < 1) {
        dbtypes[i] = VECTOR_VL_STRING;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(sha1_type_, 1, &shape0);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = VECTOR_STRING;
        hid_t str_type = CreateFLStrType(shape[1]);
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(str_type, 1, &shape0);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * shape[1];
      }
    } else if (valtype == typeid(std::set<int>)) {
      shape = shapes[i];
      if (shape.empty() || shape[0] < 1) {
        dbtypes[i] = VL_SET_INT;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_SET_INT) == 0) {
          vldts_[VL_SET_INT] = H5Tvlen_create(H5T_NATIVE_INT);
          opened_types_.insert(vldts_[VL_SET_INT]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = SET_INT;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(H5T_NATIVE_INT, 1, &shape0);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = sizeof(int) * shape[0];
      }
    } else if (valtype == typeid(set<string>)) {
      shape = shapes[i];
      if (shape.empty() || (shape[0] < 1 && shape[1] < 1)) {
        dbtypes[i] = VL_SET_VL_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_SET_VL_STRING) == 0) {
          vldts_[VL_SET_VL_STRING] = H5Tvlen_create(sha1_type_);
          opened_types_.insert(vldts_[VL_SET_VL_STRING]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] < 1 && shape[1] >= 1) {
        dbtypes[i] = VL_SET_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_SET_STRING) == 0) {
          vldts_[VL_SET_STRING] = H5Tvlen_create(sha1_type_);
          opened_types_.insert(vldts_[VL_SET_STRING]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] >= 1 && shape[1] < 1) {
        dbtypes[i] = SET_VL_STRING;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(sha1_type_, 1, &shape0);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = SET_STRING;
        hid_t str_type = CreateFLStrType(shape[1]);
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(str_type, 1, &shape0);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * shape[1];
      }
    } else if (valtype == typeid(std::list<int>)) {
      shape = shapes[i];
      if (shape.empty() || shape[0] < 1) {
        dbtypes[i] = VL_LIST_INT;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_LIST_INT) == 0) {
          vldts_[VL_LIST_INT] = H5Tvlen_create(H5T_NATIVE_INT);
          opened_types_.insert(vldts_[VL_LIST_INT]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = LIST_INT;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(H5T_NATIVE_INT, 1, &shape0);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = sizeof(int) * shape[0];
      }
    } else if (valtype == typeid(list<string>)) {
      shape = shapes[i];
      if (shape.empty() || (shape[0] < 1 && shape[1] < 1)) {
        dbtypes[i] = VL_LIST_VL_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_LIST_VL_STRING) == 0) {
          vldts_[VL_LIST_VL_STRING] = H5Tvlen_create(sha1_type_);
          opened_types_.insert(vldts_[VL_LIST_VL_STRING]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] < 1 && shape[1] >= 1) {
        dbtypes[i] = VL_LIST_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_LIST_STRING) == 0) {
          vldts_[VL_LIST_STRING] = H5Tvlen_create(sha1_type_);
          opened_types_.insert(vldts_[VL_LIST_STRING]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] >= 1 && shape[1] < 1) {
        dbtypes[i] = LIST_VL_STRING;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(sha1_type_, 1, &shape0);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = LIST_STRING;
        hid_t str_type = CreateFLStrType(shape[1]);
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(str_type, 1, &shape0);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * shape[1];
      }
    } else if (valtype == typeid(std::pair<int, int>)) {
      dbtypes[i] = PAIR_INT_INT;
      dst_sizes[i] = sizeof(int) * 2;
      field_types[i] = H5Tcreate(H5T_COMPOUND, dst_sizes[i]);
      H5Tinsert(field_types[i], "first", 0, H5T_NATIVE_INT);
      H5Tinsert(field_types[i], "second", sizeof(int), H5T_NATIVE_INT);
      opened_types_.insert(field_types[i]);
    } else if (valtype == typeid(std::map<int, int>)) {
      shape = shapes[i];
      hid_t item_type = H5Tcreate(H5T_COMPOUND, sizeof(int) * 2);
      H5Tinsert(item_type, "key", 0, H5T_NATIVE_INT);
      H5Tinsert(item_type, "val", sizeof(int), H5T_NATIVE_INT);
      if (shape.empty() || shape[0] < 1) {
        dbtypes[i] = VL_MAP_INT_INT;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_INT_INT) == 0) {
          vldts_[VL_MAP_INT_INT] = H5Tvlen_create(item_type);
          opened_types_.insert(item_type);
          opened_types_.insert(vldts_[VL_MAP_INT_INT]);
        } else {
          H5Tclose(item_type);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = MAP_INT_INT;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = sizeof(int) * 2 * shape[0];
      }
    } else if (valtype == typeid(std::map<int, double>)) {
      shape = shapes[i];
      hid_t item_type = H5Tcreate(H5T_COMPOUND, sizeof(int) + sizeof(double));
      H5Tinsert(item_type, "key", 0, H5T_NATIVE_INT);
      H5Tinsert(item_type, "val", sizeof(int), H5T_NATIVE_DOUBLE);
      if (shape.empty() || shape[0] < 1) {
        dbtypes[i] = VL_MAP_INT_DOUBLE;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_INT_DOUBLE) == 0) {
          vldts_[VL_MAP_INT_DOUBLE] = H5Tvlen_create(item_type);
          opened_types_.insert(item_type);
          opened_types_.insert(vldts_[VL_MAP_INT_DOUBLE]);
        } else {
          H5Tclose(item_type);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = MAP_INT_DOUBLE;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = (sizeof(int) + sizeof(double)) * shape[0];
        H5Tpack(field_types[i]);
        H5Tpack(item_type);
      }
    } else if (valtype == typeid(map<int, string>)) {
      shape = shapes[i];
      hid_t item_type = H5Tcreate(H5T_COMPOUND, sizeof(int) + CYCLUS_SHA1_SIZE);
      H5Tinsert(item_type, "key", 0, H5T_NATIVE_INT);
      H5Tinsert(item_type, "val", sizeof(int), sha1_type_);
      if (shape.empty() || (shape[0] < 1 && shape[1] < 1)) {
        dbtypes[i] = VL_MAP_INT_VL_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_INT_VL_STRING) == 0) {
          vldts_[VL_MAP_INT_VL_STRING] = H5Tvlen_create(item_type);
          opened_types_.insert(vldts_[VL_MAP_INT_VL_STRING]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] < 1 && shape[1] >= 1) {
        dbtypes[i] = VL_MAP_INT_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_INT_STRING) == 0) {
          vldts_[VL_MAP_INT_STRING] = H5Tvlen_create(item_type);
          opened_types_.insert(vldts_[VL_MAP_INT_STRING]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] >= 1 && shape[1] < 1) {
        dbtypes[i] = MAP_INT_VL_STRING;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * (sizeof(int) + CYCLUS_SHA1_SIZE);
      } else {
        dbtypes[i] = MAP_INT_STRING;
        hid_t str_type = CreateFLStrType(shape[1]);
        hsize_t shape0 = shape[0];
        H5Tclose(item_type);
        item_type = H5Tcreate(H5T_COMPOUND, sizeof(int) + shape[1]);
        H5Tinsert(item_type, "key", 0, H5T_NATIVE_INT);
        H5Tinsert(item_type, "val", sizeof(int), str_type);
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * (sizeof(int) + shape[1]);
      }
    } else if (valtype == typeid(map<string, int>)) {
      shape = shapes[i];
      hid_t item_type = H5Tcreate(H5T_COMPOUND, sizeof(int) + CYCLUS_SHA1_SIZE);
      H5Tinsert(item_type, "key", 0, sha1_type_);
      H5Tinsert(item_type, "val", CYCLUS_SHA1_SIZE, H5T_NATIVE_INT);
      if (shape.empty() || (shape[0] < 1 && shape[1] < 1)) {
        dbtypes[i] = VL_MAP_VL_STRING_INT;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_VL_STRING_INT) == 0) {
          vldts_[VL_MAP_VL_STRING_INT] = H5Tvlen_create(item_type);
          opened_types_.insert(vldts_[VL_MAP_VL_STRING_INT]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] < 1 && shape[1] >= 1) {
        dbtypes[i] = VL_MAP_STRING_INT;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_STRING_INT) == 0) {
          vldts_[VL_MAP_STRING_INT] = H5Tvlen_create(item_type);
          opened_types_.insert(vldts_[VL_MAP_STRING_INT]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] >= 1 && shape[1] < 1) {
        dbtypes[i] = MAP_VL_STRING_INT;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * (sizeof(int) + CYCLUS_SHA1_SIZE);
      } else {
        dbtypes[i] = MAP_STRING_INT;
        hid_t str_type = CreateFLStrType(shape[1]);
        hsize_t shape0 = shape[0];
        H5Tclose(item_type);
        item_type = H5Tcreate(H5T_COMPOUND, sizeof(int) + shape[1]);
        H5Tinsert(item_type, "key", 0, str_type);
        H5Tinsert(item_type, "val", shape[1], H5T_NATIVE_INT);
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * (sizeof(int) + shape[1]);
      }
    } else if (valtype == typeid(map<string, double>)) {
      shape = shapes[i];
      hid_t item_type = H5Tcreate(H5T_COMPOUND, sizeof(double) + CYCLUS_SHA1_SIZE);
      H5Tinsert(item_type, "key", 0, sha1_type_);
      H5Tinsert(item_type, "val", CYCLUS_SHA1_SIZE, H5T_NATIVE_DOUBLE);
      if (shape.empty() || (shape[0] < 1 && shape[1] < 1)) {
        dbtypes[i] = VL_MAP_VL_STRING_DOUBLE;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_VL_STRING_DOUBLE) == 0) {
          vldts_[VL_MAP_VL_STRING_DOUBLE] = H5Tvlen_create(item_type);
          opened_types_.insert(vldts_[VL_MAP_VL_STRING_DOUBLE]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] < 1 && shape[1] >= 1) {
        dbtypes[i] = VL_MAP_STRING_DOUBLE;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_STRING_DOUBLE) == 0) {
          vldts_[VL_MAP_STRING_DOUBLE] = H5Tvlen_create(item_type);
          opened_types_.insert(vldts_[VL_MAP_STRING_DOUBLE]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] >= 1 && shape[1] < 1) {
        dbtypes[i] = MAP_VL_STRING_DOUBLE;
        hsize_t shape0 = shape[0];
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * (sizeof(double) + CYCLUS_SHA1_SIZE);
      } else {
        dbtypes[i] = MAP_STRING_DOUBLE;
        hid_t str_type = CreateFLStrType(shape[1]);
        hsize_t shape0 = shape[0];
        H5Tclose(item_type);
        item_type = H5Tcreate(H5T_COMPOUND, sizeof(double) + shape[1]);
        H5Tinsert(item_type, "key", 0, str_type);
        H5Tinsert(item_type, "val", shape[1], H5T_NATIVE_DOUBLE);
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * (sizeof(double) + shape[1]);
      }
    } else if (valtype == typeid(map<string, string>)) {
      shape = shapes[i];
      hid_t item_type;
      if (shape.empty() || shape[0] < 1) {
        // dtype for VL_MAP_...
        item_type = H5Tcreate(H5T_COMPOUND, 2*CYCLUS_SHA1_SIZE);
        H5Tinsert(item_type, "key", 0, sha1_type_);
        H5Tinsert(item_type, "val", CYCLUS_SHA1_SIZE, sha1_type_);
      }
      if (shape.empty() || (shape[0] < 1 && shape[1] < 1 && shape[2] < 1)) {
        dbtypes[i] = VL_MAP_VL_STRING_VL_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_VL_STRING_VL_STRING) == 0) {
          vldts_[VL_MAP_VL_STRING_VL_STRING] = H5Tvlen_create(item_type);
          opened_types_.insert(vldts_[VL_MAP_VL_STRING_VL_STRING]);
        } else {
          H5Tclose(item_type);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] < 1 && shape[1] >= 1 && shape[2] < 1) {
        dbtypes[i] = VL_MAP_STRING_VL_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_STRING_VL_STRING) == 0) {
          vldts_[VL_MAP_STRING_VL_STRING] = H5Tvlen_create(item_type);
          opened_types_.insert(vldts_[VL_MAP_STRING_VL_STRING]);
        } else {
          H5Tclose(item_type);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] < 1 && shape[1] < 1 && shape[2] >= 1) {
        dbtypes[i] = VL_MAP_VL_STRING_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_VL_STRING_STRING) == 0) {
          vldts_[VL_MAP_VL_STRING_STRING] = H5Tvlen_create(item_type);
          opened_types_.insert(vldts_[VL_MAP_VL_STRING_STRING]);
        } else {
          H5Tclose(item_type);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] < 1 && shape[1] >= 1 && shape[2] >= 1) {
        dbtypes[i] = VL_MAP_STRING_STRING;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_MAP_STRING_STRING) == 0) {
          vldts_[VL_MAP_STRING_STRING] = H5Tvlen_create(item_type);
          opened_types_.insert(vldts_[VL_MAP_STRING_STRING]);
        } else {
          H5Tclose(item_type);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else if (shape[0] >= 1 && shape[1] < 1 && shape[2] < 1) {
        hsize_t shape0 = shape[0];
        dbtypes[i] = MAP_VL_STRING_VL_STRING;
        item_type = H5Tcreate(H5T_COMPOUND, 2*CYCLUS_SHA1_SIZE);
        H5Tinsert(item_type, "key", 0, sha1_type_);
        H5Tinsert(item_type, "val", CYCLUS_SHA1_SIZE, sha1_type_);
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * 2 * CYCLUS_SHA1_SIZE;
      } else if (shape[0] >= 1 && shape[1] < 1 && shape[2] >= 1) {
        hsize_t shape0 = shape[0];
        dbtypes[i] = MAP_VL_STRING_STRING;
        hid_t val_type = CreateFLStrType(shape[2]);
        item_type = H5Tcreate(H5T_COMPOUND, CYCLUS_SHA1_SIZE + shape[2]);
        H5Tinsert(item_type, "key", 0, sha1_type_);
        H5Tinsert(item_type, "val", CYCLUS_SHA1_SIZE, val_type);
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * (CYCLUS_SHA1_SIZE + shape[2]);
      } else if (shape[0] >= 1 && shape[1] >= 1 && shape[2] < 1) {
        hsize_t shape0 = shape[0];
        dbtypes[i] = MAP_STRING_VL_STRING;
        hid_t key_type = CreateFLStrType(shape[1]);
        item_type = H5Tcreate(H5T_COMPOUND, CYCLUS_SHA1_SIZE + shape[1]);
        H5Tinsert(item_type, "key", 0, key_type);
        H5Tinsert(item_type, "val", shape[1], sha1_type_);
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * (CYCLUS_SHA1_SIZE + shape[1]);
      } else {
        dbtypes[i] = MAP_STRING_STRING;
        hsize_t shape0 = shape[0];
        hid_t key_type = CreateFLStrType(shape[1]);
        hid_t val_type = CreateFLStrType(shape[2]);
        item_type = H5Tcreate(H5T_COMPOUND, shape[1] + shape[2]);
        H5Tinsert(item_type, "key", 0, key_type);
        H5Tinsert(item_type, "val", shape[1], val_type);
        field_types[i] = H5Tarray_create2(item_type, 1, &shape0);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = shape[0] * (shape[1] + shape[2]);
      }
    } else {
      throw IOError("the type for column '" + std::string(field_names[i]) + \
                    "' is not yet supported in HDF5.");
    } 
    dst_size += dst_sizes[i];
  }

  const char* title = d->title().c_str();
  int compress = 1;
  int chunk_size = 1024;
  void* fill_data = NULL;
  void* data = NULL;

  // Make the table
  status = H5TBmake_table(title, file_, title, nvals, 0, dst_size,
                          field_names, dst_offset, field_types, chunk_size, 
                          fill_data, compress, data);
  if (status < 0) {
    std::stringstream ss; 
    ss << "Failed to create HDF5 table:\n" \
       << "  file      " << path_ << "\n" \
       << "  table     " << title << "\n" \
       << "  chunksize " << chunk_size << "\n" \
       << "  rowsize   " << dst_size << "\n";
    for (int i = 0; i < nvals; ++i) {
      ss << "    #" << i << " " << field_names[i] << "\n" \
         << "      dbtype: " << dbtypes[i] << "\n" \
         << "      h5type: " << field_types[i] << "\n" \
         << "      size:   " << dst_sizes[i] << "\n" \
         << "      offset: " << dst_offset[i] << "\n";
    }
    throw IOError(ss.str());
  }

  // add dbtypes attribute
  hid_t tb_set = H5Dopen2(file_, title, H5P_DEFAULT);
  hid_t attr_space = H5Screate_simple(1, &nvals, &nvals);
  hid_t dbtypes_attr = H5Acreate2(tb_set, "cyclus_dbtypes", H5T_NATIVE_INT, 
                                  attr_space, H5P_DEFAULT, H5P_DEFAULT);
  H5Awrite(dbtypes_attr, H5T_NATIVE_INT, dbtypes);
  H5Aclose(dbtypes_attr);
  H5Sclose(attr_space);
  H5Dclose(tb_set);

  // record everything for later
  col_offsets_[d->title()] = dst_offset;
  schema_sizes_[d->title()] = dst_size;
  col_sizes_[d->title()] = dst_sizes;
  schemas_[d->title()] = dbtypes;
}

void Hdf5Back::WriteGroup(DatumList& group) {
  std::string title = group.front()->title();
  const char * c_title = title.c_str();

  size_t* offsets = col_offsets_[title];
  size_t* sizes = col_sizes_[title];
  size_t rowsize = schema_sizes_[title];

  char* buf = new char[group.size() * rowsize];
  FillBuf(title, buf, group, sizes, rowsize);

  // We cannot do the simple thing (append_records) here because of a bug in 
  // H5TB where it stupidly tries to reconstruct the datatype in memory from 
  // what it read in on disk. This works in most cases but failed where the table
  // had a column which is an array of a compound datatype of non-homogenous
  // fields (eg MAP_INT_DOUBLE). The fix here just uses the datatype present on 
  // disk - which is what we wanted anyway!
  //herr_t status = H5TBappend_records(file_, title.c_str(), group.size(), rowsize,
  //                            offsets, sizes, buf);
  herr_t status;
  hid_t dset = H5Dopen2(file_, title.c_str(), H5P_DEFAULT);
  hid_t dtype = H5Dget_type(dset);
  hsize_t nrecords_add = group.size();
  hsize_t nrecords_orig;
  hsize_t nfields;
  hsize_t dims[1];
  hsize_t offset[1];
  hsize_t count[1];
  H5TBget_table_info(file_, c_title, &nfields, &nrecords_orig);
  dims[0] = nrecords_add + nrecords_orig;
  offset[0] = nrecords_orig;
  count[0] = nrecords_add;

  status = H5Dset_extent(dset, dims);
  hid_t dspace = H5Dget_space(dset);
  hid_t memspace = H5Screate_simple(1, count, NULL);
  status = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, offset, NULL, count, NULL);
  status = H5Dwrite(dset, dtype, memspace, dspace, H5P_DEFAULT, buf);

  if (status < 0) {
    std::stringstream ss; 
    ss << "Failed to write to the HDF5 table:\n" \
       << "  file      " << path_ << "\n" \
       << "  table     " << title << "\n" \
       << "  num. rows " << group.size() << "\n"
       << "  rowsize   " << rowsize << "\n";
    for (int i = 0; i < group.front()->vals().size(); ++i) {
      ss << "    # Column " << i << "\n" \
         << "      dbtype: " << schemas_[title][i] << "\n" \
         << "      size:   " << sizes[i] << "\n" \
         << "      offset: " << offsets[i] << "\n";
    }
    throw IOError(ss.str());
  }

  H5Sclose(memspace);
  H5Sclose(dspace);
  H5Tclose(dtype);
  H5Dclose(dset);
  delete[] buf;
}

template <typename T, DbTypes U>
Digest Hdf5Back::VLWrite(const T& x) {
  hasher_.Clear();
  hasher_.Update(x);
  Digest key = hasher_.digest();
  hid_t keysds = VLDataset(U, true);
  hid_t valsds = VLDataset(U, false);
  if (vlkeys_[U].count(key) == 1)
    return key;
  hvl_t buf = VLValToBuf(x);
  AppendVLKey(keysds, U, key);
  InsertVLVal(valsds, U, key, buf);
  return key;
}

template <>
Digest Hdf5Back::VLWrite<std::string, VL_STRING>(const std::string& x) {
  hasher_.Clear();
  hasher_.Update(x);
  Digest key = hasher_.digest();
  hid_t keysds = VLDataset(VL_STRING, true);
  hid_t valsds = VLDataset(VL_STRING, false);
  if (vlkeys_[VL_STRING].count(key) == 1)
    return key;
  AppendVLKey(keysds, VL_STRING, key);
  InsertVLVal(valsds, VL_STRING, key, x);
  return key;
}

template <>
Digest Hdf5Back::VLWrite<Blob, BLOB>(const Blob& x) {
  hasher_.Clear();
  hasher_.Update(x);
  Digest key = hasher_.digest();
  hid_t keysds = VLDataset(BLOB, true);
  hid_t valsds = VLDataset(BLOB, false);
  if (vlkeys_[BLOB].count(key) == 1)
    return key;
  AppendVLKey(keysds, BLOB, key);
  InsertVLVal(valsds, BLOB, key, x.str());
  return key;
}

void Hdf5Back::FillBuf(std::string title, char* buf, DatumList& group, 
                       size_t* sizes, size_t rowsize) {
  using std::min;
  using std::string;
  using std::vector;
  using std::set;
  using std::list;
  using std::pair;
  using std::map;
  Datum::Vals vals;
  Datum::Shape shape;
  Datum::Shapes shapes;
  Datum::Vals header = group.front()->vals();
  int ncols = header.size();
  DbTypes* dbtypes = schemas_[title];

  size_t offset = 0;
  const void* val;
  size_t fieldlen;
  size_t valuelen;
  DatumList::iterator it;
  for (it = group.begin(); it != group.end(); ++it) {
    vals = (*it)->vals();
    shapes = (*it)->shapes();
    for (int col = 0; col < ncols; ++col) {
      const boost::spirit::hold_any* a = &(vals[col].second);
      switch (dbtypes[col]) {
        case BOOL:
        case INT:
        case FLOAT:
        case DOUBLE: {
          val = a->castsmallvoid();
          memcpy(buf + offset, val, sizes[col]);
          break;
        }
        case STRING: {
          const std::string s = a->cast<std::string>();
          fieldlen = sizes[col];
          valuelen = std::min(s.size(), fieldlen);
          memcpy(buf + offset, s.c_str(), valuelen);
          memset(buf + offset + valuelen, 0, fieldlen - valuelen);
          break;
        }
        case VL_STRING: {
          Digest key = VLWrite<std::string, VL_STRING>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case BLOB: {
          Digest key = VLWrite<Blob, BLOB>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case UUID: {
          boost::uuids::uuid uuid = a->cast<boost::uuids::uuid>();
          memcpy(buf + offset, &uuid, CYCLUS_UUID_SIZE);
          break;
        }
        case VECTOR_INT: {
          std::vector<int> val = a->cast<std::vector<int> >();
          fieldlen = sizes[col];
          valuelen = std::min(val.size() * sizeof(int), fieldlen);
          memcpy(buf + offset, &val[0], valuelen);
          memset(buf + offset + valuelen, 0, fieldlen - valuelen);
          break;
        }
        case VL_VECTOR_INT: {
          Digest key = VLWrite<std::vector<int>, VL_VECTOR_INT>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case VECTOR_STRING: {
          vector<string> val = a->cast<vector<string> >();
          shape = shapes[col];
          fieldlen = shape[1];
          unsigned int cnt = 0;
          for (; cnt < val.size(); ++cnt) {
            valuelen = std::min(val[cnt].size(), fieldlen);
            memcpy(buf + offset + fieldlen*cnt, val[cnt].c_str(), valuelen);
            memset(buf + offset + fieldlen*cnt + valuelen, 0, fieldlen - valuelen);
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (shape[0] - cnt));
          break;
        }
        case VECTOR_VL_STRING: {
          vector<string> val = a->cast<vector<string> >();
          Digest key;
          unsigned int cnt = 0;
          string s;
          for (; cnt < val.size(); ++cnt) {
            key = VLWrite<string, VL_STRING>(val[cnt]);
            memcpy(buf + offset + CYCLUS_SHA1_SIZE*cnt, key.val, CYCLUS_SHA1_SIZE);
          }
          memset(buf + offset + CYCLUS_SHA1_SIZE*cnt, 0, CYCLUS_SHA1_SIZE * (val.size() - cnt));
          break;
        }
        case VL_VECTOR_STRING: {
          shape = shapes[col];
          size_t strlen = shape[1];
          vector<string> givenval = a->cast<vector<string> >();
          vector<string> val = vector<string>(givenval.size());
          unsigned int cnt = 0;
          // ensure string is of specified length
          for (; cnt < givenval.size(); ++cnt)
            val[cnt] = string(givenval[cnt], 0, strlen);
          Digest key = VLWrite<vector<string>, VL_VECTOR_STRING>(val);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case VL_VECTOR_VL_STRING: {
          Digest key = VLWrite<vector<string>, VL_VECTOR_VL_STRING>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case SET_INT: {
          std::set<int> val = a->cast<std::set<int> >();
          fieldlen = sizes[col];
          valuelen = std::min(val.size() * sizeof(int), fieldlen);
          unsigned int cnt = 0;
          for (std::set<int>::iterator sit = val.begin(); sit != val.end(); ++sit) {
            memcpy(buf + offset + cnt*sizeof(int), &(*sit), sizeof(int));
            ++cnt;
          }
          memset(buf + offset + valuelen, 0, fieldlen - valuelen);
          break;
        }
        case VL_SET_INT: {
          Digest key = VLWrite<std::set<int>, VL_SET_INT>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case SET_STRING: {
          set<string> val = a->cast<set<string> >();
          shape = shapes[col];
          fieldlen = shape[1];
          unsigned int cnt = 0;
          for (set<string>::iterator sit = val.begin(); sit != val.end(); ++sit) {
            valuelen = std::min(sit->size(), fieldlen);
            memcpy(buf + offset + fieldlen*cnt, sit->c_str(), valuelen);
            memset(buf + offset + fieldlen*cnt + valuelen, 0, fieldlen - valuelen);
            ++cnt;
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (shape[0] - cnt));
          break;
        }
        case SET_VL_STRING: {
          set<string> val = a->cast<set<string> >();
          Digest key;
          unsigned int cnt = 0;
          for (set<string>::iterator sit = val.begin(); sit != val.end(); ++sit) {
            key = VLWrite<string, VL_STRING>(*sit);
            memcpy(buf + offset + CYCLUS_SHA1_SIZE*cnt, key.val, CYCLUS_SHA1_SIZE);
            ++cnt;
          }
          memset(buf+offset+CYCLUS_SHA1_SIZE*cnt, 0, CYCLUS_SHA1_SIZE*(val.size()-cnt));
          break;
        }
        case VL_SET_STRING: {
          shape = shapes[col];
          size_t strlen = shape[1];
          set<string> givenval = a->cast<set<string> >();
          set<string> val;
          // ensure string is of specified length
          for (set<string>::iterator sit = givenval.begin(); sit != givenval.end(); ++sit)
            val.insert(string((*sit), 0, strlen));
          Digest key = VLWrite<set<string>, VL_SET_STRING>(val);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case VL_SET_VL_STRING: {
          Digest key = VLWrite<set<string>, VL_SET_VL_STRING>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case LIST_INT: {
          std::list<int> val = a->cast<std::list<int> >();
          fieldlen = sizes[col];
          valuelen = std::min(val.size() * sizeof(int), fieldlen);
          unsigned int cnt = 0;
          std::list<int>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            memcpy(buf + offset + cnt*sizeof(int), &(*valit), sizeof(int));
            ++cnt;
          }
          memset(buf + offset + valuelen, 0, fieldlen - valuelen);
          break;
        }
        case VL_LIST_INT: {
          Digest key = VLWrite<std::list<int>, VL_LIST_INT>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case LIST_STRING: {
          list<string> val = a->cast<list<string> >();
          shape = shapes[col];
          fieldlen = shape[1];
          unsigned int cnt = 0;
          list<string>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            valuelen = std::min(valit->size(), fieldlen);
            memcpy(buf + offset + fieldlen*cnt, valit->c_str(), valuelen);
            memset(buf + offset + fieldlen*cnt + valuelen, 0, fieldlen - valuelen);
            ++cnt;
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (shape[0] - cnt));
          break;
        }
        case LIST_VL_STRING: {
          list<string> val = a->cast<list<string> >();
          Digest key;
          unsigned int cnt = 0;
          list<string>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            key = VLWrite<string, VL_STRING>(*valit);
            memcpy(buf + offset + CYCLUS_SHA1_SIZE*cnt, key.val, CYCLUS_SHA1_SIZE);
            ++cnt;
          }
          memset(buf+offset+CYCLUS_SHA1_SIZE*cnt, 0, CYCLUS_SHA1_SIZE*(val.size()-cnt));
          break;
        }
        case VL_LIST_STRING: {
          shape = shapes[col];
          size_t strlen = shape[1];
          list<string> givenval = a->cast<list<string> >();
          list<string> val;
          // ensure string is of specified length
          list<string>::iterator valit = givenval.begin();
          for (; valit != givenval.end(); ++valit)
            val.push_back(string((*valit), 0, strlen));
          Digest key = VLWrite<list<string>, VL_LIST_STRING>(val);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case VL_LIST_VL_STRING: {
          Digest key = VLWrite<list<string>, VL_LIST_VL_STRING>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case PAIR_INT_INT: {
          std::pair<int, int> val = a->cast<std::pair<int, int> >();
          memcpy(buf + offset, &(val.first), sizeof(int));
          memcpy(buf + offset + sizeof(int), &(val.second), sizeof(int));
          break;
        }
        case MAP_INT_INT: {
          map<int, int> val = a->cast<map<int, int> >();
          fieldlen = sizes[col];
          valuelen = min(2 * sizeof(int) * val.size(), fieldlen);
          unsigned int cnt = 0;
          for (map<int, int>::iterator valit = val.begin(); valit != val.end(); ++valit) {
            memcpy(buf + offset + 2*sizeof(int)*cnt, &(valit->first), sizeof(int));
            memcpy(buf + offset + 2*sizeof(int)*cnt + sizeof(int), &(valit->second), sizeof(int));
            ++cnt;
          }
          memset(buf + offset + valuelen, 0, fieldlen - valuelen);
          break;
        }
        case VL_MAP_INT_INT: {
          Digest key = VLWrite<map<int, int>, VL_MAP_INT_INT>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case MAP_INT_DOUBLE: {
          map<int, double> val = a->cast<map<int, double> >();
          size_t itemsize = sizeof(int) + sizeof(double);
          fieldlen = sizes[col];
          valuelen = min(itemsize * val.size(), fieldlen);
          unsigned int cnt = 0;
          for (map<int, double>::iterator valit = val.begin(); valit != val.end(); ++valit) {
            memcpy(buf + offset + itemsize*cnt, &(valit->first), sizeof(int));
            memcpy(buf + offset + itemsize*cnt + sizeof(int), &(valit->second), 
                                                              sizeof(double));
            ++cnt;
          }
          memset(buf + offset + valuelen, 0, fieldlen - valuelen);
          break;
        }
        case VL_MAP_INT_DOUBLE: {
          Digest key = VLWrite<map<int, double>, VL_MAP_INT_DOUBLE>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case MAP_INT_STRING: {
          map<int, string> val = a->cast<map<int, string> >();
          shape = shapes[col];
          int strlen = shape[1];
          fieldlen = sizeof(int) + strlen;
          unsigned int cnt = 0;
          map<int, string>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            memcpy(buf + offset + fieldlen*cnt, &(valit->first), sizeof(int));
            valuelen = std::min(static_cast<int>(valit->second.size()), strlen);
            memcpy(buf + offset + fieldlen*cnt + sizeof(int), 
                   valit->second.c_str(), valuelen);
            memset(buf + offset + fieldlen*cnt + sizeof(int) + valuelen, 0, 
                   strlen - valuelen);
            ++cnt;
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (shape[0] - cnt));
          break;
        }
        case MAP_INT_VL_STRING: {
          map<int, string> val = a->cast<map<int, string> >();
          Digest valhash;
          fieldlen = sizeof(int) + CYCLUS_SHA1_SIZE;
          unsigned int cnt = 0;
          map<int, string>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            memcpy(buf + offset + fieldlen*cnt, &(valit->first), sizeof(int));
            valhash = VLWrite<string, VL_STRING>(valit->second);
            memcpy(buf + offset + fieldlen*cnt + sizeof(int), valhash.val, 
                   CYCLUS_SHA1_SIZE);
            ++cnt;
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (val.size() - cnt));
          break;
        }
        case VL_MAP_INT_STRING: {
          shape = shapes[col];
          size_t strlen = shape[1];
          map<int, string> givenval = a->cast<map<int, string> >();
          map<int, string> val;
          // ensure string is of specified length
          map<int, string>::iterator valit = givenval.begin();
          for (; valit != givenval.end(); ++valit)
            val[valit->first] = string(valit->second, 0, strlen);
          Digest key = VLWrite<map<int, string>, VL_MAP_INT_STRING>(val);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case VL_MAP_INT_VL_STRING: {
          Digest key = VLWrite<map<int, string>, VL_MAP_INT_VL_STRING>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case MAP_STRING_INT: {
          map<string, int> val = a->cast<map<string, int> >();
          shape = shapes[col];
          int strlen = shape[1];
          fieldlen = sizeof(int) + strlen;
          unsigned int cnt = 0;
          map<string, int>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            valuelen = std::min(static_cast<int>(valit->first.size()), strlen);
            memcpy(buf + offset + fieldlen*cnt, valit->first.c_str(), valuelen);
            memset(buf + offset + fieldlen*cnt + valuelen, 0, strlen - valuelen);
            memcpy(buf + offset + fieldlen*cnt + strlen, &(valit->second), sizeof(int));
            ++cnt;
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (shape[0] - cnt));
          break;
        }
        case MAP_VL_STRING_INT: {
          map<string, int> val = a->cast<map<string, int> >();
          Digest keyhash;
          fieldlen = sizeof(int) + CYCLUS_SHA1_SIZE;
          unsigned int cnt = 0;
          map<string, int>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            keyhash = VLWrite<string, VL_STRING>(valit->first);
            memcpy(buf + offset + fieldlen*cnt, keyhash.val, CYCLUS_SHA1_SIZE);
            memcpy(buf + offset + fieldlen*cnt + CYCLUS_SHA1_SIZE, &(valit->second), 
                   sizeof(int));
            ++cnt;
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (val.size() - cnt));
          break;
        }
        case VL_MAP_STRING_INT: {
          shape = shapes[col];
          size_t strlen = shape[1];
          map<string, int> givenval = a->cast<map<string, int> >();
          map<string, int> val;
          // ensure string is of specified length
          map<string, int>::iterator valit = givenval.begin();
          for (; valit != givenval.end(); ++valit)
            val[string(valit->first, 0, strlen)] = valit->second;
          Digest key = VLWrite<map<string, int>, VL_MAP_STRING_INT>(val);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case VL_MAP_VL_STRING_INT: {
          Digest key = VLWrite<map<string, int>, VL_MAP_VL_STRING_INT>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case MAP_STRING_DOUBLE: {
          map<string, double> val = a->cast<map<string, double> >();
          shape = shapes[col];
          int strlen = shape[1];
          fieldlen = sizeof(double) + strlen;
          unsigned int cnt = 0;
          map<string, double>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            valuelen = std::min(static_cast<int>(valit->first.size()), strlen);
            memcpy(buf + offset + fieldlen*cnt, valit->first.c_str(), valuelen);
            memset(buf + offset + fieldlen*cnt + valuelen, 0, strlen - valuelen);
            memcpy(buf + offset + fieldlen*cnt + strlen, &(valit->second), 
                   sizeof(double));
            ++cnt;
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (shape[0] - cnt));
          break;
        }
        case VL_MAP_STRING_DOUBLE: {
          shape = shapes[col];
          size_t strlen = shape[1];
          map<string, double> givenval = a->cast<map<string, double> >();
          map<string, double> val;
          // ensure string is of specified length
          map<string, double>::iterator valit = givenval.begin();
          for (; valit != givenval.end(); ++valit)
            val[string(valit->first, 0, strlen)] = valit->second;
          Digest key = VLWrite<map<string, double>, VL_MAP_STRING_DOUBLE>(val);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }


        case MAP_STRING_STRING: {
          map<string, string> val = a->cast<map<string, string> >();
          shape = shapes[col];
          int keylen = shape[1];
          int vallen = shape[2];
          fieldlen = keylen + vallen;
          unsigned int cnt = 0;
          int truekeylen;
          int truevallen;
          map<string, string>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            truekeylen = std::min(static_cast<int>(valit->first.size()), keylen);
            memcpy(buf + offset + fieldlen*cnt, valit->first.c_str(), truekeylen);
            memset(buf + offset + fieldlen*cnt + truekeylen, 0, keylen - truekeylen);
            truevallen = std::min(static_cast<int>(valit->second.size()), vallen);
            memcpy(buf + offset + fieldlen*cnt + keylen, valit->second.c_str(),
                   truevallen);
            memset(buf + offset + fieldlen*cnt + keylen + truekeylen, 0, 
                   vallen - truevallen);
            ++cnt;
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (shape[0] - cnt));
          break;
        }
        case VL_MAP_STRING_STRING: {
          shape = shapes[col];
          size_t keylen = shape[1];
          size_t vallen = shape[2];
          map<string, string> givenval = a->cast<map<string, string> >();
          map<string, string> val;
          // ensure strings of specified length
          map<string, string>::iterator valit = givenval.begin();
          for (; valit != givenval.end(); ++valit)
            val[string(valit->first, 0, keylen)] = string(valit->second, 0, vallen);
          Digest key = VLWrite<map<string, string>, VL_MAP_STRING_STRING>(val);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case MAP_STRING_VL_STRING: {
          map<string, string> val = a->cast<map<string, string> >();
          Digest valhash;
          shape = shapes[col];
          size_t keylen = shape[1];
          fieldlen = CYCLUS_SHA1_SIZE + keylen;
          int truekeylen;
          unsigned int cnt = 0;
          map<string, string>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            truekeylen = std::min(valit->first.size(), keylen);
            memcpy(buf + offset + fieldlen*cnt, valit->first.c_str(), truekeylen);
            memset(buf + offset + fieldlen*cnt + truekeylen, 0, keylen - truekeylen);
            valhash = VLWrite<string, VL_STRING>(valit->second);
            memcpy(buf + offset + fieldlen*cnt + keylen, valhash.val, CYCLUS_SHA1_SIZE);
            ++cnt;
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (val.size() - cnt));
          break;
        }
        case VL_MAP_STRING_VL_STRING: {
          Digest key = VLWrite<map<string, string>, VL_MAP_STRING_VL_STRING>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        case MAP_VL_STRING_DOUBLE: {
          map<string, double> val = a->cast<map<string, double> >();
          Digest keyhash;
          fieldlen = sizeof(double) + CYCLUS_SHA1_SIZE;
          unsigned int cnt = 0;
          map<string, double>::iterator valit = val.begin();
          for (; valit != val.end(); ++valit) {
            keyhash = VLWrite<string, VL_STRING>(valit->first);
            memcpy(buf + offset + fieldlen*cnt, keyhash.val, CYCLUS_SHA1_SIZE);
            memcpy(buf + offset + fieldlen*cnt + CYCLUS_SHA1_SIZE, &(valit->second), 
                   sizeof(double));
            ++cnt;
          }
          memset(buf + offset + fieldlen*cnt, 0, fieldlen * (val.size() - cnt));
          break;
        }
        case VL_MAP_VL_STRING_DOUBLE: {
          Digest key = VLWrite<map<string, double>, VL_MAP_VL_STRING_DOUBLE>(a);
          memcpy(buf + offset, key.val, CYCLUS_SHA1_SIZE);
          break;
        }
        default: {
          std::cout << "dbtypes[col] " << dbtypes[col] << "\n";
          throw ValueError("attempted to retrieve unsupported HDF5 backend type");
        }
      }
      offset += sizes[col];
    }
  }
}

template <typename T, DbTypes U>
T Hdf5Back::VLRead(const char* rawkey) { 
  // key is used as offset
  Digest key;
  memcpy(key.val, rawkey, CYCLUS_SHA1_SIZE);
  const std::vector<hsize_t> idx = key.cast<hsize_t>();
  hid_t dset = VLDataset(U, false);
  hid_t dspace = H5Dget_space(dset);
  hid_t mspace = H5Screate_simple(CYCLUS_SHA1_NINT, vlchunk_, NULL);
  herr_t status = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, (const hsize_t*) &idx[0], 
                                      NULL, vlchunk_, NULL);
  if (status < 0)
    throw IOError("could not select hyperslab of value array for reading "
                  "in the database '" + path_ + "'.");
  hvl_t buf;
  status = H5Dread(dset, vldts_[U], mspace, dspace, H5P_DEFAULT, &buf);
  if (status < 0)
    throw IOError("failed to read in variable length data "
                  "in the database '" + path_ + "'.");
  T val = VLBufToVal<T>(buf);
  status = H5Dvlen_reclaim(vldts_[U], mspace, H5P_DEFAULT, &buf);
  if (status < 0)
    throw IOError("failed to reclaim variable length data space "
                  "in the database '" + path_ + "'.");
  H5Sclose(mspace);
  H5Sclose(dspace);
  return val;
}


hid_t Hdf5Back::VLDataset(DbTypes dbtype, bool forkeys) {
  std::string name;
  switch (dbtype) {
    case VL_STRING: {
      name = "String";
      break;
    }
    case BLOB: {
      name = "Blob";
      break;
    }
    case VL_VECTOR_INT: {
      name = "VectorInt";
      break;
    }
    case VL_VECTOR_STRING:
    case VL_VECTOR_VL_STRING: {
      name = "VectorString";
      break;
    }
    case VL_SET_INT: {
      name = "SetInt";
      break;
    }
    case VL_SET_STRING:
    case VL_SET_VL_STRING: {
      name = "SetString";
      break;
    }
    case VL_LIST_INT: {
      name = "ListInt";
      break;
    }
    case VL_LIST_STRING:
    case VL_LIST_VL_STRING: {
      name = "ListString";
      break;
    }
    case VL_MAP_INT_INT: {
      name = "MapIntInt";
      break;
    }
    case VL_MAP_INT_DOUBLE: {
      name = "MapIntDouble";
      break;
    }
    case VL_MAP_INT_STRING:
    case VL_MAP_INT_VL_STRING: {
      name = "MapIntString";
      break;
    }
    case VL_MAP_STRING_INT:
    case VL_MAP_VL_STRING_INT: {
      name = "MapStringInt";
      break;
    }
    case VL_MAP_STRING_DOUBLE:
    case VL_MAP_VL_STRING_DOUBLE: {
      name = "MapStringDouble";
      break;
    }
    case VL_MAP_STRING_STRING:
    case VL_MAP_STRING_VL_STRING:
    case VL_MAP_VL_STRING_STRING:
    case VL_MAP_VL_STRING_VL_STRING: {
      name = "MapStringString";
      break;
    }
    default: {
      throw IOError("could not determine variable length dataset name.");
      break;
    }
  }
  name += forkeys ? "Keys" : "Vals";

  // already opened
  if (vldatasets_.count(name) > 0)
    return vldatasets_[name];

  // already in db
  hid_t dset;
  hid_t dspace;
  herr_t status;
  if (H5Lexists(file_, name.c_str(), H5P_DEFAULT)) {
    dset = H5Dopen2(file_, name.c_str(), H5P_DEFAULT);
    if (forkeys) {
      // read in existing keys to vlkeys_
      dspace = H5Dget_space(dset);
      unsigned int nkeys = H5Sget_simple_extent_npoints(dspace);
      char* buf = new char [CYCLUS_SHA1_SIZE * nkeys];
      status = H5Dread(dset, sha1_type_, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
      if (status < 0)
        throw IOError("failed to read in keys for " + name);
      for (int n = 0; n < nkeys; ++n) {
        Digest d = Digest();
        memcpy(d.val, buf + (n * CYCLUS_SHA1_SIZE), CYCLUS_SHA1_SIZE);
        vlkeys_[dbtype].insert(d);
      }
      H5Sclose(dspace);
      delete[] buf;
    }
    vldatasets_[name] = dset;
    return dset;
  }

  // doesn't exist at all
  hid_t dt;
  hid_t prop;
  if (forkeys) {
    hsize_t dims[1] = {0};
    hsize_t maxdims[1] = {H5S_UNLIMITED};
    hsize_t chunkdims[1] = {512};  // this is a 10 kb chunksize
    dt = sha1_type_;
    dspace = H5Screate_simple(1, dims, maxdims);
    prop = H5Pcreate(H5P_DATASET_CREATE);
    status = H5Pset_chunk(prop, 1, chunkdims);
    if (status < 0) 
      throw IOError("could not create HDF5 array " + name);
  } else {
    hsize_t dims[CYCLUS_SHA1_NINT] = {UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX};
    hsize_t chunkdims[CYCLUS_SHA1_NINT] = {1, 1, 1, 1, 1};  // this is a single element
    dt = vldts_[dbtype];
    dspace = H5Screate_simple(CYCLUS_SHA1_NINT, dims, dims);
    prop = H5Pcreate(H5P_DATASET_CREATE);
    status = H5Pset_chunk(prop, CYCLUS_SHA1_NINT, chunkdims);
    if (status < 0) 
      throw IOError("could not create HDF5 array " + name);
  }
  dset = H5Dcreate2(file_, name.c_str(), dt, dspace, H5P_DEFAULT, prop, H5P_DEFAULT);
  vldatasets_[name] = dset;
  return dset;  
}

void Hdf5Back::AppendVLKey(hid_t dset, DbTypes dbtype, const Digest& key) {
  hid_t dspace = H5Dget_space(dset);
  hsize_t origlen = H5Sget_simple_extent_npoints(dspace);
  hsize_t newlen[1] = {origlen + 1};
  hsize_t offset[1] = {origlen};
  hsize_t extent[1] = {1};
  hid_t mspace = H5Screate_simple(1, extent, NULL);
  herr_t status = H5Dextend(dset, newlen);
  if (status < 0)
    throw IOError("could not resize key array in the database '" + path_ + "'.");
  dspace = H5Dget_space(dset);
  status = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, offset, NULL, extent, NULL);
  if (status < 0)
    throw IOError("could not select hyperslab of key array "
                  "in the database '" + path_ + "'.");
  status = H5Dwrite(dset, sha1_type_, mspace, dspace, H5P_DEFAULT, key.val);
  if (status < 0)
    throw IOError("could not write digest to key array "
                  "in the database '" + path_ + "'.");
  H5Sclose(mspace);
  H5Sclose(dspace);
  vlkeys_[dbtype].insert(key);
}

void Hdf5Back::InsertVLVal(hid_t dset, DbTypes dbtype, const Digest& key, 
                           const std::string& val) {
  hid_t dspace = H5Dget_space(dset);
  hsize_t extent[CYCLUS_SHA1_NINT] = {1, 1, 1, 1, 1};
  hid_t mspace = H5Screate_simple(CYCLUS_SHA1_NINT, extent, NULL);
  const std::vector<hsize_t> idx = key.cast<hsize_t>();
  herr_t status = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, (const hsize_t*) &idx[0], 
                                      NULL, extent, NULL);
  if (status < 0)
    throw IOError("could not select hyperslab of value array "
                  "in the database '" + path_ + "'.");
  const char* buf[1] = {val.c_str()};
  status = H5Dwrite(dset, vldts_[dbtype], mspace, dspace, H5P_DEFAULT, buf);
  if (status < 0)
    throw IOError("could not write string to value array "
                  "in the database '" + path_ + "'.");
  H5Sclose(mspace);
  H5Sclose(dspace);
};

void Hdf5Back::InsertVLVal(hid_t dset, DbTypes dbtype, const Digest& key, 
                           hvl_t buf) {
  hid_t dspace = H5Dget_space(dset);
  hsize_t extent[CYCLUS_SHA1_NINT] = {1, 1, 1, 1, 1};
  hid_t mspace = H5Screate_simple(CYCLUS_SHA1_NINT, extent, NULL);
  const std::vector<hsize_t> idx = key.cast<hsize_t>();
  herr_t status = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, (const hsize_t*) &idx[0], 
                                      NULL, extent, NULL);
  if (status < 0)
    throw IOError("could not select hyperslab of value array "
                  "in the database '" + path_ + "'.");
  status = H5Dwrite(dset, vldts_[dbtype], mspace, dspace, H5P_DEFAULT, &buf);
  if (status < 0)
    throw IOError("could not write variable length data to value array "
                  "in the database '" + path_ + "'.");
  status = H5Dvlen_reclaim(vldts_[dbtype], mspace, H5P_DEFAULT, &buf);
  if (status < 0)
    throw IOError("could not free variable length buffer "
                  "in the database '" + path_ + "'.");
  H5Sclose(mspace);
  H5Sclose(dspace);
};

hvl_t Hdf5Back::VLValToBuf(const std::vector<int>& x) {
  hvl_t buf;
  buf.len = x.size();
  size_t nbytes = buf.len * sizeof(int);
  buf.p = new char[nbytes];
  memcpy(buf.p, &x[0], nbytes);
  return buf;
};

template <>
std::vector<int> Hdf5Back::VLBufToVal<std::vector<int> >(const hvl_t& buf) {
  std::vector<int> x = std::vector<int>(buf.len);
  memcpy(&x[0], buf.p, buf.len * sizeof(int));
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::vector<std::string>& x) {
  // VL_VECTOR_STRING implemented as VL_VECTOR_VL_STRING
  hvl_t buf;
  Digest key;
  buf.len = x.size();
  size_t nbytes = CYCLUS_SHA1_SIZE * buf.len;
  buf.p = new char[nbytes];
  for (unsigned int i = 0; i < buf.len; ++i) {
    key = VLWrite<std::string, VL_STRING>(x[i]);
    memcpy((char *) buf.p + CYCLUS_SHA1_SIZE*i, key.val, CYCLUS_SHA1_SIZE);
  }
  return buf;
};

template <>
std::vector<std::string> Hdf5Back::VLBufToVal<std::vector<std::string> >(const hvl_t& buf) {
  using std::string;
  std::vector<string> x = std::vector<string>(buf.len);
  for (unsigned int i = 0; i < buf.len; ++i)
    x[i] = VLRead<string, VL_STRING>((char *) buf.p + CYCLUS_SHA1_SIZE*i);
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::set<int>& x) {
  hvl_t buf;
  buf.len = x.size();
  size_t nbytes = buf.len * sizeof(int);
  buf.p = new char[nbytes];
  unsigned int cnt = 0;
  std::set<int>::iterator it = x.begin();
  for (; it != x.end(); ++it) {
    memcpy((char *) buf.p + cnt*sizeof(int), &(*it), sizeof(int));
    ++cnt;
  }
  return buf;
};

template <>
std::set<int> Hdf5Back::VLBufToVal<std::set<int> >(const hvl_t& buf) {
  int* xraw = reinterpret_cast<int*>(buf.p);
  std::set<int> x = std::set<int>(xraw, xraw+buf.len);
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::set<std::string>& x) {
  // VL_SET_STRING implemented as VL_SET_VL_STRING
  hvl_t buf;
  Digest key;
  buf.len = x.size();
  size_t nbytes = CYCLUS_SHA1_SIZE * buf.len;
  buf.p = new char[nbytes];
  unsigned int i = 0;
  std::set<std::string>::iterator it = x.begin();
  for (; it != x.end(); ++it) {
    key = VLWrite<std::string, VL_STRING>(*it);
    memcpy((char *) buf.p + CYCLUS_SHA1_SIZE*i, key.val, CYCLUS_SHA1_SIZE);
    ++i;
  }
  return buf;
};

template <>
std::set<std::string> Hdf5Back::VLBufToVal<std::set<std::string> >(const hvl_t& buf) {
  using std::string;
  std::set<string> x;
  for (unsigned int i = 0; i < buf.len; ++i)
    x.insert(VLRead<string, VL_STRING>((char *) buf.p + CYCLUS_SHA1_SIZE*i));
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::list<int>& x) {
  hvl_t buf;
  buf.len = x.size();
  size_t nbytes = buf.len * sizeof(int);
  buf.p = new char[nbytes];
  unsigned int cnt = 0;
  std::list<int>::const_iterator it = x.begin();
  for (; it != x.end(); ++it) {
    memcpy((char *) buf.p + cnt*sizeof(int), &(*it), sizeof(int));
    ++cnt;
  }
  return buf;
};

template <>
std::list<int> Hdf5Back::VLBufToVal<std::list<int> >(const hvl_t& buf) {
  int* xraw = reinterpret_cast<int*>(buf.p);
  std::list<int> x = std::list<int>(xraw, xraw+buf.len);
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::list<std::string>& x) {
  // VL_LIST_STRING implemented as VL_LIST_VL_STRING
  hvl_t buf;
  Digest key;
  buf.len = x.size();
  size_t nbytes = CYCLUS_SHA1_SIZE * buf.len;
  buf.p = new char[nbytes];
  unsigned int i = 0;
  std::list<std::string>::const_iterator it = x.begin();
  for (; it != x.end(); ++it) {
    key = VLWrite<std::string, VL_STRING>(*it);
    memcpy((char *) buf.p + CYCLUS_SHA1_SIZE*i, key.val, CYCLUS_SHA1_SIZE);
    ++i;
  }
  return buf;
};

template <>
std::list<std::string> Hdf5Back::VLBufToVal<std::list<std::string> >(const hvl_t& buf) {
  using std::string;
  std::list<string> x;
  for (unsigned int i = 0; i < buf.len; ++i)
    x.push_back(VLRead<string, VL_STRING>((char *) buf.p + CYCLUS_SHA1_SIZE*i));
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::map<int, int>& x) {
  hvl_t buf;
  buf.len = x.size();
  size_t nbytes = 2 * sizeof(int) * buf.len;
  buf.p = new char[nbytes];
  unsigned int cnt = 0;
  std::map<int, int>::const_iterator it = x.begin();
  for (; it != x.end(); ++it) {
    memcpy((char *) buf.p + 2*sizeof(int)*cnt, &(it->first), sizeof(int));
    memcpy((char *) buf.p + 2*sizeof(int)*cnt + sizeof(int), &(it->second), sizeof(int));
    ++cnt;
  }
  return buf;
};

template <>
std::map<int, int> Hdf5Back::VLBufToVal<std::map<int, int> >(const hvl_t& buf) {
  int* xraw = reinterpret_cast<int*>(buf.p);
  std::map<int, int> x = std::map<int, int>();
  for (unsigned int i = 0; i < buf.len; ++i)
    x[xraw[2*i]] = xraw[2*i + 1];
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::map<int, double>& x) {
  hvl_t buf;
  buf.len = x.size();
  size_t itemsize = sizeof(int) + sizeof(double);
  size_t nbytes = itemsize * buf.len;
  buf.p = new char[nbytes];
  unsigned int cnt = 0;
  std::map<int, double>::const_iterator it = x.begin();
  for (; it != x.end(); ++it) {
    memcpy((char *) buf.p + itemsize*cnt, &(it->first), sizeof(int));
    memcpy((char *) buf.p + itemsize*cnt + sizeof(int), &(it->second), sizeof(double));
    ++cnt;
  }
  return buf;
};

template <>
std::map<int, double> Hdf5Back::VLBufToVal<std::map<int, double> >(const hvl_t& buf) {
  std::map<int, double> x;
  char * p = reinterpret_cast<char*>(buf.p);
  size_t itemsize = sizeof(int) + sizeof(double);
  for (unsigned int i = 0; i < buf.len; ++i)
    x[*reinterpret_cast<int*>(p + itemsize*i)] = \
      *reinterpret_cast<double*>(p + itemsize*i + sizeof(int));
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::map<int, std::string>& x) {
  // VL_MAP_INT_STRING implemented as VL_MAP_INT_VL_STRING
  hvl_t buf;
  Digest valhash;
  buf.len = x.size();
  size_t itemsize = sizeof(int) + CYCLUS_SHA1_SIZE;
  size_t nbytes = itemsize * buf.len;
  buf.p = new char[nbytes];
  unsigned int i = 0;
  std::map<int, std::string>::const_iterator it = x.begin();
  for (; it != x.end(); ++it) {
    memcpy((char *) buf.p + itemsize*i, &(it->first), sizeof(int));
    valhash = VLWrite<std::string, VL_STRING>(it->second);
    memcpy((char *) buf.p + itemsize*i + sizeof(int), valhash.val, CYCLUS_SHA1_SIZE);
    ++i;
  }
  return buf;
};

template <> std::map<int, std::string> 
Hdf5Back::VLBufToVal<std::map<int, std::string> >(const hvl_t& buf) {
  using std::string;
  std::map<int, string> x;
  char * p = reinterpret_cast<char*>(buf.p);
  size_t itemsize = sizeof(int) + CYCLUS_SHA1_SIZE;
  for (unsigned int i = 0; i < buf.len; ++i)
    x[*reinterpret_cast<int*>(p + itemsize*i)] = \
      VLRead<string, VL_STRING>(p + itemsize*i + sizeof(int));
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::map<std::string, int>& x) {
  // VL_MAP_STRING_INT implemented as VL_MAP_VL_STRING_INT
  hvl_t buf;
  Digest keyhash;
  buf.len = x.size();
  size_t itemsize = sizeof(int) + CYCLUS_SHA1_SIZE;
  size_t nbytes = itemsize * buf.len;
  buf.p = new char[nbytes];
  unsigned int i = 0;
  std::map<std::string, int>::const_iterator it = x.begin();
  for (; it != x.end(); ++it) {
    keyhash = VLWrite<std::string, VL_STRING>(it->first);
    memcpy((char *) buf.p + itemsize*i, keyhash.val, CYCLUS_SHA1_SIZE);
    memcpy((char *) buf.p + itemsize*i + CYCLUS_SHA1_SIZE, &(it->second), sizeof(int));
    ++i;
  }
  return buf;
};

template <> std::map<std::string, int> 
Hdf5Back::VLBufToVal<std::map<std::string, int> >(const hvl_t& buf) {
  using std::string;
  std::map<string, int> x;
  char * p = reinterpret_cast<char*>(buf.p);
  size_t itemsize = sizeof(int) + CYCLUS_SHA1_SIZE;
  for (unsigned int i = 0; i < buf.len; ++i)
    x[VLRead<string, VL_STRING>(p + itemsize*i)] = \
      *reinterpret_cast<int*>(p + itemsize*i + CYCLUS_SHA1_SIZE);
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::map<std::string, double>& x) {
  // VL_MAP_STRING_DOUBLE implemented as VL_MAP_VL_STRING_DOUBLE
  hvl_t buf;
  Digest keyhash;
  buf.len = x.size();
  size_t itemsize = sizeof(double) + CYCLUS_SHA1_SIZE;
  size_t nbytes = itemsize * buf.len;
  buf.p = new char[nbytes];
  unsigned int i = 0;
  std::map<std::string, double>::const_iterator it = x.begin();
  for (; it != x.end(); ++it) {
    keyhash = VLWrite<std::string, VL_STRING>(it->first);
    memcpy((char *) buf.p + itemsize*i, keyhash.val, CYCLUS_SHA1_SIZE);
    memcpy((char *) buf.p + itemsize*i + CYCLUS_SHA1_SIZE, &(it->second),
           sizeof(double));
    ++i;
  }
  return buf;
};

template <> std::map<std::string, double> 
Hdf5Back::VLBufToVal<std::map<std::string, double> >(const hvl_t& buf) {
  using std::string;
  std::map<string, double> x;
  char * p = reinterpret_cast<char*>(buf.p);
  size_t itemsize = sizeof(double) + CYCLUS_SHA1_SIZE;
  for (unsigned int i = 0; i < buf.len; ++i)
    x[VLRead<string, VL_STRING>(p + itemsize*i)] = \
      *reinterpret_cast<double*>(p + itemsize*i + CYCLUS_SHA1_SIZE);
  return x;
};

hvl_t Hdf5Back::VLValToBuf(const std::map<std::string, std::string>& x) {
  // VL_MAP_STRING_STRING, VL_MAP_STRING_VL_STRING, and VL_MAP_VL_STRING_STRING
  // implemented as VL_MAP_VL_STRING_VL_STRING
  hvl_t buf;
  Digest keyhash;
  Digest valhash;
  buf.len = x.size();
  size_t itemsize = 2*CYCLUS_SHA1_SIZE;
  size_t nbytes = itemsize * buf.len;
  buf.p = new char[nbytes];
  unsigned int i = 0;
  std::map<std::string, std::string>::const_iterator it = x.begin();
  for (; it != x.end(); ++it) {
    keyhash = VLWrite<std::string, VL_STRING>(it->first);
    memcpy((char *) buf.p + itemsize*i, keyhash.val, CYCLUS_SHA1_SIZE);
    valhash = VLWrite<std::string, VL_STRING>(it->second);
    memcpy((char *) buf.p + itemsize*i + CYCLUS_SHA1_SIZE, valhash.val,
           CYCLUS_SHA1_SIZE);
    ++i;
  }
  return buf;
};

template <> std::map<std::string, std::string> 
Hdf5Back::VLBufToVal<std::map<std::string, std::string> >(const hvl_t& buf) {
  using std::string;
  std::map<string, string> x;
  char * p = reinterpret_cast<char*>(buf.p);
  size_t itemsize = 2 * CYCLUS_SHA1_SIZE;
  for (unsigned int i = 0; i < buf.len; ++i)
    x[VLRead<string, VL_STRING>(p + itemsize*i)] = \
      VLRead<string, VL_STRING>(p + itemsize*i + CYCLUS_SHA1_SIZE);
  return x;
};

} // namespace cyclus
