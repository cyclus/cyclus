#include <string>

#include <gtest/gtest.h>

#include "boost/filesystem.hpp"
#include "hdf5.h"
#include "hdf5_hl.h"

#include "blob.h"
#include "error.h"
#include "hdf5_back.h"

#include "tools.h"

static const char* path = "testdb.h5";

class Hdf5GlobalEnv : public ::testing::Environment {
 public:
  virtual void SetUp() {
    path = "db.h5";
    db = new cyclus::Hdf5Back(path.c_str());
  }

  virtual void TearDown() {
    delete db;
    remove(path.c_str());
  }

  std::string path;
  cyclus::Hdf5Back* db;
};

Hdf5GlobalEnv* const hdf5_glb_env = new Hdf5GlobalEnv;
::testing::Environment* const hdf5_env = \
  ::testing::AddGlobalTestEnvironment(hdf5_glb_env);

class Hdf5BackTests : public ::testing::Test {
 public:
  virtual void SetUp() {
    path = hdf5_glb_env->path;
    db = hdf5_glb_env->db;
    rec.RegisterBackend(db);
  }

  virtual void TearDown() {
    rec.Close();
  }

  //
  // Test helpers
  //
  template <typename T>
  inline void ResultBasic(std::string title, const T& x, const T& y) {
    std::vector<int>* shape_ptr;
    if (shape.empty())
      shape_ptr = NULL;
    else
      shape_ptr = &shape;
    rec.NewDatum(title)
    ->AddVal("vals", x, shape_ptr)
    ->Record();
    rec.NewDatum(title)
    ->AddVal("vals", y, shape_ptr)
    ->Record();
    rec.Flush();
    qr = db->Query(title, NULL);
  }

  template <typename T>
  inline void TestBasic(std::string title, const T& x, const T& y) {
    ResultBasic<T>(title, x, y);
    T obsx = qr.GetVal<T>("vals", 0);
    EXPECT_EQ(x, obsx);
    T obsy = qr.GetVal<T>("vals", 1);
    EXPECT_EQ(y, obsy);
  }

  void TestBasicString(std::string title, std::string x, std::string y) {
    ResultBasic<std::string>(title, x, y);
    std::string obsx = qr.GetVal<std::string>("vals", 0);
    EXPECT_STREQ(x.c_str(), obsx.c_str());
    std::string obsy = qr.GetVal<std::string>("vals", 1);
    EXPECT_STREQ(y.c_str(), obsy.c_str());
  }

  void TestBasicBlob(std::string title, cyclus::Blob x, cyclus::Blob y) {
    using cyclus::Blob;
    ResultBasic<Blob>(title, x, y);
    Blob obsx = qr.GetVal<Blob>("vals", 0);
    EXPECT_STREQ(x.str().c_str(), obsx.str().c_str());
    Blob obsy = qr.GetVal<Blob>("vals", 1);
    EXPECT_STREQ(y.str().c_str(), obsy.str().c_str());
  }


  std::string path;
  cyclus::Hdf5Back* db;
  std::vector<int> shape;
  cyclus::QueryResult qr;
  cyclus::RecBackend::Deleter bdel;
  cyclus::Recorder rec;
};

//
// Actual unit tests
//

TEST_F(Hdf5BackTests, ShapeSegfault) {
  // this test should not segfault
  cyclus::Recorder r;
  FileDeleter fd("segfault.h5");
  cyclus::Hdf5Back b("segfault.h5");
  r.RegisterBackend(&b);
  std::vector<int>* shape = new std::vector<int>();
  shape->push_back(1);
  std::vector<int> foo;
  foo.push_back(42);

  r.NewDatum("bar")
  ->AddVal("foo", foo, shape)
  ->Record();

  memset(shape, '-', 1);

  r.Close();
}

TEST_F(Hdf5BackTests, ReadWriteBool) {
  shape.clear();
  TestBasic<bool>("bool", true, false);
}

TEST_F(Hdf5BackTests, ReadWriteInt) {
  shape.clear();
  TestBasic<int>("int", 42, 43);
}

