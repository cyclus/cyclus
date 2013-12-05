#include <gtest/gtest.h>

#include "csv_back.h"
#include "blob.h"

#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <streambuf>
#include <string>
#include <cerrno>

static std::string const path = "testdb.csv";
namespace fs = boost::filesystem;

class DirDel {
 public:
  DirDel(std::string path) : path_(path) {};
  ~DirDel() {
    fs::remove_all(path);
  };
 private:
  std::string path_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CsvBackTest, ReadWrite) {
  DirDel dd(path);

  cyclus::EventManager m;
  cyclus::CsvBack back(path);
  m.RegisterBackend(&back);
  m.NewEvent("DumbTitle")
  ->AddVal("animal", std::string("monkey"))
  ->AddVal("weight", 10)
  ->AddVal("height", 5.5)
  ->Record();
  m.NewEvent("DumbTitle")
  ->AddVal("animal", std::string("elephant"))
  ->AddVal("weight", 1000)
  ->AddVal("height", 7.2)
  ->Record();
  m.close();

  // make sure append works
  cyclus::EventManager m2;
  cyclus::CsvBack back2(path);
  m2.RegisterBackend(&back2);
  m2.NewEvent("DumbTitle")
  ->AddVal("animal", std::string("sea cucumber"))
  ->AddVal("weight", 1)
  ->AddVal("height", .4)
  ->Record();
  m2.close();

  std::string sid1 = boost::lexical_cast<std::string>(m.sim_id());
  std::string sid2 = boost::lexical_cast<std::string>(m2.sim_id());
  std::vector<std::string> lines(4);
  lines[0] = "SimID, animal, weight, height";
  lines[1] = "\"" + sid1 + "\", \"monkey\", 10, 5.5";
  lines[2] = "\"" + sid1 + "\", \"elephant\", 1000, 7.2";
  lines[3] = "\"" + sid2 + "\", \"sea cucumber\", 1, 0.4";

  std::string fname = path + "/" + "DumbTitle.csv";
  std::ifstream file(fname.c_str());
  std::string str;
  int count = 0;
  while (std::getline(file, str)) {
    EXPECT_EQ(lines[count], str);
    ++count;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string ReadAll(std::string fname) {
  std::ifstream in(fname.c_str(), std::ios::in | std::ios::binary);
  if (in) {
    return (std::string((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>()));
  }
  throw (errno);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CsvBackTest, Blob) {
  using cyclus::EventManager;
  using cyclus::CsvBack;
  DirDel dd(path);

  cyclus::Blob data("my name is flipper");

  EventManager m;
  CsvBack back(path);
  m.RegisterBackend(&back);
  m.NewEvent("Blobs")
  ->AddVal("data", data)
  ->Record();
  m.close();

  std::string fname = path + "/" + "Blobs.csv";
  std::ifstream file(fname.c_str());
  std::string str;
  int count = 0;
  std::string rdata;
  while (std::getline(file, str)) {
    if (count == 1) {
      std::vector<std::string> fields;
      boost::split(fields, str, boost::is_any_of(","));
      EXPECT_EQ(fields.size(), 2) << "wrong number of fields";
      boost::trim(fields[1]);
      boost::trim_if(fields[1], boost::is_any_of("\""));
      rdata = ReadAll(path + "/" + fields[1]);
    }
    ++count;
  }
  EXPECT_EQ(data.str(), rdata);
}

