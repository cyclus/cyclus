// hdf5_back.cc
#include "hdf5_back.h"

#include <cmath>
#include <string.h>

#include "blob.h"

namespace cyclus {

const hsize_t Hdf5Back::vlchunk_[CYCLUS_SHA1_NINT] = {1, 1, 1, 1, 1};

Hdf5Back::Hdf5Back(std::string path) : path_(path) {
  hasher_ = Sha1();
  if (boost::filesystem::exists(path_))
    file_ = H5Fopen(path_.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  else  
    file_ = H5Fcreate(path_.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  opened_types_ = std::set<hid_t>();
  vldatasets_ = std::map<std::string, hid_t>();
  vldts_ = std::map<DbTypes, hid_t>();
  vlkeys_ = std::map<DbTypes, std::set<Digest> >();

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
  Hdf5Back::Flush();
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
  for (it = tbl_offset_.begin(); it != tbl_offset_.end(); ++it) {
    delete[](it->second);
  }
  for (it = tbl_sizes_.begin(); it != tbl_sizes_.end(); ++it) {
    delete[](it->second);
  }
  for (dbtit = tbl_types_.begin(); dbtit != tbl_types_.end(); ++dbtit) {
    delete[](dbtit->second);
  }
};

void Hdf5Back::Notify(DatumList data) {
  std::map<std::string, DatumList> groups;
  for (DatumList::iterator it = data.begin(); it != data.end(); ++it) {
    std::string name = (*it)->title();
    if (tbl_size_.count(name) == 0) {
      if (H5Lexists(file_, name.c_str(), H5P_DEFAULT)) {
        LoadTableTypes(name, (*it)->vals().size());
      } else {
        Datum* d = *it;
        CreateTable(d);
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
    throw IOError("could not select hyperslab of string value array for reading.");
  char** buf = new char* [sizeof(char *)];
  status = H5Dread(dset, vldts_[VL_STRING], mspace, dspace, H5P_DEFAULT, buf);
  if (status < 0)
    throw IOError("failed to read in variable length string data.");
  string val = string(buf[0]);
  status = H5Dvlen_reclaim(vldts_[VL_STRING], mspace, H5P_DEFAULT, buf);
  if (status < 0)
    throw IOError("failed to reclaim variable length string data space.");
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
    throw IOError("could not select hyperslab of value array for reading.");
  char** buf = new char* [sizeof(char *)];
  status = H5Dread(dset, vldts_[BLOB], mspace, dspace, H5P_DEFAULT, buf);
  if (status < 0)
    throw IOError("failed to read in variable length data.");
  Blob val = Blob(buf[0]);
  status = H5Dvlen_reclaim(vldts_[BLOB], mspace, H5P_DEFAULT, buf);
  if (status < 0)
    throw IOError("failed to reclaim variable lenght data space.");
  delete[] buf;
  H5Sclose(mspace);
  H5Sclose(dspace);
  return val;
}

QueryResult Hdf5Back::Query(std::string table, std::vector<Cond>* conds) {
  using std::vector;
  using std::set;
  using std::list;
  using std::pair;
  using std::map;
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
  unsigned int n = 0;

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
  for (i = 0; i < nfields; ++i)
    if (field_conds.count(qr.fields[i]) == 0)
      field_conds[qr.fields[i]] = std::vector<Cond*>();
  for (n; n < nchunks; ++n) {
    hsize_t start = n * tb_chunksize;
    hsize_t count = (tb_length-start)<tb_chunksize ? tb_length - start : tb_chunksize;
    char* buf = new char [tb_typesize * count];
    hid_t memspace = H5Screate_simple(1, &count, NULL);
    status = H5Sselect_hyperslab(tb_space, H5S_SELECT_SET, &start, NULL, &count, NULL);
    status = H5Dread(tb_set, tb_type, memspace, tb_space, H5P_DEFAULT, buf);
    int offset = 0;
    bool is_valid_row;
    QueryRow row = QueryRow(nfields);
    for (i = 0; i < count; ++i) {
      offset = i * tb_typesize;
      is_valid_row = true;
      for (j = 0; j < nfields; ++j) {
        switch (qr.types[j]) {
          case BOOL: {
            bool x = *reinterpret_cast<bool*>(buf + offset);
            is_valid_row = CmpConds<bool>(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case INT: {
            int x = *reinterpret_cast<int*>(buf + offset);
            is_valid_row = CmpConds<int>(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case FLOAT: {
            float x = *reinterpret_cast<float*>(buf + offset);
            is_valid_row = CmpConds<float>(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case DOUBLE: {
            double x = *reinterpret_cast<double*>(buf + offset);
            is_valid_row = CmpConds<double>(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case STRING: {
            std::string x = std::string(buf + offset, tbl_sizes_[table][j]);
            size_t nullpos = x.find('\0');
            if (nullpos != std::string::npos)
              x.resize(nullpos);
            is_valid_row = CmpConds<std::string>(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case VL_STRING: {
            std::string x = VLRead<std::string, VL_STRING>(buf + offset);
            is_valid_row = CmpConds<std::string>(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case BLOB: {
            Blob x = VLRead<Blob, BLOB>(buf + offset);
            is_valid_row = CmpConds<Blob>(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case UUID: {
            boost::uuids::uuid x;
            memcpy(&x, buf + offset, 16);
            is_valid_row = CmpConds<boost::uuids::uuid>(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case VECTOR_INT: {
            std::vector<int> x = std::vector<int>(tbl_sizes_[table][j] / sizeof(int));
            memcpy(&x[0], buf + offset, tbl_sizes_[table][j]);
            is_valid_row = CmpConds<std::vector<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case VL_VECTOR_INT: {
            std::vector<int> x = VLRead<std::vector<int>, VL_VECTOR_INT>(buf + offset);
            is_valid_row = CmpConds<std::vector<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case SET_INT: {
            jlen = tbl_sizes_[table][j] / sizeof(int);
            int* xraw = reinterpret_cast<int*>(buf + offset);
            std::set<int> x = std::set<int>(xraw, xraw+jlen);
            is_valid_row = CmpConds<std::set<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case VL_SET_INT: {
            std::set<int> x = VLRead<std::set<int>, VL_SET_INT>(buf + offset);
            is_valid_row = CmpConds<std::set<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case LIST_INT: {
            jlen = tbl_sizes_[table][j] / sizeof(int);
            int* xraw = reinterpret_cast<int*>(buf + offset);
            std::list<int> x = std::list<int>(xraw, xraw+jlen);
            is_valid_row = CmpConds<std::list<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case VL_LIST_INT: {
            std::list<int> x = VLRead<std::list<int>, VL_LIST_INT>(buf + offset);
            is_valid_row = CmpConds<std::list<int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case PAIR_INT_INT: {
            pair<int, int> x = std::make_pair(*reinterpret_cast<int*>(buf + offset), 
                                              *reinterpret_cast<int*>(buf + offset + sizeof(int)));
            is_valid_row = CmpConds<pair<int, int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case MAP_INT_INT: {
            map<int, int> x = map<int, int>();
            jlen = tbl_sizes_[table][j] / (2*sizeof(int));
            for (unsigned int k = 0; k < jlen; ++k) {
              x[*reinterpret_cast<int*>(buf + offset + 2*sizeof(int)*k)] = \
                *reinterpret_cast<int*>(buf + offset + 2*sizeof(int)*k + sizeof(int));
            }
            is_valid_row = CmpConds<map<int, int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          case VL_MAP_INT_INT: {
            map<int, int> x = VLRead<map<int, int>, VL_MAP_INT_INT>(buf + offset);
            is_valid_row = CmpConds<map<int, int> >(&x, &(field_conds[qr.fields[j]]));
            if (is_valid_row)
              row[j] = x;
            break;
          }
          default: {
            throw IOError("querying column '" + qr.fields[j] + "' in table '" + \
                          table + "' failed due to unsupported data type.");
            break;
          }
        }
        if (!is_valid_row)
          break;
        offset += tbl_sizes_[table][j];
      }
      if (is_valid_row) {
        qr.rows.push_back(row);
        QueryRow row = QueryRow(nfields);
      } else {
        row.clear();
        row.reserve(nfields);
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
  DbTypes* dbtypes = tbl_types_[title];

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
  if (tbl_types_.count(title) > 0)
    return;
  hid_t dset = H5Dopen2(file_, title.c_str(), H5P_DEFAULT);
  LoadTableTypes(title, dset, ncols);
  H5Dclose(dset);
}

void Hdf5Back::LoadTableTypes(std::string title, hid_t dset, hsize_t ncols) {
  if (tbl_types_.count(title) > 0)
    return;

  int i;
  hid_t subt;
  hid_t t = H5Dget_type(dset);
  tbl_size_[title] = H5Tget_size(t);
  size_t* offsets = new size_t[ncols];
  size_t* sizes = new size_t[ncols];
  for (i = 0; i < ncols; ++i) {
    offsets[i] = H5Tget_member_offset(t, i);
    subt = H5Tget_member_type(t, i);
    sizes[i] = H5Tget_size(subt);
    H5Tclose(subt);
  }
  H5Tclose(t);
  tbl_offset_[title] = offsets;
  tbl_sizes_[title] = sizes;

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
  tbl_types_[title] = dbtypes;
}

std::string Hdf5Back::Name() {
  return path_;
}

void Hdf5Back::CreateTable(Datum* d) {
  Datum::Vals vals = d->vals();
  hsize_t nvals = vals.size();
  Datum::Shape shape;
  Datum::Shapes shapes = d->shapes();

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
      if (shape == NULL || (*shape)[0] < 1) {
        dbtypes[i] = VL_STRING;
        field_types[i] = sha1_type_;
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = STRING;
        field_types[i] = H5Tcopy(H5T_C_S1);
        H5Tset_size(field_types[i], (*shape)[0]);
        H5Tset_strpad(field_types[i], H5T_STR_NULLPAD);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = sizeof(char) * (*shape)[0];
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
      if (shape == NULL || (*shape)[0] < 1) {
        dbtypes[i] = VL_VECTOR_INT;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_VECTOR_INT) == 0) {
          vldts_[VL_VECTOR_INT] = H5Tvlen_create(H5T_NATIVE_INT);
          opened_types_.insert(vldts_[VL_VECTOR_INT]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = VECTOR_INT;
        field_types[i] = H5Tarray_create2(H5T_NATIVE_INT, 1, (hsize_t *) &(*shape)[0]);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = sizeof(int) * (*shape)[0];
      }
    } else if (valtype == typeid(std::set<int>)) {
      shape = shapes[i];
      if (shape == NULL || (*shape)[0] < 1) {
        dbtypes[i] = VL_SET_INT;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_SET_INT) == 0) {
          vldts_[VL_SET_INT] = H5Tvlen_create(H5T_NATIVE_INT);
          opened_types_.insert(vldts_[VL_SET_INT]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = SET_INT;
        field_types[i] = H5Tarray_create2(H5T_NATIVE_INT, 1, (hsize_t *) &(*shape)[0]);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = sizeof(int) * (*shape)[0];
      }
    } else if (valtype == typeid(std::list<int>)) {
      shape = shapes[i];
      if (shape == NULL || (*shape)[0] < 1) {
        dbtypes[i] = VL_LIST_INT;
        field_types[i] = sha1_type_;
        if (vldts_.count(VL_LIST_INT) == 0) {
          vldts_[VL_LIST_INT] = H5Tvlen_create(H5T_NATIVE_INT);
          opened_types_.insert(vldts_[VL_LIST_INT]);
        }
        dst_sizes[i] = CYCLUS_SHA1_SIZE;
      } else {
        dbtypes[i] = LIST_INT;
        field_types[i] = H5Tarray_create2(H5T_NATIVE_INT, 1, (hsize_t *) &(*shape)[0]);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = sizeof(int) * (*shape)[0];
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
      if (shape == NULL || (*shape)[0] < 1) {
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
        field_types[i] = H5Tarray_create2(item_type, 1, (hsize_t *) &(*shape)[0]);
        opened_types_.insert(item_type);
        opened_types_.insert(field_types[i]);
        dst_sizes[i] = sizeof(int) * 2 * (*shape)[0];
      }
    } else {
      throw IOError("the type for column '" + std::string(field_names[i]) + \
                    "is not yet supported in HDF5.");
    } 
    dst_size += dst_sizes[i];
  }

  herr_t status;
  const char* title = d->title().c_str();
  int compress = 0;
  int chunk_size = 1000;
  void* fill_data = NULL;
  void* data = NULL;

  // Make the table
  status = H5TBmake_table(title, file_, title, nvals, 0, dst_size,
                          field_names, dst_offset, field_types, chunk_size, 
                          fill_data, compress, data);

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
  tbl_offset_[d->title()] = dst_offset;
  tbl_size_[d->title()] = dst_size;
  tbl_sizes_[d->title()] = dst_sizes;
  tbl_types_[d->title()] = dbtypes;
}

void Hdf5Back::WriteGroup(DatumList& group) {
  std::string title = group.front()->title();

  size_t* offsets = tbl_offset_[title];
  size_t* sizes = tbl_sizes_[title];
  size_t rowsize = tbl_size_[title];

  char* buf = new char[group.size() * rowsize];
  FillBuf(title, buf, group, sizes, rowsize);

  herr_t status = H5TBappend_records(file_, title.c_str(), group.size(), rowsize,
                              offsets, sizes, buf);
  if (status < 0) {
    throw IOError("Failed to write some data to the '" + title + "' table in "
                  "the hdf5 database.");
  }
  delete[] buf;
}

void Hdf5Back::FillBuf(std::string title, char* buf, DatumList& group, 
                       size_t* sizes, size_t rowsize) {
  using std::min;
  using std::vector;
  using std::set;
  using std::list;
  using std::pair;
  using std::map;
  Datum::Vals vals;
  Datum::Vals header = group.front()->vals();
  int ncols = header.size();
  //LoadTableTypes(title, ncols);
  DbTypes* dbtypes = tbl_types_[title];

  size_t offset = 0;
  const void* val;
  size_t fieldlen;
  size_t valuelen;
  DatumList::iterator it;
  for (it = group.begin(); it != group.end(); ++it) {
    for (int col = 0; col < ncols; ++col) {
      vals = (*it)->vals();
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
    throw IOError("could not select hyperslab of value array for reading.");
  hvl_t buf;
  status = H5Dread(dset, vldts_[U], mspace, dspace, H5P_DEFAULT, &buf);
  if (status < 0)
    throw IOError("failed to read in variable length data.");
  T val = VLBufToVal<T>(buf);
  status = H5Dvlen_reclaim(vldts_[U], mspace, H5P_DEFAULT, &buf);
  if (status < 0)
    throw IOError("failed to reclaim variable lenght data space.");
  H5Sclose(mspace);
  H5Sclose(dspace);
  return val;
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
    case VL_SET_INT: {
      name = "SetInt";
      break;
    }
    case VL_LIST_INT: {
      name = "ListInt";
      break;
    }
    case VL_MAP_INT_INT: {
      name = "MapIntInt";
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
        memcpy(buf + (n * CYCLUS_SHA1_SIZE), &d.val, CYCLUS_SHA1_SIZE);
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
    throw IOError("could not resize key array.");
  dspace = H5Dget_space(dset);
  status = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, offset, NULL, extent, NULL);
  if (status < 0)
    throw IOError("could not select hyperslab of key array.");
  status = H5Dwrite(dset, sha1_type_, mspace, dspace, H5P_DEFAULT, key.val);
  if (status < 0)
    throw IOError("could not write digest to key array.");
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
    throw IOError("could not select hyperslab of value array.");
  const char* buf[1] = {val.c_str()};
  status = H5Dwrite(dset, vldts_[dbtype], mspace, dspace, H5P_DEFAULT, buf);
  if (status < 0)
    throw IOError("could not write string to value array.");
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
    throw IOError("could not select hyperslab of value array.");
  status = H5Dwrite(dset, vldts_[dbtype], mspace, dspace, H5P_DEFAULT, &buf);
  if (status < 0)
    throw IOError("could not write variable length data to value array.");
  status = H5Dvlen_reclaim(vldts_[dbtype], mspace, H5P_DEFAULT, &buf);
  if (status < 0)
    throw IOError("could not free variable length buffer.");
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

} // namespace cyclus
