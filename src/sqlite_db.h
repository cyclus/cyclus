#ifndef CYCLUS_SRC_SQLITE_DB_H_
#define CYCLUS_SRC_SQLITE_DB_H_

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "error.h"

class sqlite3;
class sqlite3_stmt;

namespace cyclus {

typedef std::vector<std::string> StrList;

class SqliteDb;

/// Thin wrapper class over sqlite3 prepared statements.  See
/// http://sqlite.org/cintro.html for an overview of how prepared statements
/// work.
class SqlStatement {
  friend class SqliteDb;

 public:
  typedef boost::shared_ptr<SqlStatement> Ptr;

  ~SqlStatement();

  /// Executes the prepared statement.
  void Exec();

  /// Executes the prepared statement.
  void Reset();

  /// Step to next row of previously executed query. Returns false when there
  /// are no more rows. Previously retrieved text or blob column data memory
  /// is deallocated.
  bool Step();

  /// Returns an int value for the specified column of the current query row.
  int GetInt(int col);

  /// Returns a double value for the specified column of the current query row.
  double GetDouble(int col);

  /// Returns a byte array value for the specified column of the current query
  /// row. This can be used for retrieving TEXT and BLOB column data.
  char* GetText(int col, int* n);

  /// Binds the templated sql parameter at index i to val.
  void BindInt(int i, int val);

  /// Binds the templated sql parameter at index i to val.
  void BindDouble(int i, double val);

  /// Binds the templated sql parameter at index i to val.
  void BindText(int i, const char* val);

  /// Binds the templated sql parameter at index i to the value pointed to by
  /// val. n is the length in bytes of the value.
  void BindBlob(int i, const void* val, int n);

 private:
  SqlStatement(sqlite3* db, std::string zSql);

  /// throws an exception if status (the return value of an sqlite function)
  /// does not represent success.
  void Must(int status);

  sqlite3* db_;
  std::string zSql_;
  sqlite3_stmt* stmt_;
};

/// An abstraction over the Sqlite native C interface to simplify database
/// creation and data insertion.
class SqliteDb {
 public:
  /// Creates a new Sqlite database to be stored at the specified path.
  ///
  /// @param path the path+name for the sqlite database file
  /// @param readonly a boolean indicating true if db is readonly
  SqliteDb(std::string path, bool readonly = false);

  virtual ~SqliteDb();

  /// Finishes any incomplete operations and closes the database.
  /// @throw IOError if failed to close the database properly
  void close();

  /// Opens the sqlite database by either opening/creating a file (default) or
  /// creating/overwriting a file (see the overwrite method).
  ///
  /// @throw IOError if failed to open existing database
  void open();

  /// Instead of opening a file of the specified name (if it already exists),
  /// overwrite it with a new empty database.
  void Overwrite();

  /// Creates a sqlite prepared statement for the given sql.  See
  /// http://sqlite.org/cintro.html for an overview of how prepared statements
  /// work.
  SqlStatement::Ptr Prepare(std::string sql);

  /// Execute an SQL command.
  ///
  /// @param cmd an Sqlite compatible SQL command
  /// @throw IOError SQL command execution failed (e.g. invalid SQL)
  void Execute(std::string cmd);

  /// Execute an SQL query and return its results
  ///
  /// @param cmd an Sqlite compatible SQL query
  /// @return a list of row entries
  /// @throw IOError SQL command execution failed (e.g. invalid SQL)
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

}  // namespace cyclus

#endif  // CYCLUS_SRC_SQLITE_DB_H_
