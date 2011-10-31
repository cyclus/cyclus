#if !defined(_LOGGER_H)
#define _LOGGER_H

#define LOG(level) \
if (level > Log::ReportLevel()) ; \
else Log().Get(level)

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

enum LogLevel {LEV_ERROR, LEV_WARNING, LEV_INFO,
               LEV_DEBUG, LEV_DEBUG1, LEV_DEBUG2, LEV_DEBUG3};

class Log {

  public:
    Log() {};
    virtual ~Log();

    std::ostringstream& Get(LogLevel level);

    static LogLevel& ReportLevel() {return report_level;};
    static LogLevel ToLogLevel(std::string text);
    static std::string ToString(LogLevel level);

  protected:
    std::ostringstream os;

  private:
    Log(const Log&);
    Log& operator =(const Log&);

    static LogLevel report_level;
    static std::vector<std::string> level_to_string;
    static std::map<std::string, LogLevel> string_to_level;

    static void initialize();
    static void addLevel(LogLevel level, std::string text);

    static int spc_per_lev_;
    static int field_width_;
};

#endif

