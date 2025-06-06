#ifndef CYCLUS_AGENTS_NULL_REGION_H_
#define CYCLUS_AGENTS_NULL_REGION_H_

#include <string>

#include "cyclus.h"

namespace cyclus {

class NullRegion : public cyclus::Region {
 public:
  NullRegion(cyclus::Context* ctx);
  virtual ~NullRegion();

  void EnterNotify();

  virtual std::string version() { return cyclus::version::describe(); }

  #pragma cyclus

  #pragma cyclus note {"doc": "A region that owns the simulation's " \
                              "institutions but exhibits null behavior. " \
                              "No parameters are given when using the " \
                              "null region."}

  private:
  #include "toolkit/region_cost.cycpp.h"
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_NULL_REGION_H_
