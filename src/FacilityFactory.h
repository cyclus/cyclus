// FacilityFactory.h

#if !defined(_FACILITYFACTORY)
#define _FACILITYFACTORY
#include <string>
#include <map>
#include <vector>

using namespace std;

class FacilityFactory
{

typedef Facility* (*FacilityInstantiator)();
typedef std::map<int, FacilityInstantiator> InstantiatorMap;

public:

Facility* getFacilityInstance( int ID );

static int registerFacilityInstantiator(int id, FacilityInstantiator func);

private:

static InstantiatorMap m_instantiatorMap;

}
