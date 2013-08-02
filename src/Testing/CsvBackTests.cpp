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
  DirDel(std::string path) : path_(path) { };
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
  m.registerBackend(&back);
  m.newEvent("DumbTitle")
  ->addVal("animal", std::string("monkey"))
  ->addVal("weight", 10)
  ->addVal("height", 5.5)
  ->record();
  m.newEvent("DumbTitle")
  ->addVal("animal", std::string("elephant"))
  ->addVal("weight", 1000)
  ->addVal("height", 7.2)
  ->record();
  m.close();

  // make sure append works
  cyclus::EventManager m2;
  cyclus::CsvBack back2(path);
  m2.registerBackend(&back2);
  m2.newEvent("DumbTitle")
  ->addVal("animal", std::string("sea cucumber"))
  ->addVal("weight", 1)
  ->addVal("height", .4)
  ->record();
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
  m.registerBackend(&back);
  m.newEvent("Blobs")
  ->addVal("data", data)
  ->record();
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

