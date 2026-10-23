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
        FixIncSchedule();
    }
    if (info.method == "ConstantRequest"){
        ConstantRequest(info.cycle_length, info.commodities);
    }
    // if (info.method == "DemandDrivenRequest"){
    //     DemandDrivenRequests(info.resource_buffers, info.commodities);
    // }
    if (info.method == "PredefinedSchedule"){
        PredefinedSchedule(info.schedule);
    }
}

void SchedulingFunctions::Scheduler(int t, std::set<std::string> in_commods){
    f_-> context()-> RegisterRequesters(t, f_); // technically i dont even need this 
    f_-> context()-> RegisterCommoditiesTraded(t, in_commods);
    t_.insert(t);
} //archetype developers can also call this method to schedule some time directly with context
// from their own scheduling function/parameters

void SchedulingFunctions::FixIncSchedule(){
    //Scheduler(FacilityTime() + 1, f_->GetInCommods());
    Scheduler(FacilityTime() + 5, f_->GetInCommods());
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
    Scheduler(0,f_->GetInCommods());
}

int SchedulingFunctions::FacilityTime(){
    return f_->context()->time();
}

}  // namespace toolkit
}  // namespace cyclus