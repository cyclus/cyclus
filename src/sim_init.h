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
  SimInit() {};

  Init(InitBackend* b, int t) {
    int num_agents = xqe.NElementsMatchingQuery("/*/agent");
    std::map<std::string, Model*> agents;  // map<name, agent>
    std::map<std::string, std::string> parents;  // map<agent, parent>
    for (int i = 0; i < num_agents; i++) {
      QueryEngine* qe = xqe.QueryElement("/*/agent", i);
      std::string name = qe->GetElementContent("name");
      std::string proto = qe->GetElementContent("prototype");
      std::string parent = GetOptionalQuery<std::string>(qe, "parent", "");
      agents[name] = ctx_->CreateModel<Model>(proto);
      parents[name] = parent;
    }

    std::map<std::string, Model*>::iterator it;
    for (it = agents.begin(); it != agents.end(); ++it) {
      std::string name = it->first;
      Model* agent = it->second;
      if (parents[name] == "") {
        agent->Build();
      } else {
        agent->Build(agents[parents[name]]);
        agents[parents[name]]->BuildNotify(agent);
      }
    }
  };

 private:
  InitBackend* back_;
};

} // namespace cyclus

#endif
