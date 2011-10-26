
#define LOG(level) \
if (level > Log::report_level) ; \
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

    static LogLevel report_level;

  protected:
    std::ostringstream os;

  private:
    Log(const Log&);

    Log& operator =(const Log&);

    std::string toString(LogLevel level);

    LogLevel messageLevel;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LogLevel Log::report_level = LOG_ERROR;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::ostringstream& Log::Get(LogLevel level) {
  //os << "- " << GetTimeSomehow();
  os << " " << toString(level) << ": ";

  int ind_level = 0;
  if(level > LOG_DEBUG) {ind_level = level - LOG_DEBUG;}

  for(int i = 0; i < ind_level; i++) {
    os << "   ";
  }

  messageLevel = level;
  return os;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Log::~Log() {
  os << std::endl;
  // fprintf used to maintain thread safety
  fprintf(stderr, "%s", os.str().c_str());
  fflush(stderr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Log::toString(LogLevel level) {
  std::string text;
  int field_width = 12;

  switch(level) {
    case LOG_ERROR:
      text = "LOG_ERROR";
      break;
    case LOG_WARNING:
      text = "LOG_WARNING";
      break;
    case LOG_INFO:
      text = "LOG_INFO";
      break;
    case LOG_DEBUG:
      text = "LOG_DEBUG";
      break;
    case LOG_DEBUG1:
      text = "LOG_DEBUG1";
      break;
    case LOG_DEBUG2:
      text = "LOG_DEBUG2";
      break;
    case LOG_DEBUG3:
      text = "LOG_DEBUG3";
      break;
    case LOG_DEBUG4:
      text = "LOG_DEBUG4";
      break;
    default:
      text = "BAD_LOG_LEVEL";
  }

  int to_add = field_width - text.size();
  for (int i = 0; i < to_add; i++) {
    text = " " + text;
  }
 
  return text;
}

