// StubRegion.cpp
// Implements the StubRegion class

#include "StubRegion.h"

#include "Logger.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::StubRegion() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::~StubRegion() {};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::initModuleMembers(QueryEngine* qe) { 
  QueryEngine* input = qe->queryElement("input");
  //retrieve input data members here. For example :  
  //string query = "tax_rate";
  //tax_rate_ = lexical_cast<double>(input->getElementContent(query));
};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::cloneModuleMembersFrom(RegionModel* src) { 
  StubRegion* src_stub = dynamic_cast<StubRegion*>(src);
  //copy data members here. For example : 
  //tax_rate_ = src_stub->taxRate();
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubRegion::str() { 
  return RegionModel::str(); 
};

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::receiveMessage(msg_ptr msg) {};

/* ------------------- */ 


/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" Model* constructStubRegion() {
  return new StubRegion();
}
extern "C" void destructStubRegion(Model* model) { 
  delete model;
}
/* ------------------- */ 
