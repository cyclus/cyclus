#include "comp_math.h"

#include <cmath>
#include <sstream>

#include "cyc_arithmetic.h"
#include "error.h"
#include "pyne.h"

namespace cyclus {
namespace compmath {

CompMap Add(const CompMap& v1, const CompMap& v2) {
  CompMap out(v1);
  for (CompMap::const_iterator it = v2.begin(); it != v2.end(); ++it) {
    int nuc = it->first;
    out[nuc] += it->second;
  }
  return out;
}

CompMap Sub(const CompMap& v1, const CompMap& v2) {
  CompMap out(v1);
  for (CompMap::const_iterator it = v2.begin(); it != v2.end(); ++it) {
    int nuc = it->first;
    out[nuc] -= it->second;
  }
  return out;
}

double Sum(const CompMap& v) {
  std::vector<double> vec;
  vec.reserve(v.size());
  for (CompMap::const_iterator it = v.begin(); it != v.end(); ++it) {
    vec.push_back(it->second);
  }
  return CycArithmetic::KahanSum(vec);
}

void ApplyThreshold(CompMap* v, double threshold) {
  if (threshold < 0) {
    std::stringstream ss;
    ss << "The threshold cannot be negative. The value provided was '"
       << threshold << "'.";
    throw ValueError(ss.str());
  }

  CompMap::iterator it = v->begin();
  while (it != v->end()) {
    if (std::abs(it->second) <= threshold) {
      v->erase(it++);
    } else {
      it++;
    }
  }
}

void Normalize(CompMap* v, double val) {
  double sum = Sum(*v);
  if (sum != val && sum != 0) {
    double mult = val / sum;
    for (CompMap::iterator it = v->begin(); it != v->end(); ++it) {
      it->second *= mult;
    }
  }
}

bool ValidNucs(const CompMap& v) {
  CompMap::const_iterator it;
  for (it = v.begin(); it != v.end(); ++it) {
    if (!pyne::nucname::isnuclide(it->first)) {
      return false;
    }
  }
  return true;
}

bool AllPositive(const CompMap& v) {
  CompMap::const_iterator it;
  for (it = v.begin(); it != v.end(); ++it) {
    if (it->second < 0) {
      return false;
    }
  }
  return true;
}

bool AlmostEq(const CompMap& v1, const CompMap& v2, double threshold) {
  // I learned at
  // http://www.ualberta.ca/~kbeach/comp_phys/fp_err.html#testing-for-equality
  // that the following is less naive than the intuitive way of doing this...
  // almost equal if :
  // (abs(x-y) < abs(x)*eps) && (abs(x-y) < abs(y)*epsilon)
  if (threshold < 0) {
    std::stringstream ss;
    ss << "The threshold cannot be negative. The value provided was '"
       << threshold << "'.";
    throw ValueError(ss.str());
  }

  if (v1.size() != v2.size()) {
    return false;
  } else if (v1.empty() && v2.empty()) {
    return true;
  }

  CompMap n1(v1);
  CompMap n2(v2);

  CompMap::iterator it;
  for (it = n1.begin(); it != n1.end(); ++it) {
    Nuc nuc = it->first;
    if (n2.count(nuc) == 0) {
      return false;
    }
    double minuend = n2[nuc];
    double subtrahend = n1[nuc];
    double diff = minuend - subtrahend;
    if (std::abs(minuend) == 0 || std::abs(subtrahend) == 0) {
      if (std::abs(diff) > std::abs(diff)*threshold) {
        return false;
      }
    } else if (std::abs(diff) > std::abs(minuend)*threshold ||
               std::abs(diff) > std::abs(subtrahend)*threshold) {
      return false;
    }
  }
  return true;
}

}  // namespace compmath
}  // namespace cyclus
