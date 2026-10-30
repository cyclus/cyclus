#ifndef CYCLUS_SRC_TOOLKIT_SCHEDULING_FUNCTION_H_
#define CYCLUS_SRC_TOOLKIT_SCHEDULING_FUNCTION_H_

#include "context.h"
#include "symbolic_functions.h" 
#include "scheduling_info.h"
#include "res_buf.h"
#include <vector>
#include <functional> 
#include <set>

namespace cyclus {

class Facility; //forward declaration 

namespace toolkit {
//these are a kit of functions for the user and potentially archetype to use to establish a scheduling type for their agent.
//

class SchedulingFunctions { 

public:
    SchedulingFunctions(Facility* fac);

// deconstructor .. 


/// Demand driver registration process for agents based on space being available in a resource buffer.
/// The resource buffer should have an associated commodity to order with it 
/// for reactor archetype after a buffer is filled, the agent does not need to trade until some cycle length. 
void DemandDrivenRequests(ResBuf<cyclus::Material> res,std::set<std::string> commods, int cycle_length);


/// Cyclus operates on a fixed increment cycle of increment = 1 with a set of all commodities a consumer registers being ordered.
/// Users should be able to change the increment.
void FixIncSchedule(int increment);

void clear();

void InitialTrade();

int FacilityTime();

void Scheduler(int t, std::set<std::string> in_commods);

void PredefinedSchedule(std::set<int> sched);

void ScheduleMethod(SchedulingInfo info);

const std::set<int>& EventTime() const { 
    return t_;
};

private:
    Facility* f_;
    std::set<int> t_; // for future deregistration of events 
};

}  // namespace toolkit
}  // namespace cyclus
#endif 