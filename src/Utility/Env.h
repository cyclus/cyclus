// Env.h
#if !defined(_ENVIRONMENT)
# define _ENVIRONMENT

#include <string>
#include "CycException.h"
#include "boost/filesystem.hpp"

class CycNoEnvVarException: public CycException {
  public: CycNoEnvVarException(std::string msg) : CycException(msg) {};
};

/**
 * A (singleton) environment utility to help locate files and find environment settings 
 */
class Env {
private:
		
  static boost::filesystem::path path_from_cwd_to_cyclus_;

  static boost::filesystem::path cwd_;

public:

  /**
   * @return path with the last item removed
   */
  static std::string pathBase(std::string path);

	/**
	 * @return the relative path from the cwd to the cyclus executable
	 */
  static std::string getCyclusPath();

	/**
   * Allows configuration and other files to be located independent
   * of the working directory from which cyclus is executed.
	 *
	 * @param path this should be argv[0] as passed to the main function
   *             (i.e. the relative path from the cwd to cyclus including
   *              the name of the cyclus executable)
	 */
  static void setCyclusRelPath(std::string path);

  /**
   * Method to check the existence of and return an environment variable
   *
   * @param var is the variable to check and return 
   */
  static std::string checkEnv(std::string var);

};

#endif

