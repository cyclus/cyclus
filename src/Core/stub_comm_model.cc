// StubCommmodel.cc
// Implements the StubCommModel class

#include "stub_comm_model.h"

#include "logger.h"

namespace cyclus {

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel::StubCommModel() {
  SetModelType("StubComm");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel::~StubCommModel() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubCommModel::str() {
  return Model::str() + "";
};

/* ------------------- */
} // namespace cyclus
