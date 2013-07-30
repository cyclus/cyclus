// Blob.hpp
#pragma once

#include <string>

namespace cyclus {
  // A type to represent variable-length array of bytes for dumping to a cyclus
  // output database.
  class Blob {
    public:
      Blob(std::string s) : str(s) { };
      Blob(const char* s) : str(s) { };
      std::string str;
  };
}
