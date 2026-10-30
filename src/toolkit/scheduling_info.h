#ifndef CYCLUS_SRC_TOOLKIT_SCHEDULING_INFO_H_
#define CYCLUS_SRC_TOOLKIT_SCHEDULING_INFO_H_

// #include "res_buf.h"
// #include "material.h"
#include <set>

namespace cyclus {

// class Material; 

namespace toolkit {

struct SchedulingInfo {
    std::string method;
    std::set<std::string> commodities;
    // ResBuf<Material> resource_buffers;
    int increment;
    int cycle_length;
    std::set<int> schedule;
};

}  // namespace toolkit
}  // namespace cyclus

#endif
