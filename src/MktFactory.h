#if !defined(_MKTFACTORY)
#define _MKTFACTORY
#include <iostream>
#include <string>
#include <map>
#include "Market.h"

using namespace std;
//-----------------------------------------------------------------------------
/*
 * The MktFactory class instantiates members of the Market class 
 * with a function called 'Create' which accepts a market type.
*/
//-----------------------------------------------------------------------------

class MktFactory {
	public:
    virtual Market * Create()const=0;
    virtual ~MktFactory(){}
};

map<string, MktFactory *> & get_mkt_map();

// This template class is a helper class. Since
// the creator classes for many of the facilities will look identical 
// its helpful to put the code in a template class. The exceptions
// will be when the constructors are called with different parameters.
// The Create() method for these classes will be different


template<class Derived>
class MktFactoryConcrete : public MktFactory 
{
	public:
     MktFactoryConcrete(const string &theMarketName)
     {
        get_mkt_map()[theMarketName]=this;
     }
	 Market * Create()const{return new Derived;}
};

#endif
