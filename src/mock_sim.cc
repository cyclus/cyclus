#include "mock_sim.h"

#include "cyclus.h"
#include "sim_init.h"
#include "pyhooks.h"
#include <sstream>

namespace cyclus {

// The code for this function was copied with minor adjustements from
// XMLFileLoader::LoadInitialAgents
void InitAgent(Agent* a, std::stringstream& config, Recorder* rec,
               SqliteBack* back) {
  XMLParser parser_;
  parser_.Init(config);
  InfileTree xqe(parser_);

  a->Agent::InfileToDb(&xqe, DbInit(a, true));
  a->InfileToDb(&xqe, DbInit(a));
  rec->Flush();

  std::vector<Cond> conds;
  conds.push_back(Cond("SimId", "==", rec->sim_id()));
  conds.push_back(Cond("SimTime", "==", static_cast<int>(0)));
  conds.push_back(Cond("AgentId", "==", a->id()));
  CondInjector ci(back, conds);

  // call manually without agent impl injected
  PrefixInjector pi(&ci, "AgentState");
  a->Agent::InitFrom(&pi);

  pi = PrefixInjector(&ci, "AgentState" + AgentSpec(a->spec()).Sanitize());
  a->InitFrom(&pi);
}

///////// MockAgent ////////////

int MockAgent::nextid_ = 0;

MockAgent::MockAgent(Context* ctx, Recorder* rec, SqliteBack* b, bool is_source)
    : ctx_(ctx),
      rec_(rec),
      back_(b),
      source_(is_source),
      cap_(cyclus::CY_LARGE_DOUBLE),
      lifetime_(-1),
      start_(0) {
  std::stringstream ss;
  if (is_source) {
    ss << "source";
  } else {
    ss << "sink";
  }
  ss << nextid_++;
  proto_ = ss.str();
}

MockAgent MockAgent::commod(std::string commod) {
  commod_ = commod;
  return *this;
}
MockAgent MockAgent::recipe(std::string recipe) {
  recipe_ = recipe;
  return *this;
}
MockAgent MockAgent::capacity(double cap) {
  cap_ = cap;
  return *this;
}
MockAgent MockAgent::start(int t) {
  start_ = t;
  return *this;
}
MockAgent MockAgent::lifetime(int duration) {
  lifetime_ = duration;
  return *this;
}

std::string MockAgent::Finalize() {
  AgentSpec spec(":agents:Sink");
  if (source_) {
    spec = AgentSpec(":agents:Source");
  }

  std::stringstream xml;
  xml << "<facility><name>" << proto_ << "</name><lifetime>" << lifetime_
      << "</lifetime><config><SrcSnkAgent>";

  if (source_) {
    xml << "<commod>" << commod_ << "</commod>";
  } else {
    xml << "<in_commods><val>" << commod_ << "</val></in_commods>";
  }
  xml << "<capacity>" << cap_ << "</capacity>";
  if (recipe_ != "") {
    xml << "<recipe_name>" << recipe_ << "</recipe_name>";
  }
  xml << "</SrcSnkAgent></config></facility>";

  Agent* a = DynamicModule::Make(ctx_, spec);
  InitAgent(a, xml, rec_, back_);

  ctx_->AddPrototype(proto_, a);

  if (start_ == 0) {
    a = ctx_->CreateAgent<Agent>(proto_);
    a->Build(NULL);
  } else {
    ctx_->SchedBuild(NULL, proto_, start_);
  }
  return proto_;
}

///////// MockSim ////////////
MockSim::MockSim(int duration)
    : ctx_(&ti_, &rec_), back_(NULL), agent(NULL) {
  Env::SetNucDataPath();
  warn_limit = 0;
  back_ = new SqliteBack(":memory:");
  rec_.RegisterBackend(back_);
  ctx_.InitSim(SimInfo(duration));
}

MockSim::MockSim(AgentSpec spec, std::string config, int duration)
    : ctx_(&ti_, &rec_), back_(NULL), agent(NULL) {
  Env::SetNucDataPath();
  warn_limit = 0;
  back_ = new SqliteBack(":memory:");
  rec_.RegisterBackend(back_);
  ctx_.InitSim(SimInfo(duration));

  Agent* a = DynamicModule::Make(&ctx_, spec);

  std::stringstream xml;
  xml << "<facility><name>agent_being_tested</name><config><foo>" << config
      << "</foo></config></facility>";
  InitAgent(a, xml, &rec_, back_);

  ctx_.AddPrototype(a->prototype(), a);
  agent = ctx_.CreateAgent<Agent>(a->prototype());
}

MockSim::MockSim(AgentSpec spec, std::string config, int duration, int lifetime)
    : ctx_(&ti_, &rec_), back_(NULL), agent(NULL) {
  Env::SetNucDataPath();
  warn_limit = 0;
  back_ = new SqliteBack(":memory:");
  rec_.RegisterBackend(back_);
  ctx_.InitSim(SimInfo(duration));

  Agent* a = DynamicModule::Make(&ctx_, spec);

  std::stringstream xml;
  xml << "<facility>"
      << "<lifetime>" << lifetime << "</lifetime>"
      << "<name>agent_being_tested</name>"
      << "<config><foo>" << config << "</foo></config>"
      << "</facility>";
  InitAgent(a, xml, &rec_, back_);

  ctx_.AddPrototype(a->prototype(), a);
  agent = ctx_.CreateAgent<Agent>(a->prototype());
}

void MockSim::DummyProto(std::string name) {
  Agent* a = DynamicModule::Make(&ctx_, AgentSpec(":agents:Source"));

  std::stringstream xml;
  xml << "<facility><name>" << name << "</name>"
      << "<config><foo>"
      << "<commod>alongunusedcommodityname</commod>"
      << "<capacity>0</capacity>"
      << "</foo></config></facility>";

  InitAgent(a, xml, &rec_, back_);
  ctx_.AddPrototype(name, a);
};

void MockSim::DummyProto(std::string name, std::string commod, double capacity) {
  Agent* a = DynamicModule::Make(&ctx_, AgentSpec(":agents:Source"));

  std::stringstream xml;
  xml << "<facility><name>" << name << "</name>"
      << "<config><foo>"
      << "<commod>" << "commod" << "</commod>"
      << "<capacity>" << capacity << "</capacity>"
      << "</foo></config></facility>";

  InitAgent(a, xml, &rec_, back_);
  ctx_.AddPrototype(name, a);
};

MockSim::~MockSim() {
  warn_limit = 42;
  rec_.Close();
  delete back_;
}

MockAgent MockSim::AddSource(std::string commod) {
  MockAgent ms(&ctx_, &rec_, back_, true);
  ms.commod(commod);
  return ms;
}

MockAgent MockSim::AddSink(std::string commod) {
  MockAgent ms(&ctx_, &rec_, back_, false);
  ms.commod(commod);
  return ms;
}

void MockSim::AddRecipe(std::string name, Composition::Ptr c) {
  ctx_.AddRecipe(name, c);
}

int MockSim::Run() {
  // Initialize Python functionality
  PyStart();

  agent->Build(NULL);
  int id = agent->id();
  ti_.RunSim();
  rec_.Flush();

  PyStop();
  return id;
}

Material::Ptr MockSim::GetMaterial(int resid) {
  return SimInit::BuildMaterial(back_, resid);
}

Product::Ptr MockSim::GetProduct(int resid) {
  return SimInit::BuildProduct(back_, resid);
}

SqliteBack& MockSim::db() { return *back_; }

}  // namespace cyclus
