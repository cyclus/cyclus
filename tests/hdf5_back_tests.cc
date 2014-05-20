#include <string>

#include <gtest/gtest.h>

#include "blob.h"
#include "error.h"
#include "hdf5.h"
#include "hdf5_back.h"
#include "hdf5_hl.h"

#include "boost/filesystem.hpp"

static const char* path = "testdb.h5";

class FileDeleter {
 public:
  FileDeleter(const char* path) : path_(path) {
    // if (!boost::filesystem::exists(path_))
    //   throw cyclus::IOError(std::string(path_) + " not found");
  }
  ~FileDeleter() {
    remove(path_);
  }
  const char* path_;
};

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
