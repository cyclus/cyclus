#ifndef BUILDINGMANAGER_H
#define BUILDINGMANAGER_H

#include "SupplyDemandManager.h"

#include <vector>
#include <utility>

/// a struct for a build order: the number of producers to build
struct BuildOrder {
  BuildOrder(int n, Producer* p) : number(n), producer(p) {};
  int number;
  Producer* producer;
};

/**
   The BuildingManager class is a managing entity that makes decisions about which objects to build given certain conditions.

   Specifically, the BuildingManager queries the SupplyDemandManager to determine if there exists unmet demand for a Product, and then
   decides which object(s) amongst that Product's producers should be built to meet that demand. This decision takes the form of an
   integer program:

   \min \sum_{i=1}^{N}n_i*c_i
   s.t. \sum_{i=1}^{N}n_i*\phi_i \ge \Phi
        n_i \in [0,\infty) \forall i \in I, n_i integer

   Where n_i is the number of objects of type i to build, c_i is the cost to build the object of type i, \phi_i is the nameplate 
   capacity of the object, and \Phi is the capacity demand. Here the set I corresponds to all producers of a given product.
 */
class BuildingManager {
 public:
  /**
     constructor, which copies a predefined supply demand manager into its manager_ member
     @param m the SupplyDemandManager to act as manager_
  */
  BuildingManager(SupplyDemandManager& m);

  /**
     given a certain product and demand, this calls the private makeBuildDecision() and returns the number of each producer to build
     @param product the product being demanded
     @param demand the additional capacity required
     @return the vector of build orders as decided
   */
  std::vector<BuildOrder> makeBuildDecision(const Product& product, double unmet_demand);
  
 private:
  /// manager of supply and demand
  SupplyDemandManager manager_;
  
  /// the vector of build orders that is populated and returned after each decision
  std::vector<BuildOrder> orders_;

  /**
     a decision is made as to how many producers of each available type to build
     this function constructs an integer program through the CyclusSolverInterface and populates orders_ with the solution
     @param p the product being demanded
     @param unmet_demand the capacity required to be built
   */
  void doMakeBuildDecision(const Product& p, double unmet_demand);
};

#endif
