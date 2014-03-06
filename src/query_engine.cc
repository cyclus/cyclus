// query_engine.cc

#include "query_engine.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QueryEngine::QueryEngine() {
  spawned_children_ = std::set<QueryEngine*>();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QueryEngine::~QueryEngine() {
  while (!spawned_children_.empty()) {
    QueryEngine* qe_child = *spawned_children_.begin();
    spawned_children_.erase(spawned_children_.begin());
    if (qe_child) {
      delete qe_child;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QueryEngine* QueryEngine::QueryElement(std::string query,
                                       int index) {
  QueryEngine* qe_child =
    GetEngineFromQuery(query, index);
  spawned_children_.insert(qe_child);
  return qe_child;
}
} // namespace cyclus

