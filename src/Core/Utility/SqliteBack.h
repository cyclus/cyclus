// SqliteBack.h
#if !defined(_SQLITEBACK_H)
#define _SQLITEBACK_H

#include "EventManager.h"
#include "SqliteDb.h"

#include <list>
#include <string>

class SqliteBack: public EventBackend {
  public:
    SqliteBack(std::string filename);
    
    virtual ~SqliteBack();
  
    void notify(EventList events);

    std::string name();

    void close();

  private:
    bool tableExists(event_ptr e); 

    std::string valType(boost::any v); 

    std::string valData(boost::any v);

    /// Queue up a table-create command for e.
    void createTable(event_ptr e);

    void writeEvent(event_ptr e);
    
    /// Execute all pending commands
    void flush();

    /// A pointer to the database managed by the SqliteBack class 
    SqliteDb* db_;

    /// Stores the database's name, declared during construction. 
    std::string path_;

    StrList cmds_;

    StrList tbl_names_;
};

#endif
