#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "csv_back.h"
#include "cyclus.h"
#include "hdf5_back.h"
#include "sqlite_back.h"
#include "xml_file_loader.h"
#include "xml_flat_loader.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace cyclus;

//-----------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------
int main(int argc, char* argv[]) {
  // verbosity help msg
  std::string vmessage = "output log verbosity. Can be text:\n\n";
  vmessage +=
      "   LEV_ERROR (least verbose, default), LEV_WARN, \n"
      "   LEV_INFO1 (through 5), and LEV_DEBUG1 (through 5).\n\n";
  vmessage +=
      "Or an integer:\n\n   0 (LEV_ERROR equiv) through 11 (LEV_DEBUG5 equiv)\n";

  // parse command line options
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "produce help message")
      ("include", "print the cyclus include directory")
      ("version", "print cyclus core and dependency versions and quit")
      ("schema",
       "dump the cyclus master schema including all installed module schemas")
      ("module-schema", po::value<std::string>(),
       "dump the schema for the named module")
      ("schema-path", po::value<std::string>(),
       "manually specify the path to the cyclus master schema")
      ("flat-schema", "use the flat master simulation schema")
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

  // po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
  po::notify(vm);

  // setup context
  Recorder rec;
  Timer ti;
  Context* ctx = new Context(&ti, &rec);

  std::string schema_path = Env::GetInstallPath() + "/share/cyclus.rng.in";
  bool flat_schema = false;
  if (vm.count("flat-schema")) {
    schema_path = Env::GetInstallPath() + "/share/cyclus-flat.rng.in";
    flat_schema = true;
  }
  if (vm.count("schema-path")) {
    schema_path = vm["schema-path"].as<std::string>();
  }

  // respond to command line args that don't run a simulation
  if (vm.count("help")) {
    std::string err_msg = "Cyclus usage requires an input file.\n";
    err_msg += "Usage:   cyclus [path/to/input/filename]\n";
    std::cout << err_msg << std::endl;
    std::cout << desc << "\n";
    return 0;
  } else if (vm.count("version")) {
    std::cout << "Cyclus Core " << version::core()
        << " (" << version::describe() << ")"
        << "\n\nDependencies:\n";
    std::cout << "   Boost    " << version::boost() << "\n";
    std::cout << "   Coin-Cbc " << version::coincbc() << "\n";
    std::cout << "   Hdf5     " << version::hdf5() << "\n";
    std::cout << "   Sqlite3  " << version::sqlite3() << "\n";
    std::cout << "   xml2     " << version::xml2() << "\n";
    return 0;
  } else if (vm.count("include")) {
    std::cout << Env::GetInstallPath() << "/include/cyclus/\n";
    return 0;
  } else if (vm.count("schema")) {
    try {
      if (flat_schema) {
        std::cout << "\n" << BuildFlatMasterSchema(schema_path) << "\n";
      } else {
        std::cout << "\n" << BuildMasterSchema(schema_path) << "\n";
      }
    } catch (cyclus::IOError err) {
      std::cout << err.what() << "\n";
      return 1;
    }
    return 0;
  } else if (vm.count("module-schema")) {
    std::string name(vm["module-schema"].as<std::string>());
    try {
      DynamicModule dyn(name);
      Model* m = dyn.ConstructInstance(ctx);
      std::cout << "<element name=\"" << name << "\">\n";
      std::cout << m->schema();
      std::cout << "</element>\n";
      ctx->DelModel(m);
      dyn.CloseLibrary();
    } catch (cyclus::IOError err) {
      std::cout << err.what() << "\n";
    }
    return 0;
  }

  // announce yourself
  std::cout << std::endl;
  std::cout << "|--------------------------------------------|" << std::endl;
  std::cout << "|                  Cyclus                    |" << std::endl;
  std::cout << "|       a nuclear fuel cycle simulator       |" << std::endl;
  std::cout << "|  from the University of Wisconsin-Madison  |" << std::endl;
  std::cout << "|--------------------------------------------|" << std::endl;
  std::cout << std::endl;

  bool success = true;

  if (vm.count("no-model")) {
    Logger::NoModel() = true;
  }

  if (vm.count("no-mem")) {
    Logger::NoMem() = true;
  }

  if (!vm.count("input-file")) {
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
  std::string path = Env::PathBase(argv[0]);
  Env::SetCyclusRelPath(path);

  // read input file and setup simulation
  std::string inputFile = vm["input-file"].as<std::string>();
  XMLFileLoader* loader;
  try {
    if (flat_schema) {
      loader = new XMLFlatLoader(ctx, schema_path, inputFile);
    } else {
      loader = new XMLFileLoader(ctx, schema_path, inputFile);
    }
    loader->LoadSim();
  } catch (Error e) {
    CLOG(LEV_ERROR) << e.what();
    return 1;
  }

  // Create the output file
  std::string output_path = "cyclus.sqlite";
  try {
    if (vm.count("output-path")) {
      output_path = vm["output-path"].as<std::string>();
    }
  } catch (Error ge) {
    CLOG(LEV_ERROR) << ge.what();
    return 1;
  }

  std::string ext = fs::path(output_path).extension().generic_string();
  RecBackend* back;
  if (ext == ".h5") {
    back = new Hdf5Back(output_path.c_str());
  } else if (ext == ".csv") {
    back = new CsvBack(output_path.c_str());
  } else {
    back = new SqliteBack(output_path);
  }
  rec.RegisterBackend(back);

  // Run the simulation
  ti.RunSim(ctx);

  if (!success) {
    std::cout << std::endl;
    std::cout << "|--------------------------------------------|" << std::endl;
    std::cout << "|                  Cyclus                    |" << std::endl;
    std::cout << "|           run *not* successful             |" << std::endl;
    std::cout << "|--------------------------------------------|" << std::endl;
    std::cout << std::endl;
    return 1;
  }

  std::cout << std::endl;
  std::cout << "|--------------------------------------------|" << std::endl;
  std::cout << "|                  Cyclus                    |" << std::endl;
  std::cout << "|              run successful                |" << std::endl;
  std::cout << "|--------------------------------------------|" << std::endl;
  std::cout << "Output location: " << output_path << std::endl;
  std::cout << "Simulation ID: " << boost::lexical_cast<std::string>
      (ctx->sim_id()) << std::endl;
  std::cout << std::endl;

  delete ctx;
  rec.close();
  delete back;
  delete loader;

  return 0;
}
