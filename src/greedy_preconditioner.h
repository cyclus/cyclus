#ifndef CYCLUS_GREEDY_PRECONDITIONER_H_
#define CYCLUS_GREEDY_PRECONDITIONER_H_

namespace cyclus {

class ExchangeGraph;
  
class GreedyPreconditioner {
 public:
  void Condition(ExchangeGraph* graph_) {};
};

} // namespace cyclus

#endif // ifndef CYCLUS_GREEDY_PRECONDITIONER_H_

