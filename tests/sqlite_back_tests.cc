#include "boost/lexical_cast.hpp"
#include <boost/uuid/uuid_io.hpp>
#include <gtest/gtest.h>

#include "blob.h"
#include "sqlite_back.h"

#include "tools.h"

// special name to tell sqlite to use in-mem db
static std::string const path = ":memory:";

class SqliteBackTests : public ::testing::Test {
 public:
  virtual void SetUp() {
    b = new cyclus::SqliteBack(path);
    r.RegisterBackend(b);
  }

  virtual void TearDown() {
    r.Close();
    delete b;
  }
  cyclus::SqliteBack* b;
  cyclus::Recorder r;
};

// For future types, since the code generation and serialization has been
// sufficiently proven with existing tests, just use the following template -
// only adjusting the typedef at the top:
//
//     TEST_F(SqliteBackTests, MapStrDouble) {
//       typedef std::map<std::string, std::map<int, std::vector<int> > > Foo;
//     
//       Foo f;
//       r.NewDatum("monty")
//           ->AddVal("python", f)
//           ->Record();
//       r.Close();
//       cyclus::QueryResult qr = b->Query("monty", NULL);
//       EXPECT_NO_THROW(f = qr.GetVal<Foo>("python"));
//     }

TEST_F(SqliteBackTests, VecPairPairDoubleDoubleMapStringDouble) {
  typedef std::vector<std::pair<std::pair<double, double>, std::map<std::string, double> > > Foo;
  
  Foo f;
  r.NewDatum("monty")
    ->AddVal("python", f)
    ->Record();
  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  EXPECT_NO_THROW(f = qr.GetVal<Foo>("python"));
}
TEST_F(SqliteBackTests, MapStrMapStrInt) {
  typedef std::map<std::string, std::map<std::string, int > > Foo;
  
  Foo f;
  r.NewDatum("monty")
    ->AddVal("python", f)
    ->Record();
  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  EXPECT_NO_THROW(f = qr.GetVal<Foo>("python"));
}

TEST_F(SqliteBackTests, MapPairStrStrInt) {
  typedef std::map< std::pair<std::string, std::string >, int > Foo;
  
  Foo f;
  r.NewDatum("monty")
    ->AddVal("python", f)
    ->Record();
  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  EXPECT_NO_THROW(f = qr.GetVal<Foo>("python"));
}

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

TEST_F(SqliteBackTests, MapIntMapStringDouble) {
  std::map<int, std::map<std::string, double> > exp;
  std::map<std::string, double> expa;
  expa["foo"] = 4.2;
  expa["bar"] = 5.2;
  exp[42] = expa;
  std::map<std::string, double> expb;
  expb["foo"] = 4.9;
  expb["baz"] = 5.1;
  exp[65] = expb;

  r.NewDatum("monty")->AddVal("count", exp)->Record();
  r.Close();

  cyclus::QueryResult qr = b->Query("monty", NULL);
  std::map<int, std::map<std::string, double> > obs;
    obs = qr.GetVal<
      std::map<int, std::map<std::string, double> > >("count", 0);
    EXPECT_EQ(obs, exp);
}

TEST_F(SqliteBackTests, MapStrPairDoubleMapIntDouble) {
  std::map<std::string, std::pair<double, std::map<int, double> > > m;
  std::map<int, double> ma;
  ma[1] = 1.1;
  ma[2] = 2.2;
  std::map<int, double> mb;
  mb[1] = 1.2;
  mb[3] = 3.3;

  m["one"] = std::make_pair(.41, ma);
  m["two"] = std::make_pair(.42, mb);
  r.NewDatum("monty")->AddVal("count", m)->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  std::map<std::string, std::pair<double, std::map<int, double> > > mnew;
  mnew = qr.GetVal<
      std::map<std::string, std::pair<double, std::map<int, double> > > >(
      "count", 0);

  EXPECT_EQ(.41, mnew["one"].first);
  EXPECT_EQ(1.1, mnew["one"].second[1]);
  EXPECT_EQ(2.2, mnew["one"].second[2]);
  EXPECT_EQ(.42, mnew["two"].first);
  EXPECT_EQ(1.2, mnew["two"].second[1]);
  EXPECT_EQ(3.3, mnew["two"].second[3]);
}

