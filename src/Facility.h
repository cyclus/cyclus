#if !defined(_FACILITY)
#define _FACILITY
#include <iostream>

	using namespace std;

//-----------------------------------------------------------------------------
/*
 * The Facility class is the abstract class used by all objects that our 
 * factory will be responsible for creating, or manufacturing. 
 *
 * This is all our factory really knows about the products it's creating
*/
//-----------------------------------------------------------------------------
class Facility
{
public:
	virtual void printMyName();
};
#endif



