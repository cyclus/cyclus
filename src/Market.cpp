// Market.cpp
// Implements the Market Class
#include "Market.h"
#include "Commodity.h"
#include "GenException.h"
#include "Logician.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Initializes the Market nextID to 0.
int Market::nextID = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 Market::Market()
{
	cout << "Used the default Market constructor!!"<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Market::Market(string mktName, int SN) : 
	name(mktName), ID(SN)
{
	myCommods = vector<Commodity*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Market::~Market()
{
	// This destructor doesn't do anything. 
	// It should delete any data allocated by this class
	// particularly the commodities in the myCommods list 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Market::printMyName(){
	cout << "this is the printMyName function in Market.cpp"<<endl; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Market::getNextID()
{
	nextID++;
	return nextID;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Market::getName()
{
	return name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Market::getSN()
{
	return ID;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Market::getCommod(int SN)
{
	vector<Commodity*>::iterator iter;
	for(iter = myCommods.begin(); iter != myCommods.end(); iter ++)
		if ((*iter)->getSN() == SN)
			return *iter;
	throw GenException("Error: tried to access Commodity not traded on this Market");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Market::getCommod(string name)
{
	vector<Commodity*>::iterator iter;
	for(iter = myCommods.begin(); iter != myCommods.end(); iter ++)
		if ((*iter)->getName() == name)
			return *iter;
	throw GenException("Error: tried to access Commodity not traded on this Market");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Market::registerMkt()
{
	// for each commodity traded on the market,
	vector<Commodity*>::iterator iter;
	for(iter = myCommods.begin(); iter!=myCommods.end(); iter++)
	{
		// insert a key value pair into the map.
		LI->mktMap.insert(make_pair(*iter, this));
	}
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Market::addCommod(string s, bool fissile, bool sepMat)
{
	Commodity* newCommod = new Commodity(s, this, fissile, sepMat);
	myCommods.push_back(newCommod);
	// add this commod into the mktMap.
	LI->mktMap.insert(make_pair(newCommod, this));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Market::addCommod(Commodity* c)
{
	// add this Commodity to the list of myCommods
	myCommods.push_back(c);
	// add this Commodity to the mktMap
	LI->mktMap.insert(make_pair(c, this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pair<vector<Commodity*>::iterator, vector<Commodity*>::iterator> Market::getCommods()
{
	return make_pair(myCommods.begin(), myCommods.end());
}



