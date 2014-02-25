#ifndef CYCLUS_SIM_INIT_H_
#define CYCLUS_SIM_INIT_H_

#include <map>
#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>

#include "composition.h"
#include "dynamic_module.h"
#include "query_engine.h"
#include "xml_parser.h"

namespace cyclus {

class Context;

/// a class that encapsulates the methods needed to load input to
/// a cyclus simulation from xml
class SimInit {
 public:
  SimInit(std::string input_file = "") : infile_(input_file) {};

  Run() {
    if (infile_ != "") {

    }
  };

 private:
  std::string infile_;
};

} // namespace cyclus

#endif
