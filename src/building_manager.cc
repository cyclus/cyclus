#include "building_manager.h"

#include <boost/any.hpp>

#include "optim/cbc_solver.h"
#include "optim/solver.h"
#include "optim/solver_interface.h"

#include "error.h"
#include "logger.h"

using boost::any_cast;

namespace cyclus {
namespace action_building {

// -------------------------------------------------------------------
BuildOrder::BuildOrder(int n, action_building::Builder* b,
                       supply_demand::CommodityProducer* cp) :
  number(n),
  builder(b),
  producer(cp) { }

// -------------------------------------------------------------------
ProblemInstance::ProblemInstance(
  Commodity& commod,
  double demand,
  cyclus::optim::SolverInterface& sinterface,
  cyclus::optim::ConstraintPtr constr,
  std::vector<cyclus::optim::VariablePtr>& soln)
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
void BuildingManager::RegisterBuilder(action_building::Builder* builder) {
  if (builders_.find(builder) != builders_.end()) {
    throw KeyError("A manager is trying to register a builder twice.");
  } else {
    builders_.insert(builder);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BuildingManager::UnRegisterBuilder(action_building::Builder* builder) {
  if (builders_.find(builder) == builders_.end()) {
    throw KeyError("A manager is trying to unregister a builder not originally registered with it.");
  } else {
    builders_.erase(builder);
  }
}

// -------------------------------------------------------------------
std::vector<action_building::BuildOrder> BuildingManager::MakeBuildDecision(
  Commodity& commodity,
  double unmet_demand) {
  using std::vector;
  using optim::SolverPtr;
  using optim::CBCSolver;
  using optim::Constraint;
  using optim::ConstraintPtr;
  using optim::ObjFuncPtr;
  using optim::SolverInterface;
  //  using optim::ProblemInstance;
  using optim::VariablePtr;
  using optim::IntegerVariable;
  using optim::ObjectiveFunction;
  vector<BuildOrder> orders;

  if (unmet_demand > 0) {
    // set up solver and interface
    cyclus::optim::SolverPtr solver(new cyclus::optim::CBCSolver());
    cyclus::optim::SolverInterface csi(solver);

    // set up objective function
    cyclus::optim::ObjFuncPtr obj(
      new cyclus::optim::ObjectiveFunction(
        cyclus::optim::ObjectiveFunction::MIN));
    csi.RegisterObjFunction(obj);

    // set up constraint
    cyclus::optim::ConstraintPtr constraint(
      new cyclus::optim::Constraint(
        cyclus::optim::Constraint::GTEQ, unmet_demand));
    csi.RegisterConstraint(constraint);

    // set up variables, constraints, and objective function
    vector<cyclus::optim::VariablePtr> solution;
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
      cyclus::optim::VariablePtr x = solution.at(i);
      LOG(LEV_DEBUG2, "buildman") << "  * Type: " << x->name()
                                  << "  * Value: " << any_cast<int>(x->value());
    }

    // construct order
    ConstructBuildOrdersFromSolution(orders, solution);
  }

  return orders;
}

// -------------------------------------------------------------------
void BuildingManager::SetUpProblem(action_building::ProblemInstance& problem) {
  solution_map_ = std::map < optim::VariablePtr,
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
  action_building::Builder* builder,
  action_building::ProblemInstance& problem) {
  using std::make_pair;
  using optim::Variable;
  using optim::VariablePtr;
  using optim::IntegerVariable;
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
  std::vector<action_building::BuildOrder>& orders,
  std::vector<optim::VariablePtr>& solution) {
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
