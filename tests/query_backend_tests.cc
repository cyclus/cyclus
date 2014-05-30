#include <string>

#include <gtest/gtest.h>

#include "blob.h"
#include "query_backend.h"

template <typename T>
inline bool NotCmpCond(T* x, cyclus::Cond* cond) {
  return !cyclus::CmpCond<T>(x, cond);
};

template <typename T>
inline bool NotCmpConds(T* x, std::vector<cyclus::Cond*>* cond) {
  return !cyclus::CmpConds<T>(x, cond);
};

TEST(QueryBackendTest, CmpCondOps) {
  using cyclus::Cond;
  using cyclus::CmpCond;

  int x = 42;
  int y = 43;
  int z = 44;
  Cond cond;

  cond = Cond("x", "<", 43);
  EXPECT_PRED2(CmpCond<int>, &x, &cond);
  EXPECT_PRED2(NotCmpCond<int>, &y, &cond);

  cond = Cond("y", ">", 42);
  EXPECT_PRED2(CmpCond<int>, &y, &cond);
  EXPECT_PRED2(NotCmpCond<int>, &x, &cond);

  cond = Cond("y", "<=", 43);
  EXPECT_PRED2(CmpCond<int>, &x, &cond);
  EXPECT_PRED2(CmpCond<int>, &y, &cond);
  EXPECT_PRED2(NotCmpCond<int>, &z, &cond);

  cond = Cond("y", ">=", 43);
  EXPECT_PRED2(NotCmpCond<int>, &x, &cond);
  EXPECT_PRED2(CmpCond<int>, &y, &cond);
  EXPECT_PRED2(CmpCond<int>, &z, &cond);

  cond = Cond("x", "==", 42);
  EXPECT_PRED2(CmpCond<int>, &x, &cond);
  EXPECT_PRED2(NotCmpCond<int>, &y, &cond);

  cond = Cond("x", "!=", 43);
  EXPECT_PRED2(CmpCond<int>, &x, &cond);
  EXPECT_PRED2(NotCmpCond<int>, &y, &cond);
}

TEST(QueryBackendTest, CmpCondFloat) {
  using cyclus::Cond;
  using cyclus::CmpCond;
  float x = 42.0;
  float y = 43.0;
  Cond cond = Cond("x", "==", (float) 42.0);
  EXPECT_PRED2(CmpCond<float>, &x, &cond);
  EXPECT_PRED2(NotCmpCond<float>, &y, &cond);
}

TEST(QueryBackendTest, CmpCondDouble) {
  using cyclus::Cond;
  using cyclus::CmpCond;
  double x = 42.0;
  double y = 43.0;
  Cond cond = Cond("x", "==", 42.0);
  EXPECT_PRED2(CmpCond<double>, &x, &cond);
  EXPECT_PRED2(NotCmpCond<double>, &y, &cond);
}

TEST(QueryBackendTest, CmpCondString) {
  using cyclus::Cond;
  using cyclus::CmpCond;
  std::string x = "wakka";
  std::string y = "jawaka";
  Cond cond = Cond("x", "==", std::string("wakka"));
  EXPECT_PRED2(CmpCond<std::string>, &x, &cond);
  EXPECT_PRED2(NotCmpCond<std::string>, &y, &cond);
}

TEST(QueryBackendTest, CmpCondBlob) {
  using cyclus::Cond;
  using cyclus::CmpCond;
  using cyclus::Blob;
  Blob x = Blob("wakka");
  Blob y = Blob("jawaka");
  Cond cond = Cond("x", "==", Blob("wakka"));
  EXPECT_PRED2(CmpCond<Blob>, &x, &cond);
  EXPECT_PRED2(NotCmpCond<Blob>, &y, &cond);
}

TEST(QueryBackendTest, CmpCondUuid) {
  using cyclus::Cond;
  using cyclus::CmpCond;
  using boost::uuids::uuid;
  uuid x = {0x12 ,0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56,
            0x78, 0x90, 0xab, 0xcd, 0xef};
  uuid y = {0x42 ,0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
            0x42, 0x42, 0x42, 0x42, 0x42};
  uuid z = x;
  Cond cond = Cond("x", "==", z);
  EXPECT_PRED2(CmpCond<uuid>, &x, &cond);
  EXPECT_PRED2(NotCmpCond<uuid>, &y, &cond);
}

TEST(QueryBackendTest, CmpConds) {
  using std::vector;
  using cyclus::Cond;
  using cyclus::CmpConds;

  int x = 42;
  int y = 43;
  Cond c0 = Cond("x", "<", 1000000);
  Cond c1 = Cond("x", "==", 42);
  Cond c2 = Cond("x", ">=", 16);
  Cond* cs[] = {&c0, &c1, &c2};
  vector<Cond*> conds = vector<Cond*>(cs, cs+3);

  EXPECT_PRED2(CmpConds<int>, &x, &conds);
  EXPECT_PRED2(NotCmpConds<int>, &y, &conds);
}