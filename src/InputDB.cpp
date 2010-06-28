// InputDB.cpp
// Implements the fake Input Database

#include "InputDB.h"
#include "Logician.h"
#include "MktFactory.h"
#include "GenException.h"
#include <iostream>

InputDB* InputDB::_instance = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputDB* InputDB::Instance() 
{
	// If we haven't created a InputDB yet, create it, and then and return it
	// either way.
	if (0 == _instance) {
		_instance = new InputDB();
	}

	return _instance;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int InputDB::getFacID() {
	int testFacID = 1;
	return testFacID;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string InputDB::getFacName(int ID) {
	string testName = "Reactor";
	return testName;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string InputDB::getMktName(int ID) {
	string testName = "testMkt";
	return testName;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
deque<Material*> InputDB::getInRecs(int ID) 
{
	// This defines a test recipe input for the fake recipeFac we're creating
	// Make a test composition
	map<Iso, NumDens> testInComp= map<Iso,NumDens>(); 
	testInComp[922350]=6.51e27;
	testInComp[922380]=1.947e29;
	// Make a test market
	MktFactory *netFlowCreator = get_mkt_map()["netFlow"];
	Market *testInMkt = netFlowCreator->Create();
	// Make a test commodity
	Commodity* testInCommod = new Commodity("testInCommod", testInMkt, 0, 0);
	// Finally, make a test material
	Material* thisMat = new Material(testInComp, sol, testInCommod);  
	// Put the test material in a list. 
	// It's the only member of the list
	deque<Material*> testInCompList;
	testInCompList = deque<Material*>(); 
	testInCompList.push_back(thisMat);

	return testInCompList;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
deque<Material*> InputDB::getOutRecs(int ID) {
	// This defines a test recipe output for the fake recipeFac we're creating
	// Make a test composition
	// We'll pretend that this recipe facility transmutes 
	// exactly half of the original u235 to pu239.
	map<Iso, NumDens> testOutComp= map<Iso,NumDens>(); 
	testOutComp[922350]=3.255e27; 
	testOutComp[922380]=1.947e29;
	testOutComp[942390]=3.255e27;
	// Make a test market
	MktFactory *netFlowCreator = get_mkt_map()["netFlow"];
	Market *testOutMkt = netFlowCreator->Create();
	// Make a test commodity
	Commodity* testOutCommod = new Commodity("testOutCommod", testOutMkt, 0, 0);
	// Finally, make a test material
	Material* thisMat = new Material(testOutComp, sol, testOutCommod);  
	// Put the test material in a list. 
	// It's the only member of the list
	deque<Material*> testOutCompList;
	testOutCompList = deque<Material*>(); 
	testOutCompList.push_back(thisMat);

	return testOutCompList;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputDB::InputDB() 
{
}
