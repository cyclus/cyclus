#include "scheduling_function.h"
#include "cyc_limits.h"
#include "facility.h"

namespace cyclus {
namespace toolkit {


SchedulingFunctions::SchedulingFunctions(Facility* fac): 
    f_(fac) {}

void SchedulingFunctions::Scheduler(int t, std::set<std::string> in_commods){
    f_-> context()-> RegisterRequesters(t, f_);
    f_-> context()-> RegisterCommoditiesTraded(t, in_commods);
    t_.insert(t);
} //archetype developers can also call this method to schedule some time directly with context
// from their own scheduling function/parameters

void SchedulingFunctions::FixIncSchedule(){
    Scheduler(FacilityTime()+  1, f_->GetInCommods());
}

void SchedulingFunctions::ConstantRequest(int cycle_length, std::set<std::string> commods){
    Scheduler(FacilityTime() + cycle_length, commods);
}

void SchedulingFunctions::DemandDrivenRequests(ResBuf<cyclus::Material> res,std::set<std::string> commods){
    if (res.space() > eps_rsrc()) {
        Scheduler(FacilityTime()+1, commods);
    }
    else {
        return;
    }
}

void SchedulingFunctions::PredefinedSchedule(std::set<int> sched){
    //in order to use this function the entire schedule of a facilities requests should be mapped. 
    //do not invoke parent tock in facilities that use this. Add additional EventSchedule() to EnterNotify() instead. 
    t_ = sched;
}

void SchedulingFunctions::clear(){
    t_.clear();
}

void SchedulingFunctions::InitialTrade(){
    Scheduler(FacilityTime(),f_->GetInCommods());
    //i changed this from the original
//   if(FacilityTime() == 0){ 
//     Scheduler(1, f_->GetInCommods());  
//   }
//   else if (FacilityTime() > 0){ //if during sim_time >0 register for sim_time
//     Scheduler(FacilityTime(),f_->GetInCommods());
//   }
}

int SchedulingFunctions::FacilityTime(){
    return f_->context()->time();
}

}  // namespace toolkit
}  // namespace cyclus