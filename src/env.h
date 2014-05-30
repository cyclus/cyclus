// env.h
#ifndef CYCLUS_SRC_ENV_H_
#define CYCLUS_SRC_ENV_H_

#include <cstdlib>
#include <string>
#include <vector>
#include "boost/filesystem.hpp"

#include "error.h"
#include "pyne.h"

namespace cyclus {

/// @class Env
/// An environment utility to help locate files and find environment
/// settings. The environment for a given simulation can be accessed via the
/// simulation's Context.
class Env {
 public:
  /// splits a string of paths by a delimeter
  ///
  /// @param s string of paths
  /// @param delim the delimeter character
  /// @param elems the vector of split strings
  static void SplitPath(const std::string &s, char delim,
                        std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
  }

  /// splits a string of paths by a delimeter
  ///
  /// @param s string of paths
  /// @param delim the delimeter character
  ///
  /// @return the vector of split strings
  static std::vector<std::string> SplitPath(const std::string &s, char delim) {
    std::vector<std::string> elems;
    Env::SplitPath(s, delim, elems);
    return elems;
  }

  /// the path basis
  /// @return path with the last item removed
  static std::string PathBase(std::string path);

  /// The relative path to the root install directory (containing bin, lib, etc.)
  /// This first checks the path of the cyclus executable itself and then tries
  /// the install directory given at run time. 
  /// @return the absolute path to the install directory
  static const std::string GetInstallPath();

  /// The relative path to the root build directory (containing bin, lib, etc.)
  /// @return the absolute path to the build directory
  static const std::string GetBuildPath();

  /// Method to check the existence of and return an environment variable
  /// @param var is the variable to check and return
  static std::string GetEnv(std::string var);

  /// @return the current value of the data environment variable
  /// CYCLUS_NUC_DATA
  static const std::string nuc_data();

  /// Returns the current rng schema.  Uses CYCLUS_RNG_SCHEMA env var if
  /// set; otherwise uses the default install location. If using the default
  /// location, set flat=true for the default flat schema.
  static const std::string rng_schema(bool flat = false);

  /// @return the current value of the module environment variable
  /// CYCLUS_PATH
  static const std::vector<std::string> cyclus_path();

  /// @return the correct environment variable delimiter based on the file system
  static const std::string EnvDelimiter();

  /// @return the correct path delimiter based on the file system
  static const std::string PathDelimiter();

  /// Initializes the path to the cyclus_nuc_data.h5 file
  ///
  /// By default, it is assumed to be located in the path given by
  /// GetInstallPath()/share; however, paths in environment variable
  /// CYCLUS_NUC_DATA are checked first.
  inline static const void SetNucDataPath() {
    pyne::NUC_DATA_PATH = nuc_data();
  }

  /// Initializes the path to the nuclear data library to p
  inline static const void SetNucDataPath(std::string p) {
    pyne::NUC_DATA_PATH = p;
    if (!boost::filesystem::exists(p))
      throw IOError("cyclus_nuc_data.h5 not found at " + p);
  }

  /// Returns the full path to a module by searching through default install
  /// and CYCLUS_PATH directories.
  static std::string FindModule(std::string path);

 private:

  /// the cwd path
  static boost::filesystem::path cwd_;

  ///the install path, cache of results from GetInstallPath()
  static std::string instdir_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_ENV_H_
