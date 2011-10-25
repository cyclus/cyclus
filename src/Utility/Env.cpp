// Env.cpp
#include "Env.h"

#include "InputXML.h"
#include "CycException.h"
#include "Logician.h"

#include <sys/stat.h>
#include <iostream>
#include <string>
#include <cstring>
#include <utility>

using namespace std;

Env* Env::instance_ = 0;
string Env::path_from_cwd_to_cyclus_ = ".";

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
std::string Env::pathBase(std::string path) {
  string base;
  int index;

  index = path.rfind("/");
  base = path.substr(0, index);
  return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Env::getCyclusPath() {
  return path_from_cwd_to_cyclus_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Env::setCyclusPath(std::string path) {
  path_from_cwd_to_cyclus_ = path;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Env::checkEnv(std::string varname) {

  string toRet;
  if ((strlen(getenv(varname.c_str()))>0)&&(getenv(varname.c_str())!=NULL)){
    toRet = getenv(varname.c_str());
  }
  else {
    throw CycException("Environment variable " + varname + " not set.");
  }
  return toRet;
}

