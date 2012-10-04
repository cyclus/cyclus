// SqliteBack.h
#if !defined(_SQLITEBACK_H)
#define _SQLITEBACK_H

#include "EventManager.h"

#include <list>
#include <string>
#include <sqlite3.h>


typedef std::list<std::string> StrList;

class SqliteBack: public EventBackend {
  public:
    SqliteBack(std::string filename);
    
    virtual ~SqliteBack();
  
    void notify(event_list events);

    void close();

    std::string name();

  private:
    /// Open a connection to Sqlite database file.
    void open();

    void executeSQL(std::string cmd);

    /**
       Utility function to determine if a file exists 
       @param filename the name of the file to search for 
     */
    bool fexists(const char *filename); 

    bool tableExists(event_ptr e); 

    std::string valType(boost::any v); 

    std::string tableName(event_ptr e); 

    /// Queue up a table-create command for e.
    void createTable(event_ptr e);

    void writeEvent(event_ptr e);
    
    /// Execute all pending commands
    void flush();

    /// A pointer to the database managed by the SqliteBack class 
    sqlite3* db_;

    /// A boolean stating whether database is currently open on disk 
    bool isOpen_;

    /// Stores the database's name, declared during construction. 
    std::string path_;

    StrList cmds_;

    StrList tbl_names_;



};

#endif
