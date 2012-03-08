// Env.h
#if !defined(_ENVIRONMENT)
# define _ENVIRONMENT

#include <string>
#include "CycException.h"
#include "boost/filesystem.hpp"

#define ENV Env::Instance()

class CycNoEnvVarException: public CycException {
  public: CycNoEnvVarException(std::string msg) : CycException(msg) {};
};

/**
 * A (singleton) environment utility to help locate files and find environment settings 
 */
class Env {
private:
		
	/**
	 * A pointer to this Env once it has been initialized.
	 */
	static Env* instance_;

  static boost::filesystem::path path_from_cwd_to_cyclus_;

  static boost::filesystem::path cwd_;

protected:
		
	/**
	 * Constructs a new Env for this simulation.
	 */
	Env();
	
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
	 * Gives all simulation objects global access to the Env by 
	 * returning a pointer to it.
	 *
	 * @return a pointer to the Env
	 */
	static Env* Instance();

  /**
   * Method to check the existence of and return an environment variable
   *
   * @param var is the variable to check and return 
   */
  static std::string checkEnv(std::string var);

};

#endif

