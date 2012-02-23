#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <sstream>
#include <vector>
//#include <boost/any.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>

// Useful Typedefs
//   Commands
typedef std::stringstream command;
//   Tables
typedef std::string table_name;
//   Columns
typedef std::string col_name;
typedef std::string data_type;
typedef std::pair<col_name, data_type> column;
//   Rows
typedef boost::spirit::hold_any data;
typedef std::pair<col_name,data> entry;
typedef std::vector<entry> row;
//   Keys
typedef std::vector<col_name> key;
typedef key primary_key;
typedef std::vector<entry> primary_key_ref;
typedef std::pair<table_name, key> foreign_key_ref;
typedef std::pair<key, foreign_key_ref> foreign_key;
//   Indicies
//typedef std::vector<col_name> index;

class Table {
 private:
  // private members
  table_name name_;
  primary_key primary_key_;
  std::vector<foreign_key> foreign_keys_;
  std::vector<col_name> col_names_;
  std::vector<column> columns_;
  std::vector<command*> row_commands_;
  //std::vector<index> indicies_;
  bool defined_;

 public:
  // constructors and destructors
  Table(table_name name);
  ~Table(){};

  // getter functions
  table_name name(){return name_;}
  bool defined(){return defined_;}
  int nRows(){return row_commands_.size();}
  command* row_command(int i){return row_commands_.at(i);}

  // setter functions
  void tableDefined();

  // table construction functions
  inline void addColumn(column col)
  {
    col_names_.push_back(col.first);
    columns_.push_back(col);
  }
  void setPrimaryKey(std::string const pk_string);
  void setPrimaryKey(column const col);
  void setPrimaryKey(primary_key const pk){primary_key_ = pk;}
  void addForeignKey(foreign_key const fk);
  //void addIndex(index i){indicies_.push_back(i);}
  void addRow(row const r);
  void updateRow(primary_key_ref const pkref, entry const e);
  void updateRow(primary_key_ref const pkref, row const r);

  // table query strings
  std::string f_keys();
  std::string p_key();
  std::string create();
  std::string writeRows();

  // cleaning up
  void flush(){this->flushRows();}
  
 private:
  // some inner working functions
  std::string updateRowPK(primary_key_ref const pkref);
  std::string stringifyData(data const d);
  void flushRows(){row_commands_.clear();}

};

#endif // END TABLE_H
