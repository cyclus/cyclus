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
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_NULL_REGION_H_
