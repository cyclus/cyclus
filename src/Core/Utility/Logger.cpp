// Logger.cpp
#include "Logger.h"
#include <cstdio>

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<string> Logger::level_to_string;
map<string, LogLevel> Logger::string_to_level;
LogLevel Logger::report_level = (Logger::initialize(), LEV_ERROR);
bool Logger::no_model = false;
bool Logger::no_mem = false;

int Logger::spc_per_lev_ = 2;
int Logger::field_width_ = 6;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostringstream& Logger::Get(LogLevel level, std::string prefix) {
  int ind_level = level - LEV_INFO1;
  if (ind_level < 0) {ind_level = 0;}

  int prefix_len = 6;
  prefix = prefix.substr(0, prefix_len);
  if (prefix.length() < prefix_len) {
    prefix = prefix + string(prefix_len - prefix.length(), ' ');
  }
  os << ToString(level) << "(" << prefix << "):";
  os << string(ind_level * spc_per_lev_, ' ');
  return os;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logger::~Logger() {
  os << endl;
  // fprintf used to maintain thread safety
  fprintf(stdout, "%s", os.str().c_str());
  fflush(stdout);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logger::initialize() {
  Logger::addLevel(LEV_ERROR, "LEV_ERROR");
  Logger::addLevel(LEV_WARN, "LEV_WARN");
  Logger::addLevel(LEV_INFO1, "LEV_INFO1");
  Logger::addLevel(LEV_INFO2, "LEV_INFO2");
  Logger::addLevel(LEV_INFO3, "LEV_INFO3");
  Logger::addLevel(LEV_INFO4, "LEV_INFO4");
  Logger::addLevel(LEV_INFO5, "LEV_INFO5");
  Logger::addLevel(LEV_DEBUG1, "LEV_DEBUG1");
  Logger::addLevel(LEV_DEBUG2, "LEV_DEBUG2");
  Logger::addLevel(LEV_DEBUG3, "LEV_DEBUG3");
  Logger::addLevel(LEV_DEBUG4, "LEV_DEBUG4");
  Logger::addLevel(LEV_DEBUG5, "LEV_DEBUG5");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LogLevel Logger::ToLogLevel(std::string text) {
  if (string_to_level.count(text) > 0) {
    return string_to_level[text];
  } else {
    return LEV_ERROR;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Logger::ToString(LogLevel level) {
  string text;
  try {
    text = level_to_string.at((int)level);
  } catch (...) {
    text = "BAD_LEVEL";
  }
  return text;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logger::addLevel(LogLevel level, std::string text) {
  // order of the following statements matters
  Logger::string_to_level[text] = level;
  text = text.substr(4);
  text = string(field_width_ - text.size(), ' ') + text;
  Logger::level_to_string.push_back(text);
}

} // namespace cyclus
