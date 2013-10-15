#include "solver.h"

#include <map>
#include <vector>

// -----------------------------------------------------------------------------
cyclus::optim::Solver::Solver() {
  index_ = std::map<cyclus::optim::VariablePtr, int>();
}

// -----------------------------------------------------------------------------
void cyclus::optim::Solver::PopulateIndices(
    std::vector<cyclus::optim::VariablePtr>& variables) {
  for (int i = 0; i < variables.size(); i++) {
    index_[variables.at(i)] = i;
  }
}
