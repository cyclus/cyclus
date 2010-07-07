// StubRegion.h
#if !defined(_STUBREGION_H)
#define _STUBREGION_H
#include <iostream>
#include "RegionModel.h"


using namespace std;

/**
 * The StubRegion class inherits from the RegionModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This region will do nothing. This RegionModel is intended as a skeleton to guide
 * the implementation of new RegionModel models. 
 *
 */
class StubRegion : public RegionModel  
{
public:
    /**
     * Constructs a StubRegion with no specified data.
     * 
     */
    StubRegion() {};

    StubRegion(string region_name)
	: RegionModel(region_name) {};
 
    StubRegion(string region_name, istream &input);
   
    /**
     * prints the name of this facType
     *
     */
    virtual void print();

    /// get model implementation name
    virtual const string getModelName() { return "StubRegion"; };

protected: 
};

extern "C" Model* construct(string region_name,istream &input) {
    return new StubRegion(region_name,input);
}

extern "C" void destruct(Model* p) {
    delete p;
}
#endif
