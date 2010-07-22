#include <iostream>
#include <cstdlib>
#include <cstring>

#include "Model.h"
#include "Logician.h"
#include "InputXML.h"

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
    XMLinput->load_file(argv[1]);

    // get commodities
    
    // get markets
    // 


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
