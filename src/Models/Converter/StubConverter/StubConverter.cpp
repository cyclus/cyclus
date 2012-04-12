// StubConverter.cpp
// Implements the StubConverter class

#include "Logger.h"
#include "CycException.h"
#include "InputXML.h"
#include "StubConverter.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubConverter::init(xmlNodePtr cur) { 
  ConverterModel::init(cur); 
};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubConverter::copy(StubConverter* src) { 
  ConverterModel::copy(src); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubConverter::copyFreshModel(Model* src) { 
  copy(dynamic_cast<StubConverter*>(src)); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string StubConverter::str() { 
  return ConverterModel::str(); 
};

/* ------------------- */ 


/* --------------------
 * all CONVERTERMODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
msg_ptr StubConverter::convert(msg_ptr convMsg, msg_ptr refMsg) {
  throw CycException("The StubConverter should not be used to convert things.");
}

/* ------------------- */ 

    
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" Model* constructStubConverter() {
    return new StubConverter();
}

/* ------------------- */ 


