#include <gtest/gtest.h>
#include <string>
#include "hdf5.h"
#include "hdf5_hl.h"

#include "Hdf5Back.h"

static const char* path = "testdb.h5";

class FileDeleter {
  public:
    FileDeleter(const char* path) : path_(path) {};
    ~FileDeleter() {remove(path_);};
    const char* path_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(Hdf5BackTest, ReadWrite) {
  FileDeleter fd(path);

  int i = 1;
  double d = 2.2;
  float f = 3.3;
  std::string str = "4.4";

  size_t strsize = str.size() + 1;
  size_t sizes[] = {16, sizeof(int), sizeof(float), sizeof(double)};
  size_t offsets[4] = {0, sizes[0], sizes[0] + sizes[1], sizes[0] + sizes[1] + sizes[2]};
  size_t size = sizes[0] + sizes[1] + sizes[2] + sizes[3];
  const char* field_names = "string,int,float,double";
  char buf[size];

  EventManager m;
  Hdf5Back back(path);
  m.registerBackend(&back);
  m.newEvent("DumbTitle")
   ->addVal("string", str)
   ->addVal("int", i)
   ->addVal("float", f)
   ->addVal("double", d)
   ->record();
  m.close();

  hid_t file = H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT);
  herr_t status = H5TBread_fields_name(file, "DumbTitle", field_names, 0, 1, size, offsets, sizes, buf);

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
}
