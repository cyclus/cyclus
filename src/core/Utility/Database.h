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

/**
   @class Database 
    
    
   The Database class offers creation and connection 
   capability to (currently) an sqlite database. A connection 
   can be as simple as querying a database that already exists, 
   or tables can be managed via creation, row addition, and 
   row updating. However, the Database class is 
   "non-intelligent" in the sense that it must be told when 
   to issue these table-related commands. In Cyclus, this 
   database management is issued through the Book Keeper. 
    
   @section Querying a Database 
   The Database class can be used to simply query a database. An 
   example is in the MassTable class. To do so, a Database object 
   must be instantiated and queries passed to it via the query() 
   method. The result is a vector of row objects, which are themselves 
   vectors of row entries as strings. 
    
   @section Database Management 
   The Database class offers an array of managment capability. It stores 
   a vector of Tables that are involved with the connected database. It 
   can create these Tables in the database, add rows to those tables, 
   and update rows in the tables. It assumes that the Tables themselves 
   offer correct commands to perform these operations. 
 */

class Database {
 private:
  /**
     A pointer to the database managed by the Database class 
   */
  sqlite3 *database_;

  /**
     A boolean stating whether database exists, i.e. was correctly 
     opened 
   */
  bool exists_;

  /**
     A boolean stating whether database is currently open on disk 
   */
  bool isOpen_;

  /**
     Stores the database's name, declared during construction. 
   */
  std::string name_;

  /**
     Stores the file path to the database, declared during construction. 
   */
  std::string path_;

  /**
     A container of pointers to each Table included in the database 
   */
  std::vector<table_ptr> tables_;

  /**
     A command which checks whether a Table exists in the 
     Database's Table container, tables_ 
     @param t a pointer the Table in question 
     @return whether t is in tables_ 
   */
  bool tableExists(table_ptr t);

  /**
     Issues a command to a Table which will alter that table, 
     e.g. create, add row, update row 
     @param cmd the command to execute 
   */
  void issueCommand(std::string cmd);

 public:
  /**
     Constructor. All Databases must have a filename. 
     @param filename the name of the database file 
   */
  Database(std::string filename);
  
  /**
     Constructor with a specified file path. 
     @param filename the name of the database file 
     @param file_path the path to the named file 
   */
  Database(std::string filename, std::string file_path);
  
  /**
     Destructor 
   */
  ~Database(){};
  
  /**
     Return the name of the Database 
   */
  std::string name(){return name_;}

  /**
     Return the path to the database 
   */
  std::string path();

  /**
     Utility function to determine if a file exists 
     @param filename the name of the file to search for 
   */
  bool fexists(const char *filename); 

  /**
     A command to open the database 
     @return whether the opening process succeded 
   */
  bool open();

  /**
     A command to close the database 
     @return whether the opening process succeded 
   */
  bool close();

  /**
     Return if the Database exists, i.e. has been instantiated 
     @return whether the database exists 
   */
  bool dbExists();

  /**
     Return if the Database is open on disk 
     @return whether the database is open 
   */
  bool isOpen();

  /**
     Issue a query to the database and return the result 
     @param a_query the query to execute 
     @return the query result 
   */
  query_result query(std::string a_query);
  
  /**
     Register a table with this Database, adding it to tables_ 
     @param t the Table to register 
   */
  void registerTable(table_ptr t);

  /**
     Unregister a table with this Database, removing it from tables_ 
     @param t the Table to remove 
   */
  void removeTable(table_ptr t);

  /**
     Issue a command to the database to create a Table 
     @param t the Table to create 
   */
  void createTable(table_ptr t);

  /**
     Issue a command to the database to write rows to a Table 
     @param t the Table to which rows will be written 
   */
  void writeRows(table_ptr t);
  
  /**
     Issue a command to the database to flush Table rows 
     @param t the Table to which rows will be flushed 
   */
  void flush(table_ptr t);
  
  /**
     Return the number of tables registered with the Database 
     @return the number of tables registered 
   */
  int nTables() {return tables_.size();}

  /**
     Return the registered table at a given position tables_ 
     @param i the position of the Table in question 
     @return the Table at position i 
   */
  table_ptr tablePtr(int i) {return tables_.at(i);}
};

#endif
