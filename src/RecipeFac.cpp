// RecipeFac.cpp
// This implements the RecipeFac class. 
#include <iostream>
#include "InputDB.h"
#include "RecipeFac.h"
#include "MktFactory.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RecipeFac::RecipeFac()
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RecipeFac::RecipeFac(string name, deque<Material*> inRecs, deque<Material*> outRecs): Facility() 
{
	name = name;
	inventory = inRecs;
	stocks = outRecs;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RecipeFac::printMyName() 
{ 
	cout << "My Name : Recipe Fac "<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RecipeFac::handleTick(int time)
{
	// Create a market. This should be changed so 
	// that it registers a market rather than adding one. 
	// That is, there should be a unique market to pointer map.
	MktFactory *netFlowCreator = get_mkt_map()["netFlow"];
	Market *netMkt = netFlowCreator->Create();
	// Create a commodity
	Commodity* recTest = new Commodity("recTest", netMkt, 0, 0);
	// Test it
	cout << "The recTest commodity has the nickname " << recTest->getName() << endl;
	// Test the time
	cout << "The time is now" << time << endl;
	// add the commodity to the commodity map?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace {
	class recCreator : public FacFactory
	{
		public:
			recCreator()
			{
				get_fac_map()["recipe"] = this;
			}
			virtual Facility * Create()const
			{
				int facID = Facility::getNextID();
				string myName = INDB->getFacName(facID);

				// grab some test recipes from the fake input database
				deque<Material*> myInRecs = INDB->getInRecs(facID);
				deque<Material*> myOutRecs = INDB->getOutRecs(facID);

				// grab some test commodities from the fake input database
				// list<Commodity*> myFeeds = INDB->getFeeds(facID);
				// list<Commodity*> myProds = INDB->getProds(facID);

				return new RecipeFac(myName, myInRecs, myOutRecs);
			}
	}facType;
}

