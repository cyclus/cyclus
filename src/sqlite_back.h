// sqlite_back.h
#ifndef CYCLUS_CORE_UTILITY_SQLITE_BACK_H_
#define CYCLUS_CORE_UTILITY_SQLITE_BACK_H_

#include <list>
#include <string>

#include "query_backend.h"
#include "sqlite_db.h"

namespace cyclus {

/// An Recorder backend that writes data to an sqlite database.  Identically
/// named Datum objects have their data placed as rows in a single table.  Handles the
/// following datum value types: int, float, double, std::string, cyclus::Blob.
/// Unsupported value types are stored as an empty string.
class SqliteBack: public QueryBackend {
 public:
  /// Creates a new sqlite backend that will write to the database file
  /// specified by path. If the file doesn't exist, a new one is created.
  /// @param path the filepath (including name) to write the sqlite file.
  SqliteBack(std::string path);

  virtual ~SqliteBack() {};

  /// Writes Datum objects immediately to the database as a single transaction.
  /// @param data group of Datum objects to write to the database together.
  void Notify(DatumList data);

  /// Returns a unique name for this backend.
  std::string Name();

  /// Finishes any incomplete tasks and closes the database/file.
  void Close();

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds);

 protected: // for testing
  /// Execute all pending commands.
  virtual void Flush();

  /// pending sql commands.
  StrList cmds_;

 private:
  /// returns true if the table name already exists.
  bool TableExists(std::string name);

  QueryResult GetTableInfo(std::string table);

  /// returns a valid sql data type name for v (e.g.  INTEGER, REAL, TEXT, etc).
  std::string ValType(boost::spirit::hold_any v);

  /// converts the value to a string insertable into the sqlite db.
  std::string ValAsString(boost::spirit::hold_any v);

  /// converts the string value in s to a c++ value corresponding the the
  /// supported sqlite datatype type in a hold_any object.
  boost::spirit::hold_any StringAsVal(std::string s, std::string type);

  /// Queue up a table-create command for d.
  void CreateTable(Datum* d);

  /// constructs an SQL INSERT command for d and queues it for db insertion.
  void WriteDatum(Datum* d);

  /// An interface to a sqlite db managed by the SqliteBack class.
  SqliteDb db_;

  /// Stores the database's path+name, declared during construction.
  std::string path_;

  /// table names already existing (created) in the sqlite db.
  StrList tbl_names_;
};
} // namespace cyclus
#endif
