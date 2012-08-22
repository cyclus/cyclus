#ifndef SUPPLYDEMAND_H
#define SUPPLYDEMAND_H

#include <string>
#include <ostream>

/**
   a simple class defining a commodity; it is currently super simple.
   the reason this class exists is so that code may be cleaner and more straightforward
   while one could have chosen to typedef a string, there may be some reason to extend
   the class in the future.
 */
class Commodity {
 public:
  /**
     constructor
     @param name the name of the commodity
  */
 Commodity(std::string name) : name_(name) {};
  
  /// the commodity's name
  std::string name() const {return name_;}

  /// equality operator
  bool operator==(const Commodity& other) const {
    return (name_ == other.name());
  } 

  /// inequality operator
  bool operator!=(const Commodity& other) const {
    return !(name_ == other.name());
  } 
  
 private:
  /// the name of the commodity
  std::string name_;
};

/**
   a comparitor so that commodities may be used in maps
   we do not care how they are compared, only that they can be
 */
struct CommodityCompare {
  inline bool operator() (const Commodity& lhs, const Commodity& rhs) { 
    return lhs.name() < rhs.name(); 
  }
};

/**
   a small class defining a producer
   a producer is associated with:
     * a commodity
     * a capactiy for production of commodity
     * a cost of production of commodity
 */
class Producer {
 public:
  /**
     constructor, sets name, commodity, capacity, and cost
   */
 Producer(std::string name, Commodity p, double capacity, double cost) :
  name_(name), commodity_(p), capacity_(capacity), production_cost_(cost) {};

  /// name getter
  std::string name() const {return name_;}
  
  /// commodity getter
  Commodity commodity() const {return commodity_;}

  /// capacity getter
  double capacity() const {return capacity_;}

  /// cost getter
  double cost() const {return production_cost_;}

  /// equality operator
  bool operator==(const Producer& other) const {
    bool test = true;
    test = test && (name_ == other.name());
    test = test && (commodity_ == other.commodity());
    test = test && (capacity_ == other.capacity());
    test = test && (production_cost_ == other.cost());
    return test;
  }
  
 private:
  /// name
  std::string name_;

  /// commodity
  Commodity commodity_;

  /// commodityion capcity
  double capacity_;

  /// production cost
  double production_cost_;
};

#include "SymbolicFunctions.h"
#include <vector>

/**
   a simple container class to hold commodity information
     * the supply of a commodity
     * the demand function
     * the producers of a commodity
*/
class CommodityInformation {
 public:
  /**
     constructor given a commodity, a demand function, and a set of produers
     @param commodity the commodity
     @param fp a shared pointer to the demand function
     @param producers a vector of producers of the commodity
  */
    CommodityInformation(Commodity commodity, FunctionPtr fp, std::vector<Producer> producers) : 
  commodity_(commodity), demand_(fp), supply_(0), producers_(producers) {};

  /**
     constructor given a commodity and a demand function
     @param commodity the commodity
     @param fp a shared pointer to the demand function
   */
 CommodityInformation(Commodity commodity, FunctionPtr fp) : 
  commodity_(commodity), demand_(fp), supply_(0) {producers_ = std::vector<Producer>();}

  /// supply
  double supply() {return supply_;}

  /**
     increase the supply by an amount
     @param amt the amount in the unit characteristic to the commodity
   */ 
  void increaseSupply(double amt) {supply_ += amt;}

  /**
     decrease the supply by an amount
     @param amt the amount in the unit characteristic to the commodity
   */ 
  void decreaseSupply(double amt) {supply_ -= amt;}

  /**
     the demand function
   */
  FunctionPtr demandFunction() {return demand_;}

  /**
     the demand at a given time
     @param time the time
   */
  double demand(int time) {return demand_->value(time);}
  
  /**
     @return the number of producers
  */
  int nProducers() {return producers_.size();}
  
  /**
     a pointer to a producer
     @param i the index in producers_ 
   */
  Producer* producer(int i) {return &producers_.at(i);}

  /**
     register producer of a commodity
     @param p the commodity
  */
  void registerProducer(const Producer& p) {producers_.push_back(Producer(p));}
  
 private:
  /// the commodity
  Commodity commodity_;
  
  /// the supply of the commodity
  double supply_;
  
  /// the demand function
  FunctionPtr demand_;

  /// a set of producers of the commodity
  std::vector<Producer> producers_;
};


#endif
