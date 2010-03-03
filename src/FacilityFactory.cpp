//FacilityFactory.cpp
#include "FacilityFactory.h"
#include "BookKeeper.h"
#include "sqlite3.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int FacilityFactory::registerFacilityInstantiator(int id, FacilityInstantiator func)

{

m_instantiatorMap.insert( std::make_pair( id, func ) );

return 1;

}
