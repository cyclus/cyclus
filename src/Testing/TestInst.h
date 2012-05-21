#if !defined _TESTINST_H_
#define _TESTINST_H_

#include "InstModel.h"

#include "CycException.h"

#include <sstream>

/**
   This is the simplest possible Institution, for testing
 */
class TestInst: public InstModel {
  
  void receiveMessage(msg_ptr msg) {
    msg->setDir(DOWN_MSG);

    Model* what_model = NULL;

    msg->trans().setRequester(what_model);

  }

  bool canBuild(Model* mdl) {return true;}

  void build(Model* mdl, Model* parent) {
    if ( parent != this->parent() ) {
      // if the requester is not this inst's parent, throw an error
      std::stringstream err("");
      err << "Model " << parent->name() << " is requesting that "
          << "BuildInst " << this->name() << " build a prototype, but "
          << "is not the BuildInst's parent.";
      throw CycOverrideException(err.str());
    }
  }

  void copyFreshModel(Model* model) { }

 public:
  void wrapAddPrototype(Model* prototype){addPrototype(prototype);}
};

#endif
