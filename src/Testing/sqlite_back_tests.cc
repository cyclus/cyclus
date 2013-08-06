

#include "sqlite_back.h"
#include "blob.h"

#include <gtest/gtest.h>
#include "boost/lexical_cast.hpp"
#include <boost/uuid/uuid_io.hpp>

static std::string const path = "testdb.sqlite";

class FlushCatcher: public cyclus::SqliteBack {
public:
  FlushCatcher(std::string path) : SqliteBack(path) { };
  cyclus::StrList cmds;

protected:
  virtual void Flush() {
    cmds.insert(cmds.end(), cmds_.begin(), cmds_.end());
    cyclus::SqliteBack::Flush();
  };
};

class FileDeleter {
public:
  FileDeleter(std::string path) {
    path_ = path;
  };

  ~FileDeleter() {
    remove(path_.c_str());
  };

private:
  std::string path_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(SqliteBackTest, Regression) {
  using cyclus::EventManager;
  FileDeleter fd(path);
  EventManager m;
  FlushCatcher back(path);
  m.RegisterBackend(&back);

  m.NewEvent("DumbTitle")
  ->AddVal("animal", std::string("monkey"))
  ->AddVal("weight", 10)
  ->AddVal("height", 5.5)
  ->AddVal("data", cyclus::Blob("banana"))
  ->Record();

  m.NewEvent("DumbTitle")
  ->AddVal("animal", std::string("elephant"))
  ->AddVal("weight", 1000)
  ->Record();

  m.NewEvent("DumbTitle")
  ->AddVal("animal", std::string("sea cucumber"))
  ->AddVal("height", 1.2)
  ->Record();

  m.close();

  std::string sid = boost::lexical_cast<std::string>(m.sim_id());

  ASSERT_EQ(back.cmds.size(), 4);
  EXPECT_EQ(back.cmds.at(0),
            "CREATE TABLE DumbTitle (SimID TEXT, animal TEXT, weight INTEGER, height REAL, data BLOB);");
  EXPECT_EQ(back.cmds.at(1),
            "INSERT INTO DumbTitle (SimID, animal, weight, height, data) VALUES ('" + sid +
            "', 'monkey', 10, 5.5, X'62616e616e61');");
  EXPECT_EQ(back.cmds.at(2),
            "INSERT INTO DumbTitle (SimID, animal, weight) VALUES ('" + sid +
            "', 'elephant', 1000);");
  EXPECT_EQ(back.cmds.at(3),
            "INSERT INTO DumbTitle (SimID, animal, height) VALUES ('" + sid +
            "', 'sea cucumber', 1.2);");
}

