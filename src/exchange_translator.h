#ifndef CYCLUS_EXCHANGE_TRANSLATOR_H_
#define CYCLUS_EXCHANGE_TRANSLATOR_H_

#include "exchange_graph.h"
#include "trade.h"

namespace cyclus {

template <class T> class ExchangeContext;
class Trader;

template <class T>
class ExchangeTranslator {
 public:
  /// @brief default constructor
  ///
  /// @param ctx the exchance context
  ExchangeTranslator(ExchangeContext<T>* ctx) {
    ctx_ = ctx;    
  };

  ExchangeGraph::Ptr Translate();

  std::vector< Trade<T> > BackTranslateSolution(std::vector<Match> matches);
  
  /* -------------------- private methods and members -------------------------- */
  ExchangeContext<T>* ctx_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_TRANSLATOR_H_
