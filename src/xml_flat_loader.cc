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
  std::map<std::string, QueryEngine*> proto_qes;
  for (int i = 0; i < num_protos; i++) {
    QueryEngine* qe = xqe.QueryElement("/*/prototype", i);
    QueryEngine* module_data = qe->QueryElement("model");
    std::string module_name = module_data->GetElementName();
    std::string prototype = qe->GetElementContent("name");
    proto_qes[prototype] = qe;

    Model* model = DynamicModule::Make(ctx_, module_name);
    model->SetModelImpl(module_name);
    model->InfileToDb(qe, di);
    ctx_->AddPrototype(prototype, model);
    ctx_->NewDatum("Prototypes")
      ->AddVal("Prototype", prototype)
      ->AddVal("AgentId", model->id())
      ->AddVal("Implementation", module_name)
      ->Record();
  }

  // build initial agents
  int num_agents = xqe.NElementsMatchingQuery("/*/agent");
  std::map<std::string, Model*> agents;  // map<name, agent>
  std::map<std::string, std::string> protos;  // map<name, prototype>
  std::map<std::string, std::string> parents;  // map<agent, parent>
  for (int i = 0; i < num_agents; i++) {
    QueryEngine* qe = xqe.QueryElement("/*/agent", i);
    std::string name = qe->GetElementContent("name");
    std::string proto = qe->GetElementContent("prototype");
    std::string parent = GetOptionalQuery<std::string>(qe, "parent", "");
    agents[name] = ctx_->CreateModel<Model>(proto);
    protos[name] = proto;
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
    agent->InfileToDb(proto_qes[protos[name]], di);
  }
}

}  // namespace cyclus
