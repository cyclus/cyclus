#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <sstream>
#include <vector>
#include <boost/intrusive_ptr.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>

#include "IntrusiveBase.h"

// Useful Typedefs
//   Pointers
class Table;
typedef boost::intrusive_ptr<Table> table_ptr;
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


/*!
   @brief
   The Table class is designed as part of the Cyclus Database
   API. Tables are responsible for holding information about
   themselves and keep a queue of command instructions for
   writing or updating rows as needed. Any Module that wishes
   to maintain specific data not already in the Core Cyclus
   Database Tables must define their own Table.
   
   @section introduction Introduction
   This class models a relational database table. It specifically
   provides support for SQL-style table creation, row insertion,
   and row updating commands; however, it can hold any command 
   that is in the form of a stringstream.
   
   @section definition Defining a Table
   A Table must be explictly defined. That is, any class using a 
   table must EXPLICITLY call the Table's tableDefined() 
   function. It is at this point that the Book Keeper is made 
   aware of the Table's existence.

   A Table is considered defined if it has at least one 
   column and a primary key.

   @section data Generic Data Management
   The Table class utilizes the hold_any class defined in 
   Boost::Spirit at
   boost/spirit/home/support/detail/hold_any.hpp. Specifically,
   the hold_any class uses small optimization and supports the
   streaming operators.

   Currently, the only check that must be made on a datum's type
   is whether or not is actually a string. In this case, the
   Table class applies single quotation marks before and after
   the datum, as required by SQL languages.
*/

class Table : IntrusiveBase<Table> {
 private:
  /**
   * Stores the table's name, declared during construction.
   */
  table_name name_;

  /**
   * A storage container for each column's title
   */
  std::vector<col_name> col_names_;

  /**
   * A storage container for each column's title and data type
   * @gidden can we combine these two?
   */
  std::vector<column> columns_;

  /**
   * Every table must have a defined primary key
   */
  primary_key primary_key_;

  /**
   * A table can have more than one foreign key
   */
  std::vector<foreign_key> foreign_keys_;

  /**
   * A storage container for the table's indicies
   * @gidden this needs to be supported
   */
  //std::vector<index> indicies_;

  /**
   * A storage container for each of the commands a table 
   * amasses during a simulation. The Book Keeper has the final say
   * on when these commands are actually executed. The Table alerts
   * the Book Keeper if the number of commands exceeds the 
   * threshold defined in Table.cpp.
   */
  std::vector<command*> row_commands_;

  /**
   * A boolean which keeps track of when a Table is defined.
   * A Table can only be defined once, and it's parameters
   * can not be changed after.
   */
  bool defined_;

 public:
  // constructors and destructors
  Table(table_name name);
  ~Table(){};

  // getter functions
  table_name name(){return name_;}
  table_ptr me();
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

  // cleaning up
  void flush(){this->flushRows();}
  
 private:
  // some inner working functions
  std::string updateRowPK(primary_key_ref const pkref);
  std::string stringifyData(data const d);
  void flushRows(){row_commands_.clear();}

};

#endif // END TABLE_H
