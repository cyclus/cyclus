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
   The InstModel class is the abstract class/interface
   used by all institution models

   @section introduction Introduction The InstModel type assists in defining the
   region-institution-facility hierarchy in Cyclus. A InstModel institution is
   an actor associated with a set of facilities for which it is responsible. An
   InstModel may be used to adjust preferences in the ResourceExchange to make
   material routing decisions based on interfacility relationships. Deployment
   is a primary differentiator between different InstModel implementations.
 */
class InstModel : public Model, public TimeListener {
  /* --------------------
   * all MODEL classes have these members
   * --------------------
   */
 public:
  /**
     Default constructor for InstModel Class
   */
  InstModel(Context* ctx);

  /**
     every model should be destructable
   */
  virtual ~InstModel() {};

  /**
     Initalize the InstModel from a QueryEngine. Calls the init function.

     @param qe a pointer to a QueryEngine object containing intialization data
   */
  virtual void InitFrom(QueryEngine* qe);

  /**
     every model should be able to print a verbose description
   */
  virtual std::string str();

  /**
     perform all tasks required when an inst enters the simulation
   */
  virtual void Build(Model* parent);

  virtual void Decommission();

  virtual void Tick(int time) {};

  virtual void Tock(int time);

 protected:
  void InitFrom(InstModel* m);

};

} // namespace cyclus

#endif // ifndef CYCLUS_INST_MODEL_H_

