// StubStubInst.cpp
// Implements the StubStubInst class


#include "StubInst.h"

#include "Logger.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubInst::StubInst() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubInst::~StubInst() {};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubInst::initModuleMembers(QueryEngine* qe) { 
  QueryEngine* input = qe->queryElement("input");
  //retrieve input data members here. For example :  
  //string query = "tax_rate";
  //tax_rate_ = lexical_cast<double>(input->getElementContent(query));
};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubInst::cloneModuleMembersFrom(InstModel* src) { 
  StubInst* src_stub = dynamic_cast<StubInst*>(src);
  //copy data members here. For example : 
  //tax_rate_ = src_stub->taxRate();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubInst::str() { 
  return InstModel::str(); 
};

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubInst::receiveMessage(msg_ptr msg) {};

/* ------------------- */ 


/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" Model* constructStubInst() {
  return new StubInst();
}

extern "C" void destructStubInst(Model* model) { 
  delete model;
}
/* ------------------- */ 



