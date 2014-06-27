#include "boost/lexical_cast.hpp"
#include <boost/uuid/uuid_io.hpp>
#include <gtest/gtest.h>

#include "blob.h"
#include "sqlite_back.h"

#include "tools.h"

static std::string const path = "testdb.sqlite";

class SqliteBackTests : public ::testing::Test {
 public:
  virtual void SetUp() {
    remove(path.c_str());
    b = new cyclus::SqliteBack(path);
    r.RegisterBackend(b);
  }

  virtual void TearDown() {
    r.Close();
    delete b;
    remove(path.c_str());
  }
  cyclus::SqliteBack* b;
  cyclus::Recorder r;
};

TEST_F(SqliteBackTests, MapStrDouble) {
  std::map<std::string, double> m;
  m["one"] = 1.1;
  m["two"] = 2.2;
  m["three"] = 5.5;

  r.NewDatum("monty")
      ->AddVal("count", m)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  m = qr.GetVal<std::map<std::string, double> >("count", 0);

  EXPECT_EQ(1.1, m["one"]);
  EXPECT_EQ(2.2, m["two"]);
  EXPECT_EQ(5.5, m["three"]);
}

TEST_F(SqliteBackTests, MapStrInt) {
  std::map<std::string, int> m;
  m["one"] = 1;
  m["two"] = 2;
  m["three"] = 5;

  r.NewDatum("monty")
      ->AddVal("count", m)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  m = qr.GetVal<std::map<std::string, int> >("count", 0);

  EXPECT_EQ(1, m["one"]);
  EXPECT_EQ(2, m["two"]);
  EXPECT_EQ(5, m["three"]);
}

TEST_F(SqliteBackTests, MapStrStr) {
  std::map<std::string, std::string> m;
  m["one"] = "1";
  m["two"] = "2";
  m["three"] = "5";

  r.NewDatum("monty")
      ->AddVal("count", m)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  m = qr.GetVal<std::map<std::string, std::string> >("count", 0);

  EXPECT_EQ("1", m["one"]);
  EXPECT_EQ("2", m["two"]);
  EXPECT_EQ("5", m["three"]);
}

TEST_F(SqliteBackTests, MapIntDouble) {
  std::map<int, double> m;
  m[1] = 1.1;
  m[2] = 2.2;
  m[3] = 5.5;

  r.NewDatum("monty")
      ->AddVal("count", m)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  m = qr.GetVal<std::map<int, double> >("count", 0);

  EXPECT_EQ(1.1, m[1]);
  EXPECT_EQ(2.2, m[2]);
  EXPECT_EQ(5.5, m[3]);
}

TEST_F(SqliteBackTests, MapIntInt) {
  std::map<int, int> m;
  m[1] = 1;
  m[2] = 2;
  m[3] = 5;

  r.NewDatum("monty")
      ->AddVal("count", m)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  m = qr.GetVal<std::map<int, int> >("count", 0);

  EXPECT_EQ(1, m[1]);
  EXPECT_EQ(2, m[2]);
  EXPECT_EQ(5, m[3]);
}

TEST_F(SqliteBackTests, MapIntStr) {
  std::map<int, std::string> m;
  m[1] = "one";
  m[2] = "two";
  m[3] = "five";

  r.NewDatum("monty")
      ->AddVal("count", m)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  m = qr.GetVal<std::map<int, std::string> >("count", 0);

  EXPECT_EQ("one", m[1]);
  EXPECT_EQ("two", m[2]);
  EXPECT_EQ("five", m[3]);
}

TEST_F(SqliteBackTests, SetInt) {
  std::set<int> s;
  s.insert(4);
  s.insert(2);

  r.NewDatum("foo")
      ->AddVal("bar", s)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("foo", NULL);
  s = qr.GetVal<std::set<int> >("bar", 0);

  EXPECT_EQ(1, s.count(4));
  EXPECT_EQ(1, s.count(2));
}

TEST_F(SqliteBackTests, SetString) {
  std::set<std::string> s;
  s.insert("four");
  s.insert("two");

  r.NewDatum("foo")
      ->AddVal("bar", s)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("foo", NULL);
  s = qr.GetVal<std::set<std::string> >("bar", 0);

  EXPECT_EQ(1, s.count("four"));
  EXPECT_EQ(1, s.count("two"));
}

TEST_F(SqliteBackTests, ListInt) {
  std::list<int> l;
  l.push_back(4);
  l.push_back(2);

  r.NewDatum("foo")
      ->AddVal("bar", l)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("foo", NULL);
  l = qr.GetVal<std::list<int> >("bar", 0);

  ASSERT_EQ(2, l.size());
  EXPECT_EQ(4, l.front());
  EXPECT_EQ(2, l.back());
}

TEST_F(SqliteBackTests, ListString) {
  std::vector<std::string> l;
  l.push_back("four");
  l.push_back("two");


  r.NewDatum("foo")
      ->AddVal("bar", l)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("foo", NULL);
  l = qr.GetVal<std::vector<std::string> >("bar", 0);

  ASSERT_EQ(2, l.size());
  EXPECT_EQ("four", l.front());
  EXPECT_EQ("two", l.back());
}

TEST_F(SqliteBackTests, VectorInt) {
  std::vector<int> vect;
  vect.push_back(4);
  vect.push_back(2);

  r.NewDatum("foo")
      ->AddVal("bar", vect)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("foo", NULL);
  vect = qr.GetVal<std::vector<int> >("bar", 0);

  ASSERT_EQ(2, vect.size());
  EXPECT_EQ(4, vect[0]);
  EXPECT_EQ(2, vect[1]);
}

TEST_F(SqliteBackTests, VectorString) {
  std::vector<std::string> vect;
  vect.push_back("four");
  vect.push_back("two");


  r.NewDatum("foo")
      ->AddVal("bar", vect)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("foo", NULL);
  vect = qr.GetVal<std::vector<std::string> >("bar", 0);

  ASSERT_EQ(2, vect.size());
  EXPECT_EQ("four", vect[0]);
  EXPECT_EQ("two", vect[1]);
}

TEST_F(SqliteBackTests, AllTogether) {
  std::vector<int> vect;
  vect.push_back(4);
  vect.push_back(2);
  std::vector<std::string> svect;
  svect.push_back("one");
  svect.push_back("two");

  r.NewDatum("DumbTitle")
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

  r.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("elephant"))
      ->AddVal("weight", 1000)
      ->AddVal("height", 4.2)
      ->AddVal("alive", false)
      ->AddVal("answer", vect)
      ->AddVal("count", svect)
      ->AddVal("data", cyclus::Blob("a very large mammal"))
      ->Record();

  r.Close();

  cyclus::QueryResult qr = b->Query("DumbTitle", NULL);

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
