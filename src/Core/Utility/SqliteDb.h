// SqliteDb.h
#if !defined(_SQLITEDB_H)
#define _SQLITEDB_H

#include <vector>
#include <string>
#include <sqlite3.h>

typedef std::vector<std::string> StrList;

/*!
An abstraction over the Sqlite native C interface to simplify database
creation and data insertion.
*/
class SqliteDb {
  public:

    /*!
    Creates a new Sqlite database to be stored at the specified path.

    @param path the path+name for the sqlite database file
    */
    SqliteDb(std::string path);

    virtual ~SqliteDb();

    /// Finishes any incomplete operations and closes the database.
    void close();

    /*!
    Opens the sqlite database by either opening/creating a file (default) or
    creating/overwriting a file (see the overwrite method).
    */
    void open();

    /*!
    Instead of opening a file of the specified name (if it already exists),
    overwrite it with a new empty database.
    */
    void overwrite();

    /*!
    Execute an SQL command.
    
    @param cmd an Sqlite compatible SQL command
    */
    void execute(std::string cmd);

    /*!
    Execute an SQL query and return its results
    
    @param cmd an Sqlite compatible SQL command
    @return a list of row entries
    */
    std::vector<StrList> query(std::string cmd);

  private:

    sqlite3* db_;

    bool isOpen_;

    std::string path_;

    /// indicates if open() will overwrite a file at 'path'.
    bool overwrite_;
};

#endif

