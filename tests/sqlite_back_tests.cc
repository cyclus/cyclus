#include "boost/lexical_cast.hpp"
#include <boost/uuid/uuid_io.hpp>
#include <gtest/gtest.h>

#include "blob.h"
#include "sqlite_back.h"

static std::string const path = "testdb.sqlite";

class FileDeleter {
 public:
  FileDeleter(std::string path) {
    path_ = path;
  }

  ~FileDeleter() {
    remove(path_.c_str());
  }

 private:
  std::string path_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(SqliteBackTest, Regression) {
  using cyclus::Recorder;
  FileDeleter fd(path);
  Recorder m;
  cyclus::SqliteBack back(path);
  m.RegisterBackend(&back);

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("monkey"))
      ->AddVal("weight", 10)
      ->AddVal("height", 5.5)
      ->AddVal("data", cyclus::Blob("banana"))
      ->Record();

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("elephant"))
      ->AddVal("weight", 1000)
      ->Record();

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("sea cucumber"))
      ->AddVal("height", 1.2)
      ->Record();

  m.Close();

  cyclus::QueryResult qr = back.Query("DumbTitle", NULL);

  std::string animal = qr.GetVal<std::string>("animal", 0);
  int weight = qr.GetVal<int>("weight", 0);
  double height = qr.GetVal<double>("height", 0);
  cyclus::Blob data = qr.GetVal<cyclus::Blob>("data", 0);
  EXPECT_EQ("monkey", animal);
  EXPECT_EQ(10, weight);
  EXPECT_EQ(5.5, height);
  EXPECT_EQ("banana", data.str());

  animal = qr.GetVal<std::string>("animal", 1);
  weight = qr.GetVal<int>("weight", 1);
  EXPECT_EQ("elephant", animal);
  EXPECT_EQ(1000, weight);

  animal = qr.GetVal<std::string>("animal", 2);
  height = qr.GetVal<double>("height", 2);
  EXPECT_EQ("sea cucumber", animal);
  EXPECT_EQ(1.2, height);
}
