#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <string>
#include <vector>
#include <sqlite3.h>

#include "Table.h"

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
  
  void registerTable(Table* t) {addTable(t);}
  void createTable(Table* t);
  void writeRows(Table* t);
  
  int nTables() {return tables_.size();}
  Table* tablePtr(int i) {return tables_.at(i);}

 private:
  sqlite3 *database_;
  std::vector<Table*> tables_;

  void addTable(Table *t){tables_.push_back(t);}
  bool tableExists(Table *t);
  void issueCommand(std::string cmd);
};

#endif
