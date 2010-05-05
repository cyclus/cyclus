#if !defined(_MARKET)
#define _MARKET
#include <iostream>
#include <map>
#include <string>

using namespace std;

//-----------------------------------------------------------------------------
/*
 * The Market class is the abstract class used by all objects that our 
 * factory will be responsible for creating, or manufacturing. 
 *
 * This is all our factory really knows about the products it's creating
*/
//-----------------------------------------------------------------------------
class Market
{
public:
	virtual void printMyName();
	virtual ~Market(){}
};

#endif



