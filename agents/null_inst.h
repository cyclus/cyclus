#ifndef CYCLUS_AGENTS_NULL_INST_H_
#define CYCLUS_AGENTS_NULL_INST_H_

#include <string>

#include "cyclus.h"

namespace cyclus {

class NullInst : public cyclus::Institution {
 public:
  NullInst(cyclus::Context* ctx);
  virtual ~NullInst();

  #pragma cyclus

  #pragma cyclus note {"doc": "An instition that owns facilities in the " \
                              "simulation but exhibits null behavior. " \
                              "No parameters are given when using the " \
                              "null institution."}
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_NULL_INST_H_
