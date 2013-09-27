#include "commodity_test_helper.h"

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
void CommodityTestHelper::SetUpProducerManager()
{
  SetUpProducers();
  manager.RegisterProducer(producer1);
  manager.RegisterProducer(producer2);
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityTestHelper::SetUpProducers()
{
  producer1->AddCommodity(commodity);
  producer1->SetCapacity(commodity,capacity);
  producer2->AddCommodity(commodity);
  producer2->SetCapacity(commodity,capacity);
}
