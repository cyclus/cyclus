#include <string>

#include <gtest/gtest.h>

#include "blob.h"
#include "hdf5.h"
#include "hdf5_back.h"
#include "hdf5_hl.h"

static const char* path = "testdb.h5";

class FileDeleter {
 public:
  FileDeleter(const char* path) : path_(path) {}
  ~FileDeleter() {
    remove(path_);
  }
  const char* path_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(Hdf5BackTest, ReadWrite) {
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

//
// Test helpers
//

template <typename T>
cyclus::QueryResult Hdf5ReadWriteResultBasic(const char* fpath, T x, T y, 
                                             cyclus::Datum::Shape shape = NULL) {
  FileDeleter fd(fpath);
  cyclus::Recorder m;
  cyclus::Hdf5Back back(fpath);
  m.RegisterBackend(&back);
  m.NewDatum("data")
  ->AddVal("x", x, shape)
  ->Record();
  m.NewDatum("data")
  ->AddVal("x", y, shape)
  ->Record();
  m.Close();
  return back.Query("data", NULL);
}

template <typename T>
void Hdf5ReadWriteTestBasic(const char* fpath, T x, T y, 
                            cyclus::Datum::Shape shape = NULL) {
  cyclus::QueryResult qr = Hdf5ReadWriteResultBasic<T>(fpath, x, y, shape);
  T obsx = qr.GetVal<T>("x", 0);
  EXPECT_EQ(x, obsx);
  T obsy = qr.GetVal<T>("x", 1);
  EXPECT_EQ(y, obsy);
}

template <>
void Hdf5ReadWriteTestBasic<std::string>(const char* fpath, std::string x, std::string y, 
                                         cyclus::Datum::Shape shape) {
  cyclus::QueryResult qr = Hdf5ReadWriteResultBasic<std::string>(fpath, x, y, shape);
  std::string obsx = qr.GetVal<std::string>("x", 0);
  EXPECT_STREQ(x.c_str(), obsx.c_str());
  std::string obsy = qr.GetVal<std::string>("x", 1);
  EXPECT_STREQ(y.c_str(), obsy.c_str());
}

template <>
void Hdf5ReadWriteTestBasic<cyclus::Blob>(const char* fpath, cyclus::Blob x, 
                                          cyclus::Blob y, 
                                          cyclus::Datum::Shape shape) {
  using cyclus::Blob;
  cyclus::QueryResult qr = Hdf5ReadWriteResultBasic<Blob>(fpath, x, y, shape);
  Blob obsx = qr.GetVal<Blob>("x", 0);
  EXPECT_STREQ(x.str().c_str(), obsx.str().c_str());
  Blob obsy = qr.GetVal<Blob>("x", 1);
  EXPECT_STREQ(y.str().c_str(), obsy.str().c_str());
}

//
// Actual unit tests
//

TEST(Hdf5BackTest, ReadWriteBool) {
  Hdf5ReadWriteTestBasic<bool>("int.h5", true, false);
}

TEST(Hdf5BackTest, ReadWriteInt) {
  Hdf5ReadWriteTestBasic<int>("int.h5", 42, 43);
}

TEST(Hdf5BackTest, ReadWriteFloat) {
  Hdf5ReadWriteTestBasic<float>("float.h5", 42.0, 43.0);
}

TEST(Hdf5BackTest, ReadWriteDouble) {
  Hdf5ReadWriteTestBasic<double>("double.h5", 42.0, 43.0);
}

TEST(Hdf5BackTest, ReadWriteString) {
  std::vector<int> shape(1);
  shape[0] = 6;
  Hdf5ReadWriteTestBasic<std::string>("string.h5", "wakka", "jawaka", &shape);
}

TEST(Hdf5BackTest, ReadWriteVLString) {
  Hdf5ReadWriteTestBasic<std::string>("vlstring.h5", "wakka", "jawaka", NULL);
}

TEST(Hdf5BackTest, ReadWriteBlob) {
  using cyclus::Blob;
  Hdf5ReadWriteTestBasic<Blob>("blob.h5", Blob("wakka"), Blob("jawaka"), NULL);
}

TEST(Hdf5BackTest, ReadWriteUuid) {
  using boost::uuids::uuid;
  uuid x = {0x12 ,0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56,
            0x78, 0x90, 0xab, 0xcd, 0xef};
  uuid y = {0x42 ,0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
            0x42, 0x42, 0x42, 0x42, 0x42};
  Hdf5ReadWriteTestBasic<uuid>("uuid.h5", x, y);
}

TEST(Hdf5BackTest, ReadWriteVectorInt) {
  using std::vector;
  vector<int> shape(1);
  shape[0] = 3;
  int x_[] = {6, 28, 496};
  vector<int> x = vector<int>(x_, x_+3);
  int y_[] = {42, 43, 44};
  vector<int> y = vector<int>(y_, y_+3);
  Hdf5ReadWriteTestBasic<vector<int> >("vector_int.h5", x, y, &shape);
}

TEST(Hdf5BackTest, ReadWriteVLVectorInt) {
  using std::vector;
  int x_[] = {6, 28, 496, 8128};
  vector<int> x = vector<int>(x_, x_+4);
  vector<int> y = vector<int>(42);
  for (int i = 0; i < 42; ++i)
    y[i] = 42;
  Hdf5ReadWriteTestBasic<vector<int> >("vl_vector_int.h5", x, y);
}

TEST(Hdf5BackTest, ReadWriteVectorString) {
  using std::string;
  using std::vector;
  vector<int> shape(2);
  shape[0] = 2;
  shape[1] = 6;
  string x_[] = {"wakka", "jawaka"};
  vector<string> x = vector<string>(x_, x_+2);
  string y_[] = {"Frank", "Zappa"};
  vector<string> y = vector<string>(y_, y_+2);
  Hdf5ReadWriteTestBasic<vector<string> >("vector_string.h5", x, y, &shape);
}

TEST(Hdf5BackTest, ReadWriteVectorVLString) {
  using std::string;
  using std::vector;
  vector<int> shape(2);
  shape[0] = 3;
  shape[1] = -1;
  string x_[] = {"wakka", "jawaka", "Hot Rats"};
  vector<string> x = vector<string>(x_, x_+3);
  string y_[] = {"Frank", "Zappa", "It Just Might Be a One‐Shot Deal"};
  vector<string> y = vector<string>(y_, y_+3);
  Hdf5ReadWriteTestBasic<vector<string> >("vector_vl_string.h5", x, y, &shape);
}

TEST(Hdf5BackTest, ReadWriteSetInt) {
  using std::set;
  std::vector<int> shape(1);
  shape[0] = 3;
  int x_[] = {6, 28, 496};
  set<int> x = set<int>(x_, x_+3);
  int y_[] = {42, 43, 44};
  set<int> y = set<int>(y_, y_+3);
  Hdf5ReadWriteTestBasic<set<int> >("set_int.h5", x, y, &shape);
}

TEST(Hdf5BackTest, ReadWriteVLSetInt) {
  using std::set;
  int x_[] = {6, 28, 496, 8128};
  set<int> x = set<int>(x_, x_+4);
  set<int> y = set<int>();
  for (int i = 0; i < 42; ++i)
    y.insert(42 + i);
  Hdf5ReadWriteTestBasic<set<int> >("vl_set_int.h5", x, y);
}

TEST(Hdf5BackTest, ReadWriteListInt) {
  using std::list;
  std::vector<int> shape(1);
  shape[0] = 3;
  int x_[] = {6, 28, 496};
  list<int> x = list<int>(x_, x_+3);
  int y_[] = {42, 43, 44};
  list<int> y = list<int>(y_, y_+3);
  Hdf5ReadWriteTestBasic<list<int> >("list_int.h5", x, y, &shape);
}

TEST(Hdf5BackTest, ReadWriteVLListInt) {
  using std::list;
  int x_[] = {6, 28, 496, 8128};
  list<int> x = list<int>(x_, x_+4);
  list<int> y = list<int>();
  for (int i = 0; i < 42; ++i)
    y.push_back(42);
  Hdf5ReadWriteTestBasic<list<int> >("vl_list_int.h5", x, y);
}

TEST(Hdf5BackTest, ReadWritePairIntInt) {
  using std::pair;
  pair<int, int> x = pair<int, int>(6, 28);
  pair<int, int> y = pair<int, int>(42, 43);
  Hdf5ReadWriteTestBasic<pair<int, int> >("pair_int_int.h5", x, y);
}

TEST(Hdf5BackTest, ReadWriteMapIntInt) {
  using std::map;
  std::vector<int> shape(1);
  shape[0] = 3;
  map<int, int> x = map<int, int>();
  x[6] = 42;
  x[28] = 43;
  x[496] = 44;
  map<int, int> y = map<int, int>();
  y[42] = 6;
  y[43] = 28;
  y[44] = 496;
  Hdf5ReadWriteTestBasic<map<int, int> >("map_int_int.h5", x, y, &shape);
}

TEST(Hdf5BackTest, ReadWriteVLMapIntInt) {
  using std::map;
  map<int, int> x = map<int, int>();
  x[6] = 42;
  x[28] = 43;
  x[496] = 44;
  x[8128] = 45;
  map<int, int> y = map<int, int>();
  for (int i = 0; i < 42; ++i)
    y[42 + i] = i;
  Hdf5ReadWriteTestBasic<map<int, int> >("vl_map_int_int.h5", x, y);
}
