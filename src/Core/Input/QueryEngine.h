//QueryEngine.h
#ifndef QUERYENGINE_H
#define QUERYENGINE_H

#include <string>
#include <set>

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
     investigates the current status and returns the number of elements
     matching a query
     @param query the query
     @return the number of elements matching the query
   */
  virtual int numElementsMatchingQuery(std::string query) = 0;


  /**
     populates a child query engine based on a query and index
     @param query the query
     @param index the index of the queried element
     @return a initialized query engine based on the query and index
   */
  QueryEngine* queryElement(std::string query, int index = 0);
    
  /**
     investigates the current status and returns a string representing
     the content of a query at a given index
     @param query the query
     @param index the index of the queried element
   */
  virtual std::string getElementContent(std::string query, 
                                        int index = 0) = 0;
  
 protected:
  /**
     every derived query engine must return a new instance initialized
     with a snippet.
     @param snippet the snippet to use for initialization
     @return a query engine initialized via the snippet
   */
  virtual QueryEngine* getEngineFromSnippet(std::string snippet) = 0;

 private:
  std::set<QueryEngine*> spawned_children_;
};

#endif
