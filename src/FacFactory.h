// FacFactory.h
#if !defined(_FACFACTORY)
#define _FACFACTORY
#include <iostream>
#include <string>
#include <map>
#include "Facility.h"

using namespace std;

/** 
 * The FacFactory class 
 * instantiates members of the Facility class 
 * with a function called 'Create' which accepts a
 * string nicknaming the facility type.
*/
class FacFactory 
{
	public:
		/**
		 * This is a pointer to a facility constructor?
		 */
		virtual Facility * Create()const=0;
		
		/**
		 * FacFactory destructor
		 */
		virtual ~FacFactory(){}
};

/**
 * The get_fac_map() is a dictionary which stores
 * facility nicknames and pointers to their constructors.
 */
map<string, FacFactory *> & get_fac_map();


/** This template class is a helper class. Since
 * the creator classes for many of the facilities will look identical 
 * its helpful to put the code in a template class. The exceptions
 * will be when the constructors are called with different parameters.
 * The Create() method for these classes will be different
 */
template<class Derived>
class FacFactoryConcrete : public FacFactory 
{
	public:
     FacFactoryConcrete(const string &theFacilityName)
     {
        get_fac_map()[theFacilityName]=this;
     }
	 Facility * Create()const{return new Derived;}
};

#endif
