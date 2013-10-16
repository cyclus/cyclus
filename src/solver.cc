#include "solver.h"

#include <map>
#include <vector>

namespace cyclus {

// -----------------------------------------------------------------------------
Solver::Solver() {
  index_ = std::map<VariablePtr, int>();
}

// -----------------------------------------------------------------------------
void Solver::PopulateIndices(std::vector<VariablePtr>& variables) {
  for (int i = 0; i < variables.size(); i++) {
    index_[variables.at(i)] = i;
  }
}

} // namespace cyclus
