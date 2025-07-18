#include "bid_portfolio.h"

#include "trader.h"

namespace cyclus {

std::string GetTraderPrototype(Trader* bidder) {
  return bidder->manager()->prototype();
}

std::string GetTraderSpec(Trader* bidder) {
  return bidder->manager()->spec();
}

}  // namespace cyclus