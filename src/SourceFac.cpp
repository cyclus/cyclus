// SourceFac.cpp
// Implements the SourceFac class
#include <iostream>
#include "SourceFac.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SourceFac::printMyName() 
{ 
	cout << "My Name : Source Fac "<< endl;
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace {
     FacFactoryConcrete<SourceFac> facType("source");
}

