#include <gtest/gtest.h>
#include <string>
#include "hdf5.h"
#include "hdf5_hl.h"

#include "hdf5_back.h"
#include "blob.h"

static const char* path = "testdb.h5";

class FileDeleter {
 public:
  FileDeleter(const char* path) : path_(path) {};
  ~FileDeleter() {
    remove(path_);
  };
  const char* path_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(Hdf5BackTest, ReadWrite) {
  using std::vector;
  using std::string;
  using cyclus::Recorder;
  using cyclus::Hdf5Back;
  FileDeleter fd(path);

  int i = 1;
  double d = 2.2;
  float f = 3.3;
  std::string str = "4.4";
  const char* blob = "banana";

  size_t strsize = str.size() + 1;
  size_t sizes[] = {16, sizeof(int), sizeof(float), sizeof(double), sizeof(char*)};
  size_t offsets[] = {0, sizes[0], sizes[0] + sizes[1], sizes[0] + sizes[1] + sizes[2], sizes[0] + sizes[1] + sizes[2] + sizes[3]};
  size_t size = sizes[0] + sizes[1] + sizes[2] + sizes[3] + sizes[4];
  const char* field_names = "string,int,float,double,blob";
  char buf[size];

  // creation
  Recorder m;
  Hdf5Back back(path);
  m.RegisterBackend(&back);
  m.NewDatum("DumbTitle")
  ->AddVal("string", str)
  ->AddVal("int", i)
  ->AddVal("float", f)
  ->AddVal("double", d)
  ->AddVal("blob", cyclus::Blob(blob))
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
  char* blob2;
  memcpy(&str2, buf + offsets[0], strsize);
  memcpy(&i2, buf + offsets[1], sizes[1]);
  memcpy(&f2, buf + offsets[2], sizes[2]);
  memcpy(&d2, buf + offsets[3], sizes[3]);
  memcpy(&blob2, buf + offsets[4], sizes[4]);

  EXPECT_STREQ(str.c_str(), str2);
  EXPECT_EQ(i, i2);
  EXPECT_FLOAT_EQ(f, f2);
  EXPECT_DOUBLE_EQ(d, d2);
  EXPECT_STREQ(blob, blob2);

  H5Fclose(file);

  // query read
  string expfields[] = {"SimId", "string", "int", "float", "double", "blob"};
  string exptypes[] = {"TEXT", "TEXT", "INTEGER", "REAL", "REAL", "BLOB"};
  cyclus::QueryResult qr = back.Query("DumbTitle", NULL);
  for (int i = 0; i < qr.fields.size(); i++) {
    EXPECT_STREQ(qr.fields[i].c_str(), expfields[i].c_str());
    EXPECT_STREQ(qr.types[i].c_str(), exptypes[i].c_str());
  }
}
