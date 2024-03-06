// Implements file reader for an XML format
#include "xml_file_loader.h"

#include <algorithm>
#include <fstream>
#include <set>
#include <streambuf>

#include <boost/filesystem.hpp>
#include <libxml++/libxml++.h>

#include "agent.h"
#include "blob.h"
#include "context.h"
#include "cyc_std.h"
#include "env.h"
#include "error.h"
#include "exchange_solver.h"
#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "infile_tree.h"
#include "logger.h"
#include "sim_init.h"
#include "toolkit/infile_converters.h"

namespace cyclus {

namespace fs = boost::filesystem;

void LoadRawStringstreamFromFile(std::stringstream& stream, std::string file) {
  std::ifstream file_stream(file.c_str());
  if (!file_stream) {
    throw IOError("The file '" + file + "' could not be loaded.");
  }

  stream << file_stream.rdbuf();
  file_stream.close();
}

void LoadStringstreamFromFile(std::stringstream& stream, std::string file, std::string format) {
  std::string inext;
  if (format == "none") {
    LoadRawStringstreamFromFile(stream, file);
    inext = fs::path(file).extension().string();
  } else {
    stream << file;
  }
  if (inext == ".json" || format == "json") {
    std::string inxml = cyclus::toolkit::JsonToXml(stream.str());
    stream.str(inxml);
  } else if (inext == ".py" || format == "py") {
    std::string inxml = cyclus::toolkit::PyToXml(stream.str());
    stream.str(inxml);
  }
}

std::string LoadStringFromFile(std::string file, std::string format) {
  std::stringstream input;
  LoadStringstreamFromFile(input, file, format);
  return input.str();
}

std::vector<AgentSpec> ParseSpecs(std::string infile, std::string format) {
  std::stringstream input;
  LoadStringstreamFromFile(input, infile, format);
  XMLParser parser_;
  parser_.Init(input);
  InfileTree xqe(parser_);

  std::vector<AgentSpec> specs;
  std::set<std::string> unique;

  std::string p = "/simulation/archetypes/spec";
  int n = xqe.NMatches(p);
  for (int i = 0; i < n; ++i) {
    AgentSpec spec(xqe.SubTree(p, i));
    if (unique.count(spec.str()) == 0) {
      specs.push_back(spec);
      unique.insert(spec.str());
    }
  }

  if (specs.size() == 0) {
    throw ValidationError("failed to parse archetype specs from input file");
  }

  return specs;
}

std::string BuildMasterSchema(std::string schema_path, std::string infile, std::string format) {
  Timer ti;
  Recorder rec;
  Context ctx(&ti, &rec);

  std::stringstream schema("");
  LoadStringstreamFromFile(schema, schema_path);
  std::string master = schema.str();

  std::vector<AgentSpec> specs = ParseSpecs(infile, format);

  std::map<std::string, std::string> subschemas;

  // force element types to exist so we always replace the config string
  subschemas["region"] = "";
  subschemas["inst"] = "";
  subschemas["facility"] = "";

  for (int i = 0; i < specs.size(); ++i) {
    Agent* m = DynamicModule::Make(&ctx, specs[i]);
    subschemas[m->kind()] += "<element name=\"" + specs[i].alias() + "\">\n";
    subschemas[m->kind()] += m->schema() + "\n";
    subschemas[m->kind()] += "</element>\n";
    ctx.DelAgent(m);
  }

  // replace refs in master rng template file
  std::map<std::string, std::string>::iterator it;
  for (it = subschemas.begin(); it != subschemas.end(); ++it) {
    std::string search_str = std::string("@") + it->first + std::string("_REFS@");
    size_t pos = master.find(search_str);
    if (pos != std::string::npos) {
      master.replace(pos, search_str.size(), it->second);
    }
  }

  return master;
}

Composition::Ptr ReadRecipe(InfileTree* qe) {
  bool atom_basis;
  std::string basis_str = qe->GetString("basis");
  if (basis_str == "atom") {
    atom_basis = true;
  } else if (basis_str == "mass") {
    atom_basis = false;
  } else {
    throw IOError(basis_str + " basis is not 'mass' or 'atom'.");
  }

  double value;
  int key;
  std::string query = "nuclide";
  int nnucs = qe->NMatches(query);
  CompMap v;
  for (int i = 0; i < nnucs; i++) {
    InfileTree* nuclide = qe->SubTree(query, i);
    key = pyne::nucname::id(nuclide->GetString("id"));
    value = strtod(nuclide->GetString("comp").c_str(), NULL);
    v[key] = value;
    CLOG(LEV_DEBUG3) << "  Nuclide: " << key << " Value: " << v[key];
  }

  if (atom_basis) {
    return Composition::CreateFromAtom(v);
  } else {
    return Composition::CreateFromMass(v);
  }
}

XMLFileLoader::XMLFileLoader(Recorder* r,
                             QueryableBackend* b,
                             std::string schema_file,
                             const std::string input_file,
                             const std::string format, bool ms_print) : b_(b), rec_(r) {
  ctx_ = new Context(&ti_, rec_);

  schema_path_ = schema_file;
  file_ = input_file;
  format_ = format;
  std::stringstream input;
  LoadStringstreamFromFile(input, file_, format);
  parser_ = boost::shared_ptr<XMLParser>(new XMLParser());
  parser_->Init(input);
  ms_print_ = ms_print;
  std::stringstream ss;
  parser_->Document()->write_to_stream_formatted(ss);
  ctx_->NewDatum("InputFiles")
      ->AddVal("Data", Blob(ss.str()))
      ->Record();
}

XMLFileLoader::~XMLFileLoader() {
  delete ctx_;
}

std::string XMLFileLoader::master_schema() {
  return BuildMasterSchema(schema_path_, file_, format_);
}

void XMLFileLoader::LoadSim() {
  std::stringstream ss(master_schema());
  if(ms_print_){
    std::cout << master_schema() << std::endl;
  }
  parser_->Validate(ss);
  LoadControlParams();  // must be first
  LoadSolver();
  LoadRecipes();
  LoadPackages();
  LoadSpecs();
  LoadInitialAgents();  // must be last
  SimInit::Snapshot(ctx_);
  rec_->Flush();
}

void XMLFileLoader::LoadSolver() {
  using std::string;
  InfileTree xqe(*parser_);
  InfileTree* qe;
  std::string query = "/*/commodity";

  std::map<std::string, double> commod_priority;
  std::string name;
  double priority;
  int num_commods = xqe.NMatches(query);
  for (int i = 0; i < num_commods; i++) {
    qe = xqe.SubTree(query, i);
    name = qe->GetString("name");
    priority = OptionalQuery<double>(qe, "solution_priority", -1);
    commod_priority[name] = priority;
  }

  ProcessCommodities(&commod_priority);
  std::map<std::string, double>::iterator it;
  for (it = commod_priority.begin(); it != commod_priority.end(); ++it) {
    ctx_->NewDatum("CommodPriority")
        ->AddVal("Commodity", it->first)
        ->AddVal("SolutionPriority", it->second)
        ->Record();
  }

  // now load the solver info
  string config = "config";
  string greedy = "greedy";
  string coinor = "coin-or";
  string solver_name = greedy;
  bool exclusive = ExchangeSolver::kDefaultExclusive;
  if (xqe.NMatches("/*/control/solver") == 1) {
    qe = xqe.SubTree("/*/control/solver");
    if (qe->NMatches(config) == 1) {
      solver_name = qe->SubTree(config)->GetElementName(0);
    }
    exclusive = cyclus::OptionalQuery<bool>(qe, "allow_exclusive_orders",
                                            exclusive);

    // @TODO remove this after release 1.5
    // check for deprecated input values
    if (qe->NMatches(std::string("exclusive_orders_only")) != 0) {
      std::stringstream ss;
      ss << "Use of 'exclusive_orders_only' is deprecated."
         << " Please see http://fuelcycle.org/user/input_specs/control.html";
      Warn<DEPRECATION_WARNING>(ss.str());
    }
  }

  if (!exclusive) {
    std::stringstream ss;
    ss << "You have set allow_exclusive_orders to False."
       << " Many archetypes (e.g., :cycamore:Reactor) will not work"
       << " as intended with this feature turned off.";
    Warn<VALUE_WARNING>(ss.str());
  }

  ctx_->NewDatum("SolverInfo")
      ->AddVal("Solver", solver_name)
      ->AddVal("ExclusiveOrders", exclusive)
      ->Record();

  // now load the actual solver
  if (solver_name == greedy) {
    query = string("/*/control/solver/config/greedy/preconditioner");
    string precon_name = cyclus::OptionalQuery<string>(&xqe, query, greedy);
    ctx_->NewDatum("GreedySolverInfo")
      ->AddVal("Preconditioner", precon_name)
      ->Record();
  } else if (solver_name == coinor) {
    query = string("/*/control/solver/config/coin-or/timeout");
    double timeout = cyclus::OptionalQuery<double>(&xqe, query, -1);
    query = string("/*/control/solver/config/coin-or/verbose");
    bool verbose = cyclus::OptionalQuery<bool>(&xqe, query, false);
    query = string("/*/control/solver/config/coin-or/mps");
    bool mps = cyclus::OptionalQuery<bool>(&xqe, query, false);
    ctx_->NewDatum("CoinSolverInfo")
      ->AddVal("Timeout", timeout)
      ->AddVal("Verbose", verbose)
      ->AddVal("Mps", mps)
      ->Record();
  } else {
    throw ValueError("unknown solver name: " + solver_name);
  }
}

void XMLFileLoader::ProcessCommodities(
  std::map<std::string, double>* commod_priority) {
  double max = std::max_element(
                 commod_priority->begin(),
                 commod_priority->end(),
                 SecondLT< std::pair<std::string, double> >())->second;
  if (max < 1) {
    max = 0;  // in case no priorities are specified
  }

  std::map<std::string, double>::iterator it;
  for (it = commod_priority->begin();
       it != commod_priority->end();
       ++it) {
    if (it->second < 1) {
      it->second = max + 1;
    }
    CLOG(LEV_INFO1) << "Commodity priority for " << it->first
                    << " is " << it->second;
  }
}

void XMLFileLoader::LoadRecipes() {
  InfileTree xqe(*parser_);

  std::string query = "/*/recipe";
  int num_recipes = xqe.NMatches(query);
  for (int i = 0; i < num_recipes; i++) {
    InfileTree* qe = xqe.SubTree(query, i);
    std::string name = qe->GetString("name");
    CLOG(LEV_DEBUG3) << "loading recipe: " << name;
    Composition::Ptr comp = ReadRecipe(qe);
    comp->Record(ctx_);
    ctx_->AddRecipe(name, comp);
  }
}

void XMLFileLoader::LoadPackages() {
  InfileTree xqe(*parser_);
  // create default package
  ctx_->AddPackage("default", 0, std::numeric_limits<int>::max(), "first");

  std::string query = "/*/package";
  int num_packages = xqe.NMatches(query);
  for (int i = 0; i < num_packages; i++) {
    InfileTree* qe = xqe.SubTree(query, i);
    std::string name = cyclus::OptionalQuery<std::string>(qe, "name", "default");
    CLOG(LEV_DEBUG3) << "loading package: " << name;
    
    double fill_min = cyclus::OptionalQuery<double>(qe, "fill_min", eps());
    double fill_max = cyclus::OptionalQuery<double>(qe, "fill_max", std::numeric_limits<double>::max());
    
    std::string strategy = cyclus::OptionalQuery<std::string>(qe, "strategy", "first");

    boost::shared_ptr<Package> p = Package::Create(name, fill_min, fill_max, strategy);
    ctx_->AddPackage(name, fill_min, fill_max, strategy);
  }
}

void XMLFileLoader::LoadSpecs() {
  std::vector<AgentSpec> specs = ParseSpecs(file_, format_);
  for (int i = 0; i < specs.size(); ++i) {
    specs_[specs[i].alias()] = specs[i];
  }
}

void XMLFileLoader::LoadInitialAgents() {
  std::map<std::string, std::string> schema_paths;
  schema_paths["Region"] = "/*/region";
  schema_paths["Inst"] = "/*/region/institution";
  schema_paths["Facility"] = "/*/facility";

  InfileTree xqe(*parser_);

  // create prototypes
  std::string prototype;  // defined here for force-create AgentExit tbl
  std::map<std::string, std::string>::iterator it;
  for (it = schema_paths.begin(); it != schema_paths.end(); it++) {
    int num_agents = xqe.NMatches(it->second);
    for (int i = 0; i < num_agents; i++) {
      InfileTree* qe = xqe.SubTree(it->second, i);
      prototype = qe->GetString("name");
      std::string alias = qe->SubTree("config")->GetElementName(0);
      AgentSpec spec = specs_[alias];

      Agent* agent = DynamicModule::Make(ctx_, spec);

      // call manually without agent impl injected to keep all Agent state in a
      // single, consolidated db table
      agent->Agent::InfileToDb(qe, DbInit(agent, true));

      agent->InfileToDb(qe, DbInit(agent));
      rec_->Flush();

      std::vector<Cond> conds;
      conds.push_back(Cond("SimId", "==", rec_->sim_id()));
      conds.push_back(Cond("SimTime", "==", static_cast<int>(0)));
      conds.push_back(Cond("AgentId", "==", agent->id()));
      CondInjector ci(b_, conds);
      PrefixInjector pi(&ci, "AgentState");

      // call manually without agent impl injected
      agent->Agent::InitFrom(&pi);

      pi = PrefixInjector(&ci, "AgentState" + spec.Sanitize());
      agent->InitFrom(&pi);
      ctx_->AddPrototype(prototype, agent);
    }
  }

  // build initial agent instances
  int nregions = xqe.NMatches(schema_paths["Region"]);
  for (int i = 0; i < nregions; ++i) {
    InfileTree* qe = xqe.SubTree(schema_paths["Region"], i);
    std::string region_proto = qe->GetString("name");
    Agent* reg = BuildAgent(region_proto, NULL);

    int ninsts = qe->NMatches("institution");
    for (int j = 0; j < ninsts; ++j) {
      InfileTree* qe2 = qe->SubTree("institution", j);
      std::string inst_proto = qe2->GetString("name");
      Agent* inst = BuildAgent(inst_proto, reg);

      int nfac = qe2->NMatches("initialfacilitylist/entry");
      for (int k = 0; k < nfac; ++k) {
        InfileTree* qe3 = qe2->SubTree("initialfacilitylist/entry", k);
        std::string fac_proto = qe3->GetString("prototype");

        int number = atoi(qe3->GetString("number").c_str());
        for (int z = 0; z < number; ++z) {
          Agent* fac = BuildAgent(fac_proto, inst);
        }
      }
    }
  }
}

Agent* XMLFileLoader::BuildAgent(std::string proto, Agent* parent) {
  Agent* m = ctx_->CreateAgent<Agent>(proto);
  m->Build(parent);
  if (parent != NULL) {
    parent->BuildNotify(m);
  }
  return m;
}

void XMLFileLoader::LoadControlParams() {
  InfileTree xqe(*parser_);
  std::string query = "/*/control";
  InfileTree* qe = xqe.SubTree(query);

  std::string handle;
  if (qe->NMatches("simhandle") > 0) {
    handle = qe->GetString("simhandle");
  }

  // get duration
  std::string dur_str = qe->GetString("duration");
  int dur = strtol(dur_str.c_str(), NULL, 10);
  // get start month
  std::string m0_str = qe->GetString("startmonth");
  int m0 = strtol(m0_str.c_str(), NULL, 10);
  // get start year
  std::string y0_str = qe->GetString("startyear");
  int y0 = strtol(y0_str.c_str(), NULL, 10);
  // get decay mode
  std::string d = OptionalQuery<std::string>(qe, "decay", "manual");

  SimInfo si(dur, y0, m0, handle, d);

  si.explicit_inventory = OptionalQuery<bool>(qe, "explicit_inventory", false);
  si.explicit_inventory_compact = OptionalQuery<bool>(qe, "explicit_inventory_compact", false);

  // get time step duration
  si.dt = OptionalQuery<int>(qe, "dt", kDefaultTimeStepDur);

  // get epsilon
  double eps_ = OptionalQuery<double>(qe, "tolerance_generic", 1e-6);
  cy_eps = si.eps = eps_;

  // get epsilon resources
  double eps_rsrc_ = OptionalQuery<double>(qe, "tolerance_resource", 1e-6);
  cy_eps_rsrc = si.eps_rsrc = eps_rsrc_;

    // get seed
  si.seed = OptionalQuery<int>(qe, "seed", kDefaultSeed);

  // get stride
  si.stride = OptionalQuery<int>(qe, "stride", kDefaultStride);

  ctx_->InitSim(si);
}

}  // namespace cyclus
