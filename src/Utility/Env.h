// Env.h
#if !defined(_ENVIRONMENT)
# define _ENVIRONMENT

#include "Logician.h"
#include <utility>

#define ENV Env::Instance()

using namespace std;

/**
 * A (singleton) environment utility to help locate files and find environment settings 
 */
class Env
{
private:
		
	/**
	 * A pointer to this Env once it has been initialized.
	 */
	static Env* instance_;


protected:
		
	/**
	 * Constructs a new Env for this simulation.
	 */
	Env();
	
public:

	/**
	 * Gives all simulation objects global access to the Env by 
	 * returning a pointer to it.
	 *
	 * @return a pointer to the Env
	 */
	static Env* Instance();


  /**
   * implement a searchpath paradigm
   *
   * @param filename
   * @param inputPath
   * @param envPath
   * @param builtinPath
   *
   * @return file path
   */
   string searchPathForFile(string filename, string inputPath, string envPath, string builtinPath);

  /**
   * Method to check the existence of and return an environment variable
   *
   * @param var is the variable to check and return 
   */
  string checkEnv(string var);



};
#endif
