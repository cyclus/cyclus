
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "SqliteDb.h"

static std::string const path = "testdb.sqlite";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(SqliteDbTest, CreateAndInsert) {
  using cyclus::SqliteDb;
  using cyclus::StrList;
  remove(path.c_str());
  std::vector<StrList> result;
  SqliteDb db(path);

  ASSERT_NO_THROW(db.open());

  ASSERT_NO_THROW(db.execute("create table t1 (data1 TEXT, data2 TEXT);"));
  ASSERT_NO_THROW(db.execute("insert into t1 (data1, data2) values ('hello', 'goodbye');"));
  ASSERT_NO_THROW(result = db.query("select * from t1"));
  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result.front().size(), 2);
  EXPECT_EQ(result.front().front(), "hello");
  EXPECT_EQ(result.front().back(), "goodbye");

  ASSERT_NO_THROW(db.close());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(SqliteDbTest, OpenExistingAndRetrieve) {
  using cyclus::SqliteDb;
  using cyclus::StrList;
  std::vector<StrList> result;
  SqliteDb db(path);

  ASSERT_NO_THROW(db.open());

  ASSERT_NO_THROW(result = db.query("select * from t1"));
  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result.front().size(), 2);
  EXPECT_EQ(result.front().front(), "hello");
  EXPECT_EQ(result.front().back(), "goodbye");

  ASSERT_NO_THROW(db.close());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(SqliteDbTest, Overwrite) {
  using cyclus::SqliteDb;
  using cyclus::StrList;
  std::vector<StrList> result;
  SqliteDb db(path);
  db.overwrite();

  ASSERT_NO_THROW(db.open());

  ASSERT_NO_THROW(result = db.query("select * from sqlite_master"));
  EXPECT_EQ(result.size(), 0);

  ASSERT_NO_THROW(db.close());
  remove(path.c_str());
}

