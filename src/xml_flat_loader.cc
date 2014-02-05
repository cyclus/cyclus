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
    DynamicModule dyn(names[i]);
    Model* m = dyn.ConstructInstance(&ctx);
    subschemas += "<element name=\"" + names[i] + "\">\n";
    subschemas += m->schema() + "\n";
    subschemas += "</element>\n";
    ctx.DelModel(m);
    dyn.CloseLibrary();
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

  int num_protos = xqe.NElementsMatchingQuery("/*/prototype");
  for (int i = 0; i < num_protos; i++) {
    QueryEngine* qe = xqe.QueryElement("/*/prototype", i);
    QueryEngine* module_data = qe->QueryElement("model");
    std::string module_name = module_data->GetElementName();

    Model* model = modules_[module_name]->ConstructInstance(ctx_);
    model->InitFrom(qe);
    model->SetModelImpl(module_name);
    model->InitModuleMembers(module_data->QueryElement(module_name));

    CLOG(LEV_DEBUG3) << "Module '" << model->name()
                     << "' has had its module members initialized:";
    CLOG(LEV_DEBUG3) << " * Type: " << model->ModelType();
    CLOG(LEV_DEBUG3) << " * Implementation: " << model->ModelImpl() ;
    CLOG(LEV_DEBUG3) << " * ID: " << model->id();

    // register module
    ctx_->AddPrototype(model->name(), model);
  }

  int num_agents = xqe.NElementsMatchingQuery("/*/agent");
  std::map<std::string, Model*> agents; // map<name, agent>
  std::map<std::string, std::string> parents; // map<agent, parent>
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
      agent->Deploy();
    } else {
      agent->Deploy(agents[parents[name]]);
    }
  }
}

} // namespace cyclus

