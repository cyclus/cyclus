#include <iostream>
#include "RecipeFac.h"


void RecipeFac::printMyName() 
{ 
	cout << "My Name : Recipe Fac "<< endl;
};

namespace {
     FacFactoryConcrete<RecipeFac> facType("recipe");
}

