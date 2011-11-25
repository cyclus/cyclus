// Logger.h

#if !defined(_LOGGER_H)
#define _LOGGER_H

/*!
@def LOG(level)

@brief allows easy logging via the streaming operator similar to std::cout

This is the primary way to use the Log class. This macro returns an
anonymous instance of the Log class (not assigned to any variable) that can be
streamed into just like any string stream (e.g.  std::cout).  This macro does a
check on the given #LogLevel 'level' argument; if the specified level is not within
the report-level range, the macro does nothing, limiting the performance impact
of logging statements.

@param level #LogLevel category or type of log statement.

@warning do not place any state-changing expressions with this macro as they
         may not run if the report level excludes the specified log 'level'.

*/
#define LOG(level) \
if (level > Log::ReportLevel()) ; \
else Log().Get(level)

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

/*!
@enum LogLevel

@brief categorical (verbosity) levels for log statements.

*/
enum LogLevel {
               LEV_ERROR, //!< Use for errors that require model code or input file modification (use extremely sparingly)
               LEV_WARNING, //!< Use to report questionable simulation state (use extremely sparingly)
               LEV_INFO, //!< Information helpful for non-debugging simulation runs (use sparingly)
               LEV_DEBUG, //!< overview debugging information (use occasionally)
               LEV_DEBUG1, //!< debugging information (use often)
               LEV_DEBUG2, //!< debugging information (use very often)
               LEV_DEBUG3 //!< debugging information (use most often)
               };

/*!

@brief A logging tool providing finer grained control over standard output for debugging.

The Log class is primarily intended to aid debugging.
The Log class should be used via the LOG macro
exclusively.  Usage details are outlined in @ref logging "Using the Logger".

*/
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

