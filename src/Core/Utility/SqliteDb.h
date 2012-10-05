// SqliteDb.h
#if !defined(_SQLITEDB_H)
#define _SQLITEDB_H

#include <vector>
#include <string>
#include <sqlite3.h>

typedef std::vector<std::string> StrList;

class SqliteDb {
  public:

    SqliteDb(std::string filename);

    virtual ~SqliteDb();

    void close();

    void open();

    void overwrite();

    void execute(std::string cmd);

    std::vector<StrList> query(std::string cmd);

  private:

    sqlite3* db_;

    bool isOpen_;

    std::string path_;

    bool overwrite_;
};

#endif

