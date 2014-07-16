#include "recorder.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "datum.h"
#include "logger.h"
#include "rec_backend.h"

namespace cyclus {

Recorder::Recorder() : index_(0) {
  uuid_ = boost::uuids::random_generator()();
  set_dump_count(kDefaultDumpCount);
}

Recorder::Recorder(unsigned int dump_count) : index_(0) {
  uuid_ = boost::uuids::random_generator()();
  set_dump_count(dump_count);
}

Recorder::Recorder(boost::uuids::uuid simid) : index_(0), uuid_(simid) {
  set_dump_count(kDefaultDumpCount);
}

Recorder::~Recorder() {
  try {
    Flush();
  } catch (Error err) {
    CLOG(LEV_ERROR) << "Error in Recorder destructor: " << err.what();
  }

  for (int i = 0; i < data_.size(); ++i) {
    delete data_[i];
  }
}

unsigned int Recorder::dump_count() {
  return dump_count_;
}

boost::uuids::uuid Recorder::sim_id() {
  return uuid_;
}

void Recorder::set_dump_count(unsigned int count) {
  for (int i = 0; i < data_.size(); ++i) {
    delete data_[i];
  }
  data_.clear();
  data_.reserve(count);
  for (int i = 0; i < count; ++i) {
    Datum* d = new Datum(this, "");
    d->AddVal("SimId", uuid_);
    data_.push_back(d);
  }
  dump_count_ = count;
}

Datum* Recorder::NewDatum(std::string title) {
  Datum* d = data_[index_];
  d->title_ = title;
  d->vals_.resize(1);
  d->shapes_.resize(1);

  index_++;
  return d;
}

void Recorder::AddDatum(Datum* d) {
  if (index_ >= data_.size()) {
    NotifyBackends();
  }
}

void Recorder::Flush() {
  DatumList tmp = data_;
  tmp.resize(index_);
  index_ = 0;
  std::list<RecBackend*>::iterator it;
  for (it = backs_.begin(); it != backs_.end(); it++) {
    (*it)->Notify(tmp);
    (*it)->Flush();
  }
}

void Recorder::NotifyBackends() {
  index_ = 0;
  std::list<RecBackend*>::iterator it;
  for (it = backs_.begin(); it != backs_.end(); it++) {
    (*it)->Notify(data_);
  }
}

void Recorder::RegisterBackend(RecBackend* b) {
  backs_.push_back(b);
}

void Recorder::Close() {
  Flush();
  backs_.clear();
}

}  // namespace cyclus
