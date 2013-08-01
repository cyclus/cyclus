// sqlite_back.h
#pragma once

#include "event_backend.h"
#include "SqliteDb.h"

#include <list>
#include <string>

/*!
An EventManager backend that writes data to an sqlite database.  Identically
named events have their data placed as rows in a single table.  Handles the
following event value types: int, float, double, std::string, cyclus::Blob.
Unsupported value types are stored as an empty string.
*/
class SqliteBack: public EventBackend {
 public:
  /*!
  Creates a new sqlite backend that will write to the database file
  specified by path. If the file doesn't exist, a new one is created.

  @param path the filepath (including name) to write the sqlite file.
  */
  SqliteBack(std::string path);

  virtual ~SqliteBack() {};

  /*!
  Write events immediately to the database as a single transaction.

  @param events group of events to write to the database together.
  */
  void Notify(EventList events);

  /// Returns a unique name for this backend.
  std::string Name();

  /// Finishes any incomplete tasks and closes the database/file.
  void Close();

 protected: // for testing

  /// Execute all pending commands.
  virtual void Flush();

  /// pending sql commands.
  StrList cmds_;

 private:

  /// returns true if the table name already exists.
  bool TableExists(std::string name);

  /// returns a valid sql data type name for v (e.g.  INT, REAL, VARCHAR(128), etc).
  std::string ValType(boost::spirit::hold_any v);

  /// converts the value to a string insertable into the sqlite db.
  std::string ValAsString(boost::spirit::hold_any v);

  /// Queue up a table-create command for e.
  void CreateTable(Event* e);

  /// constructs an SQL INSERT command for e and queues it for db insertion.
  void WriteEvent(Event* e);

  /// An interface to a sqlite db managed by the SqliteBack class.
  SqliteDb db_;

  /// Stores the database's path+name, declared during construction.
  std::string path_;

  /// table names already existing (created) in the sqlite db.
  StrList tbl_names_;
};

