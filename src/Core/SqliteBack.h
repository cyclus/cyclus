// SqliteBack.h
#if !defined(_SQLITEBACK_H)
#define _SQLITEBACK_H

#include "EventManager.h"
#include <list>
#include <string>

class SqliteBack: public EventBackend {
}

#include <string>
#include <sqlite3.h>

// Useful Typedefs
//   query results
typedef std::list<std::string> StrList;

class SqliteBack {
  public:
    SqliteBack(std::string filename);
    
    virtual ~SqliteBack();
  
    void notify(event_list events);

    void close();
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

    std::string tableName(event_ptr e); 

    void createTable(event_ptr e);

    void writeRow(event_ptr e);
    
    /**
       Issue a command to the database to flush Table rows 
       @param t the Table to which rows will be flushed 
     */
    void flush();

    /**
       A pointer to the database managed by the SqliteBack class 
     */
    sqlite3* db_;

    /**
       A boolean stating whether database is currently open on disk 
     */
    bool isOpen_;

    /**
       Stores the database's name, declared during construction. 
     */
    std::string name_;

    StrList cmds_;

    StrList tbl_names_;



};

#endif
