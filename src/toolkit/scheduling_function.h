#ifndef CYCLUS_SRC_TOOLKIT_SCHEDULING_FUNCTION_H_
#define CYCLUS_SRC_TOOLKIT_SCHEDULING_FUNCTION_H_

#include "context.h"
#include "symbolic_functions.h" //I may add a symbolic functions later...
#include "res_buf.h"
#include <vector>

namespace cyclus {

class Facility;

namespace toolkit {
//these are a kit of functions to help the archetype developer start their own DRE event scheduling.    
//the dev can use as many functions as they want in the GetSchedulingTime() override ... 
// because of that fact there is an time_stamp set with an associated commodity pair (which times they want to trade what) getter function 
//

class SchedulingFunctions { 

public:
    SchedulingFunctions(Facility* fac);

// deconstructor .. 

// void DemandDrivenRequests(ResBuf<cyclus::Material> res,std::set<std::string> commods);

// void ConstantRequest(int cycle_length, std::set<std::string> commods);

void FixIncSchedule();

void clear();

void InitialTrade();

int FacilityTime();

void Scheduler(int t, std::set<std::string> in_commods);

//void PredefinedSchedule(std::set<int> sched);

const std::set<int>& EventTime() const {//schedule
    return t_;
}

private:
    Facility* f_;
    std::set<int> t_; //this may be sketchy with clear(), should i not have a variable and just have sched. func return event times? 
    std::set<std::string> in_commods_;
};

}  // namespace toolkit
}  // namespace cyclus
#endif 