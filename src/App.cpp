#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include "boost/program_options.hpp"

#include "Model.h"
#include "Logician.h"
#include "BookKeeper.h"
#include "Timer.h"
#include "InputXML.h"
#include "CycException.h"
#include "Env.h"
#include "Logger.h"

using namespace std;
namespace po = boost::program_options;

//-----------------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {

  // parse command line options
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("verbosity,v", po::value<string>(), "set output log verbosity level")
    ("input-file", po::value<string>(), "input file")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  po::positional_options_description p;
  p.add("input-file", 1);

  //po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
  po::notify(vm);

  // tell ENV the path between the cwd and the cyclus executable
  string path = ENV->pathBase(argv[0]);
  ENV->setCyclusPath(path);

  // announce yourself
  cout << "|--------------------------------------------|" << endl;
  cout << "|                  Cyclus                    |" << endl;
  cout << "|       a nuclear fuel cycle simulator       |" << endl;
  cout << "|  from the University of Wisconsin-Madison  |" << endl;
  cout << "|--------------------------------------------|" << endl;

  // respond to command line args
  if (vm.count("help")) {
    string err_msg = "Cyclus usage requires an input file.\n";
    err_msg += "Usage:   cyclus [path/to/input/filename]\n";
    cout << err_msg << endl;
    cout << desc << "\n";
    return 0;
  }

  if (! vm.count("input-file")) {
    string err_msg = "Cyclus usage requires an input file.\n";
    err_msg += "Usage:   cyclus [path/to/input/filename]\n";
    cout << err_msg << endl;
    cout << desc << "\n";
    return 0;
  }

  if (vm.count("verbosity")) {
    Log::ReportLevel() = Log::ToLogLevel(vm["verbosity"].as<string>());
  }

  ////// logging example //////
  // use the LOG macro where its arg is the log level or type
  // LEV_DEBUG is the type used for this logging statement
  // the macro statment returns a string stream that can be used like cout
  const int count = 3;
  LOG(LEV_DEBUG) << "A loop with " << count << " iterations";
  for (int i = 0; i != count; ++i) {
     LOG(LEV_DEBUG1) << "the counter i = " << i;
  }
  ////// end logging example //////

  // initialize simulation
  try {
    // read input file and setup simulation
    XMLinput->load_file(vm["input-file"].as<string>()); 

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
  } catch (CycException gen_exception) {
    cout << gen_exception.what() << endl;
  };

  return 0;
}

