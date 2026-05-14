#include "scheduling_function.h"
#include "cyc_limits.h"

namespace cyclus {
namespace toolkit {

SchedulingFunctions::SchedulingFunctions(Facility* fac): //does this even need to be facility? 
    f(fac) {}

void SchedulingFunctions::FixIncSchedule(){
    t_.insert(f->context()->time() + 1); //all these can be converted to context()->time() instead? //can be initialized only in enter notify
}

void SchedulingFunctions::ConstantRequest(int cycle_length){
    t_.insert(f->context()->time() + cycle_length);
}

void SchedulingFunctions::DemandDrivenRequests(ResBuf<cyclus::Material> res){
    if (res.space() > eps_rsrc()) {
        t_.insert(f->context()->time()+1);
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

}  // namespace toolkit
}  // namespace cyclus