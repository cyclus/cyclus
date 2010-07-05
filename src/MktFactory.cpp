// MktFactory.cpp
// Implements the MktFactory class

#include <iostream>
#include "MktFactory.h"
#include "Market.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<string, MktFactory *> & 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
get_mkt_map()
{
     static map<string,MktFactory *> theMarketMap;
	 return theMarketMap;
}

