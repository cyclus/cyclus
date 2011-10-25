// StubConverter.cpp
// Implements the StubConverter class
#include <iostream>

#include "StubConverter.h"

#include "Logician.h"
#include "CycException.h"
#include "InputXML.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubConverter::StubConverter() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubConverter::~StubConverter() {};
    
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
void StubConverter::print() { 
  ConverterModel::print(); 
};

/* --------------------
 * all CONVERTERMODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubConverter::handleTick(int time)
{
  // The StubConverter isn't terribly interested in the tick.
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubConverter::handleTock(int time)
{
  // The StubConverter isn't terribly interested in the tock.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Message* convert(Message* convMsg, Message* refMsg)
{
  throw CycException("The StubConverter should not be used to convert things.");
}
    

