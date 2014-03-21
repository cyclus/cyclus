#ifndef CYCLUS_INTEGRATION_TESTS_MODELS_NULL_REGION_H_
#define CYCLUS_INTEGRATION_TESTS_MODELS_NULL_REGION_H_

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

#endif  // CYCLUS_INTEGRATION_TESTS_MODELS_NULL_REGION_H_
