#include "scheduling_function.h"
#include "cyc_limits.h"
#include "facility.h"

namespace cyclus {
namespace toolkit {

SchedulingFunctions::SchedulingFunctions(Facility* fac): 
    f_(fac)
    {}

void SchedulingFunctions::ScheduleMethod(SchedulingInfo info){
    if (info.method == "fixed"){
        FixIncSchedule(info.increment);
    }
    // if (info.method == "DemandDrivenRequest"){
    //     DemandDrivenRequests(info.resource_buffers, info.commodities, info.cycle_length);
    // }
    if (info.method == "PredefinedSchedule"){
        PredefinedSchedule(info.schedule);
    }
}

void SchedulingFunctions::Scheduler(int t, std::set<std::string> in_commods){
    if (*t_.lower_bound(FacilityTime()) <= t){ // means no events are scheduled before the closest one  
        f_-> context()-> RegisterRequesters(t, f_); // is this needed? 
        f_-> context()-> RegisterCommoditiesTraded(t, in_commods);
        t_.insert(t);
    }
} //archetype developers can also call this method to schedule some time directly with context
// from their own scheduling function/parameters

void SchedulingFunctions::FixIncSchedule(int increment = 1){
    ///
    Scheduler(FacilityTime() + increment, f_->GetInCommods());
}

void SchedulingFunctions::DemandDrivenRequests(ResBuf<cyclus::Material> res,std::set<std::string> commods, int cycle_length = 0){
    // specify that you also need a cycle length and if you don tnee material you use it x cycle length in the future 

    if (res.space() > eps_rsrc()) {
        Scheduler(FacilityTime()+1, commods);
    }
    else if (cycle_length > 0){
        Scheduler(FacilityTime()+cycle_length, commods);
    }
}

void SchedulingFunctions::PredefinedSchedule(std::set<int> sched){
    //// loop through a schedule set and ensure that in the agents EventQuest function
    // TBD 
}

void SchedulingFunctions::clear(){
    /// for future deregistration of events 
    t_.clear();
}

void SchedulingFunctions::InitialTrade(){
    /// establish an initial trade of all commodities 
    Scheduler(0,f_->GetInCommods());
}

int SchedulingFunctions::FacilityTime(){
    /// Get the time at which the facility is scheduling event to ensure all
    // schedules are in the future.
    return f_->context()->time();
}

}  // namespace toolkit
}  // namespace cyclus