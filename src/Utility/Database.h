#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <string>
#include <vector>
#include <sqlite3.h>


// Useful Typedefs
//   query results
typedef std::vector<std::string> query_row;
typedef std::vector<query_row> query_result;


class Database
{
 public:
  Database(std::string filename);
  ~Database();
  
  bool open(std::string filename);
  query_result query(std::string a_query);
  void close();
  
 private:
  sqlite3 *database;
};

#endif
