// xml_flat_loader.cc

#include "xml_flat_loader.h"

#include "context.h"
#include "env.h"
#include "error.h"
#include "logger.h"
#include "model.h"
#include "recorder.h"
#include "timer.h"
#include "xml_query_engine.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string BuildFlatMasterSchema(std::string schema_path) {
  Timer ti;
  Recorder rec;
  Context ctx(&ti, &rec);

  std::stringstream schema("");
  LoadStringstreamFromFile(schema, schema_path);
  std::string master = schema.str();

  std::vector<std::string> names = Env::ListModules();
  std::string subschemas;
  for (int i = 0; i < names.size(); ++i) {
    Model* m = DynamicModule::Make(&ctx, names[i]);
    subschemas += "<element name=\"" + names[i] + "\">\n";
    subschemas += m->schema() + "\n";
    subschemas += "</element>\n";
    ctx.DelModel(m);
  }

  // replace refs in master rng template file
  std::string search_str = std::string("@MODEL_SCHEMAS@");
  size_t pos = master.find(search_str);
  if (pos != std::string::npos) {
    master.replace(pos, search_str.size(), subschemas);
  }

  return master;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string XMLFlatLoader::master_schema() {
  return BuildFlatMasterSchema(schema_path_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFlatLoader::LoadInitialAgents() {
  XMLQueryEngine xqe(*parser_);
  DbInit di;

  // create prototypes
  int num_protos = xqe.NElementsMatchingQuery("/*/prototype");
  for (int i = 0; i < num_protos; i++) {
    QueryEngine* qe = xqe.QueryElement("/*/prototype", i);
    QueryEngine* module_data = qe->QueryElement("model");
    std::string module_name = module_data->GetElementName();
    std::string prototype = qe->GetElementContent("name");
    proto_qes_[prototype] = qe;

    Model* model = DynamicModule::Make(ctx_, module_name);
    model->set_model_impl(module_name);
    model->InfileToDb(qe, di);
    ctx_->AddPrototype(prototype, model);
    ctx_->NewDatum("Prototypes")
      ->AddVal("Prototype", prototype)
      ->AddVal("AgentId", model->id())
      ->AddVal("Implementation", module_name)
      ->Record();
  }

  // create initial agents
  int num_agents = xqe.NElementsMatchingQuery("/*/agent");
  std::map<std::string, std::string> protos;  // map<name, prototype>
  std::map<std::string, std::string> parents;  // map<agent, parent>
  std::set<std::string> agents; // set<agent_name>
  for (int i = 0; i < num_agents; i++) {
    QueryEngine* qe = xqe.QueryElement("/*/agent", i);
    std::string name = qe->GetElementContent("name");
    std::string proto = qe->GetElementContent("prototype");
    std::string parent = GetOptionalQuery<std::string>(qe, "parent", "");
    protos[name] = proto;
    parents[name] = parent;
    agents.insert(name);
  }

  // build agents starting at roots (no parent) down.
  std::map<std::string, Model*> built; // map<agent_name, agent_ptr>
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
    }
    if (it == agents.end()) {
      it = agents.begin();
    }
  }
}

}  // namespace cyclus
