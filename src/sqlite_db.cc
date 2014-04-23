// sqlite_db.cc

#include "sqlite_db.h"

#include "error.h"
#include "logger.h"
#include "datum.h"

#include <fstream>

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SqliteDb::SqliteDb(std::string path, bool readonly)
  : db_(NULL),
    isOpen_(false),
    path_(path),
    readonly_(readonly),
    overwrite_(false) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SqliteDb::~SqliteDb() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteDb::Overwrite() {
  overwrite_ = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteDb::close() {
  if (isOpen_) {
    if (sqlite3_close(db_) == SQLITE_OK) {
      isOpen_ = false;
    } else {
      throw IOError("Failed to close database: " + path_);
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteDb::open() {
  if (isOpen_) {
    return;
  }

  // if the file already exists, delete it
  if (overwrite_) {
    std::ifstream ifile(path_.c_str());
    if (ifile) {
      remove(path_.c_str());
    }
  }

  if (readonly_ && sqlite3_open_v2(path_.c_str(), &db_, SQLITE_OPEN_READONLY,
                                   NULL) == SQLITE_OK) {
    isOpen_ = true;
  } else if (sqlite3_open(path_.c_str(), &db_) == SQLITE_OK) {
    isOpen_ = true;
  } else {
    sqlite3_close(db_);
    throw IOError("Unable to create/open database " + path_);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SqlStatement::Ptr SqliteDb::Prepare(std::string sql) {
  open();
  return SqlStatement::Ptr(new SqlStatement(db_, sql));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteDb::Execute(std::string sql) {
  open();

  sqlite3_stmt* statement;
  int result =
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &statement, NULL);
  if (result != SQLITE_OK) {
    std::string error = sqlite3_errmsg(db_);
    throw IOError("SQL error: " + sql + " " + error);
  }

  result = sqlite3_step(statement);
  if (result != SQLITE_DONE && result != SQLITE_ROW && result != SQLITE_OK) {
    std::string error = sqlite3_errmsg(db_);
    throw IOError("SQL error: " + sql + " " + error);
  }

  sqlite3_finalize(statement);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<StrList> SqliteDb::Query(std::string sql) {
  open();
  sqlite3_stmt* statement;
  // query the database
  int check_query =
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &statement, NULL);
  if (check_query != SQLITE_OK) {
    std::string error = sqlite3_errmsg(db_);
    throw IOError("SQL error: " + error);
  }

  std::vector<StrList> results;
  int cols = sqlite3_column_count(statement);
  int result = 0;
  while (true) {
    result = sqlite3_step(statement);
    if (result != SQLITE_ROW) {
      break;
    }

    StrList values;
    for (int col = 0; col < cols; col++) {
      std::string  val;
      char* ptr = (char*)sqlite3_column_text(statement, col);
      if (ptr) {
        val = ptr;
      } else {
        val = "";
      }
      values.push_back(val);  // now we will never push NULL
    }
    results.push_back(values);
  }

  // collect errors
  if (result != SQLITE_DONE && result != SQLITE_OK) {
    std::string error = sqlite3_errmsg(db_);
    throw IOError("SQL error: " + error);
  }
  sqlite3_finalize(statement);
  return results;
}

} // namespace cyclus
