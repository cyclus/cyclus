#if !defined(_MARKET)
#define _MARKET
#include <iostream>
#include <map>
#include <string>

using namespace std;

//-----------------------------------------------------------------------------
/* The Market Class.
 * The Market class is the abstract class used by all objects that our 
 * factory will be responsible for creating, or manufacturing. 
 *
 * This is all our factory really knows about the products it's creating
 */
//-----------------------------------------------------------------------------
class Market
{
public:
	/**
	 * Markets should be able to return pointers to themselves. 
	 */
	Market* getName;

	/**
	 * Every market should be able to write its name.
	 */
	virtual void printMyName();

	/**
	 * Markets should not be indestructible.
	 */
	virtual ~Market();

protected:
	/**
	 * Markets need names
	 */
	string myName;

};

#endif



