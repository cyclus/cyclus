#include <iostream>
#include "MktFactory.h"
#include "Market.h"

using namespace std;


// a dictionary of market constructors
map<string, MktFactory *> & 

// a Market to grab the Market from the map
get_mkt_map()
{
     static map<string,MktFactory *> theMarketMap;
	 return theMarketMap;
}

