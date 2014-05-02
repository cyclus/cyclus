// hdf5_back.cc
#include "hdf5_back.h"

#include <cmath>
#include <string.h>

#include "blob.h"

#define STR_SIZE 16

namespace cyclus {

Hdf5Back::Hdf5Back(std::string path) : path_(path) {
  file_ = H5Fcreate(path_.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  string_type_ = H5Tcopy(H5T_C_S1);
  H5Tset_size(string_type_, STR_SIZE);
  H5Tset_strpad(string_type_, H5T_STR_NULLPAD);

  blob_type_ = H5Tcopy(H5T_C_S1);
  H5Tset_size(blob_type_, H5T_VARIABLE);
}

Hdf5Back::~Hdf5Back() {
  Flush();
  H5Fclose(file_);
  H5Tclose(string_type_);
  H5Tclose(blob_type_);

  std::map<std::string, size_t*>::iterator it;
  for (it = tbl_offset_.begin(); it != tbl_offset_.end(); ++it) {
    delete[](it->second);
  }
  for (it = tbl_sizes_.begin(); it != tbl_sizes_.end(); ++it) {
    delete[](it->second);
  }
};

void Hdf5Back::Notify(DatumList data) {
  std::map<std::string, DatumList> groups;
  for (DatumList::iterator it = data.begin(); it != data.end(); ++it) {
    std::string name = (*it)->title();
    if (tbl_size_.count(name) == 0) {
      Datum* d = *it;
      CreateTable(d);
    }
    groups[name].push_back(*it);
  }

  std::map<std::string, DatumList>::iterator it;
  for (it = groups.begin(); it != groups.end(); ++it) {
    WriteGroup(it->second);
  }
}

QueryResult Hdf5Back::Query(std::string table, std::vector<Cond>* conds) {
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

  std::cout << "tb_length " << tb_length << "\n";
  std::cout << "chunksize " << tb_chunksize << "\n";
  std::cout << "nchunks " << nchunks << "\n";

  QueryResult qr = GetTableInfo(tb_set, tb_type);
  for (n; n <= nchunks; n++) {
    hsize_t start = n * tb_chunksize;
    hsize_t count = (tb_length - start) < tb_chunksize ? tb_length - start : tb_chunksize;
    char* buf = new char [tb_typesize * count];
    hid_t memspace = H5Screate_simple(1, &count, NULL);
    status = H5Sselect_hyperslab(tb_space, H5S_SELECT_SET,  &start, NULL, &count, NULL);
    status = H5Dread(tb_set, tb_type, memspace, tb_space, H5P_DEFAULT, buf);
    int offset = 0;
    for (int i = 0; i < count; i++) {
      offset = i * tb_typesize;
      std::cout << i << "/" << count << "  ";
      for (int j = 0; j < qr.types.size(); j++) {
        switch (qr.types[j]) {
          case INT: {
            std::cout << *reinterpret_cast<int*>(buf + offset) << "  ";
            break;
          }
          case FLOAT: {
            std::cout << *reinterpret_cast<float*>(buf + offset) << "  ";
            break;
          }
          case DOUBLE: {
            std::cout << *reinterpret_cast<double*>(buf + offset) << "  ";
            break;
          }
          //case UUID: {
          //  std::cout << *reinterpret_cast<boost::uuids::uuid*>(buf + offset) << "  ";
          //  break;
          //}
          case UUID: {
            boost::uuids::uuid u;
            memcpy(buf + offset, &u, 16);
            std::cout << u << "  ";
            break;
          }
        }
        offset += tbl_sizes_[table][j];
      }
      std::cout << "\n";
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

QueryResult Hdf5Back::GetTableInfo(hid_t dset, hid_t dt) {
  int i;
  char * colname;
  hsize_t ncols = H5Tget_nmembers(dt);
  std::string fieldname;
  std::string fieldtype;

  // get types from db
  int dbtypes[ncols];
  hid_t dbtypes_attr = H5Aopen(dset, "cyclus_dbtypes", H5P_DEFAULT);
  hid_t dbtypes_type = H5Aget_type(dbtypes_attr);
  H5Aread(dbtypes_attr, dbtypes_type, dbtypes);
  H5Tclose(dbtypes_type);
  H5Aclose(dbtypes_attr);

  QueryResult qr;
  for (i = 0; i < ncols; i++) {
    colname = H5Tget_member_name(dt, i);
    fieldname = std::string(colname);
    free(colname);
    qr.fields.push_back(fieldname);
    qr.types.push_back(static_cast<DbTypes>(dbtypes[i]));
  }
  return qr;
}

std::string Hdf5Back::Name() {
  return path_;
}

void Hdf5Back::CreateTable(Datum* d) {
  Datum::Vals vals = d->vals();
  hsize_t nvals = vals.size();

  size_t dst_size = 0;
  size_t* dst_offset = new size_t[nvals];
  size_t* dst_sizes = new size_t[nvals];
  hid_t field_types[nvals];
  DbTypes dbtypes[nvals];
  const char* field_names[nvals];
  for (int i = 0; i < nvals; ++i) {
    dst_offset[i] = dst_size;
    field_names[i] = vals[i].first;
    const std::type_info& valtype = vals[i].second.type();
    if (valtype == typeid(int)) {
      dbtypes[i] = INT;
      field_types[i] = H5T_NATIVE_INT;
      dst_sizes[i] = sizeof(int);
      dst_size += sizeof(int);
    } else if (valtype == typeid(float)) {
      dbtypes[i] = FLOAT;
      field_types[i] = H5T_NATIVE_FLOAT;
      dst_sizes[i] = sizeof(float);
      dst_size += sizeof(float);
    } else if (valtype == typeid(double)) {
      dbtypes[i] = DOUBLE;
      field_types[i] = H5T_NATIVE_DOUBLE;
      dst_sizes[i] = sizeof(double);
      dst_size += sizeof(double);
    } else if (valtype == typeid(std::string)) {
      dbtypes[i] = STRING;
      field_types[i] = string_type_;
      dst_sizes[i] = STR_SIZE;
      dst_size += STR_SIZE;
    } else if (valtype == typeid(Blob)) {
      dbtypes[i] = BLOB;
      field_types[i] = blob_type_;
      dst_sizes[i] = sizeof(char*);
      dst_size += sizeof(char*);
    } else if (valtype == typeid(boost::uuids::uuid)) {
      dbtypes[i] = UUID;
      field_types[i] = string_type_;
      dst_sizes[i] = STR_SIZE;
      dst_size += STR_SIZE;
    } 
  }

  herr_t status;
  const char* title = d->title().c_str();
  int compress = 1;
  int chunk_size = 1000;
  void* fill_data = NULL;
  void* data = NULL;

  // Make the table
  status = H5TBmake_table(title, file_, title, nvals, 0, dst_size,
                          field_names, dst_offset, field_types, chunk_size, 
                          fill_data, compress, data);

  // add dbtypes attribute
  hid_t tb_set = H5Dopen2(file_, title, H5P_DEFAULT);
  hid_t attr_space = H5Screate(H5S_SCALAR);
  hid_t dbtypes_type = H5Tarray_create2(H5T_NATIVE_INT, 1, &nvals);
  hid_t dbtypes_attr = H5Acreate2(tb_set, "cyclus_dbtypes", dbtypes_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
  H5Awrite(dbtypes_attr, dbtypes_type, dbtypes);
  H5Aclose(dbtypes_attr);
  H5Tclose(dbtypes_type);
  H5Sclose(attr_space);
  H5Dclose(tb_set);

  // record everything for later
  tbl_offset_[d->title()] = dst_offset;
  tbl_size_[d->title()] = dst_size;
  tbl_sizes_[d->title()] = dst_sizes;
}

void Hdf5Back::WriteGroup(DatumList& group) {
  std::string title = group.front()->title();

  size_t* offsets = tbl_offset_[title];
  size_t* sizes = tbl_sizes_[title];
  size_t rowsize = tbl_size_[title];

  char* buf = new char[group.size() * rowsize];
  FillBuf(buf, group, sizes, rowsize);

  herr_t status = H5TBappend_records(file_, title.c_str(), group.size(), rowsize,
                              offsets, sizes, buf);
  if (status < 0) {
    throw IOError("Failed to write some data to hdf5 output db");
  }
  delete[] buf;
}

void Hdf5Back::FillBuf(char* buf, DatumList& group, size_t* sizes,
                       size_t rowsize) {
  Datum::Vals header = group.front()->vals();
  int valtype[header.size()];
  enum Type {STR, NUM, UUID, BLOB};
  for (int col = 0; col < header.size(); ++col) {
    if (header[col].second.type() == typeid(std::string)) {
      valtype[col] = STR;
    } else if (header[col].second.type() == typeid(boost::uuids::uuid)) {
      valtype[col] = UUID;
    } else if (header[col].second.type() == typeid(Blob)) {
      valtype[col] = BLOB;
    } else {
      valtype[col] = NUM;
    }
  }

  size_t offset = 0;
  const void* val;
  DatumList::iterator it;
  for (it = group.begin(); it != group.end(); ++it) {
    for (int col = 0; col < header.size(); ++col) {
      const boost::spirit::hold_any* a = &((*it)->vals()[col].second);
      switch (valtype[col]) {
        case NUM: {
          val = a->castsmallvoid();
          memcpy(buf + offset, val, sizes[col]);
          break;
        }
        case STR: {
          const std::string s = a->cast<std::string>();
          size_t slen = std::min(s.size(), static_cast<size_t>(STR_SIZE));
          memcpy(buf + offset, s.c_str(), slen);
          memset(buf + offset + slen, 0, STR_SIZE - slen);
          break;
        }
        case BLOB: {
          // TODO: fix this memory leak, but the copied bytes must remain
          // valid until the hdf5 file is flushed or closed.
          std::string s = a->cast<Blob>().str();
          char* v = new char[strlen(s.c_str())];
          strcpy(v, s.c_str());
          memcpy(buf + offset, &v, sizes[col]);
          break;
        }
        case UUID: {
          boost::uuids::uuid uuid = a->cast<boost::uuids::uuid>();
          memcpy(buf + offset, &uuid, STR_SIZE);
          break;
        }
      }
      offset += sizes[col];
    }
  }
}

} // namespace cyclus
