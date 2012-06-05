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

/**
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
     Stores the table's name, declared during construction. 
   */
  table_name name_;
  
  /**
     A storage container for each column's title 
   */
  std::vector<std::string> col_names_;
  
  /**
     A storage container for each column's title and data type 
     (\@gidden can we combine these two? )
   */
  std::vector<std::string> col_types_;

  /**
     Every table must have a defined primary key 
   */
  primary_key primary_key_;

  /**
     A table can have more than one foreign key 
   */
  std::vector<foreign_key> foreign_keys_;

  /* /\** */
  /*  *  A storage container for the table's indicies */
  /*  * (\@gidden this needs to be supported )*/
  /*  *\/ */
  /* std::vector<index> indicies_; */

  /**
     A storage container for each of the commands a table 
     amasses during a simulation. The Book Keeper has the final say 
     on when these commands are actually executed. The Table alerts 
     the Book Keeper if the number of commands exceeds the 
     threshold defined in Table.cpp. 
   */
  std::vector<command*> row_commands_;

  /**
     A boolean which keeps track of when a Table is defined. 
     A Table can only be defined once, and it's parameters 
     can not be changed after. 
   */
  bool defined_;

  /**
     Update a row's primary key, given a primary key reference 
     @param pkref the reference to the primary key to be added 
   */  
  std::string updateRowPK(primary_key_ref const pkref);

  /**
     Turn a Boost::Sprirt::hold_any into a string via its stream 
     operator. Note: if data d IS a string, we add single quotation 
     marks to it, as is required by SQL. 
     @param d the data to be stringified 
   */  
  std::string stringifyData(data const d);

  /**
     Clear out all row commands. This is only invoked via the BookKeeper 
   */  
  void flushRows(){row_commands_.clear();}
  
 public:  
  /**
     The constructor, a table name is required 
     @param name the name of the table to be constructed 
   */
  Table(table_name name);

  /**
     The default destructor. 
   */
  ~Table(){};

  /**
     Return the name of the table 
   */
  table_name name(){return name_;}

  /**
     * This function takes all of the necessary components of a table 
     and * defines that table according to them. It then calls the 
     necessary * tableDefined() function. 
     * 
     * @param columns a vector of the columns (names and types) in this 
     table * @param pks a vector of column names holding the primary key 
   */
  void defineTable(std::vector<column> columns, primary_key pks);

  /**
     After a table is fully described, it must be told 
     explicity that it is defined. This function will then register 
     that table with the Book Keeper. 
   */
  void tableDefined();
  
  /**
     A boolean recording if the table has been defined. It is set to 
     true when tableDefined() is called. 
   */
  bool defined(){return defined_;}

  /**
     Return the current number of row commands this table 
     is holding. 
   */
  int nRows(){return row_commands_.size();}

  /**
     Return a pointer to a command at position i in the 
     row commands container. 
     @param i the integer position of the command in the container 
   */
  command* row_command(int i){return row_commands_.at(i);}

  /**
     Add a column to the list of this table's columns. 

     @param name the new column's name
     @param data_type the new column's data type (e.g. int)
   */
  void addField(std::string name, std::string data_type);

  /**
     Set the primary key given a column name. This name string 
     is turned into a primary key and the following function is called. 
     @param pk_string the single column name making up the primary key 
   */
  void setPrimaryKey(std::string const pk_string);

  /**
     Set the primary key given a primary key. 
     @param pk the primary key to be copied 
   */
  void setPrimaryKey(primary_key const pk){primary_key_ = pk;}

  /**
     Set the foreign key given a foreign key 
     @param fk the foreign key to be copied 
   */
  void addForeignKey(foreign_key const fk);

  /*
     Add an index to the list of indicies 
     @param i the index to add 
     (\@gidden add this functionality )
   */
  //void addIndex(index i){indicies_.push_back(i);}

  /**
     Return an SQL command to create the table in string form 
   */
  std::string create();

  /**
     Add a row to the vector of row commands 
     @param r the row to add to row_commands_ 
   */
  void addRow(row const r);

  /**
     Given a primary key reference, update a row with some 
     entry, e 
     @param pkref the primary key which will be updated 
     @param e the entry to update this row 
   */
  void updateRow(primary_key_ref const pkref, entry const e);

  /**
     Given a primary key reference, update a row with some 
     row of entries, r 
     @param pkref the primary key which will be updated 
     @param r the entry updates for this row 
   */
  void updateRow(primary_key_ref const pkref, row const r);

  /**
     Return the table's foreign keys as a string for writing 
   */
  std::string f_keys();

  /**
     Return the table's primary key as a string for writing 
   */
  std::string p_key();

  /**
     Clear out all row commands because they were just sent. 
     This function is only called from the BookKeeper. 
   */
  void flush(){this->flushRows();}

};

#endif // END TABLE_H
