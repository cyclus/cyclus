// error.cc
#include "error.h"

namespace cyclus {

const std::string Error::kPrefix("cyclus error: ");

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Error::Error(std::string msg) : msg_(msg) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* Error::what() const throw() {
  std::string thrown = Error::kPrefix + msg_;
  return thrown.c_str();
}

}  // namespace cyclus
