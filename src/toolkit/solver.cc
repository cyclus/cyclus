#include "solver.h"

#include <map>
#include <vector>

namespace cyclus {
namespace toolkit {

// -----------------------------------------------------------------------------
Solver::Solver() {
  index_ = std::map<Variable::Ptr, int>();
}

// -----------------------------------------------------------------------------
void Solver::PopulateIndices(std::vector<Variable::Ptr>& variables) {
  for (int i = 0; i < variables.size(); i++) {
    index_[variables.at(i)] = i;
  }
}

} // namespace toolkit
} // namespace cyclus
