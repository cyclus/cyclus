#include "error.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Error::Error(std::string msg) : msg_(msg) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* Error::what() const throw() {
  return msg_.c_str();
}

unsigned int warn_limit = 42;

bool warn_as_error = false;

std::map<Warnings, unsigned int> warn_count = std::map<Warnings,
                                                       unsigned int>();

std::map<Warnings, std::string> warn_prefixes() {
  std::map<Warnings, std::string> wp;
  wp[WARNING] = "Warning";
  wp[VALUE_WARNING] = "Value Warning";
  wp[KEY_WARNING] = "Key Warning";
  wp[STATE_WARNING] = "State Warning";
  wp[IO_WARNING] = "I/O Warning";
  wp[CAST_WARNING] = "Cast Warning";
  wp[VALIDATION_WARNING] = "Validation Warning";
  wp[DEPRECATION_WARNING] = "Deprecation Warning";
  wp[PENDING_DEPRECATION_WARNING] = "Pending Deprecation Warning";
  wp[EXPERIMENTAL_WARNING] = "Experimental Warning";
  return wp;
}
std::map<Warnings, std::string> warn_prefix = warn_prefixes();

}  // namespace cyclus
