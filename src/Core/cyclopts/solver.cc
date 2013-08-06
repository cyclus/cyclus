#include "solver.h"

#include <map>
#include <vector>

// -----------------------------------------------------------------------------
cyclus::cyclopts::Solver::Solver() {
  index_ = std::map<cyclus::cyclopts::VariablePtr, int>();
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::Solver::PopulateIndices(
    std::vector<cyclus::cyclopts::VariablePtr>& variables) {
  for (int i = 0; i < variables.size(); i++) {
    index_[variables.at(i)] = i;
  }
}
