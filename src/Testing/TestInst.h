#if !defined _TESTINST_H_
#define _TESTINST_H_


#include "InstModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Institution, for testing
class TestInst: public InstModel {
  
  void receiveMessage(msg_ptr msg) {
    msg->setDir(DOWN_MSG);

    Model* what_model = NULL;

    msg->setRequester(what_model);

  }

  void copyFreshModel(Model* model) { }
};

#endif
