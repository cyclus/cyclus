// error.cc
#include "error.h"

namespace cyclus {

static const std::string kErrorPrefix("cyclus error: ");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Error::Error() : msg_(kPrefix) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Error::Error(std::string msg) {
  msg_ = kPrefix + msg;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* Error::what() const throw() {
  return msg_.c_str();
}

} // namespace cyclus