TEST_F(Hdf5BackTests, ReadWriteFloat) {
  shape.clear();
  TestBasic<float>("float", 42.0, 43.0);
}

TEST_F(Hdf5BackTests, ReadWriteDouble) {
  shape.clear();
  TestBasic<double>("double", 42.0, 43.0);
}

TEST_F(Hdf5BackTests, ReadWriteString) {
  shape.resize(1);
  shape[0] = 6;
  TestBasicString("string", "wakka", "jawaka");
}

TEST_F(Hdf5BackTests, ReadWriteVLString) {
  shape.clear();
  TestBasicString("vl_string", "wakka", "jawaka");
}

TEST_F(Hdf5BackTests, ReadWriteBlob) {
  using cyclus::Blob;
  shape.clear();
  TestBasicBlob("blob", Blob("wakka"), Blob("jawaka"));
}

TEST_F(Hdf5BackTests, ReadWriteUuid) {
  using boost::uuids::uuid;
  shape.clear();
  uuid x = {0x12 ,0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56,
            0x78, 0x90, 0xab, 0xcd, 0xef};
  uuid y = {0x42 ,0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
            0x42, 0x42, 0x42, 0x42, 0x42};
  TestBasic<uuid>("uuid", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVectorInt) {
  using std::vector;
  shape.resize(1);
  shape[0] = 3;
  int x_[] = {6, 28, 496};
  vector<int> x = vector<int>(x_, x_+3);
  int y_[] = {42, 43, 44};
  vector<int> y = vector<int>(y_, y_+3);
  TestBasic<vector<int> >("vector_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLVectorInt) {
  using std::vector;
  int x_[] = {6, 28, 496, 8128};
  vector<int> x = vector<int>(x_, x_+4);
  vector<int> y = vector<int>(42);
  for (int i = 0; i < 42; ++i)
    y[i] = 42;
  TestBasic<vector<int> >("vl_vector_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVectorFloat) {
  using std::vector;
  shape.resize(1);
  shape[0] = 3;
  float x_[] = {6.0, 28.0, 496.0};
  vector<float> x = vector<float>(x_, x_+3);
  float y_[] = {42.0, 43.0, 44.0};
  vector<float> y = vector<float>(y_, y_+3);
  TestBasic<vector<float> >("vector_float", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLVectorFloat) {
  using std::vector;
  shape.clear();
  float x_[] = {6.0, 28.0, 496.0, 8128.0};
  vector<float> x = vector<float>(x_, x_+4);
  vector<float> y = vector<float>(42);
  for (int i = 0; i < 42; ++i)
    y[i] = 42.0 + i;
  TestBasic<vector<float> >("vl_vector_float", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVectorDouble) {
  using std::vector;
  shape.resize(1);
  shape[0] = 3;
  double x_[] = {6.0, 28.0, 496.0};
  vector<double> x = vector<double>(x_, x_+3);
  double y_[] = {42.0, 43.0, 44.0};
  vector<double> y = vector<double>(y_, y_+3);
  TestBasic<vector<double> >("vector_double", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLVectorDouble) {
  using std::vector;
  shape.clear();
  double x_[] = {6.0, 28.0, 496.0, 8128.0};
  vector<double> x = vector<double>(x_, x_+4);
  vector<double> y = vector<double>(42);
  for (int i = 0; i < 42; ++i)
    y[i] = 42.0 + i;
  TestBasic<vector<double> >("vl_vector_double", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVectorString) {
  using std::string;
  using std::vector;
  shape.resize(2);
  shape[0] = 2;
  shape[1] = 6;
  string x_[] = {"wakka", "jawaka"};
  vector<string> x = vector<string>(x_, x_+2);
  string y_[] = {"Frank", "Zappa"};
  vector<string> y = vector<string>(y_, y_+2);
  TestBasic<vector<string> >("vector_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVectorVLString) {
  using std::string;
  using std::vector;
  shape.resize(2);
  shape[0] = 3;
  shape[1] = -1;
  string x_[] = {"wakka", "jawaka", "Hot Rats"};
  vector<string> x = vector<string>(x_, x_+3);
  string y_[] = {"Frank", "Zappa", "It Just Might Be a One‐Shot Deal"};
  vector<string> y = vector<string>(y_, y_+3);
  TestBasic<vector<string> >("vector_vl_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLVectorString) {
  using std::string;
  using std::vector;
  shape.resize(2);
  shape[0] = -1;
  shape[1] = 10;
  string x_[] = {"wakka", "jawaka", "Hot Rats"};
  vector<string> x = vector<string>(x_, x_+3);
  string y_[] = {"One‐Shot"};
  vector<string> y = vector<string>(y_, y_+1);
  TestBasic<vector<string> >("vl_vector_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLVectorVLString) {
  using std::string;
  using std::vector;
  shape.clear();
  string x_[] = {"wakka", "jawaka", "Hot Rats"};
  vector<string> x = vector<string>(x_, x_+3);
  string y_[] = {"Frank", "Zappa", "It", "Just", "Might", "Be", "a", "One‐Shot", "Deal"};
  vector<string> y = vector<string>(y_, y_+9);
  TestBasic<vector<string> >("vl_vector_vl_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteSetInt) {
  using std::set;
  shape.resize(1);
  shape[0] = 3;
  int x_[] = {6, 28, 496};
  set<int> x = set<int>(x_, x_+3);
  int y_[] = {42, 43, 44};
  set<int> y = set<int>(y_, y_+3);
  TestBasic<set<int> >("set_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLSetInt) {
  using std::set;
  shape.clear();
  int x_[] = {6, 28, 496, 8128};
  set<int> x = set<int>(x_, x_+4);
  set<int> y = set<int>();
  for (int i = 0; i < 42; ++i)
    y.insert(42 + i);
  TestBasic<set<int> >("vl_set_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteSetString) {
  using std::string;
  using std::vector;
  using std::set;
  shape.resize(2);
  shape[0] = 2;
  shape[1] = 6;
  string x_[] = {"wakka", "jawaka"};
  set<string> x = set<string>(x_, x_+2);
  string y_[] = {"Frank", "Zappa"};
  set<string> y = set<string>(y_, y_+2);
  TestBasic<set<string> >("set_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteSetVLString) {
  using std::string;
  using std::vector;
  using std::set;
  shape.resize(2);
  shape[0] = 3;
  shape[1] = -1;
  string x_[] = {"wakka", "jawaka", "Hot Rats"};
  set<string> x = set<string>(x_, x_+3);
  string y_[] = {"Frank", "Zappa", "It Just Might Be a One‐Shot Deal"};
  set<string> y = set<string>(y_, y_+3);
  TestBasic<set<string> >("set_vl_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLSetString) {
  using std::string;
  using std::vector;
  using std::set;
  shape.resize(2);
  shape[0] = -1;
  shape[1] = 10;
  string x_[] = {"wakka", "jawaka", "Hot Rats"};
  set<string> x = set<string>(x_, x_+3);
  string y_[] = {"One‐Shot"};
  set<string> y = set<string>(y_, y_+1);
  TestBasic<set<string> >("vl_set_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLSetVLString) {
  using std::string;
  using std::vector;
  using std::set;
  shape.clear();
  string x_[] = {"wakka", "jawaka", "Hot Rats"};
  set<string> x = set<string>(x_, x_+3);
  string y_[] = {"Frank", "Zappa", "It", "Just", "Might", "Be", "a", "One‐Shot", "Deal"};
  set<string> y = set<string>(y_, y_+9);
  TestBasic<set<string> >("vl_set_vl_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteListInt) {
  using std::list;
  shape.resize(1);
  shape[0] = 3;
  int x_[] = {6, 28, 496};
  list<int> x = list<int>(x_, x_+3);
  int y_[] = {42, 43, 44};
  list<int> y = list<int>(y_, y_+3);
  TestBasic<list<int> >("list_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLListInt) {
  using std::list;
  shape.clear();
  int x_[] = {6, 28, 496, 8128};
  list<int> x = list<int>(x_, x_+4);
  list<int> y = list<int>();
  for (int i = 0; i < 42; ++i)
    y.push_back(42);
  TestBasic<list<int> >("vl_list_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteListString) {
  using std::string;
  using std::vector;
  using std::list;
  shape.resize(2);
  shape[0] = 2;
  shape[1] = 6;
  string x_[] = {"wakka", "jawaka"};
  list<string> x = list<string>(x_, x_+2);
  string y_[] = {"Frank", "Zappa"};
  list<string> y = list<string>(y_, y_+2);
  TestBasic<list<string> >("list_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteListVLString) {
  using std::string;
  using std::vector;
  using std::list;
  shape.resize(2);
  shape[0] = 3;
  shape[1] = -1;
  string x_[] = {"wakka", "jawaka", "Hot Rats"};
  list<string> x = list<string>(x_, x_+3);
  string y_[] = {"Frank", "Zappa", "It Just Might Be a One‐Shot Deal"};
  list<string> y = list<string>(y_, y_+3);
  TestBasic<list<string> >("list_vl_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLListString) {
  using std::string;
  using std::vector;
  using std::list;
  shape.resize(2);
  shape[0] = -1;
  shape[1] = 10;
  string x_[] = {"wakka", "jawaka", "Hot Rats"};
  list<string> x = list<string>(x_, x_+3);
  string y_[] = {"One‐Shot"};
  list<string> y = list<string>(y_, y_+1);
  TestBasic<list<string> >("vl_list_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLListVLString) {
  using std::string;
  using std::vector;
  using std::list;
  shape.clear();
  string x_[] = {"wakka", "jawaka", "Hot Rats"};
  list<string> x = list<string>(x_, x_+3);
  string y_[] = {"Frank", "Zappa", "It", "Just", "Might", "Be", "a", "One‐Shot", "Deal"};
  list<string> y = list<string>(y_, y_+9);
  TestBasic<list<string> >("vl_list_vl_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWritePairIntInt) {
  using std::pair;
  shape.clear();
  pair<int, int> x = pair<int, int>(6, 28);
  pair<int, int> y = pair<int, int>(42, 43);
  TestBasic<pair<int, int> >("pair_int_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapIntInt) {
  using std::map;
  shape.resize(1);
  shape[0] = 3;
  map<int, int> x = map<int, int>();
  x[6] = 42;
  x[28] = 43;
  x[496] = 44;
  map<int, int> y = map<int, int>();
  y[42] = 6;
  y[43] = 28;
  y[44] = 496;
  TestBasic<map<int, int> >("map_int_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapIntInt) {
  using std::map;
  shape.clear();
  map<int, int> x = map<int, int>();
  x[6] = 42;
  x[28] = 43;
  x[496] = 44;
  x[8128] = 45;
  map<int, int> y = map<int, int>();
  for (int i = 0; i < 42; ++i)
    y[42 + i] = i;
  TestBasic<map<int, int> >("vl_map_int_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapIntDouble) {
  using std::map;
  shape.resize(1);
  shape[0] = 3;
  map<int, double> x;
  x[6] = 42.0;
  x[28] = 43.0;
  x[496] = 44.0;
  map<int, double> y;
  y[42] = 6.0;
  y[43] = 28.0;
  y[44] = 496.0;
  TestBasic<map<int, double> >("map_int_double", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapIntDouble) {
  using std::map;
  shape.clear();
  map<int, double> x = map<int, double>();
  x[6] = 42.0;
  x[28] = 43.0;
  x[496] = 44.0;
  x[8128] = 45.0;
  map<int, double> y = map<int, double>();
  for (int i = 0; i < 42; ++i)
    y[42 + i] = 1.0*i;
  TestBasic<map<int, double> >("vl_map_int_double", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapIntString) {
  using std::map;
  using std::string;
  shape.resize(2);
  shape[0] = 3;
  shape[1] = 4;
  map<int, string> x;
  x[6] = "jazz";
  x[28] = "from";
  x[496] = "hell";
  map<int, string> y;
  y[42] = "help";
  y[43] = "imma";
  y[44] = "rock";
  TestBasic<map<int, string> >("map_int_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapIntString) {
  using std::map;
  using std::string;
  shape.resize(2);
  shape[0] = -1;
  shape[1] = 4;
  map<int, string> x;
  x[6] = "abso";
  x[28] = "lute";
  x[496] = "ly  ";
  x[8128] = "free";
  map<int, string> y = map<int, string>();
  for (int i = 0; i < 42; ++i)
    y[42 + i] = "Arf!";
  TestBasic<map<int, string> >("vl_map_int_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapIntVLString) {
  using std::map;
  using std::string;
  shape.resize(2);
  shape[0] = 3;
  shape[1] = -1;
  map<int, string> x;
  x[6] = "you";
  x[28] = "are";
  x[496] = "probably";
  map<int, string> y;
  y[42] = "wondering";
  y[43] = "why";
  y[44] = "I'm here!";
  TestBasic<map<int, string> >("map_int_vl_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapIntVLString) {
  using std::map;
  using std::string;
  shape.clear();
  map<int, string> x;
  x[6] = "I am lonesome";
  x[28] = "Cowboy Burt";
  map<int, string> y = map<int, string>();
  for (int i = 0; i < 42; ++i)
    y[42 + i] = "Come smell my friend's shirt";
  TestBasic<map<int, string> >("vl_map_int_vl_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapStringInt) {
  using std::map;
  using std::string;
  shape.resize(2);
  shape[0] = 3;
  shape[1] = 4;
  map<string, int> x;
  x["jazz"] = 6;
  x["from"] = 28;
  x["hell"] = 496;
  map<string, int> y;
  y["help"] = 42;
  y["imma"] = 43;
  y["rock"] = 44;
  TestBasic<map<string, int> >("map_string_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapStringInt) {
  using std::map;
  using std::string;
  shape.resize(2);
  shape[0] = -1;
  shape[1] = 4;
  map<string, int> x;
  x["abso"] = 6;
  x["lute"] = 28;
  x["ly  "] = 496;
  x["free"] = 8128;
  map<string, int> y;
  y["Arf!"] = 42;
  y["Arf?"] = 43;
  TestBasic<map<string, int> >("vl_map_string_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapVLStringInt) {
  using std::map;
  using std::string;
  shape.resize(2);
  shape[0] = 3;
  shape[1] = -1;
  map<string, int> x;
  x["you"] = 6;
  x["are"] = 28;
  x["probably"] = 496;
  map<string, int> y;
  y["wondering"] = 42;
  y["why"] = 43;
  y["I'm here!"] = 44;
  TestBasic<map<string, int> >("map_vl_string_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapVLStringInt) {
  using std::map;
  using std::string;
  shape.clear();
  map<string, int> x;
  x["I am lonesome"] = 6;
  x["Cowboy Burt"] = 28;
  map<string, int> y;
  y["Come smell my friend's shirt"] = 42;
  TestBasic<map<string, int> >("vl_map_vl_string_int", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapStringDouble) {
  using std::map;
  using std::string;
  shape.resize(2);
  shape[0] = 3;
  shape[1] = 4;
  map<string, double> x;
  x["jazz"] = 6.0;
  x["from"] = 28.0;
  x["hell"] = 496.0;
  map<string, double> y;
  y["help"] = 42.0;
  y["imma"] = 43.0;
  y["rock"] = 44.0;
  TestBasic<map<string, double> >("map_string_double", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapStringDouble) {
  using std::map;
  using std::string;
  shape.resize(2);
  shape[0] = -1;
  shape[1] = 4;
  map<string, double> x;
  x["abso"] = 6.0;
  x["lute"] = 28.0;
  x["ly  "] = 496.0;
  x["free"] = 8128.0;
  map<string, double> y;
  y["Arf!"] = 42.0;
  y["Arf?"] = 43.0;
  TestBasic<map<string, double> >("vl_map_string_double", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapVLStringDouble) {
  using std::map;
  using std::string;
  shape.resize(2);
  shape[0] = 3;
  shape[1] = -1;
  map<string, double> x;
  x["you"] = 6.0;
  x["are"] = 28.0;
  x["probably"] = 496.0;
  map<string, double> y;
  y["wondering"] = 42.0;
  y["why"] = 43.0;
  y["I'm here!"] = 44.0;
  TestBasic<map<string, double> >("map_vl_string_double", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapVLStringDouble) {
  using std::map;
  using std::string;
  shape.clear();
  map<string, double> x;
  x["I am lonesome"] = 6.0;
  x["Cowboy Burt"] = 28.0;
  map<string, double> y;
  y["Come smell my friend's shirt"] = 42.0;
  TestBasic<map<string, double> >("vl_map_vl_string_double", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapStringString) {
  using std::map;
  using std::string;
  shape.resize(3);
  shape[0] = 3;
  shape[1] = 4;
  shape[2] = 2;
  map<string, string> x;
  x["jazz"] = "ja";
  x["from"] = "wa";
  x["hell"] = "ka";
  map<string, string> y;
  y["help"] = "wa";
  y["imma"] = "kk";
  y["rock"] = "a!";
  TestBasic<map<string, string> >("map_string_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapStringString) {
  using std::map;
  using std::string;
  shape.resize(3);
  shape[0] = -1;
  shape[1] = 4;
  shape[2] = 2;
  map<string, string> x;
  x["abso"] = "jo";
  x["lute"] = "e&";
  x["ly  "] = "ma";
  x["free"] = "ry";
  map<string, string> y;
  y["Arf!"] = "sa";
  y["Arf?"] = "id";
  TestBasic<map<string, string> >("vl_map_string_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapStringVLString) {
  using std::map;
  using std::string;
  shape.resize(3);
  shape[0] = 3;
  shape[1] = 4;
  shape[2] = -1;
  map<string, string> x;
  x["jazz"] = "you";
  x["from"] = "are";
  x["hell"] = "probably";
  map<string, string> y;
  y["help"] = "wondering";
  y["imma"] = "why";
  y["rock"] = "I'm here!";
  TestBasic<map<string, string> >("map_string_vl_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapStringVLString) {
  using std::map;
  using std::string;
  shape.resize(3);
  shape[0] = -1;
  shape[1] = 4;
  shape[2] = -1;
  map<string, string> x;
  x["Arf!"] = "I am lonesome";
  x["Arf?"] = "Cowboy Burt";
  map<string, string> y;
  y["Arf~"] = "Come smell my friend's shirt";
  TestBasic<map<string, string> >("vl_map_string_vl_string", x, y);
}


TEST_F(Hdf5BackTests, ReadWriteMapVLStringString) {
  using std::map;
  using std::string;
  shape.resize(3);
  shape[0] = 3;
  shape[1] = -1;
  shape[2] = 2;
  map<string, string> x;
  x["jazzy"] = "ja";
  x["frommmmmmm"] = "wa";
  x["helllll"] = "ka";
  map<string, string> y;
  y["help"] = "wa";
  y["I'm a "] = "kk";
  y["rock!"] = "a!";
  TestBasic<map<string, string> >("map_vl_string_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapVLStringString) {
  using std::map;
  using std::string;
  shape.resize(3);
  shape[0] = -1;
  shape[1] = -1;
  shape[2] = 2;
  map<string, string> x;
  x["absoluetly"] = "jo";
  x["free"] = "e&";
  x["Freak"] = "ma";
  x["out"] = "ry";
  map<string, string> y;
  y["Joe's"] = "sa";
  y["Garage"] = "id";
  TestBasic<map<string, string> >("vl_map_vl_string_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteMapVLStringVLString) {
  using std::map;
  using std::string;
  shape.resize(3);
  shape[0] = 3;
  shape[1] = -1;
  shape[2] = -1;
  map<string, string> x;
  x["jazzy"] = "you";
  x["frommmmmmmm"] = "are";
  x["helllll"] = "probably";
  map<string, string> y;
  y["help"] = "wondering";
  y["I'm a "] = "why";
  y["rock!"] = "I'm here!";
  TestBasic<map<string, string> >("map_vl_string_vl_string", x, y);
}

TEST_F(Hdf5BackTests, ReadWriteVLMapVLStringVLString) {
  using std::map;
  using std::string;
  shape.clear();
  map<string, string> x;
  x["Arf!"] = "I am lonesome";
  x["She said"] = "Cowboy Burt";
  map<string, string> y;
  y["panchromatic"] = "Come smell my friend's shirt";
  TestBasic<map<string, string> >("vl_map_vl_string_vl_string", x, y);
}



//
// Multi-faceted unit tests
//

TEST(Hdf5BackTest, ReadWriteAll) {
  using std::vector;
  using std::string;
  using cyclus::Recorder;
  using cyclus::Hdf5Back;
  using cyclus::Cond;
  FileDeleter fd(path);

  int i = 42;
  double d = 2.2;
  float f = 3.3;
  std::string str = "apple";

  size_t strsize = str.size() + 1;
  size_t sizes[] = {16, sizeof(int), sizeof(float), sizeof(double)};
  size_t offsets[] = {0, sizes[0], sizes[0] + sizes[1], sizes[0] + sizes[1] +
                      sizes[2]};
  size_t size = sizes[0] + sizes[1] + sizes[2] + sizes[3];
  const char* field_names = "string,int,float,double";
  char buf[size];

  // creation
  Recorder m;
  Hdf5Back back(path);
  vector<int> string_shape = vector<int>(1);
  string_shape[0] = 16;
  m.RegisterBackend(&back);
  m.NewDatum("DumbTitle")
  ->AddVal("string", str, &string_shape)
  ->AddVal("int", i)
  ->AddVal("float", f)
  ->AddVal("double", d)
  ->Record();
  m.Close();

  // raw read
  hid_t file = H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT);
  herr_t status = H5TBread_fields_name(file, "DumbTitle", field_names, 0, 1, size,
                                       offsets, sizes, buf);

  int i2 = 0;
  float f2 = 0;
  double d2 = 0;
  char str2[strsize];
  memcpy(&str2, buf + offsets[0], strsize);
  memcpy(&i2, buf + offsets[1], sizes[1]);
  memcpy(&f2, buf + offsets[2], sizes[2]);
  memcpy(&d2, buf + offsets[3], sizes[3]);

  EXPECT_STREQ(str.c_str(), str2);
  EXPECT_EQ(i, i2);
  EXPECT_FLOAT_EQ(f, f2);
  EXPECT_DOUBLE_EQ(d, d2);

  H5Fclose(file);

  // query read
  string expfields[] = {"SimId", "string", "int", "float", "double"};
  cyclus::DbTypes exptypes[] = {cyclus::UUID, cyclus::STRING, cyclus::INT, 
                                cyclus::FLOAT, cyclus::DOUBLE};
  cyclus::QueryResult qr = back.Query("DumbTitle", NULL);
  for (int i = 0; i < qr.fields.size(); i++) {
    EXPECT_STREQ(qr.fields[i].c_str(), expfields[i].c_str());
    EXPECT_EQ(qr.types[i], exptypes[i]);
  }
  std::vector<Cond> conds = std::vector<Cond>();
  conds.push_back(Cond("int", "==", 42));
  qr = back.Query("DumbTitle", &conds);
  EXPECT_EQ(qr.rows.size(), 1);

  conds.push_back(Cond("int", ">=", 43));
  qr = back.Query("DumbTitle", &conds);
  EXPECT_EQ(qr.rows.size(), 0);

  conds[1] = Cond("string", "==", str);
  qr = back.Query("DumbTitle", &conds);
  EXPECT_EQ(qr.rows.size(), 1);
}
