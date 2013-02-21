// SqliteBack.h
#if !defined(_SQLITEBACK_H)
#define _SQLITEBACK_H
#include "EventManager.h"
#include "SqliteDb.h"

#include <list>
#include <string>

/*!
An EventManager backend that writes data to an sqlite database.  Identically
named events have their data placed as rows in a single table.  Handles the
following event value types: int, float, double, std::string (up to 128
characters long).  Unsupported value types are stored as an empty string.
*/
class SqliteBack: public EventBackend {
  public:
    /*!
    Creates a new sqlite backend that will write to (or overwrite) a file.

    @param filename the filepath (including name) to write the sqlite file.
    */
    SqliteBack(std::string path);

    virtual ~SqliteBack();

    /*!
    Write events immediately to the database as a single transaction.

    @param events group of events to write to the database together.
    */
    void notify(EventList events);

    /// Returns a unique name for this backend.
    std::string name();

    /// Finishes any incomplete tasks and closes the database/file.
    void close();

  protected: // for testing

    /// Execute all pending commands.
    virtual void flush();

    /// pending sql commands.
    StrList cmds_;

  private:

    /// returns true if the table for e already exists.
    bool tableExists(event_ptr e);

    /// returns a valid sql data type name for v (e.g.  INT, REAL, VARCHAR(128), etc).
    std::string valType(boost::any v);

    /// converts the value to a string insertable into the sqlite db.
    std::string valAsString(boost::any v);

    /// Queue up a table-create command for e.
    void createTable(event_ptr e);

    /// constructs an SQL INSERT command for e and queues it for db insertion.
    void writeEvent(event_ptr e);

    /// A pointer to the database managed by the SqliteBack class.
    SqliteDb* db_;

    /// Stores the database's path+name, declared during construction.
    std::string path_;

    /// table names already existing (created) in the sqlite db.
    StrList tbl_names_;
};

#endif
