#ifndef CYCLUS_INTEGRATION_TESTS_MODELS_NULL_INST_H_
#define CYCLUS_INTEGRATION_TESTS_MODELS_NULL_INST_H_

#include <string>

#include "cyclus.h"

namespace cyclus {

class NullInst : public cyclus::Institution {
 public:
  NullInst(cyclus::Context* ctx);
  virtual ~NullInst();

  #pragma cyclus
};

}  // namespace cyclus

#endif  // CYCLUS_INTEGRATION_TESTS_MODELS_NULL_INST_H_
