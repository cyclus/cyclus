// Env.cpp
#include "Env.h"

#include "InputXML.h"
#include "Logger.h"

#include <sys/stat.h>
#include <iostream>
#include <string>
#include <cstring>
#include <utility>

using namespace std;

Env* Env::instance_ = 0;
boost::filesystem::path Env::path_from_cwd_to_cyclus_;
boost::filesystem::path Env::cwd_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Env::Env() {
  cwd_ = boost::filesystem::current_path();
  CLOG(LEV_INFO4) << "Current working directory: " << cwd_;
}

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
std::string Env::pathBase(std::string path) {
  string base;
  int index;

  index = path.rfind("/");
  base = path.substr(0, index);
  return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Env::getCyclusPath() {
  // return the join of cwd_ and rel path to cyclus
  return (cwd_ / path_from_cwd_to_cyclus_).string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Env::setCyclusRelPath(std::string path) {
  path_from_cwd_to_cyclus_ = boost::filesystem::path(path);
  CLOG(LEV_INFO4) << "Cyclus rel path: " << path;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Env::checkEnv(std::string varname) {
  char* pVar = getenv (varname.c_str());
  if (pVar == NULL) {
    throw CycNoEnvVarException("Environment variable " + varname + " not set.");
  } else if (strlen(pVar) == 0) {
    throw CycNoEnvVarException("Environment variable " + varname
                               + " set to an empty string.");
  }
  return pVar;
}

