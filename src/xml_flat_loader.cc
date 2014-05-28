// xml_flat_loader.cc

#include "xml_flat_loader.h"

#include "context.h"
#include "env.h"
#include "error.h"
#include "logger.h"
#include "agent.h"
#include "recorder.h"
#include "timer.h"
#include "infile_tree.h"

namespace cyclus {

std::string BuildFlatMasterSchema(std::string schema_path, std::string infile) {
  Timer ti;
  Recorder rec;
  Context ctx(&ti, &rec);

  std::stringstream schema("");
  LoadStringstreamFromFile(schema, schema_path);
  std::string master = schema.str();

  std::vector<AgentSpec> specs = ParseSpecs(infile);
  std::string subschemas;
  for (int i = 0; i < specs.size(); ++i) {
    Agent* m = DynamicModule::Make(&ctx, specs[i]);
    subschemas += "<element name=\"" + specs[i].alias() + "\">\n";
    subschemas += m->schema() + "\n";
    subschemas += "</element>\n";
    ctx.DelAgent(m);
  }

  // replace refs in master rng template file
  std::string search_str = std::string("@MODEL_SCHEMAS@");
  size_t pos = master.find(search_str);
  if (pos != std::string::npos) {
    master.replace(pos, search_str.size(), subschemas);
  }

  return master;
}

std::string XMLFlatLoader::master_schema() {
  return BuildFlatMasterSchema(schema_path_, file_);
}

void XMLFlatLoader::LoadInitialAgents() {
  InfileTree xqe(*parser_);

  // create prototypes
  int num_protos = xqe.NMatches("/*/prototype");
  for (int i = 0; i < num_protos; i++) {
    InfileTree* qe = xqe.SubTree("/*/prototype", i);
    std::string prototype = qe->GetString("name");
    AgentSpec spec = specs_[qe->GetString("archetype")];

    Agent* agent = DynamicModule::Make(ctx_, spec);

    // call manually without agent impl injected to keep all Agent state in a
    // single, consolidated db table
    agent->Agent::InfileToDb(qe, DbInit(agent, true));

    agent->InfileToDb(qe, DbInit(agent));
    rec_->Flush();

    std::vector<Cond> conds;
    conds.push_back(Cond("SimId", "==", rec_->sim_id()));
    conds.push_back(Cond("AgentId", "==", agent->id()));
    conds.push_back(Cond("SimTime", "==", static_cast<int>(0)));
    CondInjector ci(b_, conds);
    PrefixInjector pi(&ci, "AgentState");

    // call manually without agent impl injected
    agent->Agent::InitFrom(&pi);

    pi = PrefixInjector(&ci, "AgentState" + spec.Sanitize());
    agent->InitFrom(&pi);
    ctx_->AddPrototype(prototype, agent);
  }

  // retrieve agent hierarchy and initial inventories
  int num_agents = xqe.NMatches("/*/agent");
  std::map<std::string, std::string> protos;  // map<name, prototype>
  std::map<std::string, std::string> parents;  // map<agent, parent>
  std::set<std::string> agents; // set<agent_name>
  std::map<std::string, InfileTree*> invs; // map<agent, qe>;
  for (int i = 0; i < num_agents; i++) {
    InfileTree* qe = xqe.SubTree("/*/agent", i);
    std::string name = qe->GetString("name");
    std::string proto = qe->GetString("prototype");
    std::string parent = OptionalQuery<std::string>(qe, "parent", "");
    protos[name] = proto;
    parents[name] = parent;
    invs[name] = qe;
    agents.insert(name);
  }

  // build agents starting at roots (no parent) down.
  std::map<std::string, Agent*> built; // map<agent_name, agent_ptr>
  std::set<std::string>::iterator it = agents.begin();
  while (agents.size() > 0) {
    std::string name = *it;
    std::string proto = protos[name];
    std::string parent = parents[name];
    if (parent == "") {
      built[name] = BuildAgent(proto, NULL);
      ++it;
      agents.erase(name);
    } else if (built.count(parent) > 0) {
      built[name] = BuildAgent(proto, built[parent]);
      ++it;
      agents.erase(name);
    } else {
      ++it;
    }
    if (it == agents.end()) {
      it = agents.begin();
    }
  }
}

}  // namespace cyclus
