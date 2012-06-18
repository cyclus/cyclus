#ifndef SUPPLYDEMAND_H
#define SUPPLYDEMAND_H

#include <string>

/**
   a simple class defining a product; it is currently super simple
 */
class Product {
 public:
  /**
     constructor
  */
 Product(std::string name) : name_(name) {};
  
  /**
     the product's name
   */
  std::string name() const {return name_;}
  
 private:
  /// the name of the product
  std::string name_;
};

/**
   a comparitor so that products may be used in maps
 */
struct ProductCompare {
  inline bool operator() (const Product& lhs, const Product& rhs) { 
    return lhs.name() < rhs.name(); 
  }
};

/**
   a small class defining a producer
   a producer is associated with:
     * a product
     * a production capactiy
     * a production cost
 */
class Producer {
 public:
  /**
     constructor, sets product, capacity, and cost
   */
 Producer(std::string name, Product p, double capacity, double cost) :
  name_(name), product_(p), capacity_(capacity), production_cost_(cost) {};

  /// name getter
  std::string name() {return name_;}
  
  /// product getter
  Product product() {return product_;}

  /// capacity getter
  double capacity() {return capacity_;}

  /// cost getter
  double cost() {return production_cost_;}

 private:
  /// name
  std::string name_;

  /// product
  Product product_;

  /// production capcity
  double capacity_;

  /// production cost
  double production_cost_;
};

#include "SymbolicFunctions.h"
#include <vector>

/**
   a simple container class to hold product information
     * the supply of a product
     * the demand function
     * the producers of a product
*/
class ProductInformation {
 public:
  ///constructors
 ProductInformation(Product product, FunctionPtr fp, std::vector<Producer> producers) : 
    product_(product), demand_(fp), supply_(0), producers_(producers) {};
  ProductInformation(Product p, FunctionPtr fp) : 
    product_(p), demand_(fp), supply_(0) {producers_ = std::vector<Producer>();}
    /// supply
    double supply() {return supply_;}
    void increaseSupply(double amt) {supply_ += amt;}
    /// demand
    double demand(int time) {return demand_->value(time);}
    /// producers
    int nProducers() {return producers_.size();}
    Producer* producer(int i) {return &producers_.at(i);}
    void registerProducer(const Producer& p) {producers_.push_back(Producer(p));}
    
 private:
    Product product_;
    double supply_;
    FunctionPtr demand_;
    std::vector<Producer> producers_;
};


#endif
