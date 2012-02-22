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
  Database(char* filename);
  ~Database();
  
  bool open(char* filename);
  query_result query(char* query);
  void close();
  
 private:
  sqlite3 *database;
};

#endif
