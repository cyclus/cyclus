// Facility.cpp
// Implements the Facility class
#include "Facility.h"
#include "GenException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Facility::Facility()
{
	cout << "Used default Facility Constructor!"<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Facility::Facility(string name, int SN, list<Commodity*> feeds, list<Commodity*> prods)
{
  
	this->name = name;
	ID = SN;
	inventory = deque<Material*>();
	stocks = deque<Material*>();
	wastes = deque<Material*>();
	list<Commodity*> stockCommods = feeds;
	list<Commodity*> invCommods = prods;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Facility::getNextID()
{
	nextID++;
	return nextID;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Facility::beginCycle(int time)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string Facility::getName() const
{
	return name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Facility::getSN() const
{
	return ID;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Facility::handleTick(int)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Facility::handleTock(int)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Facility::sendMaterial(Material* mat, int time)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Facility::receiveMaterial(Material* mat, int time)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Facility::handleStart(int time)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Facility::handleEnd(int time)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Facility::pickRequestCommod()
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Facility::pickOfferCommod()
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Facility::decommission()
{
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Facility::printMyName()
{cout << "this is the printMyName function in Facility.cpp"<<endl; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Facility::~Facility()
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Initialize the Facility nextID to zero.
int Facility::nextID = 0;

