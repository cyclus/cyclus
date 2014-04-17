#ifndef CYCLUS_TESTS_COMMODITY_TEST_HELPER_H_
#define CYCLUS_TESTS_COMMODITY_TEST_HELPER_H_

#include <string>
#include "commodity.h"
#include "commodity_producer.h"
#include "commodity_producer_manager.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
  cyclus::CommodityProducer* producer1;

  /// second producer
  cyclus::CommodityProducer* producer2;

  /// production capacity
  double capacity;

  /// number of producers
  int nproducers;

  /// a manager of commodity producers
  cyclus::CommodityProducerManager manager;

  /// initialize the producers
  void SetUpProducers();

  /// initialize the producer manager
  void SetUpProducerManager();
};

#endif  // CYCLUS_TESTS_COMMODITY_TEST_HELPER_H_
