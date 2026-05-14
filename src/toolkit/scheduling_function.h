#ifndef CYCLUS_SRC_TOOLKIT_SCHEDULING_FUNCTION_H_
#define CYCLUS_SRC_TOOLKIT_SCHEDULING_FUNCTION_H_

#include "context.h"
#include "symbolic_functions.h" //I may add a symbolic functions later...
#include "facility.h"
#include "res_buf.h"
#include <vector>

namespace cyclus {
namespace toolkit {
//these are a kit of functions to help the archetype developer start their own DRE event scheduling.    

class SchedulingFunctions { 

public:
    SchedulingFunctions(Facility* fac);

// deconstructor .. 

void DemandDrivenRequests(ResBuf<cyclus::Material> res);

void ConstantRequest(int cycle_length);

void FixIncSchedule();

void clear();

void PredefinedSchedule(std::set<int> sched);

const std::set<int>& EventTime() const {//schedule
    return t_;
  }

private:
    Facility* f;
    std::set<int> t_; //this may be sketchy with clear(), should i not have a variable and just have sched. func return event times? 
};

}  // namespace toolkit
}  // namespace cyclus
#endif 