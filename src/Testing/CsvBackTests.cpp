#include <gtest/gtest.h>

#include "CsvBack.h"
#include "boost/lexical_cast.hpp"
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

static std::string const path = "testdb.csv";
namespace fs = boost::filesystem;

class DirDel {
  public:
    DirDel(std::string path) : path_(path) { };
    ~DirDel() {fs::remove_all(path);};
  private:
    std::string path_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(CsvBackTest, Regression) {
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
  lines[1] = std::string("\"") + sid1 + std::string("\", \"monkey\", 10, 5.5");
  lines[2] = std::string("\"") + sid1 + std::string("\", \"elephant\", 1000, 7.2");
  lines[3] = std::string("\"") + sid2 + std::string("\", \"sea cucumber\", 1, 0.4");

  std::string fname = path + "/" + "DumbTitle.csv";
  std::ifstream file(fname.c_str());
  std::string str;
  int count = 0;
  while(std::getline(file, str)) {
    EXPECT_EQ(lines[count], str);
    ++count;
  }
}

