/*! \file cyc_arithmetic.h
    \brief Declares the CycArithmetic class, which holds arithmetic algorithms
    \author Kathryn D. Huff
 */
#ifndef CYCLUS_SRC_CYC_ARITHMETIC_H_
#define CYCLUS_SRC_CYC_ARITHMETIC_H_

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

namespace cyclus {
/// @brief CycArithmetic is a toolkit for arithmetic
class CycArithmetic {
 public:
  /// sums the materials in the vector in an intelligent way,
  /// to avoid floating point issues.
  /// @param input is the list of values to add to each other
  /// @return is the sum of all the values in the input vector
  static double KahanSum(std::vector<double> input);

  /// orders the vector from smallest value to largest value.
  /// This helps for addition algorithms.
  /// @param to_sort is the vector of values to sort
  /// @returns a set sorted from smallest to largest
  static std::vector<double> sort_ascending(std::vector<double> to_sort);

  /// orders the values in a map from smallest value to largest value.
  /// This helps for addition algorithms.
  /// @param to_sort is a map of values to sort (according to the value of the double)
  /// @returns a set sorted from smallest to largest
  static std::vector<double> sort_ascending(std::map<int, double> to_sort);
};
}  // namespace cyclus
#endif  // CYCLUS_SRC_CYC_ARITHMETIC_H_
