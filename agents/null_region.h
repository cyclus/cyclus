#ifndef CYCLUS_AGENTS_NULL_REGION_H_
#define CYCLUS_AGENTS_NULL_REGION_H_

#include <string>

#include "cyclus.h"

namespace cyclus {

class NullRegion : public cyclus::Region {
 public:
  NullRegion(cyclus::Context* ctx);
  virtual ~NullRegion();

  #pragma cyclus

  #pragma cyclus note {"doc": "A region that owns the simulation's " \
                              "institutions but exhibits null behavior. " \
                              "No parameters are given when using the " \
                              "null region."}
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_NULL_REGION_H_
