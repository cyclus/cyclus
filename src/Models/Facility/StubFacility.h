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
    
    /// constructor to make an empty instance of this model
    StubFacility(string facility_name)
	: FacilityModel(facility_name) {};

    /// constructor to build an instance based on input
    StubFacility(string facility_name,istream &input);

    /**
     * prints the name of this facType
     *
     */
    virtual void print();

    /// get model implementation name
    virtual const string getModelName() { return "StubFacility"; };

protected: 
};

extern "C" Model* construct(string facility_name,istream &input) {
    return new StubFacility(facility_name,input);
}

extern "C" void destruct(Model* p) {
    delete p;
}
#endif
