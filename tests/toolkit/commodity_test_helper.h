#ifndef CYCLUS_TESTS_COMMODITY_TEST_HELPER_H_
#define CYCLUS_TESTS_COMMODITY_TEST_HELPER_H_

#include <string>
#include "toolkit/commodity.h"
#include "toolkit/commodity_producer.h"
#include "toolkit/commodity_producer_manager.h"

namespace cyclus {
namespace toolkit {

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
  Commodity commodity;

  /// first producer
  CommodityProducer* producer1;

  /// second producer
  CommodityProducer* producer2;

  /// production capacity
  double capacity;

  /// number of producers
  int nproducers;

  /// a manager of commodity producers
  CommodityProducerManager manager;

  /// initialize the producers
  void SetUpProducers();

  /// initialize the producer manager
  void SetUpProducerManager();
};

} // namespace toolkit
} // namespace cyclus

#endif  // CYCLUS_TESTS_COMMODITY_TEST_HELPER_H_
