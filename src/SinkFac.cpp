// SinkFac.cpp
// Implements the SinkFac class
#include <iostream>
#include "Facility.h"
#include "FacFactory.h"
#include "SinkFac.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFac::printMyName() 
{ 
	cout << "My Name : SinkFac "<< endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace {
     FacFactoryConcrete<SinkFac> facType("sink");
}

