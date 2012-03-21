// Env.h
#if !defined(_ENVIRONMENT)
# define _ENVIRONMENT

#include <string>
#include "boost/filesystem.hpp"

#include "CycException.h"

class CycNoEnvVarException: public CycException {
  public: CycNoEnvVarException(std::string msg) : CycException(msg) {};
};

/**
 * @class Env
 *
 *  A (singleton) environment utility to help locate 
 * files and find environment settings 
 */
class Env {
private:
  /**
   *  the relative path from cwd to cyclus
   */
  static boost::filesystem::path path_from_cwd_to_cyclus_;

  /**
   *  the cwd path
   */
  static boost::filesystem::path cwd_;

public:

  /**
   *  the path basis
   *
   * @return path with the last item removed
   */
  static std::string pathBase(std::string path);

  /**
   *  the relative cyclus path
   * 
   * @return the relative path from the cwd to the cyclus executable
   */
  static std::string getCyclusPath();
  
  /**
   *  Allows configuration and other files to be located independent
   * of the working directory from which cyclus is executed.
   *
   * @param path this should be argv[0] as passed to the main function
   * (i.e. the relative path from the cwd to cyclus including
   * the name of the cyclus executable)
   */
  static void setCyclusRelPath(std::string path);
  
  /**
   *  Method to check the existence of and return an environment variable
   *
   * @param var is the variable to check and return 
   */
  static std::string checkEnv(std::string var);

};

#endif

