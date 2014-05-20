#include "boost/lexical_cast.hpp"
#include <boost/uuid/uuid_io.hpp>
#include <gtest/gtest.h>

#include "blob.h"
#include "sqlite_back.h"

#include "tools.h"

static std::string const path = "testdb.sqlite";

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(SqliteBackTest, Regression) {
  using cyclus::Recorder;
  FileDeleter fd(path);
  Recorder m;
  cyclus::SqliteBack back(path);
  m.RegisterBackend(&back);

  std::vector<int> vect;
  vect.push_back(4);
  vect.push_back(2);
  std::vector<std::string> svect;
  svect.push_back("one");
  svect.push_back("two");

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("monkey"))
      ->AddVal("weight", 10)
      ->AddVal("height", 5.5)
      ->AddVal("alive", true)
      ->AddVal("answer", vect)
      ->AddVal("count", svect)
      ->AddVal("data", cyclus::Blob("banana"))
      ->Record();

  vect[1] = 3;
  svect[1] = "three";

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("elephant"))
      ->AddVal("weight", 1000)
      ->AddVal("height", 4.2)
      ->AddVal("alive", false)
      ->AddVal("answer", vect)
      ->AddVal("count", svect)
      ->AddVal("data", cyclus::Blob("a very large mammal"))
      ->Record();

  m.Close();

  cyclus::QueryResult qr = back.Query("DumbTitle", NULL);

  std::string animal = qr.GetVal<std::string>("animal", 0);
  int weight = qr.GetVal<int>("weight", 0);
  double height = qr.GetVal<double>("height", 0);
  bool alive = qr.GetVal<bool>("alive", 0);
  cyclus::Blob data = qr.GetVal<cyclus::Blob>("data", 0);
  std::vector<int> vget = qr.GetVal<std::vector<int> >("answer", 0);
  std::vector<std::string> svget = qr.GetVal<std::vector<std::string> >("count", 0);

  EXPECT_EQ("monkey", animal);
  EXPECT_EQ(10, weight);
  EXPECT_EQ(5.5, height);
  EXPECT_TRUE(alive);
  EXPECT_EQ("banana", data.str());
  ASSERT_EQ(2, vget.size());
  EXPECT_EQ(4, vget[0]);
  EXPECT_EQ(2, vget[1]);
  ASSERT_EQ(2, svget.size());
  EXPECT_EQ("one", svget[0]);
  EXPECT_EQ("two", svget[1]);

  animal = qr.GetVal<std::string>("animal", 1);
  weight = qr.GetVal<int>("weight", 1);
  height = qr.GetVal<double>("height", 1);
  alive = qr.GetVal<bool>("alive", 1);
  data = qr.GetVal<cyclus::Blob>("data", 1);
  vget = qr.GetVal<std::vector<int> >("answer", 1);
  svget = qr.GetVal<std::vector<std::string> >("count", 1);
  EXPECT_EQ("elephant", animal);
  EXPECT_EQ(1000, weight);
  EXPECT_DOUBLE_EQ(4.2, height);
  EXPECT_FALSE(alive);
  EXPECT_EQ("a very large mammal", data.str());
  ASSERT_EQ(2, vget.size());
  EXPECT_EQ(4, vget[0]);
  EXPECT_EQ(3, vget[1]);
  ASSERT_EQ(2, svget.size());
  EXPECT_EQ("one", svget[0]);
  EXPECT_EQ("three", svget[1]);
}
