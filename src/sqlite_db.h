// sqlite_db.h
#ifndef CYCLUS_SQLITEDB_H_
#define CYCLUS_SQLITEDB_H_

#include <vector>
#include <string>
#include <sqlite3.h>

namespace cyclus {

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
  @param readonly a boolean indicating true if db is readonly
  */
  SqliteDb(std::string path, bool readonly = false);

  virtual ~SqliteDb();

  /*!
  Finishes any incomplete operations and closes the database.
  @throw IOError if failed to close the database properly
  */
  void close();

  /*!
  Opens the sqlite database by either opening/creating a file (default) or
  creating/overwriting a file (see the overwrite method).

  @throw IOError if failed to open existing database
  */
  void open();

  /*!
  Instead of opening a file of the specified name (if it already exists),
  overwrite it with a new empty database.
  */
  void Overwrite();

  /*!
  Execute an SQL command.

  @param cmd an Sqlite compatible SQL command
  @throw IOError SQL command execution failed (e.g. invalid SQL)
  */
  void Execute(std::string cmd);

  /*!
  Execute an SQL query and return its results

  @param cmd an Sqlite compatible SQL query
  @return a list of row entries
  @throw IOError SQL command execution failed (e.g. invalid SQL)
  */
  std::vector<StrList> Query(std::string cmd);

 private:

  sqlite3* db_;

  bool isOpen_;

  std::string path_;

  /// indicates if open() will overwrite a file at 'path'.
  bool overwrite_;

  /// indicates true if the db is readonly false otherwise
  bool readonly_;
};
} // namespace cyclus
#endif // ifndef CYCLUS_SQLITEDB_H_

