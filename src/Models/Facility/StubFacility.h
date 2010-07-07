// StubFacility.h
#if !defined(_STUBFACILITY_H)
#define _STUBFACILITY_H
#include <iostream>
#include "FacilityModel.h"


using namespace std;

/**
 * The StubFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility will do nothing. This FacilityModel is intended as a skeleton to guide
 * the implementation of new FacilityModel models. 
 *
 */
class StubFacility : public FacilityModel  
{
public:
    /**
     * Constructs a StubFacility with no specified data.
     * 
     */
    StubFacility() {};

    StubFacility(string facility_name)
	: FacilityModel(facility_name) {};
    
    /**
     * prints the name of this facType
     *
     */
    virtual void print();

protected: 
};

extern "C" Model* construct(string facility_name) {
    return new StubFacility(facility_name);
}

extern "C" void destruct(Model* p) {
    delete p;
}
#endif
