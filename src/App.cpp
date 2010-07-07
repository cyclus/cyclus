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
    string keyword, name, modelImpl;
    cin >> keyword;
    while (keyword != "end")
    {
	if ( "Market" == keyword )
	{
	    LI->addMarket(Model::create(keyword,cin));

	}
	else if ( "Facility" == keyword )
	{
	    LI->addFacility(Model::create(keyword,cin));
	}
	else if ( "Region" == keyword )
	{
	    LI->addRegion(Model::create(keyword,cin));
	}
	else if ( "Recipe" == keyword )
	{
	    cin >> name;
	    LI->addRecipe(name,new Material(cin));
	}
	else
	    cerr << "Keyword: " << keyword << " is not supported at this time." << endl;
	cin >> keyword;
    }

    cout << "Here is a list of " << LI->getNumMarkets() << " markets:" << endl;
    LI->printMarkets();
    cout << "Here is a list of " << LI->getNumFacilities() << " facilities:" << endl;
    LI->printFacilities();
    cout << "Here is a list of " << LI->getNumRegions() << " regions:" << endl;
    LI->printRegions();
    cout << "Here is a list of " << LI->getNumRecipes() << " recipes:" << endl;
    LI->printRecipes();



    return 0;
}
