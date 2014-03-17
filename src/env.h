// env.h
#ifndef CYCLUS_SRC_ENV_H_
#define CYCLUS_SRC_ENV_H_

#include <string>

#include "boost/filesystem.hpp"

#include "pyne.h"

namespace cyclus {

/// @class Env
/// An environment utility to help locate files and find environment
/// settings. The environment for a given simulation can be accessed via the
/// simulation's Context.
class Env {
 public:
  /// the path basis
  /// @return path with the last item removed
  static std::string PathBase(std::string path);

  /// the relative cyclus path
  /// @return the relative path from the cwd to the cyclus executable
  static std::string GetCyclusPath();

  /// the cyclus output path
  /// @return the full path to the directory of the cyclus output
  static std::string GetCyclusOutputPath();

  /// the relative path to the root install directory (containing bin, lib, etc.)
  /// @return the absolute path to the build directory
  static const std::string GetInstallPath();

  /// Returns the default path for installed cyclus simulation agent
  /// modules.
  inline static const std::string module_install_path() {
    return Env::GetInstallPath() + "/lib/cyclus";
  }

  /// the relative path to the root build directory (containing bin, lib, etc.)
  /// @return the absolute path to the build directory
  static const std::string GetBuildPath();

  /// Allows configuration and other files to be located independent
  /// of the working directory from which cyclus is executed.
  ///
  /// @param path this should be argv[0] as passed to the main function
  /// (i.e. the relative path from the cwd to cyclus including
  /// the name of the cyclus executable)
  static void SetCyclusRelPath(std::string path);

  /// Allows the user to set the cyclus output path
  /// @param path this should be an argument flagged -o as passed to the
  /// main function (i.e. the relative path from the cwd to the desired
  /// output directory)
  static void SetCyclusOutputPath(std::string path);

  /// Method to check the existence of and return an environment variable
  /// @param var is the variable to check and return
  static std::string CheckEnv(std::string var);

  /// @return the name of the environment variable used for module
  /// installations, currently set to CYCLUS_MODULE_PATH
  static const std::string ModuleEnvVarName();

  /// @return the current value of the module environment variable
  static const std::string ModuleEnvVar();

  /// Returns names of all dynamically loadable, discoverable modules.
  /// Returned lists includes modules installed in the default location
  /// (module_install_path) as well as modules in directories listed in the
  /// CYCLUS_MODULE_PATH environment variable.
  ///
  /// @return module names without the "lib" prefix or file extension.
  /// Equivalent to the module class name (e.g. SourceFacility).
  static std::vector<std::string> ListModules();

  /// @return the correct environment variable delimiter based on the file system
  static const std::string EnvDelimiter();

  /// @return the correct path delimiter based on the file system
  static const std::string PathDelimiter();

  /// Environment searches for a library and, if found, sets the path. The search
  /// occurs across the default install location as well as any included in the
  /// CYCLUS_MODULE_PATH environment variable.
  /// @param name the name of the library to search for
  /// @param path_found the variable to set with the path to the library
  /// @return true if the library is found, false if not
  static bool FindModuleLib(std::string name,
                            boost::filesystem::path& path_found);

  /// Initializes the path to the nuclear data library to a default location
  inline static const void SetNucDataPath() {
    pyne::NUC_DATA_PATH = Env::GetBuildPath() + "/share/cyclus_nuc_data.h5";
  }

  /// Initializes the path to the nuclear data library to p
  inline static const void SetNucDataPath(std::string p) {
    pyne::NUC_DATA_PATH = p;
  }

 private:
  /// the relative path from cwd to cyclus
  static boost::filesystem::path path_from_cwd_to_cyclus_;

  /// the cwd path
  static boost::filesystem::path cwd_;

  /// the cwd path
  static boost::filesystem::path path_to_output_dir_;

  /// Taken directly from
  /// http://www.boost.org/doc/libs/1_31_0/libs/filesystem/doc/index.htm. This
  /// function recursively searches a directory and its sub-directories for the
  /// file name, returning a bool, and if successful, the path to the file that
  /// was found.
  /// @param dir_path the directory path to search
  /// @param file_name the file to search for
  /// @param path_found the path, which is populated if the file is found
  /// @return true if the file is found, false if it isn't
  static bool FindFile(const boost::filesystem::path& dir_path,
                       const std::string& file_name,
                       boost::filesystem::path& path_found);
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_ENV_H_
