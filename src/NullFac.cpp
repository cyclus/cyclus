#include <iostream>
#include "NullFac.h"

void NullFac::printMyName() 
{ 
	cout << "My Name : Null Fac "<< endl;
};


namespace {
     FacFactoryConcrete<NullFac> theCreator("null");
}


