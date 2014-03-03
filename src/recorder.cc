// recorder.cc

#include "recorder.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "rec_backend.h"
#include "datum.h"
#include "logger.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Recorder::Recorder() : index_(0) {
  uuid_ = boost::uuids::random_generator()();
  set_dump_count(kDefaultDumpCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Recorder::Recorder(boost::uuids::uuid simid)
    : index_(0), uuid_(simid) {
  set_dump_count(kDefaultDumpCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Recorder::~Recorder() {
  for (int i = 0; i < data_.size(); ++i) {
    delete data_[i];
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unsigned int Recorder::dump_count() {
  return dump_count_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
boost::uuids::uuid Recorder::sim_id() {
  return uuid_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Datum* Recorder::NewDatum(std::string title) {
  Datum* d = data_[index_];
  d->title_ = title;
  d->vals_.resize(1);

  index_++;
  if (index_ >= data_.size()) {
    NotifyBackends();
  }
  return d;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Recorder::AddDatum(Datum* d) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Recorder::Flush() {
  DatumList tmp = data_;
  tmp.resize(index_);
  index_ = 0;
  std::list<RecBackend*>::iterator it;
  for (it = backs_.begin(); it != backs_.end(); it++) {
    (*it)->Notify(tmp);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Recorder::NotifyBackends() {
  index_ = 0;
  std::list<RecBackend*>::iterator it;
  for (it = backs_.begin(); it != backs_.end(); it++) {
    (*it)->Notify(data_);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Recorder::RegisterBackend(RecBackend* b) {
  backs_.push_back(b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Recorder::Close() {
  for (int i = index_; i < data_.size(); ++i) {
    delete data_[i];
  }
  data_.resize(index_);
  NotifyBackends();
  std::list<RecBackend*>::iterator it;
  for (it = backs_.begin(); it != backs_.end(); it++) {
    (*it)->Flush();
  }
  backs_.clear();
  set_dump_count(kDefaultDumpCount);
}
} // namespace cyclus

