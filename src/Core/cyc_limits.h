#ifndef CYCLIMITS_H
#define CYCLIMITS_H

namespace cyclus {
  /// a generic epsilon value
  inline double eps() { return 1e-6; }
  
  /// an epsilon value to be used by resources
  inline double eps_rsrc() { return 1e-6; }
}

#endif
