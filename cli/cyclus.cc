#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "csv_back.h"
#include "cyclus.h"
#include "hdf5_back.h"
#include "sqlite_back.h"
#include "xml_file_loader.h"
#include "xml_flat_loader.h"

#include "sim_init.h"
#include "query_backend.h"
#include <boost/uuid/uuid_generators.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace cyclus;

struct ArgInfo {
  po::variables_map vm; // holds parsed/specified cli opts and values
  po::options_description desc; // holds cli opts description;
  bool flat_schema;
  std::string schema_path;
  std::string output_path;
};

// Describes and parses cli arguments.  Returns the error code that main should
// return early OR -1 if main should not return early.
int ParseCliArgs(ArgInfo* ai, int argc, char* argv[]);

// Processes and handles args that don't run a simulation. Returns the error
// code that main should return early OR -1 if main should not return early.
int EarlyExitArgs(const ArgInfo& ai);

// Using cli flags, retrieves and sets global params for the simulation.
void GetSimInfo(ArgInfo* ai);

static std::string usage = "Usage:   cyclus [opts] [input-file]";

//-----------------------------------------------------------------------
// Main entry point for the test application...
//-----------------------------------------------------------------------
int main(int argc, char* argv[]) {
  // close all dlopen'd modules AFTER everything else destructs
  DynamicModule::Closer cl;

  // tell ENV the path between the cwd and the cyclus executable
  std::string path = Env::PathBase(argv[0]);
  Env::SetCyclusRelPath(path);

  // tell pyne about the path to nuc data
  Env::SetNucDataPath();

  // handle cli option flags
  ArgInfo ai;
  int ret = ParseCliArgs(&ai, argc, argv);
  if (ret > -1 ) {
    return ret;
  }
  GetSimInfo(&ai);
  ret = EarlyExitArgs(ai);
  if (ret > -1 ) {
    return ret;
  }

  // process positional args
  if (!ai.vm.count("input-file")) {
    std::cout << "No input file specified.\n"
              << usage << "\n\n"
              << ai.desc << "\n";
    return 1;
  }
  std::string infile = ai.vm["input-file"].as<std::string>();

  // announce yourself
  std::cout << "              :                                                               " << std::endl;
  std::cout << "          .CL:CC CC             _Q     _Q  _Q_Q    _Q    _Q              _Q   " << std::endl;
  std::cout << "        CC;CCCCCCCC:C;         /_\\)   /_\\)/_/\\\\)  /_\\)  /_\\)            /_\\)  " << std::endl;
  std::cout << "        CCCCCCCCCCCCCl       __O|/O___O|/O_OO|/O__O|/O__O|/O____________O|/O__" << std::endl;
  std::cout << "     CCCCCCf     iCCCLCC     /////////////////////////////////////////////////" << std::endl;
  std::cout << "     iCCCt  ;;;;;.  CCCC                                                      " << std::endl;
  std::cout << "    CCCC  ;;;;;;;;;. CClL.                          c                         " << std::endl;
  std::cout << "   CCCC ,;;       ;;: CCCC  ;                   : CCCCi                       " << std::endl;
  std::cout << "    CCC ;;         ;;  CC   ;;:                CCC`   `C;                     " << std::endl;
  std::cout << "  lCCC ;;              CCCC  ;;;:             :CC .;;. C;   ;    :   ;  :;;   " << std::endl;
  std::cout << "  CCCC ;.              CCCC    ;;;,           CC ;    ; Ci  ;    :   ;  :  ;  " << std::endl;
  std::cout << "   iCC :;               CC       ;;;,        ;C ;       CC  ;    :   ; .      " << std::endl;
  std::cout << "  CCCi ;;               CCC        ;;;.      .C ;       tf  ;    :   ;  ;.    " << std::endl;
  std::cout << "  CCC  ;;               CCC          ;;;;;;; fC :       lC  ;    :   ;    ;:  " << std::endl;
  std::cout << "   iCf ;;               CC         :;;:      tC ;       CC  ;    :   ;     ;  " << std::endl;
  std::cout << "  fCCC :;              LCCf      ;;;:         LC :.  ,: C   ;    ;   ; ;   ;  " << std::endl;
  std::cout << "  CCCC  ;;             CCCC    ;;;:           CCi `;;` CC.  ;;;; :;.;.  ; ,;  " << std::endl;
  std::cout << "    CCl ;;             CC    ;;;;              CCC    CCL                     " << std::endl;
  std::cout << "   tCCC  ;;        ;; CCCL  ;;;                  tCCCCC.                      " << std::endl;
  std::cout << "    CCCC  ;;     :;; CCCCf  ;                     ,L                          " << std::endl;
  std::cout << "     lCCC   ;;;;;;  CCCL                                                      " << std::endl;
  std::cout << "     CCCCCC  :;;  fCCCCC                                                      " << std::endl;
  std::cout << "      . CCCC     CCCC .                                                       " << std::endl;
  std::cout << "       .CCCCCCCCCCCCCi                                                        " << std::endl;
  std::cout << "          iCCCCCLCf                                                           " << std::endl;
  std::cout << "           .  C. ,                                                            " << std::endl;
  std::cout << "              :                                                               " << std::endl;

  // create db backends
  FullBackend* fback = new SqliteBack(ai.output_path);
  RecBackend* rback = NULL;
  std::string ext = fs::path(ai.output_path).extension().generic_string();
  if (ext == ".h5") { // not queryable
    rback = new Hdf5Back(ai.output_path.c_str());
  } else if (ext == ".csv") { // not queryable
    rback = new CsvBack(ai.output_path.c_str());
  }

  // read input file and initialize db from input file
  boost::uuids::uuid simid;
  try {
    if (ai.flat_schema) {
      XMLFlatLoader l(fback, ai.schema_path, infile);
      simid = l.LoadSim();
    } else {
      XMLFileLoader l(fback, ai.schema_path, infile);
      simid = l.LoadSim();
    }
  } catch (Error e) {
    CLOG(LEV_ERROR) << e.what();
    return 1;
  }

  // initialize sim from output db
  SimInit si;
  si.Init(fback, simid);
  si.recorder()->RegisterBackend(fback);
  if (rback != NULL) {
    si.recorder()->RegisterBackend(rback);
  }

  // Run the simulation
  si.timer()->RunSim();

  delete fback;
  if (rback != NULL) {
    delete rback;
  }

  std::cout << std::endl;
  std::cout << "Status: Cyclus run successful!" << std::endl;
  std::cout << "Output location: " << ai.output_path << std::endl;
  std::cout << "Simulation ID: " << boost::lexical_cast<std::string>
               (si.context()->sim_id()) << std::endl;

  return 0;
};

