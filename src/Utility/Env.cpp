// Env.cpp
// Implements the Env class
#include "Env.h"

#include "InputXML.h"
#include "GenException.h"
#include "Logician.h"

#include <sys/stat.h>
#include <iostream>
#include <string>
#include <cstring>
#include <utility>

using namespace std;

Env* Env::instance_ = 0;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Env::Env() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Env* Env::Instance() {
	// If we haven't created an ENV yet, create it, and then and return it
	// either way.
	if (0 == instance_) {
		instance_ = new Env();
	}

	return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Env::searchPathForFile(string filename, string inputPath,
                              string envPath, string builtinPath) {
  struct stat stat_info;
  int stat_result = -1;
  string::size_type begin = 0;
  string::size_type end = 0;
  string searchFilename;

  string searchPath = "./";  // initialize search path with this directory
  
  if (inputPath.size() > 0)
    searchPath += ":" + inputPath;
  if (envPath.size() > 0)
    searchPath += ":" + envPath;
  if (builtinPath.size() > 0)
    searchPath += ":" + builtinPath + ":";

  while (stat_result != 0 && begin < searchPath.length())
  {
    end = searchPath.find(":",begin);
    string thisDir = searchPath.substr(begin,end-begin);
    if (thisDir[thisDir.length()-1] != '/'){ 
      thisDir += "/";
    }
    searchFilename = thisDir +  filename;
    
    stat_result = stat(searchFilename.c_str(),&stat_info);
    begin = end + 1;
  }

  return searchFilename;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Env::checkEnv(string varname) {

  string toRet;
  if ((strlen(getenv(varname.c_str()))>0)&&(getenv(varname.c_str())!=NULL)){
    toRet = getenv(varname.c_str());
  }
  else {
    throw GenException("Environment variable " + varname + " not set.");
  }
  return toRet;
}

