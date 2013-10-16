#include "solver.h"

#include <map>
#include <vector>

// -----------------------------------------------------------------------------
cyclus::Solver::Solver() {
  index_ = std::map<cyclus::VariablePtr, int>();
}

// -----------------------------------------------------------------------------
void cyclus::Solver::PopulateIndices(
    std::vector<cyclus::VariablePtr>& variables) {
  for (int i = 0; i < variables.size(); i++) {
    index_[variables.at(i)] = i;
  }
}
