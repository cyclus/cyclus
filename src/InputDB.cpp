// InputDB.cpp
// Implements the fake Input Database

#include "InputDB.h"
#include "Logician.h"
#include "MktFactory.h"
#include "GenException.h"
#include "Material.h"
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
Region* InputDB::getRegion(int ID){
	RegionType testType = FC;
	string testName = "TestRegion";
	int testID = 1;
	int testDur = 1;
	Region* testRegion = new Region(testType, testName, testID, testDur);
	
	return testRegion;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Inst* InputDB::getInst(int ID) {
	string testName = "TestInst";
	int testID = 1; 
	Region* testRegion = getRegion(ID); 
	double testPhi = 1.0; 
	int testDur = 1;
	Inst* testInst = new Inst(testName, testID, testRegion, testPhi, testDur);

	return testInst;
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
map<Iso, NumDens> InputDB::getInRecs(int ID) 
{
	// This defines a test recipe input for the fake recipeFac we're creating
	// Make a test composition
	map<Iso, NumDens> testInComp= map<Iso,NumDens>(); 
	testInComp[922350]=6.51e27;
	testInComp[922380]=1.947e29;

	return testInComp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Market* InputDB::getInMkt(int ID){
	// Make a test market
	MktFactory *netFlowCreator = get_mkt_map()["netFlow"];
	Market *testInMkt = netFlowCreator->Create();

	return testInMkt;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
list<Commodity*> InputDB::getFeeds(int ID){
	// You need a market to make a commodity
	Market* testInMkt = getInMkt(ID);
	// Make a test commodity
	Commodity* testInCommod = new Commodity("testInCommod", testInMkt, 0, 0);
	// Make a list to hold this commodity
	list<Commodity*> inCommodList = list<Commodity*>();
	// Put the commodity in the list
	inCommodList.push_back(testInCommod);
	
	return inCommodList;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<Iso, NumDens> InputDB::getOutRecs(int ID) {
	// This defines a test recipe output for the fake recipeFac we're creating
	// Make a test composition
	// We'll pretend that this recipe facility transmutes 
	// exactly half of the original u235 to pu239.
	map<Iso, NumDens> testOutComp= map<Iso,NumDens>(); 
	testOutComp[922350]=3.255e27; 
	testOutComp[922380]=1.947e29;
	testOutComp[942390]=3.255e27;

	return testOutComp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Market* InputDB::getOutMkt(int ID){
	// Make a test market
	MktFactory *netFlowCreator = get_mkt_map()["netFlow"];
	Market *testOutMkt = netFlowCreator->Create();

	return testOutMkt;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
list<Commodity*> InputDB::getProds(int ID){
	// You need a market to make a commodity
	Market* testOutMkt = getOutMkt(ID);
	// Make a test commodity
	Commodity* testOutCommod = new Commodity("testOutCommod", testOutMkt, 0, 0);
	// Make a list to hold this commodity
	list<Commodity*> outCommodList = list<Commodity*>();
	// Put the commodity in the list
	outCommodList.push_back(testOutCommod);

	return outCommodList;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputDB::InputDB() 
{
}
