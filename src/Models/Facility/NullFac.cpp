// NullFac.cpp
// Implements the NullFac class
#include <iostream>
#include <map>
#include <string>
#include "NullFac.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void NullFac::printMyName() 
{ 
	cout << "My Name : Null Fac and bob is "<< bob << endl;
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NullFac::NullFac() : Facility()
{	
	bob = 0;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NullFac::NullFac(double a, double b) : Facility()
{	
	bob = a+b;
};




