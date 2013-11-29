// error.cc
#include "error.h"

namespace cyclus {

const std::string Error::kPrefix("cyclus error: ");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Error::Error() : msg_(Error::kPrefix) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Error::Error(std::string msg) {
  msg_ = Error::kPrefix + msg;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* Error::what() const throw() {
  return msg_.c_str();
}

} // namespace cyclus
