#include <sstream>

#include "InstModel.h"
#include "CycException.h"
#include "TestInst.h"

void TestInst::receiveMessage(msg_ptr msg) {
  msg->setDir(DOWN_MSG);
}

bool TestInst::canBuild(Model* mdl) {return true;}

void TestInst::build(Model* mdl, Model* parent) {
    if ( parent != this->parent() ) {
      // if the requester is not this inst's parent, throw an error
      std::stringstream err("");
      err << "Model " << parent->name() << " is requesting that "
          << "BuildInst " << this->name() << " build a prototype, but "
          << "is not the BuildInst's parent.";
      throw CycOverrideException(err.str());
    }
  }

void TestInst::copyFreshModel(Model* model) { }

void TestInst::wrapAddPrototype(Model* prototype){
  addPrototype(prototype);
}

extern "C" Model* constructTestInst() { 
  return new TestInst();
}
