#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include "boost/program_options.hpp"
#include "boost/shared_ptr.hpp"

#include "Model.h"
#include "BookKeeper.h"
#include "Timer.h"
#include "InputXML.h"
#include "CycException.h"
#include "Env.h"
#include "Logger.h"

using namespace std;
namespace po = boost::program_options;

//-----------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------
int main(int argc, char* argv[]) {
  // verbosity help msg
  std::string vmessage = "output log verbosity. Can be text:\n\n";
  vmessage += "   LEV_ERROR (least verbose, default), LEV_WARN, \n   LEV_INFO1 (through 5), and LEV_DEBUG1 (through 5).\n\n";
  vmessage += "Or an integer:\n\n   0 (LEV_ERROR equiv) through 11 (LEV_DEBUG5 equiv)\n";

  // parse command line options
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("no-model", "only print log entries from cyclus core code")
    ("no-mem", "exclude memory log statement from logger output")
    ("verb,v", po::value<string>(), vmessage.c_str())
    ("output-path,o", po::value<string>(), "output path")
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

  // announce yourself
  cout << endl;
  cout << "|--------------------------------------------|" << endl;
  cout << "|                  Cyclus                    |" << endl;
  cout << "|       a nuclear fuel cycle simulator       |" << endl;
  cout << "|  from the University of Wisconsin-Madison  |" << endl;
  cout << "|--------------------------------------------|" << endl;
  cout << endl;

  // respond to command line args
  if (vm.count("help")) {
    string err_msg = "Cyclus usage requires an input file.\n";
    err_msg += "Usage:   cyclus [path/to/input/filename]\n";
    cout << err_msg << endl;
    cout << desc << "\n";
    return 0;
  }

  if (vm.count("no-model")) {
    Logger::NoModel() = true;
  }

  if (vm.count("no-mem")) {
    Logger::NoMem() = true;
  }

  if (! vm.count("input-file")) {
    string err_msg = "Cyclus usage requires an input file.\n";
    err_msg += "Usage:   cyclus [path/to/input/filename]\n";
    cout << err_msg << endl;
    cout << desc << "\n";
    return 0;
  }

  if (vm.count("verb")) {
    std::string v_level = vm["verb"].as<string>();
    if (v_level.length() < 3) {
      Logger::ReportLevel() = (LogLevel)strtol(v_level.c_str(), NULL, 10);
    } else {
      Logger::ReportLevel() = Logger::ToLogLevel(v_level);
    }
  }

  // tell ENV the path between the cwd and the cyclus executable
  string path = Env::pathBase(argv[0]);
  Env::setCyclusRelPath(path);

  // Create the output file
  try {
    if (vm.count("output-path")){
      BI->createDB(vm["output-path"].as<string>());
    } else { 
      BI->createDB();
    }
  } catch (CycException ge) {
    CLOG(LEV_ERROR) << ge.what();
  }

  // read input file and setup simulation
  try {
    XMLinput->load_file(vm["input-file"].as<string>()); 
  } catch (CycIOException ge) {
    CLOG(LEV_ERROR) << ge.what();
    return 0;
  } catch (CycException e) {
    CLOG(LEV_ERROR) << e.what();
  }

  Model::printModelList();
  
  // Run the simulation 
  try {
    TI->runSim();
  } catch (CycException err) {
    CLOG(LEV_ERROR) << err.what();
  }

  // Close the output file
  try {
    BI->closeDB();
  } catch (CycException ge) {
    CLOG(LEV_ERROR) << ge.what();
  }

  return 0;
}
