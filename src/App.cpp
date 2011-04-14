#include <iostream>
#include <cstdlib>
#include <cstring>

#include "Model.h"
#include "Logician.h"
#include "BookKeeper.h"
#include "Timer.h"
#include "InputXML.h"

using namespace std;


//-----------------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // announce yourself
    cout << "|--------------------------------------------|" << endl;
    cout << "|                  Cyclus                    |" << endl;
    cout << "|       a nuclear fuel cycle simulator       |" << endl;
    cout << "|  from the University of Wisconsin-Madison  |" << endl;
    cout << "|--------------------------------------------|" << endl;

    // read input file

    // setup simulation
    // =====================

    // load markets
    // create_market_method
    //  * get market model name
    //  * search cache for market methods
    //     * if not found load market
    XMLinput->load_file(argv[1]); // should probably check that the file exists.

    // get commodities
    
    // get markets


    cout << "Here is a list of " << LI->getNumConverters() << " converters:" << endl;
    LI->printConverters();
    cout << "Here is a list of " << LI->getNumMarkets() << " markets:" << endl;
    LI->printMarkets();
    cout << "Here is a list of " << LI->getNumFacilities() << " facilities:" << endl;
    LI->printFacilities();
    cout << "Here is a list of " << LI->getNumRegions() << " regions:" << endl;
    LI->printRegions();
    cout << "Here is a list of " << LI->getNumRecipes() << " recipes:" << endl;
    LI->printRecipes();
    
    // Run the simulation 
    TI->runSim();

    // Create the output file
    BI->createDB("cyclus.h5");
    // open it
    BI->openDB();
    // close it
    BI->closeDB();


    return 0;
}
