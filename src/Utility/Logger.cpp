
#include "Logger.h"
#include <cstdio>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> Log::level_to_string;
std::map<std::string, LogLevel> Log::string_to_level;
LogLevel Log::report_level = (Log::initialize(), LEV_ERROR);

int Log::spc_per_lev_ = 3;
int Log::field_width_ = 12;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::ostringstream& Log::Get(LogLevel level) {
  int ind_level = 0;
  if(level > LEV_DEBUG) {ind_level = level - LEV_DEBUG;}
  os << ToString(level) << ": ";
  os << std::string(ind_level * spc_per_lev_, ' ');
  return os;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Log::~Log() {
  os << std::endl;
  // fprintf used to maintain thread safety
  fprintf(stdout, "%s", os.str().c_str());
  fflush(stdout);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Log::initialize() {
  Log::addLevel(LEV_ERROR, "LEV_ERROR");
  Log::addLevel(LEV_WARNING, "LEV_WARNING");
  Log::addLevel(LEV_INFO, "LEV_INFO");
  Log::addLevel(LEV_DEBUG, "LEV_DEBUG");
  Log::addLevel(LEV_DEBUG1, "LEV_DEBUG1");
  Log::addLevel(LEV_DEBUG2, "LEV_DEBUG2");
  Log::addLevel(LEV_DEBUG3, "LEV_DEBUG3");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LogLevel Log::ToLogLevel(std::string text) {
  if (string_to_level.count(text) > 0) {
    return string_to_level[text];
  } else {
    return LEV_ERROR;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Log::ToString(LogLevel level) {
  std::string text;
  try {
    text = level_to_string.at((int)level);
  } catch (...) {
    text = "BAD_LEVEL";
  }
  return text;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Log::addLevel(LogLevel level, std::string text) {
  // order of the following statements matters
  Log::string_to_level[text] = level;
  text = std::string(field_width_ - text.size(), ' ') + text;
  Log::level_to_string.push_back(text);
}

