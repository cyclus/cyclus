
#include <gtest/gtest.h>

#include "logger.h"

using cyclus::Logger;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(Logging, test) {
  cyclus::LogLevel lev = cyclus::LEV_ERROR;
  Logger::ReportLevel() = lev;
  CLOG(lev) << "hi mom!";
}
