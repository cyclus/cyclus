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
  
  std::string name(){return name_;}

  bool open(std::string filename);
  query_result query(std::string a_query);
  void close();
  
  void registerTable(table_ptr t) {addTable(t);}
  void createTable(table_ptr t);
  void writeRows(table_ptr t);
  
  int nTables() {return tables_.size();}
  table_ptr tablePtr(int i) {return tables_.at(i);}

 private:
  sqlite3 *database_;
  std::string name_;
  std::vector<table_ptr> tables_;

  void addTable(table_ptr t){tables_.push_back(t);}
  bool tableExists(table_ptr t);
  void issueCommand(std::string cmd);
};

#endif
