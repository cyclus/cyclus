// StubConverter.cpp
// Implements the StubConverter class
#include <iostream>

#include "StubConverter.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

/*
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubConverter::init(xmlNodePtr cur)
{ 
  ConverterModel::init(cur);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void StubConverter::copy(StubConverter* src)
{
  ConverterModel::copy(src);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubConverter::print() 
{ 
  ConverterModel::print(); 
};

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
  throw GenException("The StubConverter should not be used to convert things.");
}
    

