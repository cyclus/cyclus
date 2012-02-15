/*!
@file Logger.h

Code providing rudimentary logging capability for the Cyclus core. Details
outlining proper use of this logging functionality are outlined @ref Logger "here".

*/

#if !defined(_LOGGER_H)
#define _LOGGER_H

/*!
@def LOG(level, prefix)

@brief allows easy logging via the streaming operator similar to std::cout

This is the primary way to use the Logger class. This macro returns an anonymous
instance of the Logger class (not assigned to any variable) that can be streamed
into just like any string stream (e.g.  std::cout).  This macro does a check on
the given #LogLevel 'level' argument; if the specified level is not higher than
or equal to the report-level cutoff, the macro does nothing, limiting the
performance impact of logging statements. Each module/model should have its
own unique prefix.  This will allow users/developers to filter the log
output more readily.

@param level #LogLevel category or type of log statement.
@param prefix A std::string value that functions as a unique identifier for the
              module.

@warning do not place any state-changing expressions with this macro as they
         may not run if the report level excludes the specified log 'level'.

*/
#define LOG(level, prefix) \
if (level > Logger::ReportLevel()) ; \
else Logger().Get(level) << prefix << ": "

#define CLOG(level) \
if (level > Logger::ReportLevel()) ; \
else Logger().Get(level) << "cyclog: "

#define MLOG(level) \
if (level > Logger::ReportLevel()) ; \
else Logger().Get(level) << "cyclog-mem: "

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

/*!

@brief A logging tool providing finer grained control over standard output for debugging.

Built-in logging functionality has been provided to aid debugging.  @e std::cout
statements should be generally avoided.  The LOG(level) macro should be used
for all logging/debugging needs.  The LOG macro uses the Logger class to
provide this functionality.  The Logger class should generally not be accessed
directly.   The macro returns a string stream object that can be used
exactlyl as @e std::cout for printing output.  Streamed in content is flushed
to stdout as soon as execution passes beyond the terminating semi-colon of
the log statement.

A brief description of when to use which log level is given with the #LogLevel
enum.  Developers working on models set the prefix to the
a unique module/model-specific identifier. This will allow developers to
more easily filter the logger output in order to isolate information most
relevant to their work.

@warning do not place any state-changing expressions with the LOG macro as they
         may not run if the report level excludes the specified @e level.

@section examples Examples

@code

LOG(LEV_ERROR, "module name") << "This is my error statement. " << "and more info...";
LOG(LEV_DEBUG2, "module name") << "This is my debug statement. " << "and more info...";

@endcode

The command-line specified verbosity is used to determine the logger
report-cutoff.  Available levels are described in the #LogLevel enum. In the
above example if the command line verbosity were set to #LEV_DEBUG1, the first statement would
print, while the second would not.  Any expression placed with a log statment
that is not printed will not be executed. An example of what not to do follows:

@code

LOG(LEV_DEBUG2, "module name") << "The expression myobject.setName(newname): " 
                               << myobject.setName(newname)
                               << " might not ever execute"
                               << " depending on the verbosity level.";

@endcode

*/
class Logger {

  public:
    Logger() {};
    virtual ~Logger();

    /*!
    @brief Returns a string stream by reference that is flushed to stdout by
    the Logger class destructor.
    */
    std::ostringstream& Get(LogLevel level);

    /*!
    @brief Returns the report level cutoff by reference for printing
    LOG(level) statements
    */
    static LogLevel& ReportLevel() {return report_level;};

    /*!
    @brief Converts a string into a corresponding LogLevel value.

    For strings that do not correspond to any particular LogLevel enum value,
    the method returns the LogLevel value `LEV_ERROR`.  This method is
    primarily intended for translating command line verbosity argument(s) into
    appropriate report levels.  LOG(level) statements
    */
    static LogLevel ToLogLevel(std::string text);

    /*!
    @brief Converts a LogLevel enum value into a corrsponding string.

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

    /*!
    Used to map LogLevel enum values into strings
    */
    static std::vector<std::string> level_to_string;

    /*!
    Used to map strings into LogLevel enum values
    */
    static std::map<std::string, LogLevel> string_to_level;

    /*!
    Used to populate the level_to_string and string_to_level vector/map before
    cyclus code execution begins
    */
    static void initialize();
    
    /*!
    Used by the initialize method to populate the level_to_string and
    string_to_level static variables.
    */
    static void addLevel(LogLevel level, std::string text);

    /*!
    The number of spaces indentation between different LogLevel enum values.
    */
    static int spc_per_lev_;

    /*!
    The width (in characters) of the printed LOG(level) statements' prefixes.
    This should be equal to or greater than the length of the longest LogLevel enum
    value converted to a string (i.e. `LEV_WARNING` is the longest with 11
    characters).
    */
    static int field_width_; };

#endif

