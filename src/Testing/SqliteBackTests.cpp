
#include <gtest/gtest.h>

#include "SqliteBack.h"

static std::string const path = "testdb.sqlite";

class FlushCatcher: public SqliteBack {
  public:
    FlushCatcher(std::string sim_id, std::string path) : SqliteBack(sim_id, path) { };
    StrList cmds;

  protected:
    virtual void flush() {
      cmds.insert(cmds.end(), cmds_.begin(), cmds_.end());
      SqliteBack::flush();
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
TEST(SqliteBackTest, CmdGenRegression) {
  FileDeleter fd(path);
  EventManager m;
  FlushCatcher back("fake-uuid", path);
  m.registerBackend(&back);

  m.newEvent("DumbTitle")
   ->addVal("animal", std::string("monkey"))
   ->addVal("weight", 10)
   ->addVal("height", 5.5)
   ->record();

  m.newEvent("DumbTitle")
   ->addVal("animal", std::string("elephant"))
   ->addVal("weight", 1000)
   ->record();

  m.newEvent("DumbTitle")
   ->addVal("animal", std::string("sea cucumber"))
   ->addVal("height", 1.2)
   ->record();

  m.close();

  ASSERT_EQ(back.cmds.size(), 4);
  EXPECT_EQ(back.cmds.at(0), "CREATE TABLE DumbTitle (" + kShortSimId + " INTEGER, animal VARCHAR(128), height REAL, weight INTEGER);");
  EXPECT_EQ(back.cmds.at(1), "INSERT INTO DumbTitle (" + kShortSimId + ", animal, height, weight) VALUES (1, \"monkey\", 5.5, 10);");
  EXPECT_EQ(back.cmds.at(2), "INSERT INTO DumbTitle (" + kShortSimId + ", animal, weight) VALUES (1, \"elephant\", 1000);");
  EXPECT_EQ(back.cmds.at(3), "INSERT INTO DumbTitle (" + kShortSimId + ", animal, height) VALUES (1, \"sea cucumber\", 1.2);");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(SqliteBackTest, MultiSim) {
  FileDeleter fd(path);
  EventManager m1, m2;

  FlushCatcher back1("uuid-1", path);
  m1.registerBackend(&back1);
  m1.newEvent("DumbTitle")
   ->addVal("animal", std::string("monkey"))
   ->addVal("weight", 10)
   ->record();
  m1.close();

  FlushCatcher back2("uuid-2", path);
  m2.registerBackend(&back2);
  m2.newEvent("DumbTitle")
   ->addVal("animal", std::string("elephant"))
   ->addVal("weight", 7)
   ->record();
  m2.close();

  ASSERT_EQ(back1.cmds.size(), 2);
  EXPECT_EQ(back1.cmds.at(0), "CREATE TABLE DumbTitle (" + kShortSimId + " INTEGER, animal VARCHAR(128), weight INTEGER);");
  EXPECT_EQ(back1.cmds.at(1), "INSERT INTO DumbTitle (" + kShortSimId + ", animal, weight) VALUES (1, \"monkey\", 10);");
  ASSERT_EQ(back2.cmds.size(), 1);
  EXPECT_EQ(back2.cmds.at(0), "INSERT INTO DumbTitle (" + kShortSimId + ", animal, weight) VALUES (2, \"elephant\", 7);");
}
