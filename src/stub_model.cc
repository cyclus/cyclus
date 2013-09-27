// Stubmodel.cc
// Implements the StubModel class

#include "stub_model.h"

#include "logger.h"

namespace cyclus {

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubModel::StubModel(Context* ctx) : Model(ctx) {
  SetModelType("Stub");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubModel::~StubModel() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubModel::str() {
  return Model::str() + "";
};

/* ------------------- */
} // namespace cyclus
