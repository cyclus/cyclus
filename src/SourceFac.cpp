#include <iostream>
#include "SourceFac.h"


void SourceFac::printMyName() 
{ 
	cout << "My Name : Source Fac "<< endl;
};



namespace {
     FacFactoryConcrete<SourceFac> theCreator("source");
}

