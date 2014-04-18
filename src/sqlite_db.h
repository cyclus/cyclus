// sqlite_db.h
#ifndef CYCLUS_SQLITEDB_H_
#define CYCLUS_SQLITEDB_H_

#include <vector>
#include <string>
#include <sqlite3.h>
#include <boost/shared_ptr.hpp>

#include "error.h"

namespace cyclus {

typedef std::vector<std::string> StrList;

class SqliteDb;

class SqlStatement {
  friend class SqliteDb;
 public:
  typedef boost::shared_ptr<SqlStatement> Ptr;

  ~SqlStatement() {
    Must(sqlite3_finalize(stmt_));
  }

  void Exec() {
    Must(sqlite3_step(stmt_));
    Must(sqlite3_reset(stmt_));
  }

  std::vector<StrList> Query() {
    std::vector<StrList> results;
    int cols = sqlite3_column_count(stmt_);
    while (true) {
      if(sqlite3_step(stmt_) != SQLITE_ROW) {
        break;
      }

      StrList values;
      for (int col = 0; col < cols; col++) {
        char* val = (char*)sqlite3_column_text(stmt_, col);
        if (val == NULL) {
          values.push_back("");
        } else {
          values.push_back(val);
        }
      }
      results.push_back(values);
    }
    return results;
  }

  void BindInt(int i, int val) {
    Must(sqlite3_bind_int(stmt_, i, val));
  }

  void BindDouble(int i, double val) {
    Must(sqlite3_bind_double(stmt_, i, val));
  }

  void BindText(int i, const char* val) {
    Must(sqlite3_bind_text(stmt_, i, val, -1, SQLITE_TRANSIENT));
  }

  void BindBlob(int i, const void* val, int n) {
    Must(sqlite3_bind_blob(stmt_, i, val, n, SQLITE_TRANSIENT));
  }

 private:
  SqlStatement(sqlite3* db, std::string zSql) : db_(db), zSql_(zSql) {
    Must(sqlite3_prepare_v2(db_, zSql.c_str(), -1, &stmt_, NULL));
  }

  void Must(int status) {
    if (status != SQLITE_OK && status != SQLITE_DONE && status != SQLITE_ROW) {
      std::string err = sqlite3_errmsg(db_);
      throw IOError("SQL error [" + zSql_ + "]: " + err);
    }
  }

  sqlite3* db_;
  std::string zSql_;
  sqlite3_stmt* stmt_;
};

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

  SqlStatement::Ptr Prepare(std::string sql);

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

