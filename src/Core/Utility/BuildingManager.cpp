#include "BuildingManager.h"

#include <boost/any.hpp>

#include "Logger.h"
#include "Solver.h"
#include "SolverInterface.h"
#include "CBCSolver.h"
#include "CycException.h"

using namespace std;
using boost::any_cast;
using namespace Cyclopts;
using namespace SupplyDemand;
using namespace ActionBuilding;

// -------------------------------------------------------------------
BuildOrder::BuildOrder(int n, ActionBuilding::Builder* b,
                       SupplyDemand::CommodityProducer* cp) :
  number(n),
  builder(b),
  producer(cp)
{}

// -------------------------------------------------------------------
ProblemInstance::ProblemInstance(Commodity& commod, double demand, 
                                 Cyclopts::SolverInterface& sinterface, 
                                 Cyclopts::ConstraintPtr constr, 
                                 std::vector<Cyclopts::VariablePtr>& soln) :
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
std::vector<ActionBuilding::BuildOrder> BuildingManager::makeBuildDecision(Commodity& commodity, 
                                                                           double unmet_demand) 
{
  vector<BuildOrder> orders;

  if (unmet_demand > 0) 
    {
      // set up solver and interface
      SolverPtr solver(new CBCSolver());
      SolverInterface csi(solver);

      // set up objective function
      ObjFuncPtr obj(new ObjectiveFunction(ObjectiveFunction::MIN));
      csi.registerObjFunction(obj);

      // set up constraint
      ConstraintPtr constraint(new Constraint(Constraint::GTEQ,unmet_demand));
      csi.registerConstraint(constraint);

      // set up variables, constraints, and objective function
      vector<VariablePtr> solution;
      ProblemInstance problem(commodity,unmet_demand,csi,constraint,solution);
      setUpProblem(problem);

      // report problem
      LOG(LEV_DEBUG2,"buildman") << "Building Manager is solving a decision problem with:";
      LOG(LEV_DEBUG2,"buildman") << "  * Objective Function: " << obj->print();
      LOG(LEV_DEBUG2,"buildman") << "  * Constraint: " << constraint->print();
  
      // solve
      csi.solve();

      // report solution
      LOG(LEV_DEBUG2,"buildman") << "Building Manager has solved a decision problem with:";
      LOG(LEV_DEBUG2,"buildman") << "  * Types of Prototypes to build: " << solution.size();
      for (int i = 0; i < solution.size(); i++)
        {
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
  solution_map_ = map< VariablePtr, pair<Builder*,CommodityProducer*> >();

  set<Builder*>::iterator builder_it;
  for (builder_it = builders_.begin(); builder_it != builders_.end(); builder_it++)
    {
      Builder* builder = (*builder_it);
      
      set<CommodityProducer*>::iterator producer_it;
      for (producer_it = builder->beginningProducer(); producer_it != builder->endingProducer(); producer_it++)
        {
          CommodityProducer* producer = (*producer_it);
          if (producer->producesCommodity(problem.commodity))
            {
              addProducerVariableToProblem(producer,builder,problem);
            }
        }
    }
}

// -------------------------------------------------------------------
void BuildingManager::addProducerVariableToProblem(SupplyDemand::CommodityProducer* producer,
                                                   ActionBuilding::Builder* builder,
                                                   ActionBuilding::ProblemInstance& problem)
{
  VariablePtr x(new IntegerVariable(0,Variable::INF));
  problem.solution.push_back(x);
  problem.interface.registerVariable(x);
  solution_map_.insert(make_pair(x,make_pair(builder,producer)));

  double constraint_modifier = producer->productionCapacity(problem.commodity);
  problem.interface.addVarToConstraint(x,constraint_modifier,problem.constraint);

  double obj_modifier = producer->productionCost(problem.commodity);
  problem.interface.addVarToObjFunction(x,obj_modifier);
}

// -------------------------------------------------------------------
void BuildingManager::constructBuildOrdersFromSolution(std::vector<ActionBuilding::BuildOrder>& orders,
                                                       std::vector<Cyclopts::VariablePtr>& solution)
{
  // construct the build orders
  for (int i = 0; i < solution.size(); i++) 
    {
      int number = any_cast<int>(solution.at(i)->value());
      if (number > 0) 
        {
          Builder* builder = solution_map_[solution.at(i)].first;
          CommodityProducer* producer = solution_map_[solution.at(i)].second;
          BuildOrder order(number,builder,producer);
          orders.push_back(order);
        }
    }
}
