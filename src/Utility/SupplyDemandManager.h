#ifndef SUPPLYDEMANDMANAGER_H
#define SUPPLYDEMANDMANAGER_H

#include "SupplyDemand.h"
#include "SymbolicFunctions.h"

#include <vector>
#include <map>

/**
   This is a manager class that manages a set of products. Those products have a certain
   demand function associated with them and a list of producers who can produce the
   products.

   The SupplyDemandManager simply keeps track of this information and provides the
   demand and supply of a product at a given time. What to do with this information is 
   left to the user of the SupplyDemandManager.
 */
class SupplyDemandManager {
 public:
  /**
     constructor
   */
  SupplyDemandManager();

  /**
     register a new product with the manager, along with all the necessary
     information
     @param product the product
     @param fp a smart pointer to the demand function
     @param producers the list of producers of product
   */
  void registerProduct(const Product& product, const FunctionPtr fp, 
                       const std::vector<Producer>& producers);

  /**
     calls the registerProducer() function of the ProductInformation
     instance associated with the product
     @param the product gaining a new producer
     @param the producer to be registered
   */
  void registerProducer(const Product& prodcut, const Producer& producer);

  /**
     the demand for a product at a given time
     @param p the product
     @param time the time
   */
  double demand(const Product& p, int time);

  /**
     returns the current supply of product p
     @param p the product
     @return the current supply of the product
   */
  double supply(const Product& p);

  /**
     increase the supply of a given product by an amount
     @param p the product
     @param amt the amount to increase
   */
  void increaseSupply(const Product& p, double amt);
  
  /**
     return the number of producers of a given product
     @param p the product
     @return the number of producers of product p
   */
  int nProducers(const Product& p);

  /**
     return a specific producer of a product
     @param p the product
     @param index the producer's index
     @return a pointer to the producer of product p at index
   */
  Producer* producer(const Product& p, int index);
  
 private:
  /// a container of all products known to the manager
  std::map<Product,ProductInformation,ProductCompare> products_;
};

#endif
