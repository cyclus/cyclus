
#include "Hdf5Back.h"
#include <cmath>

#define STR_SIZE 16

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Hdf5Back::Hdf5Back(std::string path)
    : path_(path) {
  file_ = H5Fcreate(path_.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  string_type_ = H5Tcopy(H5T_C_S1);
  H5Tset_size(string_type_, STR_SIZE);
  H5Tset_strpad(string_type_, H5T_STR_NULLPAD);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Hdf5Back::notify(EventList evts) {
  std::map<std::string, EventList> groups;
  for (EventList::iterator it = evts.begin(); it != evts.end(); ++it) {
    std::string name = (*it)->title();
    if (tbl_size_.count(name) == 0) {
      event_ptr ev = *it;
      createTable(ev);
    }
    groups[name].push_back(*it);
  }

  std::map<std::string, EventList>::iterator it;
  for (it = groups.begin(); it != groups.end(); ++it) {
    writeGroup(it->second);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Hdf5Back::close() {
  H5Fclose(file_);
  H5Tclose(string_type_);

  std::map<std::string, size_t*>::iterator it;
  for (it = tbl_offset_.begin(); it != tbl_offset_.end(); ++it) {
    delete[] (it->second);
  }
  for (it = tbl_sizes_.begin(); it != tbl_sizes_.end(); ++it) {
    delete[] (it->second);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Hdf5Back::name() {
  return path_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Hdf5Back::createTable(event_ptr ev) {
  Event::Vals vals = ev->vals();

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
    } else if (vals[i].second.type() == typeid(float)) {
      field_types[i] = H5T_NATIVE_FLOAT;
      dst_sizes[i] = sizeof(float);
      dst_size += sizeof(float);
    }
  }

  herr_t status;
  const char* title = ev->title().c_str();
  int compress = 0;
  int chunk_size = 50000;
  void* fill_data = NULL;
  void* data = NULL;

  status = H5TBmake_table(title, file_, title, vals.size(), 0, dst_size,
      field_names, dst_offset, field_types, chunk_size, fill_data, compress,
      data);

  // record everything for later
  tbl_offset_[ev->title()] = dst_offset;
  tbl_size_[ev->title()] = dst_size;
  tbl_sizes_[ev->title()] = dst_sizes;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Hdf5Back::writeGroup(EventList& group) {
  std::string title = group[0]->title();
  herr_t status;

  size_t* offsets = tbl_offset_[title];
  size_t* sizes = tbl_sizes_[title];
  size_t rowsize = tbl_size_[title];

  char* buf = new char[group.size() * rowsize];
  fillBuf(buf, group, sizes, rowsize);

  status = H5TBappend_records(file_, title.c_str(), group.size(), rowsize, offsets, sizes, buf);
  delete[] buf;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Hdf5Back::fillBuf(char* buf, EventList& group, size_t* sizes, size_t rowsize) {
  Event::Vals header = group[0]->vals();
  size_t offset = 0;
  for (int col = 0; col < header.size(); ++col) {
    size_t field_size = sizes[col];
    const std::type_info& ti = header[col].second.type();
    const void* val;
    if (ti == typeid(int)) {
      for (int row = 0; row < group.size(); ++row) {
        const boost::spirit::hold_any* a = &group[row]->vals()[col].second;
        val = boost::spirit::any_cast<int>(a);
        memcpy(buf + rowsize * row + offset, val, field_size);
      }
    } else if (ti == typeid(double)) {
      for (int row = 0; row < group.size(); ++row) {
        const boost::spirit::hold_any* a = &group[row]->vals()[col].second;
        val = boost::spirit::any_cast<double>(a);
        memcpy(buf + rowsize * row + offset, val, field_size);
      }
    } else if (ti == typeid(std::string)) {
      for (int row = 0; row < group.size(); ++row) {
        const boost::spirit::hold_any* a = &group[row]->vals()[col].second;
        const std::string* s = boost::spirit::any_cast<std::string>(a);
        val = s->c_str();
        size_t slen = std::min(s->size(), (unsigned long)STR_SIZE);
        memcpy(buf + rowsize * row + offset, val, slen);
        memset(buf + rowsize * row + offset + s->size(), 0, STR_SIZE - slen);
      }
    } else if (ti == typeid(float)) {
      for (int row = 0; row < group.size(); ++row) {
        const boost::spirit::hold_any* a = &group[row]->vals()[col].second;
        val = boost::spirit::any_cast<float>(a);
        memcpy(buf + rowsize * row + offset, val, field_size);
      }
    }
    offset += field_size;
  }
}

