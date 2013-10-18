
#include "context.h"
#include "model.h"

namespace cyclus {

/// @class Exchanger
class Exchanger : virtual public Model {
 public:
  Exchanger(Context* ctx) : Model(ctx) { };
    
  virtual ~Exchanger() {};
};

} // namespace cyclus
