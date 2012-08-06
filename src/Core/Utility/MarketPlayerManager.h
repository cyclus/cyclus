// MarketPlayerManager.h
#ifndef MARKETPLAYERMANAGER_H
#define MARKETPLAYERMANAGER_H

class MarketPlayerManager;
#include "MarketPlayer.h"

class MarketPlayerManager {
 public:
  MarketPlayerManager();
  void playerEnteringMarket(MarketPlayer* m);
  void playerLeavingMarket(MarketPlayer* m);
};

#endif
