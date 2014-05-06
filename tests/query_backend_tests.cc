#include <string>

#include <gtest/gtest.h>

#include "blob.h"
#include "query_backend.h"

template <typename T>
inline bool NotCmpCond(T* x, cyclus::Cond* cond) {
  return !cyclus::CmpCond<T>(x, cond);
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(QueryBackendTest, CmpCond) {
  using std::vector;
  using std::string;
  using cyclus::Cond;
  using cyclus::CmpCond;

  int x = 42;
  int y = 43;

  Cond cond = Cond("x", "==", 42);
  EXPECT_PRED2(CmpCond<int>, &x, &cond);
  EXPECT_PRED2(NotCmpCond<int>, &y, &cond);
}
