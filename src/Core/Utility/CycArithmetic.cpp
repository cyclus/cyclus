/*! \file CycArithmetic.cpp
    \brief Implements the CycArithmetic class for various arithmetic algorithms 
    \author Kathryn D. Huff
 */
#include <iostream>

#include "CycException.h"
#include "CycArithmetic.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double CycArithmetic::KahanSum(vector<double> input){
  vector<double> sorted = sort_ascending(input);
  // http://en.wikipedia.org/wiki/Kahan_summation_algorithm
  double y, t;
  double sum = 0.0;
  //A running compensation for lost low-order bits.
  double c = 0.0; 
  for(vector<double>::iterator i = sorted.begin(); i!=sorted.end(); ++i){
    y = *i - c;
    //So far, so good: c is zero.
    t = sum + y;
    //Alas, sum is big, y small, so low-order digits of y are lost.
    c = (t - sum) - y;
    //(t - sum) recovers the high-order part of y; subtracting y recovers -(low part of y)
    sum = t;
    //Algebraically, c should always be zero. Beware eagerly optimising compilers!
    //Next time around, the lost low part will be added to y in a fresh attempt.
  }
  return sum;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<double> CycArithmetic::sort_ascending(vector<double> to_sort){
  sort(to_sort.begin(), to_sort.end());
  return to_sort;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<double> CycArithmetic::sort_ascending(map<int, double> to_sort){ 
  vector<double> vec_to_sort;
  map<int, double>::const_iterator it;
  for(it=to_sort.begin(); it!=to_sort.end(); ++it){
    vec_to_sort.push_back((*it).second);
  }
  return sort_ascending(vec_to_sort);
}
