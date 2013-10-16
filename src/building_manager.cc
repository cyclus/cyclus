#include "building_manager.h"

#include <boost/any.hpp>

#include "cbc_solver.h"
#include "solver.h"
#include "solver_interface.h"

#include "error.h"
#include "logger.h"

namespace cyclus {
namespace action_building {

// -------------------------------------------------------------------
BuildOrder::BuildOrder(int n, Builder* b,
                       supply_demand::CommodityProducer* cp) :
  number(n),
  builder(b),
  producer(cp) { }

// -------------------------------------------------------------------
ProblemInstance::ProblemInstance(
  Commodity& commod,
  double demand,
  SolverInterface& sinterface,
  ConstraintPtr constr,
  std::vector<VariablePtr>& soln)
  : commodity(commod),
    unmet_demand(demand),
    interface(sinterface),
    constraint(constr),
    solution(soln) { }

// -------------------------------------------------------------------
BuildingManager::BuildingManager() { }

// -------------------------------------------------------------------
BuildingManager::~BuildingManager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BuildingManager::RegisterBuilder(Builder* builder) {
  if (builders_.find(builder) != builders_.end()) {
    throw KeyError("A manager is trying to register a builder twice.");
  } else {
    builders_.insert(builder);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BuildingManager::UnRegisterBuilder(Builder* builder) {
  if (builders_.find(builder) == builders_.end()) {
    throw KeyError("A manager is trying to unregister a builder not originally registered with it.");
  } else {
    builders_.erase(builder);
  }
}

// -------------------------------------------------------------------
std::vector<BuildOrder> BuildingManager::MakeBuildDecision(
    Commodity& commodity,
    double unmet_demand) {
  using std::vector;
  using boost::any_cast;

  vector<BuildOrder> orders;
  
  if (unmet_demand > 0) {
    // set up solver and interface
    SolverPtr solver(new CBCSolver());
    SolverInterface csi(solver);

    // set up objective function
    ObjFuncPtr obj(
      new ObjectiveFunction(
        ObjectiveFunction::MIN));
    csi.RegisterObjFunction(obj);

    // set up constraint
    ConstraintPtr constraint(
      new Constraint(
        Constraint::GTEQ, unmet_demand));
    csi.RegisterConstraint(constraint);

    // set up variables, constraints, and objective function
    vector<VariablePtr> solution;
    ProblemInstance problem(commodity, unmet_demand, csi, constraint, solution);
    SetUpProblem(problem);

    // report problem
    LOG(LEV_DEBUG2, "buildman") <<
                                "Building Manager is solving a decision problem with:";
    LOG(LEV_DEBUG2, "buildman") << "  * Objective Function: " << obj->Print();
    LOG(LEV_DEBUG2, "buildman") << "  * Constraint: " << constraint->Print();

    // solve
    csi.Solve();

    // report solution
    LOG(LEV_DEBUG2, "buildman") <<
                                "Building Manager has solved a decision problem with:";
    LOG(LEV_DEBUG2, "buildman") << "  * Types of Prototypes to build: " <<
                                solution.size();
    for (int i = 0; i < solution.size(); i++) {
      VariablePtr x = solution.at(i);
      LOG(LEV_DEBUG2, "buildman") << "  * Type: " << x->name()
                                  << "  * Value: " << any_cast<int>(x->value());
    }

    // construct order
    ConstructBuildOrdersFromSolution(orders, solution);
  }

  return orders;
}

// -------------------------------------------------------------------
void BuildingManager::SetUpProblem(ProblemInstance& problem) {
  solution_map_ = std::map < VariablePtr,
  std::pair<Builder*, supply_demand::CommodityProducer*> > ();

  std::set<Builder*>::iterator builder_it;
  for (builder_it = builders_.begin(); builder_it != builders_.end();
       builder_it++) {
    Builder* builder = (*builder_it);

    std::set<supply_demand::CommodityProducer*>::iterator producer_it;
    for (producer_it = builder->BeginningProducer();
         producer_it != builder->EndingProducer(); producer_it++) {
      supply_demand::CommodityProducer* producer = (*producer_it);
      if (producer->ProducesCommodity(problem.commodity)) {
        AddProducerVariableToProblem(producer, builder, problem);
      }
    }
  }
}

// -------------------------------------------------------------------
void BuildingManager::AddProducerVariableToProblem(
  supply_demand::CommodityProducer* producer,
  Builder* builder,
  ProblemInstance& problem) {
  using std::make_pair;
  
  VariablePtr x(new IntegerVariable(0, Variable::INF));
  problem.solution.push_back(x);
  problem.interface.RegisterVariable(x);
  solution_map_.insert(make_pair(x, make_pair(builder, producer)));

  double constraint_modifier = producer->ProductionCapacity(problem.commodity);
  problem.interface.AddVarToConstraint(x, constraint_modifier,
                                       problem.constraint);

  double obj_modifier = producer->ProductionCost(problem.commodity);
  problem.interface.AddVarToObjFunction(x, obj_modifier);
}

// -------------------------------------------------------------------
void BuildingManager::ConstructBuildOrdersFromSolution(
  std::vector<BuildOrder>& orders,
  std::vector<VariablePtr>& solution) {
  using boost::any_cast;

  // construct the build orders
  for (int i = 0; i < solution.size(); i++) {
    int number = any_cast<int>(solution.at(i)->value());
    if (number > 0) {
      Builder* builder = solution_map_[solution.at(i)].first;
      supply_demand::CommodityProducer* producer = \
                                                   solution_map_[solution.at(i)].second;
      BuildOrder order(number, builder, producer);
      orders.push_back(order);
    }
  }
}
} // namespace action_building
} // namespace cyclus