TEST_F(SqliteBackTests, MAP_STRING_VECTOR_PAIR_INT_PAIR_STRING_STRING) {
  std::map<std::string,
      std::vector<std::pair<int, std::pair<std::string, std::string> > > > exp;
  
  std::vector<std::pair<int, std::pair<std::string, std::string> > > va;
  va.push_back(std::make_pair(42, std::make_pair("foo", "bar")));
  va.push_back(std::make_pair(43, std::make_pair("foo", "baz")));
  exp["hi"] = va;
  std::vector<std::pair<int, std::pair<std::string, std::string> > > vb;
  vb.push_back(std::make_pair(-3, std::make_pair("baz", "bar")));
  vb.push_back(std::make_pair(-4, std::make_pair("baz", "foo")));
  exp["mom"] = vb;
  
  r.NewDatum("monty")->AddVal("count", exp)->Record();
  r.Close();

  cyclus::QueryResult qr = b->Query("monty", NULL);
  std::map<std::string,
      std::vector<
        std::pair<int, std::pair<std::string, std::string> > > > obs;
  obs = qr.GetVal<
    std::map<std::string,
      std::vector<
        std::pair<int, std::pair<std::string, std::string> > > > >("count", 0);
  EXPECT_EQ(obs, exp);
}

TEST_F(SqliteBackTests, MapStrVectorDouble) {
  std::map<std::string, std::vector<double> > m;
  std::vector<double> av;
  av.push_back(1.1);
  av.push_back(3.3);
  std::vector<double> bv;
  bv.push_back(2.2);
  bv.push_back(4.4);
  bv.push_back(6.6);
  m["one"] = av;
  m["two"] = bv;

  r.NewDatum("monty")
      ->AddVal("count", m)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  std::map<std::string, std::vector<double> > mnew;
  mnew = qr.GetVal<std::map<std::string, std::vector<double> > >("count", 0);

  EXPECT_EQ(1.1, mnew["one"][0]);
  EXPECT_EQ(3.3, mnew["one"][1]);
  EXPECT_EQ(2.2, mnew["two"][0]);
  EXPECT_EQ(4.4, mnew["two"][1]);
  EXPECT_EQ(6.6, mnew["two"][2]);
}

TEST_F(SqliteBackTests, MapStrMapIntDouble) {
  std::map<std::string, std::map<int, double> > m;
  std::map<int, double> ma;
  ma[11] = 1.1;
  ma[33] = 3.3;
  std::map<int, double> mb;
  mb[22] = 2.2;
  mb[44] = 4.4;
  mb[66] = 6.6;
  m["one"] = ma;
  m["two"] = mb;

  r.NewDatum("monty")
      ->AddVal("count", m)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("monty", NULL);
  std::map<std::string, std::map<int, double> > mnew;
  mnew = qr.GetVal<std::map<std::string, std::map<int, double> > >("count", 0);

  EXPECT_EQ(1.1, mnew["one"][11]);
  EXPECT_EQ(3.3, mnew["one"][33]);
  EXPECT_EQ(2.2, mnew["two"][22]);
  EXPECT_EQ(4.4, mnew["two"][44]);
  EXPECT_EQ(6.6, mnew["two"][66]);
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

TEST_F(SqliteBackTests, VectorDouble) {
  std::vector<double> vect;
  vect.push_back(4.4);
  vect.push_back(2.2);

  r.NewDatum("foo")
      ->AddVal("bar", vect)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("foo", NULL);
  vect = qr.GetVal<std::vector<double> >("bar", 0);

  ASSERT_EQ(2, vect.size());
  EXPECT_DOUBLE_EQ(4.4, vect[0]);
  EXPECT_DOUBLE_EQ(2.2, vect[1]);
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

TEST_F(SqliteBackTests, ColumnTypes) {
  using std::map;
  using std::string;
  FileDeleter fd(path);

  int i = 42;

  // creation
  r.NewDatum("IntTable")
      ->AddVal("intcol", i)
      ->Record();
  r.Close();

  map<string, cyclus::DbTypes> coltypes = b->ColumnTypes("IntTable");
  EXPECT_EQ(2, coltypes.size());  // injects simid
  EXPECT_EQ(cyclus::INT, coltypes["intcol"]);
}

TEST_F(SqliteBackTests, Tables) {
  using std::set;
  using std::string;
  FileDeleter fd(path);

  int i = 42;

  // creation
  r.NewDatum("IntTable")
      ->AddVal("intcol", i)
      ->Record();
  r.Close();

  set<string> tabs = b->Tables();
  EXPECT_LE(1, tabs.size());
  EXPECT_EQ(1, tabs.count("IntTable"));
}

TEST_F(SqliteBackTests, ListPairIntInt) {
  std::list<std::pair<int, int> > l;
  l.push_back(std::make_pair(4, 2));
  l.push_back(std::make_pair(5, 3));

  r.NewDatum("foo")
      ->AddVal("bar", l)
      ->Record();

  r.Close();
  cyclus::QueryResult qr = b->Query("foo", NULL);
  l = qr.GetVal<std::list<std::pair<int, int> > >("bar", 0);

  ASSERT_EQ(2, l.size());
  EXPECT_EQ(std::make_pair(4, 2), l.front());
  EXPECT_EQ(std::make_pair(5, 3), l.back());
}
