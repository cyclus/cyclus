#include <string>
#include <iostream>

#include <gtest/gtest.h>

#include "boost/filesystem.hpp"

#include "blob.h"
#include "error.h"
#include "hdf5.h"
#include "hdf5_back.h"
#include "hdf5_hl.h"
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
::testing::Environment* const hdf5_env =
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
    if (shape.empty()) {
      shape_ptr = NULL;
    } else {
      shape_ptr = &shape;
    }
    rec.NewDatum(title)
        ->AddVal("vals", x, shape_ptr)
        ->Record();
    rec.Flush();
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

TEST(Hdf5BackTest, ColumnTypes) {
  using std::map;
  using std::string;
  using cyclus::Recorder;
  using cyclus::Hdf5Back;
  FileDeleter fd(path);

  int i = 42;

  // creation
  Recorder m;
  Hdf5Back back(path);
  m.RegisterBackend(&back);
  m.NewDatum("IntTable")
      ->AddVal("intcol", i)
      ->Record();
  m.Close();

  map<string, cyclus::DbTypes> coltypes = back.ColumnTypes("IntTable");
  EXPECT_EQ(2, coltypes.size());  // injects simid
  EXPECT_EQ(cyclus::INT, coltypes["intcol"]);
}

TEST(Hdf5BackTest, Tables) {
  using std::set;
  using std::string;
  using cyclus::Recorder;
  using cyclus::Hdf5Back;
  FileDeleter fd(path);

  int i = 42;

  // creation
  Recorder m;
  Hdf5Back back(path);
  m.RegisterBackend(&back);
  m.NewDatum("IntTable")
      ->AddVal("intcol", i)
      ->Record();
  m.Close();

  set<string> tabs = back.Tables();
  EXPECT_LE(1, tabs.size());
  EXPECT_EQ(1, tabs.count("IntTable"));
}
