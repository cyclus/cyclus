#include "BuildingManager.h"

#include <boost/any.hpp>

#include "cbc_solver.h"
#include "solver.h"
#include "solver_interface.h"

#include "CycException.h"
#include "Logger.h"

using boost::any_cast;

namespace cyclus {
namespace ActionBuilding {

// -------------------------------------------------------------------
BuildOrder::BuildOrder(int n, ActionBuilding::Builder* b,
                       SupplyDemand::CommodityProducer* cp) :
  number(n),
  builder(b),
  producer(cp)
{}

// -------------------------------------------------------------------
ProblemInstance::ProblemInstance(Commodity& commod, double demand, 
                                 cyclopts::SolverInterface& sinterface, 
                                 cyclopts::ConstraintPtr constr, 
                                 std::vector<cyclopts::VariablePtr>& soln) :
  commodity(commod),
  unmet_demand(demand),
  interface(sinterface),
  constraint(constr),
  solution(soln)
{}

// -------------------------------------------------------------------
BuildingManager::BuildingManager() {}

// -------------------------------------------------------------------
BuildingManager::~BuildingManager() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildingManager::registerBuilder(ActionBuilding::Builder* builder) {
  if (builders_.find(builder) != builders_.end())
    {
      throw CycDoubleRegistrationException("A manager is trying to register a builder twice.");
    }
  else
    {
      builders_.insert(builder);
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildingManager::unRegisterBuilder(ActionBuilding::Builder* builder) {
  if (builders_.find(builder) == builders_.end())
    {
      throw CycNotRegisteredException("A manager is trying to unregister a builder not originally registered with it.");
    }
  else
    {
      builders_.erase(builder);
    }
}

// -------------------------------------------------------------------
std::vector<ActionBuilding::BuildOrder> BuildingManager::makeBuildDecision(
    Commodity& commodity, 
    double unmet_demand) {
  using std::vector;
  using cyclopts::SolverPtr;
  using cyclopts::CBCSolver;
  using cyclopts::Constraint;
  using cyclopts::ConstraintPtr;
  using cyclopts::ObjFuncPtr;
  using cyclopts::SolverInterface;
//  using cyclopts::ProblemInstance;
  using cyclopts::VariablePtr;
  using cyclopts::IntegerVariable;
  using cyclopts::ObjectiveFunction;
  vector<BuildOrder> orders;
  
  if (unmet_demand > 0) {
    // set up solver and interface
    SolverPtr solver(new CBCSolver());
    SolverInterface csi(solver);

    // set up objective function
    ObjFuncPtr obj(new ObjectiveFunction(ObjectiveFunction::MIN));
    csi.RegisterObjFunction(obj);

    // set up constraint
    ConstraintPtr constraint(new Constraint(Constraint::GTEQ,unmet_demand));
    csi.RegisterConstraint(constraint);

    // set up variables, constraints, and objective function
    vector<VariablePtr> solution;
    ProblemInstance problem(commodity,unmet_demand,csi,constraint,solution);
    setUpProblem(problem);

    // report problem
    LOG(LEV_DEBUG2,"buildman") << "Building Manager is solving a decision problem with:";
    LOG(LEV_DEBUG2,"buildman") << "  * Objective Function: " << obj->Print();
    LOG(LEV_DEBUG2,"buildman") << "  * Constraint: " << constraint->Print();
  
    // solve
    csi.Solve();

    // report solution
    LOG(LEV_DEBUG2,"buildman") << "Building Manager has solved a decision problem with:";
    LOG(LEV_DEBUG2,"buildman") << "  * Types of Prototypes to build: " << solution.size();
    for (int i = 0; i < solution.size(); i++) {
      VariablePtr x = solution.at(i);
      LOG(LEV_DEBUG2,"buildman") << "  * Type: " << x->name()
                                 << "  * Value: " << any_cast<int>(x->value());
    }
  
    // construct order
    constructBuildOrdersFromSolution(orders,solution);
  }

  return orders;
}

// -------------------------------------------------------------------
void BuildingManager::setUpProblem(ActionBuilding::ProblemInstance& problem)
{
  solution_map_ = std::map< cyclopts::VariablePtr, 
                    std::pair<Builder *, SupplyDemand::CommodityProducer *> >();

  std::set<Builder*>::iterator builder_it;
  for (builder_it = builders_.begin(); builder_it != builders_.end(); builder_it++)
    {
      Builder* builder = (*builder_it);
      
      std::set<SupplyDemand::CommodityProducer *>::iterator producer_it;
      for (producer_it = builder->beginningProducer(); producer_it != builder->endingProducer(); producer_it++)
        {
          SupplyDemand::CommodityProducer * producer = (*producer_it);
          if (producer->producesCommodity(problem.commodity))
            {
              addProducerVariableToProblem(producer,builder,problem);
            }
        }
    }
}

// -------------------------------------------------------------------
void BuildingManager::addProducerVariableToProblem(
    SupplyDemand::CommodityProducer* producer,
    ActionBuilding::Builder* builder,
    ActionBuilding::ProblemInstance& problem) {
  using std::make_pair;
  using cyclopts::Variable;
  using cyclopts::VariablePtr;
  using cyclopts::IntegerVariable;
  VariablePtr x(new IntegerVariable(0,Variable::INF));
  problem.solution.push_back(x);
  problem.interface.RegisterVariable(x);
  solution_map_.insert(make_pair(x,make_pair(builder,producer)));

  double constraint_modifier = producer->productionCapacity(problem.commodity);
  problem.interface.AddVarToConstraint(x,constraint_modifier,problem.constraint);

  double obj_modifier = producer->productionCost(problem.commodity);
  problem.interface.AddVarToObjFunction(x,obj_modifier);
}

// -------------------------------------------------------------------
void BuildingManager::constructBuildOrdersFromSolution(std::vector<ActionBuilding::BuildOrder>& orders,
                                                       std::vector<cyclopts::VariablePtr>& solution)
{
  // construct the build orders
  for (int i = 0; i < solution.size(); i++) 
    {
      int number = any_cast<int>(solution.at(i)->value());
      if (number > 0) 
        {
          Builder* builder = solution_map_[solution.at(i)].first;
          SupplyDemand::CommodityProducer* producer = \
                                                solution_map_[solution.at(i)].second;
          BuildOrder order(number,builder,producer);
          orders.push_back(order);
        }
    }
}
} // namespace ActionBuilding
} // namespace cyclus
