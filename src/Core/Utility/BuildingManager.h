#ifndef BUILDINGMANAGER_H
#define BUILDINGMANAGER_H

#include "CommodityProducer.h"
#include "Builder.h"
#include "SolverTools.h"

#include <vector>
#include <map>

namespace ActionBuilding {
  /// a struct for a build order: the number of producers to build
  struct BuildOrder 
  {
    // constructor
    BuildOrder(int n, ActionBuilding::Builder* b,
               SupplyDemand::CommodityProducer* cp);

    // constituents
    int number;
    ActionBuilding::Builder* builder;
    SupplyDemand::CommodityProducer* producer;
  };

  /// a struct for a problem instance
  struct ProblemInstance 
  {
    /// constructor
    ProblemInstance(Commodity& commod, double demand, 
                    Cyclopts::SolverInterface& sinterface, 
                    Cyclopts::ConstraintPtr constr, 
                    std::vector<Cyclopts::VariablePtr>& soln);
    
    // constituents
    Commodity& commodity;
    double unmet_demand;
    Cyclopts::SolverInterface& interface;
    Cyclopts::ConstraintPtr constraint;
    std::vector<Cyclopts::VariablePtr>& solution;
  };
 
  /**
     The BuildingManager class is a managing entity that makes decisions
     about which objects to build given certain conditions.

     Specifically, the BuildingManager queries the SupplyDemandManager 
     to determine if there exists unmet demand for a Commodity, and then
     decides which object(s) amongst that Commodity's producers should be 
     built to meet that demand. This decision takes the form of an
     integer program:

     \f[
     \min \sum_{i=1}^{N}n_i*c_i \\
     s.t. \sum_{i=1}^{N}n_i*\phi_i \ge \Phi \\
     n_i \in [0,\infty) \forall i \in I, n_i integer
     \f]

     Where n_i is the number of objects of type i to build, c_i is the 
     cost to build the object of type i, \f$\phi_i\f$ is the nameplate 
     capacity of the object, and \f$\Phi\f$ is the capacity demand. Here
     the set I corresponds to all producers of a given commodity.
  */
  class BuildingManager 
  {
  public:
    /**
       constructor
    */
    BuildingManager();
    
    /**
       virtual destructor
    */
    virtual ~BuildingManager();

    /**
       register a builder with the manager
       @param builder the builder
     */
    void registerBuilder(ActionBuilding::Builder* builder);

    /**
       unregister a builder with the manager
       @param builder the builder
     */
    void unRegisterBuilder(ActionBuilding::Builder* builder);
    
    /**
       given a certain commodity and demand, a decision is made as to 
       how many producers of each available type to build this 
       function constructs an integer program through the 
       SolverInterface
       @param commodity the commodity being demanded
       @param unmet_demand the additional capacity required
       @return a vector of build orders as decided
    */
    std::vector<ActionBuilding::BuildOrder> makeBuildDecision(Commodity& commodity, 
                                                              double unmet_demand);

    // protected: @MJGFlag - should be protected. revise when tests can
    // be found by classes in the Utility folder
    /**
       set up the constraint problem
       @param problem the problem instance
     */
    void setUpProblem(ActionBuilding::ProblemInstance& problem);

    /**
       add a variable to the constraint problem
       @param producer the producer to add
       @param builder the builder of that producer
       @param problem the problem instance
     */
    void addProducerVariableToProblem(SupplyDemand::CommodityProducer* producer,
                                      ActionBuilding::Builder* builder,
                                      ActionBuilding::ProblemInstance& problem);

    /**
       given a solution to the constraint problem, construct the 
       appropriate set of build orders
       @param orders the set of orders to fill
       @param solution the solution determining how to fill the orders
     */
    void constructBuildOrdersFromSolution(std::vector<ActionBuilding::BuildOrder>& orders,
                                          std::vector<Cyclopts::VariablePtr>& solution);
  private:
    /// the set of registered builders
    std::set<Builder*> builders_;
    
    /// a map of variables to their associated builder and producer
    std::map<Cyclopts::VariablePtr, 
      std::pair<ActionBuilding::Builder*,SupplyDemand::CommodityProducer*> > solution_map_;
  };

  //#include "BuildingManagerTests.h"
  //friend class BuildingManagerTests; 
  // @MJGFlag - removed for the same reason as above
}

#endif
