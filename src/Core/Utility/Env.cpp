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

boost::filesystem::path Env::path_from_cwd_to_cyclus_;
boost::filesystem::path Env::cwd_ = boost::filesystem::current_path();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// note that this is not used - Env is a pure static class

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Env::pathBase(std::string path) {
  string base;
  int index;

  index = path.rfind("/");
  base = path.substr(0, index);
  return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Env::getCyclusPath() {
  // return the join of cwd_ and rel path to cyclus
  boost::filesystem::path path;
  string bin_key = "/bin";
  size_t bin_found;
  string to_ret;
  if(path_from_cwd_to_cyclus_.has_root_path()) {
    path = path_from_cwd_to_cyclus_.normalize();
  } else {
    path = (cwd_ / path_from_cwd_to_cyclus_).normalize();
  }
  to_ret=path.string();
  bin_found=to_ret.rfind(bin_key);
  if (bin_found!=string::npos){
    to_ret.replace (bin_found,bin_key.length(),"/");
  }
  CLOG(LEV_DEBUG4) << "Cyclus absolute path retrieved: " 
                  << to_ret;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string Env::getBuildPath() {
  // return the join of cwd_ and rel path to cyclus MINUS the bin directory
  string to_ret = pathBase(getCyclusPath());
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Env::setCyclusRelPath(std::string path) {
  path_from_cwd_to_cyclus_ = boost::filesystem::path(path);
  CLOG(LEV_DEBUG3) << "Cyclus rel path: " << path;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Env::checkEnv(std::string varname) {
  char* pVar = getenv (varname.c_str());
  if (pVar == NULL) {
    throw CycNoEnvVarException("Environment variable " + varname + " not set.");
  } else if (strlen(pVar) == 0) {
    throw CycNoEnvVarException("Environment variable " + varname
                               + " set to an empty string.");
  }
  return pVar;
}

