// StubInst.cpp
// Implements the StubInst class
#include <iostream>

#include "StubInst.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


StubInst::StubInst(xmlNodePtr cur)
    : InstModel(cur) // assign & incrememt ID
{

}

void StubInst::print() 
{ 
    cout << "\tThis is a StubInst model with name " << name 
	 << " and ID: " << ID << endl;



};






