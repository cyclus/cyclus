
#include "comp_math.h"

#include <cmath>
#include <sstream>

#include "cyc_arithmetic.h"
#include "error.h"

namespace cyclus {
namespace compmath {

Composition::Vect Add(const Composition::Vect& v1,
                      const Composition::Vect& v2) {
  Composition::Vect out(v1);
  Composition::Vect vv2(v2);
  for (Composition::Vect::const_iterator it = v2.begin(); it != v2.end(); ++it) {
    int iso = it->first;
    out[iso] += vv2[iso];
  }
  return out;
}

Composition::Vect Sub(const Composition::Vect& v1,
                      const Composition::Vect& v2) {
  Composition::Vect out(v1);
  Composition::Vect vv2(v2);
  for (Composition::Vect::const_iterator it = v2.begin(); it != v2.end(); ++it) {
    int iso = it->first;
    out[iso] -= vv2[iso];
  }
  return out;
}

void ApplyThreshold(Composition::Vect* v, double threshold) {
  if (threshold < 0) {
    std::stringstream ss;
    ss << "The threshold cannot be negative. The value provided was '"
       << threshold << "'.";
    throw ValueError(ss.str());
  }

  Composition::Vect::iterator it;
  for (it = v->begin(); it != v->end(); ++it) {
    if (std::abs(it->second) <= threshold) {
      v->erase(it);
    }
  }
}

void Normalize(Composition::Vect* v, double val) {
  std::vector<double> vec;
  for (Composition::Vect::iterator it = v->begin(); it != v->end(); ++it) {
    vec.push_back(it->second);
  }

  double sum = CycArithmetic::KahanSum(vec);
  if (sum != val && sum != 0) {
    for (Composition::Vect::iterator it = v->begin(); it != v->end(); ++it) {
      (*v)[it->first] = it->second / sum * val;
    }
  }
}

bool ValidIsos(const Composition::Vect& v) {
  int min = 1001;
  int max = 1182949;

  Composition::Vect::const_iterator it;
  for (it = v.begin(); it != v.end(); ++it) {
    cyclus::Iso iso = it->first;
    if (iso < min || iso > max) {
      return false;
    }
  }
  return true;
}

bool AllPositive(const Composition::Vect& v) {
  Composition::Vect::const_iterator it;
  for (it = v.begin(); it != v.end(); ++it) {
    if (it->second < 0) {
      return false;
    }
  }
  return true;
}

bool AlmostEq(const Composition::Vect& v1,
              const Composition::Vect& v2,
              double threshold) {
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

  Composition::Vect n1(v1);
  Composition::Vect n2(v2);

  Composition::Vect::iterator it;
  for (it = n1.begin(); it != n1.end(); ++it) {
    cyclus::Iso iso = it->first;
    if (n2.count(iso) == 0) {
      return false;
    }
    double minuend = n2[iso];
    double subtrahend = n1[iso];
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

} // namespace compmath
} // namespace cyclus

