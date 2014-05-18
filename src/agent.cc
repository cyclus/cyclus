// agent.cc
// Implements the Agent Class
#include "agent.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include "context.h"
#include "error.h"
#include "logger.h"
#include "resource.h"

namespace cyclus {

// static members
int Agent::next_id_ = 0;

void Agent::InitFrom(Agent* m) {
  id_ = next_id_++;
  prototype_ = m->prototype_;
  kind_ = m->kind_;
  spec_ = m->spec_;
  lifetime_ = m->lifetime_;
  ctx_ = m->ctx_;
}

std::string Agent::InformErrorMsg(std::string msg) {
  std::stringstream ret;
  ret << "A(n) " << spec_ << " named " << prototype_
      << " at time " << context()->time()
      << " received the following error:\n"
      << msg;
  return ret.str();
}

void Agent::InfileToDb(InfileTree* qe, DbInit di) {
  std::string proto = qe->GetString("name");
  int lifetime = OptionalQuery<int>(qe, "lifetime", -1);
  di.NewDatum("Agent")
    ->AddVal("Prototype", proto)
    ->AddVal("Lifetime", lifetime)
    ->Record();
}

void Agent::InitFrom(QueryableBackend* b) {
  QueryResult qr = b->Query("Agent", NULL);
  prototype_ = qr.GetVal<std::string>("Prototype");
  lifetime_ = qr.GetVal<int>("Lifetime");
}

void Agent::Snapshot(DbInit di) {
  di.NewDatum("Agent")
    ->AddVal("Prototype", prototype_)
    ->AddVal("Lifetime", lifetime_)
    ->Record();
}

Agent::Agent(Context* ctx)
  : ctx_(ctx),
    id_(next_id_++),
    kind_("Agent"),
    parent_id_(-1),
    enter_time_(-1),
    lifetime_(-1),
    parent_(NULL),
    spec_("UNSPECIFIED") {
  ctx_->agent_list_.insert(this); 
  MLOG(LEV_DEBUG3) << "Agent ID=" << id_ << ", ptr=" << this << " created.";
}

Agent::~Agent() {
  MLOG(LEV_DEBUG3) << "Deleting agent '" << prototype() << "' ID=" << id_;
  context()->agent_list_.erase(this);

  std::set<Agent*>::iterator it;  
  if (parent_ != NULL) {
    CLOG(LEV_DEBUG2) << "Agent '" << parent_->prototype() << "' ID=" << parent_->id()
                     << " has removed child '" << prototype() << "' ID="
                     << id() << " from its list of children.";
    it = find(parent_->children_.begin(), parent_->children_.end(), this);
    if (it != parent_->children_.end()) {
      parent_->children_.erase(it);
    }
  }

  // set children's parents to NULL
  for (it = children_.begin(); it != children_.end(); ++it) {
    Agent* child = *it;
    child->parent_ = NULL;
    child->parent_id_ = -1;
  }
}

std::string Agent::str() {
  std::stringstream ss;
  ss << kind_ << "_" << prototype_
     << " ( "
     << "ID=" << id_
     << ", implementation=" << spec_
     << ",  name=" << prototype_
     << ",  parentID=" << parent_id_
     << " ) " ;
  return ss.str();
}

void Agent::Build(Agent* parent) {
  CLOG(LEV_DEBUG1) << "Agent '" << prototype()
                   << "' is entering the simulation.";
  CLOG(LEV_DEBUG3) << "It has:";
  CLOG(LEV_DEBUG3) << " * Implementation: " << spec_;
  CLOG(LEV_DEBUG3) << " * ID: " << id();

  Connect(parent);
  enter_time_ = ctx_->time();
  EnterNotify();
  this->AddToTable();
}

void Agent::EnterNotify() {
  ctx_->RegisterAgent(this);
}

void Agent::Connect(Agent* parent) {
  if (parent == this) {
    throw KeyError("Agent " + prototype() +
                   "is trying to add itself as its own child.");
  }
  if (parent != NULL) {
    parent_ = parent;
    parent_id_ = parent->id();
    parent->children_.insert(this);
  } 
}

void Agent::Decommission() {
  CLOG(LEV_INFO3) << prototype() << "(" << this << ")" << " is being decommissioned";
  
  ctx_->NewDatum("AgentExit")
  ->AddVal("AgentId", id())
  ->AddVal("ExitTime", ctx_->time())
  ->Record();
  ctx_->UnregisterAgent(this);
  ctx_->DelAgent(this);
}

std::string Agent::PrintChildren() {
  std::stringstream ss("");
  ss << "Children of " << prototype() << ":" << std::endl;
  
  std::set<Agent*>::iterator it;  
  for (it = children_.begin(); it != children_.end(); ++it) {
    Agent* child = *it;
    std::vector<std::string> print_outs = GetTreePrintOuts(child);
    for (int j = 0; j < print_outs.size(); j++) {
      ss << "\t" << print_outs.at(j);
    }
  }
  return ss.str();
}

std::vector<std::string> Agent::GetTreePrintOuts(Agent* m) {
  std::vector<std::string> ret;
  std::stringstream ss("");
  ss << m->prototype() << std::endl;
  ret.push_back(ss.str());
  std::set<Agent*>::iterator it;  
  for (it = children_.begin(); it != children_.end(); ++it) {
    Agent* child = *it;
    std::vector<std::string> outs = GetTreePrintOuts(child);
    for (int j = 0; j < outs.size(); j++) {
      ss.str("");
      ss << "\t" << outs.at(j) << std::endl;
      ret.push_back(ss.str());
    }
  }
  return ret;
}

void Agent::AddToTable() {
  ctx_->NewDatum("AgentEntry")
  ->AddVal("AgentId", id_)
  ->AddVal("Kind", kind_)
  ->AddVal("Implementation", spec_)
  ->AddVal("Prototype", prototype_)
  ->AddVal("ParentId", parent_id_)
  ->AddVal("Lifetime", lifetime_)
  ->AddVal("EnterTime", enter_time_)
  ->Record();
}
} // namespace cyclus
