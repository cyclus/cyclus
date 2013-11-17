#ifndef CYCLUS_GREEDY_SOLVER_H_
#define CYCLUS_GREEDY_SOLVER_H_

#include "exchange_solver.h"

namespace cyclus {

class ExchangeGraph;
  
class GreedySolver: public ExchangeSolver {
 public:
  explicit GreedySolver(ExchangeGraph& g);
  virtual void Solve();
};

  
} // namespace cyclus

#endif // ifndef CYCLUS_GREEDY_SOLVER_H_
