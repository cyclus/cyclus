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
  QueryResult q = GetTableInfo(table);
  return q;
}

std::string Hdf5Back::Name() {
  return path_;
}

void Hdf5Back::CreateTable(Datum* d) {
  Datum::Vals vals = d->vals();

  size_t dst_size = 0;
  size_t* dst_offset = new size_t[vals.size()];
  size_t* dst_sizes = new size_t[vals.size()];
  hid_t field_types[vals.size()];
  const char* field_names[vals.size()];
  for (int i = 0; i < vals.size(); ++i) {
    dst_offset[i] = dst_size;
    field_names[i] = vals[i].first;
    if (vals[i].second.type() == typeid(int)) {
      field_types[i] = H5T_NATIVE_INT;
      dst_sizes[i] = sizeof(int);
      dst_size += sizeof(int);
    } else if (vals[i].second.type() == typeid(double)) {
      field_types[i] = H5T_NATIVE_DOUBLE;
      dst_sizes[i] = sizeof(double);
      dst_size += sizeof(double);
    } else if (vals[i].second.type() == typeid(std::string)) {
      field_types[i] = string_type_;
      dst_sizes[i] = STR_SIZE;
      dst_size += STR_SIZE;
    } else if (vals[i].second.type() == typeid(Blob)) {
      field_types[i] = blob_type_;
      dst_sizes[i] = sizeof(char*);
      dst_size += sizeof(char*);
    } else if (vals[i].second.type() == typeid(boost::uuids::uuid)) {
      field_types[i] = string_type_;
      dst_sizes[i] = STR_SIZE;
      dst_size += STR_SIZE;
    } else if (vals[i].second.type() == typeid(float)) {
      field_types[i] = H5T_NATIVE_FLOAT;
      dst_sizes[i] = sizeof(float);
      dst_size += sizeof(float);
    }
  }

  herr_t status;
  const char* title = d->title().c_str();
  int compress = 1;
  int chunk_size = 1000;
  void* fill_data = NULL;
  void* data = NULL;

  status = H5TBmake_table(title, file_, title, vals.size(), 0, dst_size,
                          field_names, dst_offset, field_types, chunk_size, fill_data, compress,
                          data);

  // record everything for later
  tbl_offset_[d->title()] = dst_offset;
  tbl_size_[d->title()] = dst_size;
  tbl_sizes_[d->title()] = dst_sizes;
}

void Hdf5Back::WriteGroup(DatumList& group) {
  std::string title = group.front()->title();
  herr_t status;

  size_t* offsets = tbl_offset_[title];
  size_t* sizes = tbl_sizes_[title];
  size_t rowsize = tbl_size_[title];

  char* buf = new char[group.size() * rowsize];
  FillBuf(buf, group, sizes, rowsize);

  status = H5TBappend_records(file_, title.c_str(), group.size(), rowsize,
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
          const char* data = a->cast<Blob>().str().c_str();
          memcpy(buf + offset, &data, sizes[col]);
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
