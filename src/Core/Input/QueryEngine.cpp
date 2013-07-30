// QueryEngine.cpp

#include "QueryEngine.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QueryEngine::QueryEngine() {
  spawned_children_ = set<QueryEngine*>();
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
QueryEngine* QueryEngine::queryElement(std::string query, 
                                       int index) {
  QueryEngine* qe_child = 
    getEngineFromQuery(query,index) ;
  spawned_children_.insert(qe_child);
  return qe_child;
}
} // namespace cyclus
