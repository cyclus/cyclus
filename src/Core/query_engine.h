//query_engine.h
#ifndef QUERYENGINE_H
#define QUERYENGINE_H

#include <string>
#include <set>

namespace cyclus {

/**
   This is a base class that defines the API used by any engine
   representing an input format.
 */
class QueryEngine {
 public:
  /// constructor
  QueryEngine();

  /// virtual destructor
  virtual ~QueryEngine();

  /**
     @return the number of elements in the current query state
   */
  virtual int NElements() = 0;

  /**
     investigates the current status and returns a string representing
     the name of a given index
     @param index the index of the queried element
   */
  virtual std::string GetElementName(int index = 0) = 0;

  /**
     investigates the current status and returns the number of elements
     matching a query
     @param query the query
     @return the number of elements matching the query
   */
  virtual int NElementsMatchingQuery(std::string query) = 0;

  /**
     investigates the current status and returns a string representing
     the content of a query at a given index
     @param query the query
     @param index the index of the queried element
   */
  virtual std::string GetElementContent(std::string query,
                                        int index = 0) = 0;

  /**
     populates a child query engine based on a query and index
     @param query the query
     @param index the index of the queried element
     @return a initialized query engine based on the query and index
   */
  QueryEngine* QueryElement(std::string query, int index = 0);

 protected:
  /**
     every derived query engine must return a new instance initialized
     by a query.
     @param query the query
     @param index the index of the queried element
     @return a query engine initialized via the snippet
   */
  virtual QueryEngine* GetEngineFromQuery(std::string query,
                                          int index) = 0;

 private:
  std::set<QueryEngine*> spawned_children_;
};

} // namespace cyclus

#endif
