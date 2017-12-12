#include <gtest/gtest.h>

#include "cyclus.h"

using cyclus::Logger;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(Logging, test) {
  cyclus::LogLevel lev = cyclus::LEV_ERROR;
  Logger::ReportLevel() = lev;
  CLOG(lev) << "hi mom!";
}
