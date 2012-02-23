// BookKeeper.h
#if !defined(_BOOKKEEPER_H)
#define _BOOKKEEPER_H

#include <string>
#include <vector>

#include "Database.h"
#include "Table.h"
#include "CycException.h"

#define BI BookKeeper::Instance()
#define NUMISOS 1500

/*!
   @brief
   The BookKeeper is a (singleton) class for handling I/O.
   
   @section introduction Introduction
   The Cyclus Book Keeper is a singleton member of the 
   BookKeeperClass. The Book Keeper Instance (BI) controls 
   reading and writing from the simulation database. The 
   Book Keeper creates the database file (cyclus.h5) and 
   maintains functions with which the LogicianClass and 
   various models can write individual datasets to that file.
   
   @section singletonInstance Singleton Instance
   In order to utilize the database functions stewarded by the 
   Book Keeper, a model must include the Book Keeper header file 
   and call BookKeeperClass functions via the singleton Book 
   Keeper Instance LI.
   
   @section writeDB Writing the Database File
   In the long term, the function that writes the output for 
   Cyclus may be of any format. For now, the only supported format 
   is hdf5. The Book Keeper creates a simulation data file in the 
   build directory (called cyclus.h5) as soon as it is initialized.

   @section writeToDB Writing to the Database
   It is an open question whether the data for the simulation should 
   be kept entirely in memory until the end of the simulation. It 
   may be best to open and write to the file all in one fluid process 
   at the end of the simulation to save time. Alternatively though, 
   there may be some value in writing the simulation data to the file 
   as it is generated in order to facilitate a future in which the 
   simulation might have a dynamic start and stop capability.
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
   *True iff the db is open.
   */
  bool dbIsOpen_;

  /**
   * True iff the db is exists.
   */
  bool dbExists_;

  /**
   * Utility function to determine if a file exists
   * @param filename the name of the file to search for
   */
  bool fexists(const char *filename); 

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
   * Creates a database file with the default name, cyclus.sqlite. 
   */
  void createDB(){createDB("cyclus.sqlite");}
 
  /**
   * Creates a database file with the name indicated. 
   * This function queries the environment variable CYCLUS_OUTPUT_DIR.
   * If the named file in CYCLUS_OUTPUT_DIR already exists, this function
   * will delete it and create a new file.
   *
   * @param name is the name of the sqlite database file. Should end in .sqlite
   */
  void createDB(std::string name);

  /**
   * Adds a table to the database's vector of tables and then issues
   * the command to create that table.
   *
   * @param t is the table to be registered
   */
  void registerTable(Table* t);
  
  /**
   * Tables alert the BookKeeper when they have maxed out their queue of
   * row commands. The BookKeeper then invokes the Database's writeRows
   * function and then flushes the Table's row commands container.
   *
   * @param t is the table in question
   */
  void tableAtThreshold(Table* t);

  /**
   * Returns the database this BookKeeper is maintaining.
   */
  Database* getDB() {return db_;}

  /**
   * Closes the database this BookKeeper is maintaining.
   * However, before issuing the close command, any Tables
   * with row commands remaining will have those commands issued.
   */
  void closeDB();
  
  /**
   * Returns whether it's open
   */
  bool isOpen(){return dbIsOpen_;};
  
  /**
   * Returns whether it exists
   */
  bool exists(){return dbExists_;};

  /**
   * Returns the name of the database
   */
  std::string name(){return dbName_;};

};

#endif

