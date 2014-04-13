#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "sqlite_db.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SqliteDbTests : public ::testing::Test {
 public:
  virtual void SetUp() {
    path = "testdb.sqlite";
    v1 = "stuff";
    v2 = "thing";
    db = new cyclus::SqliteDb(path);
    db->open();
    db->Execute("create table t1 (data1 TEXT, data2 TEXT);");
    db->Execute(
        "insert into t1 (data1, data2) values ('" + v1 + "', '" + v2 + "');");
  }

  virtual void TearDown() {
    db->close();
    delete db;
    remove(path.c_str());
  }

  cyclus::SqliteDb* db;
  std::string path, v1, v2;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SqliteDbTests, OpenExistingAndRetrieve) {
  using cyclus::StrList;
  std::vector<StrList> result;

  ASSERT_NO_THROW(result = db->Query("select * from t1"));
  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result.front().size(), 2);
  EXPECT_EQ(result.front().front(), v1);
  EXPECT_EQ(result.front().back(), v2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SqliteDbTests, Overwrite) {
  using cyclus::StrList;
  std::vector<StrList> result;

  db->close();
  db->Overwrite();

  ASSERT_NO_THROW(db->open());

  ASSERT_NO_THROW(result = db->Query("select * from sqlite_master"));
  EXPECT_EQ(result.size(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SqliteDbTests, CreateAndInsert) {
  using cyclus::SqliteDb;
  using cyclus::StrList;
  std::vector<StrList> result;

  std::string new_path = "new" + path;
  SqliteDb db(new_path);

  ASSERT_NO_THROW(db.open());

  ASSERT_NO_THROW(db.Execute("create table t1 (data1 TEXT, data2 TEXT);"));
  ASSERT_NO_THROW(db.Execute(
      "insert into t1 (data1, data2) values ('hello', 'goodbye');"));
  ASSERT_NO_THROW(result = db.Query("select * from t1"));
  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result.front().size(), 2);
  EXPECT_EQ(result.front().front(), "hello");
  EXPECT_EQ(result.front().back(), "goodbye");

  ASSERT_NO_THROW(db.close());
  remove(new_path.c_str());
}
