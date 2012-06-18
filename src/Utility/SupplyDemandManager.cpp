#include "SupplyDemandManager.h"

using namespace std;

// --------------------------------------------------------------------------------------
SupplyDemandManager::SupplyDemandManager() {
  products_ = map<Product,ProductInformation,ProductCompare>();
}

// --------------------------------------------------------------------------------------
void SupplyDemandManager::registerProduct(const Product& product, const FunctionPtr fp, 
                                          const std::vector<Producer>& producers) {
  products_.insert(pair<Product,ProductInformation>
                   (product,ProductInformation(product,fp,producers)));  
}

// --------------------------------------------------------------------------------------
void SupplyDemandManager::registerProducer(const Product& product, 
                                           const Producer& producer) {
  products_.find(product)->second.registerProducer(producer);
}

// --------------------------------------------------------------------------------------
double SupplyDemandManager::demand(const Product& p, int time) {
  return products_.find(p)->second.demand(time);
}

// --------------------------------------------------------------------------------------
double SupplyDemandManager::supply(const Product& p) {
  return products_.find(p)->second.supply();
}

// --------------------------------------------------------------------------------------
void SupplyDemandManager::increaseSupply(const Product& p, double amt) { 
  products_.find(p)->second.increaseSupply(amt); 
}

// --------------------------------------------------------------------------------------
int SupplyDemandManager::nProducers(const Product& p) {
  return products_.find(p)->second.nProducers(); 
}

// --------------------------------------------------------------------------------------
Producer* SupplyDemandManager::producer(const Product& p, int index) {
  return products_.find(p)->second.producer(index); 
}
