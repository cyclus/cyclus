// inst_model.h
#ifndef CYCLUS_INST_MODEL_H_
#define CYCLUS_INST_MODEL_H_

#include "time_listener.h"
#include "model.h"
#include "region_model.h"

#include <map>
#include <set>
#include <list>

namespace cyclus {

class QueryEngine;

// Usefull Typedefs
typedef std::set<std::string> PrototypeSet;
typedef std::set<std::string>::iterator PrototypeIterator;

/**
   The Institution class is the abstract class/interface
   used by all institution models

   @section introduction Introduction The Institution type assists in defining the
   region-institution-facility hierarchy in Cyclus. A Institution institution is
   an actor associated with a set of facilities for which it is responsible. An
   Institution may be used to adjust preferences in the ResourceExchange to make
   material routing decisions based on interfacility relationships. Deployment
   is a primary differentiator between different Institution implementations.
 */
class Institution : public Agent, public TimeListener {
  /* --------------------
   * all MODEL classes have these members
   * --------------------
   */
 public:
  /**
     Default constructor for Institution Class
   */
  Institution(Context* ctx);

  /**
     every model should be destructable
   */
  virtual ~Institution() {};

  // DO NOT call Agent class implementation of this method
  virtual void InfileToDb(QueryEngine* qe, DbInit di) {};

  // DO NOT call Agent class implementation of this method
  virtual void InitFrom(QueryBackend* b) {};

  // DO NOT call Agent class implementation of this method
  virtual void Snapshot(DbInit di) {};

  virtual void InitInv(Inventories& inv) {};

  virtual Inventories SnapshotInv() { return Inventories(); };

  /**
     every model should be able to print a verbose description
   */
  virtual std::string str();

  /**
     perform all tasks required when an inst enters the simulation
   */
  virtual void Build(Agent* parent);

  virtual void DoRegistration();

  virtual void Decommission();

  virtual void Tick(int time) {};

  virtual void Tock(int time);

 protected:
  void InitFrom(Institution* m);

};

} // namespace cyclus

#endif // ifndef CYCLUS_INST_MODEL_H_

