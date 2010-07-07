#include <iostream>
#include <cstdlib>
#include <cstring>

#include "Model.h"
#include "Logician.h"

using namespace std;


//-----------------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // read input file

    // setup simulation
    // =====================

    // load markets
    // create_market_method
    //  * get market model name
    //  * search cache for market methods
    //     * if not found load market
    string keyword, modelName, modelImpl;
    cin >> keyword;
    while (keyword != "end")
    {
	if ( "Market" == keyword )
	{
	    cin >> modelName >> modelImpl;
	    mdl_ctor* model_creator = Model::load(keyword,modelImpl);
	    
	    Model* model = model_creator(modelName);
	    
	    LI->addMarket(model);
	}
	else if ( "Facility" == keyword )
	{
	    cin >> modelName >> modelImpl;
	    mdl_ctor* model_creator = Model::load(keyword,modelImpl);
	    
	    Model* model = model_creator(modelName);
	    
	    LI->addFacility(model);
	}
	else if ( "Region" == keyword )
	{
	    cin >> modelName >> modelImpl;
	    mdl_ctor* model_creator = Model::load(keyword,modelImpl);
	    
	    Model* model = model_creator(modelName);
	    
	    LI->addRegion(model);
	}
	else if ( "Recipe" == keyword )
	{
	    LI->addRecipe(new Material(cin));
	}
	else
	    cerr << "Model type: " << keyword << " is not supported at this time." << endl;
	cin >> keyword;
    }

    cout << "Here is a list of " << LI->getNumMarkets() << " markets:" << endl;
    LI->printMarkets();
    cout << "Here is a list of " << LI->getNumFacilities() << " facilities:" << endl;
    LI->printFacilities();
    cout << "Here is a list of " << LI->getNumRegions() << " regions:" << endl;
    LI->printRegions();



    return 0;
}
