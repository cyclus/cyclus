// QueryEngine.cpp

#include "QueryEngine.h"

using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QueryEngine::QueryEngine() {
  spawned_children_ = set<QueryEngine*>();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QueryEngine::~QueryEngine() {
  while (!spawned_children_.empty()) {
    QueryEngine* qe = *spawned_children_.begin();
    spawned_children_.erase(spawned_children_.begin());
    if (qe) {
      delete qe;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QueryEngine* QueryEngine::queryElement(std::string query, 
                                       int index) {
  QueryEngine* qe = 
    getEngineFromSnippet(getElementContent(query,index));
  spawned_children_.insert(qe);
  return qe;
}
