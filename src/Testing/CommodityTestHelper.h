#include <string>
#include "Commodity.h"
#include "CommodityProducer.h"
#include "CommodityProducerManager.h"

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
  Commodity commodity;

  /// first producer
  SupplyDemand::CommodityProducer* producer1;

  /// second producer
  SupplyDemand::CommodityProducer* producer2;
  
  /// production capacity
  double capacity;

  /// number of producers
  int nproducers;
  
  /// a manager of commodity producers
  SupplyDemand::CommodityProducerManager manager;

  /// initialize the producers
  void setUpProducers();

  /// initialize the producer manager
  void setUpProducerManager();
};
