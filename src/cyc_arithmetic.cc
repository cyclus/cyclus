/*! \file cyc_arithmetic.cc
    \brief Implements the CycArithmetic class for various arithmetic algorithms
    \author Kathryn D. Huff
 */
#include <iostream>

#include "error.h"
#include "cyc_arithmetic.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double CycArithmetic::KahanSum(std::vector<double> input) {
  sort_inplace_ascending(input);
  // http://en.wikipedia.org/wiki/Kahan_summation_algorithm
  double y, t;
  double sum = 0.0;
  // A running compensation for lost low-order bits.
  double c = 0.0;
  for (int i = 0; i < input.size(); i++) {
    y = input[i] - c;
    // So far, so good: c is zero.
    t = sum + y;
    // Alas, sum is big, y small, so low-order digits of y are lost.
    c = (t - sum) - y;
    // (t - sum) recovers the high-order part of y; subtracting y recovers -(low part of y)
    sum = t;
    // Algebraically, c should always be zero. Beware eagerly optimizing compilers!
    // Next time around, the lost low part will be added to y in a fresh attempt.
  }
  return sum;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CycArithmetic::sort_inplace_ascending(std::vector<double>& to_sort) {
  sort(to_sort.begin(), to_sort.end());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<double> CycArithmetic::sort_ascending(std::vector<double> to_sort) {
  sort(to_sort.begin(), to_sort.end());
  return to_sort;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<double> CycArithmetic::sort_ascending(std::map<int, double>
                                                  to_sort) {
  std::vector<double> vec_to_sort;
  std::map<int, double>::const_iterator it;
  for (it = to_sort.begin(); it != to_sort.end(); ++it) {
    vec_to_sort.push_back((*it).second);
  }
  return sort_ascending(vec_to_sort);
}

}  // namespace cyclus