int ParseCliArgs(ArgInfo* ai, int argc, char* argv[]) {
  // verbosity help msg
  std::string vmessage = "output log verbosity. Can be text:\n\n";
  vmessage +=
      "   LEV_ERROR (least verbose, default), LEV_WARN, \n"
      "   LEV_INFO1 (through 5), and LEV_DEBUG1 (through 5).\n\n";
  vmessage +=
      "Or an integer:\n\n   0 (LEV_ERROR equiv) through 11 (LEV_DEBUG5 equiv)\n";

  // parse command line options
  ai->desc.add_options()
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
  try {
    po::store(po::parse_command_line(argc, argv, ai->desc), ai->vm);
  } catch(std::exception err) {
    std::cout << "Invalid arguments.\n"
              <<  usage << "\n\n"
              << ai->desc << "\n";
    return 1;
  }
  po::notify(ai->vm);

  po::positional_options_description p;
  p.add("input-file", 1);

  po::store(po::command_line_parser(argc, argv).
            options(ai->desc).positional(p).run(), ai->vm);
  po::notify(ai->vm);
  return -1;
}

int EarlyExitArgs(const ArgInfo& ai) {
  // respond to command line args that don't run a simulation
  if (ai.vm.count("help")) {
    std::cout << usage << "\n\n"
              << ai.desc << "\n";
    return 0;
  } else if (ai.vm.count("version")) {
    std::cout << "Cyclus Core " << version::core()
              << " (" << version::describe() << ")"
              << "\n\nDependencies:\n";
    std::cout << "   Boost    " << version::boost() << "\n";
    std::cout << "   Coin-Cbc " << version::coincbc() << "\n";
    std::cout << "   Hdf5     " << version::hdf5() << "\n";
    std::cout << "   Sqlite3  " << version::sqlite3() << "\n";
    std::cout << "   xml2     " << version::xml2() << "\n";
    return 0;
  } else if (ai.vm.count("include")) {
    std::cout << Env::GetInstallPath() << "/include/cyclus/\n";
    return 0;
  } else if (ai.vm.count("schema")) {
    try {
      if (ai.flat_schema) {
        std::cout << "\n" << BuildFlatMasterSchema(ai.schema_path) << "\n";
      } else {
        std::cout << "\n" << BuildMasterSchema(ai.schema_path) << "\n";
      }
    } catch (cyclus::IOError err) {
      std::cout << err.what() << "\n";
      return 1;
    }
    return 0;
  } else if (ai.vm.count("module-schema")) {
    std::string name(ai.vm["module-schema"].as<std::string>());
    try {
      Recorder rec;
      Timer ti;
      Context* ctx = new Context(&ti, &rec);
      Model* m = DynamicModule::Make(ctx, name);
      std::cout << "<element name=\"" << name << "\">\n";
      std::cout << m->schema();
      std::cout << "</element>\n";
      ctx->DelModel(m);
    } catch (cyclus::IOError err) {
      std::cout << err.what() << "\n";
    }
    return 0;
  }
  return -1; // main should not return early
};

void GetSimInfo(ArgInfo* ai) {
  // schema info
  ai->schema_path = Env::GetInstallPath() + "/share/cyclus.rng.in";
  ai->flat_schema = false;
  if (ai->vm.count("flat-schema")) {
    ai->schema_path = Env::GetInstallPath() + "/share/cyclus-flat.rng.in";
    ai->flat_schema = true;
  }
  if (ai->vm.count("schema-path")) {
    ai->schema_path = ai->vm["schema-path"].as<std::string>();
  }

  // logging params
  if (ai->vm.count("no-model")) {
    Logger::NoModel() = true;
  }
  if (ai->vm.count("no-mem")) {
    Logger::NoMem() = true;
  }
  if (ai->vm.count("verb")) {
    std::string v_level = ai->vm["verb"].as<std::string>();
    if (v_level.length() < 3) {
      Logger::ReportLevel() = (LogLevel)strtol(v_level.c_str(), NULL, 10);
    } else {
      Logger::ReportLevel() = Logger::ToLogLevel(v_level);
    }
  }

  // output path
  ai->output_path = "cyclus.sqlite";
  if (ai->vm.count("output-path")) {
    ai->output_path = ai->vm["output-path"].as<std::string>();
  }
}

