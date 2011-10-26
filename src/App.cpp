#include <iostream>
#include <cstdlib>
#include <cstring>

#include "Model.h"
#include "Logician.h"
#include "BookKeeper.h"
#include "Timer.h"
#include "InputXML.h"
#include "CycException.h"
#include "Env.h"
#include "Logger.h"

using namespace std;

//-----------------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {

  ////// logging example //////

  // set the reporting/printing cutoff level (do once per session/run
  Log::report_level = LOG_DEBUG2;

  const int count = 3;

  // use the LOG macro where its arg is the log level or type
  // LOG_DEBUG is the type used for this logging statement
  // the macro statment returns a string stream that can be used like cout
  LOG(LOG_DEBUG) << "A loop with " << count << " iterations";
  for (int i = 0; i != count; ++i) {
     LOG(LOG_DEBUG1) << "the counter i = " << i;
  }

  ////// end logging example //////

  // tell ENV the path between the cwd and the cyclus executable
  string path = ENV->pathBase(argv[0]);
  ENV->setCyclusPath(path);

  // announce yourself
  cout << "|--------------------------------------------|" << endl;
  cout << "|                  Cyclus                    |" << endl;
  cout << "|       a nuclear fuel cycle simulator       |" << endl;
  cout << "|  from the University of Wisconsin-Madison  |" << endl;
  cout << "|--------------------------------------------|" << endl;

  if(argc<2) {
    string err_msg = "Cyclus usage requires an input file.\n";
    err_msg += "Usage:   ./cyclus [path/to/input/filename]\n";
    cout << err_msg << endl;
    return 0;
  }

  // parse arguments
  try {
    // read input file and setup simulation
    XMLinput->load_file(argv[1]); 


    cout << "Here is a list of " << LI->getNumModels(CONVERTER) << " converters:" << endl;
    LI->printModelList(CONVERTER);
    cout << "Here is a list of " << LI->getNumModels(MARKET) << " markets:" << endl;
    LI->printModelList(MARKET);
    cout << "Here is a list of " << LI->getNumModels(FACILITY) << " facilities:" << endl;
    LI->printModelList(FACILITY);
    cout << "Here is a list of " << LI->getNumModels(REGION) << " regions:" << endl;
    LI->printModelList(REGION);
    cout << "Here is a list of " << LI->getNumRecipes() << " recipes:" << endl;
    LI->printRecipes();
    
    // Run the simulation 
    TI->runSim();

    // Create the output file
    BI->createDB();

    BI->writeModelList(INST);
    BI->writeModelList(REGION);
    BI->writeModelList(FACILITY);
    BI->writeModelList(MARKET);
    BI->writeTransList();
    BI->writeMatHist();

    BI->closeDB();
  } catch (CycException ge) {
    cout << ge.what() << endl;
  };

  return 0;
}
