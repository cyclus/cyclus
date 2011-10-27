
#define LOG(level) \
if (level > Log::ReportLevel()) ; \
else Log().Get(level)

#include <iostream>
#include <string>
#include <sstream>

enum LogLevel {LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_DEBUG, LOG_DEBUG1,
               LOG_DEBUG2, LOG_DEBUG3, LOG_DEBUG4};

class Log {
  public:
    Log() {};

    virtual ~Log();

    std::ostringstream& Get(LogLevel level = LOG_INFO);

    static LogLevel& ReportLevel() {return report_level;};

    static LogLevel ToLogLevel(std::string text);

  protected:
    std::ostringstream os;

  private:
    Log(const Log&);

    Log& operator =(const Log&);

    LogLevel messageLevel;

    static LogLevel report_level;

    static std::map<LogLevel, std::string> level_to_string;
    static std::map<std::string, LogLevel> string_to_level;
    static void addLevel(LogLevel level, std::string text);
    static void initialize();
    static std::string ToString(LogLevel level);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::map<LogLevel, std::string> Log::level_to_string;
std::map<std::string, LogLevel> Log::string_to_level;
LogLevel Log::report_level = (Log::initialize(), LOG_ERROR);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::ostringstream& Log::Get(LogLevel level) {
  //os << "- " << GetTimeSomehow();
  os << " " << ToString(level) << ": ";

  int ind_level = 0;
  if(level > LOG_DEBUG) {ind_level = level - LOG_DEBUG;}

  for(int i = 0; i < ind_level; i++) {
    os << "    ";
  }

  messageLevel = level;
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
  Log::addLevel(LOG_ERROR, "LOG_ERROR");
  Log::addLevel(LOG_WARNING, "LOG_WARNING");
  Log::addLevel(LOG_INFO, "LOG_INFO");
  Log::addLevel(LOG_DEBUG, "LOG_DEBUG");
  Log::addLevel(LOG_DEBUG1, "LOG_DEBUG1");
  Log::addLevel(LOG_DEBUG2, "LOG_DEBUG2");
  Log::addLevel(LOG_DEBUG3, "LOG_DEBUG3");
  Log::addLevel(LOG_DEBUG4, "LOG_DEBUG4");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LogLevel Log::ToLogLevel(std::string text) {
  return string_to_level[text];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Log::ToString(LogLevel level) {
  std::string text;
  try {
    text = level_to_string[level];
  } catch (...) {
    text = "BAD_LOG_LEVEL";
  }
  return text;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Log::addLevel(LogLevel level, std::string text) {
  Log::string_to_level[text] = level;

  int field_width = 12;
  int to_add = field_width - text.size();
  for (int i = 0; i < to_add; i++) {
    text = " " + text;
  }
 
  Log::level_to_string[level] = text;
}

