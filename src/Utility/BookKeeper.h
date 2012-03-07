// BookKeeper.h
#if !defined(_BOOKKEEPER_H)
#define _BOOKKEEPER_H

#include <string>
#include <vector>

#include "Database.h"
#include "Table.h"
#include "CycException.h"

#define BI BookKeeper::Instance()

typedef std::string file_path;

/*!
   @brief
   The BookKeeper is a (singleton) class for handling I/O.
   
   @section introduction Introduction
   The Cyclus Book Keeper is a singleton member of the 
   BookKeeperClass. The Book Keeper Instance (BI) manages the 
   access to the simulation's output database. Individual
   modules create Tables as per their need, and registers them
   with the Book Keeper. All interaction with the Database is 
   handled by the Book Keeper.

   @section singletonInstance Singleton Instance
   In order to utilize the database functions stewarded by the 
   Book Keeper, a model must include the Book Keeper header file 
   and call BookKeeperClass functions via the singleton Book 
   Keeper Instance BI.
   
   @section writeDB Writing the Database File
   In the long term, the function that writes the output for 
   Cyclus may be of any format. The Book Keeper API is independent
   of data format. Currently, it interacts with the TableClass 
   and DatabaseClass, both of which are written with respect to
   SQL, specifically SQLite. The Book Keeper creates a simulation 
   data file as soon as it is initialized.

   @section writeToDB Writing to the Database
   Under the current Book Keeper paradigm, rows of data are written 
   at intervals described in the TableClass. When a Table reaches 
   a threshold number of row commands, it alerts the Book Keeper,
   who is allowed to act accordingly.
*/

class BookKeeper {
private:
  /**
   * A pointer to this BookKeeper once it has been initialized.
   */
  static BookKeeper* instance_;
  
  /**
   * The output database for the simulation this BookKeeper is 
   * responsible for.
   */
  Database* db_;
  
  /**
   * Stores the final filename we'll use for the DB, since we use it 
   * in multiple places and don't want there to be any ambiguity.
   */
  std::string dbName_;
  
  /**
   * True iff the db is open.
   */
  bool dbIsOpen_;
  
  /**
   * A boolean to determine if logging is on.
   */
  static bool logging_on_;

protected:
  /**
   * The (protected) constructor for this class, which can only be 
   * called indirectly by the client.
   */
  BookKeeper();

public:
  /**
   * Gives all simulation objects global access to the BookKeeper by 
   * returning a pointer to it.
   * Like the Highlander, there can be only one.
   *
   * @return a pointer to the BookKeeper
   */
  static BookKeeper* Instance();

  /**
   * Return the state of logging being on or off
   */
  bool loggingIsOn();

  /**
   * Turn on logging
   */
  void turnLoggingOn();

  /**
   * Turn off logging
   */
  void turnLoggingOff();
  
  /**
   * Creates a database file with the default name, cyclus.sqlite. 
   */
  void createDB(){createDB("cyclus.sqlite");}
 
  /**
   * Creates a database file with the name indicated. 
   * This function queries the environment variable CYCLUS_OUTPUT_DIR.
   *
   * @param name is the name of the sqlite database file. Should end in .sqlite
   */
  void createDB(std::string name);

  /**
   * Creates a database given a file_path. This is the master create function.
   * If the named file in fpath already exists, this function
   * will delete it and create a new file.
   *
   * @param name is the name of the sqlite database file. Should end in .sqlite
   * @param fpath the path to the file
   */
  void createDB(std::string name, file_path fpath);

  /**
   * Returns the database this Book Keeper is maintaining.
   */
  Database* getDB() {return db_;}

  /**
   * Returns the name of the database
   */
  std::string dbName(){return dbName_;}
  
  /**
   * Returns whether or not the database exists
   */
  bool dbExists();

  /**
   * Returns whether the database open (and it exists)
   */
  bool dbIsOpen();

  /**
   * Opens the database this Book Keeper is maintaining.
   */
  void openDB();

  /**
   * Closes the database this Book Keeper is maintaining.
   * However, before issuing the close command, any Tables
   * with row commands remaining will have those commands issued.
   */
  void closeDB();

  /**
   * Adds a table to the database's vector of tables and then issues
   * the command to create that table.
   *
   * @param t is the table to be registered
   */
  void registerTable(table_ptr t);

  /**
   * Unregister a table with the Book Keeper's database
   *
   * @param t is the table to be removed
   */
  void removeTable(table_ptr t);
  
  /**
   * Returns the number of tables registered with the BookKeeper
   */
  int nTables();

  /**
   * Tables alert the BookKeeper when they have maxed out their queue of
   * row commands. The BookKeeper then invokes the Database's writeRows
   * function and then flushes the Table's row commands container.
   *
   * @param t is the table in question
   */
  void tableAtThreshold(table_ptr t);
  
};

#endif

