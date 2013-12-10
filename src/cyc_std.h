#ifndef CYCLUS_CYC_STD_H_
#define CYCLUS_CYC_STD_H_

#include <utility>
#include <functional>

/// @brief a collection of tools that are standard-library like

namespace cyclus {

/// @brief a less-than comparison for pairs
template<class T> struct SecondLT : std::binary_function<T, T, bool> {
  bool operator() (const T& x, const T& y) const { return x.second < y.second; }
};

} // namespace cyclus

#endif // CYCLUS_CYC_STD_H_
