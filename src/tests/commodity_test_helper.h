#ifndef COMMODITYTESTHELPER_H
#define COMMODITYTESTHELPER_H

#include <string>
#include "commodity.h"
#include "commodity_producer.h"
#include "commodity_producer_manager.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CommodityTestHelper {
 public:
  /// constructor
  CommodityTestHelper();

  /// destructor
  ~CommodityTestHelper();

  /// commodity name
  std::string commodity_name;

  /// commodity
  cyclus::Commodity commodity;

  /// first producer
  cyclus::supply_demand::CommodityProducer* producer1;

  /// second producer
  cyclus::supply_demand::CommodityProducer* producer2;
  
  /// production capacity
  double capacity;

  /// number of producers
  int nproducers;
  
  /// a manager of commodity producers
  cyclus::supply_demand::CommodityProducerManager manager;

  /// initialize the producers
  void SetUpProducers();

  /// initialize the producer manager
  void SetUpProducerManager();
};

#endif
