
#define LOG(level) \
if (level > Log::ReportLevel()) ; \
else Log().Get(level) << " [MJG]: "

#include <iostream>
#include <string>
#include <sstream>

enum LogLevel {LEV_WARNING, LEV_INFO,
               LEV_DEBUG, LEV_DEBUG1, LEV_DEBUG2, LEV_DEBUG3};

class Log {
  public:
    Log() {};

    virtual ~Log();

    std::ostringstream& Get(LogLevel level = LEV_INFO);

    static LogLevel& ReportLevel() {return report_level;};

    static LogLevel ToLogLevel(std::string text);
    static std::string ToString(LogLevel level);

  protected:
    std::ostringstream os;

  private:
    Log(const Log&);

    Log& operator =(const Log&);

    LogLevel messageLevel;

    static LogLevel report_level;

    static std::vector<std::string> level_to_string;
    static std::map<std::string, LogLevel> string_to_level;

    static void initialize();
    static void addLevel(LogLevel level, std::string text);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> Log::level_to_string;
std::map<std::string, LogLevel> Log::string_to_level;
LogLevel Log::report_level = (Log::initialize(), LEV_WARNING);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::ostringstream& Log::Get(LogLevel level) {
  //os << "- " << GetTimeSomehow();
  os << " " << ToString(level) << ": ";

  int ind_level = 0;
  if(level > LEV_DEBUG) {ind_level = level - LEV_DEBUG;}

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
  Log::addLevel(LEV_WARNING, "LEV_WARNING");
  Log::addLevel(LEV_INFO, "LEV_INFO");
  Log::addLevel(LEV_DEBUG, "LEV_DEBUG");
  Log::addLevel(LEV_DEBUG1, "LEV_DEBUG1");
  Log::addLevel(LEV_DEBUG2, "LEV_DEBUG2");
  Log::addLevel(LEV_DEBUG3, "LEV_DEBUG3");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LogLevel Log::ToLogLevel(std::string text) {
  return string_to_level[text];
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
  Log::string_to_level[text] = level;

  int field_width = 12;
  int to_add = field_width - text.size();
  for (int i = 0; i < to_add; i++) {
    text = " " + text;
  }
 
  Log::level_to_string.push_back(text);
}

