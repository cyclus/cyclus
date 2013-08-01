#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include "boost/program_options.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/lexical_cast.hpp"
#include <boost/uuid/uuid_io.hpp>

#include "Model.h"
#include "Timer.h"
#include "error.h"
#include "Env.h"
#include "Logger.h"
#include "XMLFileLoader.h"
#include "EventManager.h"
#include "sqlite_back.h"
#include "hdf5_back.h"
#include "csv_back.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

//-----------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------
int main(int argc, char* argv[]) {
  using cyclus::Logger;
  using cyclus::Env;
  using cyclus::Model;
  using cyclus::Timer;
  using cyclus::Logger;
  using cyclus::LogLevel;
  using cyclus::LEV_ERROR;
  using cyclus::EventManager;
  using cyclus::EventBackend;
  using cyclus::Hdf5Back;
  using cyclus::SqliteBack;
  using cyclus::CsvBack;
  using cyclus::XMLFileLoader;
  using cyclus::CycException;

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
    ("verb,v", po::value<std::string>(), vmessage.c_str())
    ("output-path,o", po::value<std::string>(), "output path")
    ("input-file", po::value<std::string>(), "input file")
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
  std::cout << std::endl;
  std::cout << "|--------------------------------------------|" << std::endl;
  std::cout << "|                  Cyclus                    |" << std::endl;
  std::cout << "|       a nuclear fuel cycle simulator       |" << std::endl;
  std::cout << "|  from the University of Wisconsin-Madison  |" << std::endl;
  std::cout << "|--------------------------------------------|" << std::endl;
  std::cout << std::endl;

  bool success = true;

  // respond to command line args
  if (vm.count("help")) {
    std::string err_msg = "Cyclus usage requires an input file.\n";
    err_msg += "Usage:   cyclus [path/to/input/filename]\n";
    std::cout << err_msg << std::endl;
    std::cout << desc << "\n";
    return 0;
  }

  if (vm.count("no-model")) {
    Logger::NoModel() = true;
  }

  if (vm.count("no-mem")) {
    Logger::NoMem() = true;
  }

  if (! vm.count("input-file")) {
    std::string err_msg = "Cyclus usage requires an input file.\n";
    err_msg += "Usage:   cyclus [path/to/input/filename]\n";
    std::cout << err_msg << std::endl;
    std::cout << desc << "\n";
    return 0;
  }

  if (vm.count("verb")) {
    std::string v_level = vm["verb"].as<std::string>();
    if (v_level.length() < 3) {
      Logger::ReportLevel() = (LogLevel)strtol(v_level.c_str(), NULL, 10);
    } else {
      Logger::ReportLevel() = Logger::ToLogLevel(v_level);
    }
  }

  // tell ENV the path between the cwd and the cyclus executable
  std::string path = Env::pathBase(argv[0]);
  Env::setCyclusRelPath(path);

  // read input file and setup simulation
  try {
    std::string inputFile = vm["input-file"].as<std::string>();
    std::set<std::string> module_types = Model::dynamic_module_types();
    XMLFileLoader loader(inputFile);
    loader.init();
    loader.load_control_parameters();
    loader.load_recipes();
    loader.load_dynamic_modules(module_types);
  } catch (Error e) {
    success = false;
    CLOG(LEV_ERROR) << e.what();
  }

  // Create the output file
  std::string output_path = "cyclus.sqlite";
  try {
    if (vm.count("output-path")){
      output_path = vm["output-path"].as<std::string>();
    }
  } catch (Error ge) {
    success = false;
    CLOG(LEV_ERROR) << ge.what();
  }

  std::string ext = fs::path(output_path).extension().generic_string();
  EventBackend* back;
  if (ext == ".h5") {
    back = new Hdf5Back(output_path.c_str());
  } else if (ext == ".csv") {
    back = new CsvBack(output_path.c_str());
  } else {
    back = new SqliteBack(output_path);
  }
  EM->registerBackend(back);

  // sim construction - should be handled by some entity
  Model::constructSimulation();

  // print the model list
  Model::printModelList();
  
  // Run the simulation 
  try {
    TI->runSim();
  } catch (Error err) {
    success = false;
    CLOG(LEV_ERROR) << err.what();
  }

  EM->close();
  delete back;

  // Close Dynamically loaded modules 
  try {
    Model::unloadModules();
  } catch (cyclus::Error err) {
    success = false;
    CLOG(LEV_ERROR) << err.what();
  }

  if (success) {
    std::cout << std::endl;
    std::cout << "|--------------------------------------------|" << std::endl;
    std::cout << "|                  Cyclus                    |" << std::endl;
    std::cout << "|              run successful                |" << std::endl;
    std::cout << "|--------------------------------------------|" << std::endl;
    std::cout << "Output location: " << output_path << std::endl;
    std::cout << "Simulation ID: " << boost::lexical_cast<std::string>(EM->sim_id()) << std::endl;
    std::cout << std::endl;
  } else {
    std::cout << std::endl;
    std::cout << "|--------------------------------------------|" << std::endl;
    std::cout << "|                  Cyclus                    |" << std::endl;
    std::cout << "|           run *not* successful             |" << std::endl;
    std::cout << "|--------------------------------------------|" << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
