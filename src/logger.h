/**
   @file logger.h

   Code providing rudimentary logging capability for the Cyclus core.
   Details outlining proper use of this logging functionality can be found
   at http://cyclus.github.com/devdoc/main.html
 */

#ifndef CYCLUS_LOGGER_H_
#define CYCLUS_LOGGER_H_


#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

namespace cyclus {

/**
   @def LOG(level, prefix)

   allows easy logging via the streaming operator similar to std::cout;
   this is the primary way to use the Logging functionality.

   @param level LogLevel category or type of log statement.

   @param prefix A std::string value that functions as a unique
   identifier for the module. Prefixes longer than 6 characters will be
   truncated.

   @warning do not place any state-changing expressions with this macro
   as they may not run if the report level excludes the specified log
   'level'.
 */
#define LOG(level, prefix) \
  if ((level > cyclus::Logger::ReportLevel()) | cyclus::Logger::NoModel()) ; \
  else cyclus::Logger().Get(level, prefix)

#define CLOG(level) \
  if (level > cyclus::Logger::ReportLevel()) ; \
  else cyclus::Logger().Get(level, "core")

#define MLOG(level) \
  if ((level > cyclus::Logger::ReportLevel()) | cyclus::Logger::NoMem()) ; \
  else cyclus::Logger().Get(level, "memory")

/**
   @enum LogLevel

   categorical (verbosity) levels for log statements.
 */
enum LogLevel {
  LEV_ERROR, //!< Use for errors that require model code or input file modification (use extremely sparingly)
  LEV_WARN, //!< Use to report questionable simulation state (use extremely sparingly)
  LEV_INFO1, //!< Information helpful for simulation users and developers alike - least verbose.
  LEV_INFO2, //!< Information helpful for simulation users and developers alike.
  LEV_INFO3, //!< Information helpful for simulation users and developers alike.
  LEV_INFO4, //!< Information helpful for simulation users and developers alike.
  LEV_INFO5, //!< Information helpful for simulation users and developers alike - most verbose.
  LEV_DEBUG1, //!< debugging information - least verbose
  LEV_DEBUG2, //!< debugging information
  LEV_DEBUG3, //!< debugging information
  LEV_DEBUG4, //!< debugging information
  LEV_DEBUG5 //!< debugging information - most verbose
};

/**
   A logging tool providing finer grained control over standard output
   for debugging and other purposes.

   @warning do not place any state-changing expressions with the LOG
   macro as they may not run if the report level excludes the specified level.
 */
class Logger {

 public:
  Logger() {};
  virtual ~Logger();

  /*!
  Returns a string stream by reference that is flushed to stdout by
  the Logger class destructor.
  */
  std::ostringstream& Get(LogLevel level, std::string prefix);

  /*!
  Use to get/set the (global) log level report cutoff.

  @return the report level cutoff by reference
  */
  static LogLevel& ReportLevel() {
    return report_level;
  };

  /// Set whether or not agent/model log entries should be printed
  static bool& NoModel() {
    return no_model;
  };

  /// Set whether or not agent/model log entries should be printed
  static bool& NoMem() {
    return no_mem;
  };

  /*!
  Converts a string into a corresponding LogLevel value.

  For strings that do not correspond to any particular LogLevel enum value,
  the method returns the LogLevel value `LEV_ERROR`.  This method is
  primarily intended for translating command line verbosity argument(s) into
  appropriate report levels.  LOG(level) statements
  */
  static LogLevel ToLogLevel(std::string text);

  /*!
  Converts a LogLevel enum value into a corrsponding string.

  For a level argments that have no corresponding string value, the string
  `BAD_LEVEL` is returned.  This method is primarily intended for translating
  LOG(level) statement levels into appropriate strings for output to stdout.
  */
  static std::string ToString(LogLevel level);

 protected:
  std::ostringstream os;

 private:
  Logger(const Logger&);

  Logger& operator =(const Logger&);

  /*!
  Cuttoff for outputing LOG(level) statement content.  Statments where
  level==report_level will print.
  */
  static LogLevel report_level;

  /// Indicates whether or not agent/model log entries should be printed
  static bool no_model;

  /// Indicates whether or not memory management log entries should be printed
  static bool no_mem;

  /// Used to map LogLevel enum values into strings
  static std::vector<std::string> level_to_string;

  /// Used to map strings into LogLevel enum values
  static std::map<std::string, LogLevel> string_to_level;

  /*!
  Used to populate the level_to_string and string_to_level vector/map before
  cyclus code execution begins
  */
  static void Initialize();

  /*!
  Used by the initialize method to populate the level_to_string and
  string_to_level static variables.
  */
  static void AddLevel(LogLevel level, std::string text);

  /*!
  The number of spaces indentation between different LogLevel enum values.
  */
  static int spc_per_lev_;

  /*!
  The width (in characters) of the printed LOG(level) statements' prefixes.
  This should be equal to or greater than the length of the longest LogLevel enum
  value converted to a string (i.e. `LEV_WARN` is the longest with 11
  characters).
  */
  static int field_width_;
};
} // namespace cyclus
#endif // ifndef CYCLUS_LOGGER_H_

