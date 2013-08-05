#include "CommodityTestHelper.h"

//using namespace supply_demand;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityTestHelper::CommodityTestHelper() :
  commodity_name("commod"),
  commodity(commodity_name),
  capacity(5.0),
  nproducers(2)
{
  using cyclus::supply_demand::CommodityProducer;
  producer1 = new CommodityProducer();
  producer2 = new CommodityProducer();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityTestHelper::~CommodityTestHelper() 
{
  delete producer1;
  delete producer2;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityTestHelper::setUpProducerManager()
{
  setUpProducers();
  manager.registerProducer(producer1);
  manager.registerProducer(producer2);
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityTestHelper::setUpProducers()
{
  producer1->addCommodity(commodity);
  producer1->setCapacity(commodity,capacity);
  producer2->addCommodity(commodity);
  producer2->setCapacity(commodity,capacity);
}
