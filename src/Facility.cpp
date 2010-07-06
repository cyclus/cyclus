// Facility.cpp
// Implements the Facility class
#include "Facility.h"
#include "GenException.h"
#include "Inst.h"
#include "Message.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Facility::Facility()
{
	cout << "Used default Facility Constructor!"<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Facility::Facility(string name, Inst* i, int SN, list<Commodity*> feeds, list<Commodity*> prods)
{
  
	this->name = name;
	myInst = i;
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
Inst* Facility::getInst() 
{
	return myInst;
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
void Facility::sendMaterial(Material* mat, Communicator* rec, int time)
{	
	// Figure out who we're shipping to.
	Facility* receiver = dynamic_cast<Facility*>(rec);
	
	// If we're shipping to another Facility, just do it.
	if (0 != receiver) {
		mat->logTrans(time, ID, receiver->getSN());
		receiver->receiveMaterial(mat, time);
	}
	// Otherwise (for now) there's a problem. It's conceivable that we may want 
	// to make it possible to send Material to a non-Facility for some kind of
	// "in-transit" status.
	else throw GenException("Tried to send Material to a non-Facility");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Facility::receiveMaterial(Material* mat, int time)
{
	// Append the Material to the stocks.
	this->stocks.push_back(mat);
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

