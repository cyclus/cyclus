
#include "Hdf5Back.h"

using namespace H5;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Hdf5Back::Hdf5Back(std::string path)
    : path_(path) {
  file_ = new H5File(H5std_string(path_), H5F_ACC_TRUNC );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Hdf5Back::notify(EventList evts) {
  std::map<std::string, EventList> sets;
  for (EventList::iterator it = evts.begin(); it != evts.end(); ++it) {
    std::string name = (*it)->title();
    if (mtypes_.count(name) == 0) {
      event_ptr ev = *it;
      CompType* mtype = createMemType(ev);
      mtypes_[ev->title()] = mtype;
      datasets_[ev->title()] = createDataSet(ev->title(), mtype);
    }
    sets[name].push_back(*it);
  }

  std::map<std::string, EventList>::iterator it;
  for (it = sets.begin(); it != sets.end(); ++it) {
    writeSet(it->second);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Hdf5Back::close() {
  delete file_;
  std::map<std::string, CompType*>::iterator it;
  for (it = mtypes_.begin(); it != mtypes_.end(); ++it) {
    delete (it->second);
  }

  std::map<std::string, DataSet*>::iterator it2;
  for (it2 = datasets_.begin(); it2 != datasets_.end(); ++it2) {
    delete (it2->second);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Hdf5Back::name() {
  return path_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CompType* Hdf5Back::createMemType(event_ptr ev) {
  Event::Vals vals = ev->vals();
  std::vector<size_t> offsets;
  offsets.resize(vals.size());
  size_t size = 0;
  size_t strSize = 16;
  for (int i = 0; i < vals.size(); ++i) {
    if (vals[i].second.type() == typeid(int)) {
      offsets[i] = size;
      size += sizeof(int);
    } else if (vals[i].second.type() == typeid(double)) {
      offsets[i] = size;
      size += sizeof(double);
    } else if (vals[i].second.type() == typeid(std::string)) {
      offsets[i] = size;
      size += strSize;
    } else if (vals[i].second.type() == typeid(float)) {
      offsets[i] = size;
      size += sizeof(float);
    }
  }

  CompType* mtype = new CompType(size);
  for (int i = 0; i < vals.size(); ++i) {
    H5std_string field(vals[i].first);
    if (vals[i].second.type() == typeid(int)) {
      mtype->insertMember(field, offsets[i], PredType::NATIVE_INT);
    } else if (vals[i].second.type() == typeid(double)) { mtype->insertMember(field, offsets[i], PredType::NATIVE_DOUBLE);
    } else if (vals[i].second.type() == typeid(std::string)) {
      mtype->insertMember(field, offsets[i], StrType(0, strSize));
    } else if (vals[i].second.type() == typeid(float)) {
      mtype->insertMember(field, offsets[i], PredType::NATIVE_FLOAT);
    }
  }

  dataset_sizes_[ev->title()] = size;
  return mtype;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DataSet* Hdf5Back::createDataSet(std::string title, CompType* mtype) {
  int length = 0;
  int rank = 1;
  hsize_t dim[] = {length};
  hsize_t maxdims[] = {H5S_UNLIMITED};

  DataSpace space(rank, dim, maxdims);

  int chunk_size = 50000;
  DSetCreatPropList cparms;
  hsize_t chunk_dims[] = {chunk_size};
  cparms.setChunk(rank, chunk_dims);

  return new DataSet(file_->createDataSet(title, *mtype, space, cparms));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Hdf5Back::writeSet(EventList& set) {
  std::string title = set[0]->title();
  CompType* mtype = mtypes_[title];
  size_t rowsize = dataset_sizes_[title];

  char* buf = new char[set.size() * rowsize];
  fillBuf(buf, set, mtype, rowsize);

  DataSet* dataset = datasets_[title];
  hsize_t hoffset[] = {dataset->getSpace().getSimpleExtentNpoints()};
  hsize_t dim[] = {set.size() + hoffset[0]};
  hsize_t dim2[] = {set.size()};
  dataset->extend(dim);

  int rank = 1;
  DataSpace fspace = dataset->getSpace();
  DataSpace mspace(rank, dim2);
  fspace.selectHyperslab(H5S_SELECT_SET, dim2, hoffset);
  dataset->write(buf, *mtype, mspace, fspace);

  delete[] buf;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Hdf5Back::fillBuf(char* buf, EventList& set, CompType* mtype, size_t rowsize) {
  Event::Vals header = set[0]->vals();
  size_t offset = 0;
  for (int col = 0; col < header.size(); ++col) {
    H5std_string field(header[col].first);
    int num = mtype->getMemberIndex(field);
    size_t field_size = mtype->getMemberDataType(num).getSize();
    const std::type_info& ti = header[col].second.type();
    const void* val;
    if (ti == typeid(int)) {
      for (int row = 0; row < set.size(); ++row) {
        const boost::any* a = &set[row]->vals()[col].second;
        val = boost::any_cast<int>(a);
        memcpy(buf + rowsize * row + offset, val, field_size);
      }
    } else if (ti == typeid(double)) {
      for (int row = 0; row < set.size(); ++row) {
        const boost::any* a = &set[row]->vals()[col].second;
        val = boost::any_cast<double>(a);
        memcpy(buf + rowsize * row + offset, val, field_size);
      }
    } else if (ti == typeid(std::string)) {
      for (int row = 0; row < set.size(); ++row) {
        const boost::any* a = &set[row]->vals()[col].second;
        val = boost::any_cast<std::string>(a)->c_str();
        memcpy(buf + rowsize * row + offset, val, field_size);
      }
    } else if (ti == typeid(float)) {
      for (int row = 0; row < set.size(); ++row) {
        const boost::any* a = &set[row]->vals()[col].second;
        val = boost::any_cast<float>(a);
        memcpy(buf + rowsize * row + offset, val, field_size);
      }
    }
    offset += field_size;
  }
}

