#ifndef CYCLUS_SRC_XML_FILE_LOADER_H_
#define CYCLUS_SRC_XML_FILE_LOADER_H_

#include <map>
#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>

#include "composition.h"
#include "dynamic_module.h"
#include "infile_tree.h"
#include "xml_parser.h"
#include "timer.h"
#include "recorder.h"
#include "package.h"

namespace cyclus {

class Context;

/// Reads the given file path into the passed stream without modification.
void LoadRawStringstreamFromFile(std::stringstream& stream, std::string file);

/// Reads the given file path as XML into the passed stream.
/// The format may be "none", "xml", "json", or "py".
void LoadStringstreamFromFile(std::stringstream& stream, std::string file,
                              std::string format="none");

/// Reads the given file path and returns an XML string.
/// The format may be "none", "xml", "json", or "py".
std::string LoadStringFromFile(std::string file, std::string format="none");

/// Returns a list of the full module+agent spec for all agents in the given
/// input file.
std::vector<AgentSpec> ParseSpecs(std::string infile, std::string format="none");

/// Builds and returns a master cyclus input xml schema that includes the
/// sub-schemas defined by all installed cyclus modules (e.g. facility agents).
/// This is used to validate simulation input files.
std::string BuildMasterSchema(std::string schema_path, std::string infile,
                              std::string format="none");

/// Creates a composition from the recipe in the query engine.
Composition::Ptr ReadRecipe(InfileTree* qe);

/// Handles initialization of a database with information from
/// a cyclus xml input file.
///
/// @warning the LoadSim method is NOT idempotent. Only one / simulation should
/// ever be initialized per XMLFileLoader object.
class XMLFileLoader {
 public:
  /// Create a new loader reading from the xml simulation input file and writing
  /// to and initializing the backends in r. r must already have b registered.
  /// schema_file identifies the master xml rng schema used to validate the
  /// input file. The format specifies the input file format from one of:
  /// "none", "xml", "json", or "py".
  XMLFileLoader(Recorder* r, QueryableBackend* b, std::string schema_file, 
                const std::string input_file="", const std::string format="none", bool ms_print=false);

  virtual ~XMLFileLoader();

  /// Load an entire simulation from the inputfile.
  ///
  /// @param use_flat_schema whether or not to use the flat schema
  virtual void LoadSim();

 protected:
  /// Load agent specs from the input file to a map by alias
  void LoadSpecs();

  /// Method to load the simulation exchange solver.
  void LoadSolver();

  /// Method to load the simulation control parameters.
  void LoadControlParams();

  /// Method to load recipes from either the primary input file
  /// or a recipeBook catalog.
  void LoadRecipes();

  /// loads a specific recipe
  void LoadRecipe(InfileTree* qe);

  /// Loads packages
  void LoadPackages();

  /// Creates all initial agent instances from the input file.
  virtual void LoadInitialAgents();

  virtual std::string master_schema();

  /// Processes commodity priorities, such that any without a defined priority
  /// (i.e., are nonpositive), are given priority lower than the last known
  /// commodity
  void ProcessCommodities(std::map<std::string, double>* commodity_priority);

  /// Creates and builds an agent, notifying its parent. The agent init info is
  /// translated and stored in the output db.
  Agent* BuildAgent(std::string proto, Agent* parent);

  Recorder* rec_;
  Timer ti_;
  Context* ctx_;
  QueryableBackend* b_;
  
  /// flag to indicate printing master schema
  bool ms_print_;

  /// filepath to the schema
  std::string schema_path_;

  // map<specalias, spec>
  std::map<std::string, AgentSpec> specs_;

  /// the parser
  boost::shared_ptr<XMLParser> parser_;

  /// the input file name
  std::string file_;

  /// the input file format
  std::string format_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_XML_FILE_LOADER_H_
