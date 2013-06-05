
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
    if (! tableExists(name)) {
      createMemType(*it);
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
  for (it = types_.begin(); it != types_.end(); ++it) {
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
void Hdf5Back::createMemType(event_ptr ev) {
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
    } else if (vals[i].second.type() == typeid(double)) {
      mtype->insertMember(field, offsets[i], PredType::NATIVE_DOUBLE);
    } else if (vals[i].second.type() == typeid(std::string)) {
      mtype->insertMember(field, offsets[i], StrType(0, strSize));
    } else if (vals[i].second.type() == typeid(float)) {
      mtype->insertMember(field, offsets[i], PredType::NATIVE_FLOAT);
    }
  }

  int length = 0;
  int rank = 1;
  hsize_t dim[] = {length};
  hsize_t maxdims[] = {H5S_UNLIMITED};
  DataSpace space(rank, dim, maxdims);

  DSetCreatPropList cparms;
  hsize_t chunk_dims[] = {50000};
  cparms.setChunk(rank, chunk_dims);

  DataSet* dataset = new DataSet(file_->createDataSet(ev->title(), *mtype, space, cparms));

  datasets_[ev->title()] = dataset;
  types_[ev->title()] = mtype;
  table_sizes_[ev->title()] = size;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Hdf5Back::tableExists(std::string name) {
  return types_.count(name) == 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Hdf5Back::writeSet(EventList set) {
  // pack events into a contiguous memory buffer
  std::string name = set[0]->title();
  CompType* mtype = types_[name];
  size_t rowsize = table_sizes_[name];
  char* buf = new char[set.size() * rowsize];
  size_t offset = 0;
  for (int i = 0; i < set[0]->vals().size(); ++i) {
    Event::Vals vals = set[0]->vals();
    H5std_string field(vals[i].first);
    int num = mtype->getMemberIndex(field);
    size_t size = mtype->getMemberDataType(num).getSize();
    const std::type_info& ti = vals[i].second.type();
    for (int j = 0; j < set.size(); ++j) {
      const void* val;
      const boost::any* a = &set[j]->vals()[i].second;
      if (ti == typeid(int)) {
        val = boost::any_cast<int>(a);
      } else if (ti == typeid(double)) {
        val = boost::any_cast<double>(a);
      } else if (ti == typeid(std::string)) {
        val = boost::any_cast<std::string>(a)->c_str();
      } else if (ti == typeid(float)) {
        val = boost::any_cast<float>(a);
      }
      memcpy(buf + rowsize * j + offset, val, size);
    }
    offset += size;
  }

  DataSet* dataset = datasets_[name];
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

