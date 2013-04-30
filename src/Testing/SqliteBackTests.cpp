
#include <gtest/gtest.h>
#include <iostream>

#include "SqliteBack.h"

static std::string const path = "testdb.sqlite";

class FlushCatcher: public SqliteBack {
  public:
    FlushCatcher(std::string path) : SqliteBack(path) { };
    StrList cmds;

  protected:
    virtual void flush() {
      cmds.insert(cmds.end(), cmds_.begin(), cmds_.end());
      cmds_.clear();
    };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(SqliteBackTest, CmdGenRegression) {
  FlushCatcher back(path);
  EventManager m;
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

  remove(path.c_str());
}
