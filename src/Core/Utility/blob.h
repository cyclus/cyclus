// blob.h
#ifndef CYCLUS_CORE_UTILITY_BLOB_H_
#define CYCLUS_CORE_UTILITY_BLOB_H_

#include <string>

namespace cyclus {
/// A type to represent variable-length array of bytes for dumping to a cyclus
/// output database.
class Blob {
 public:
  Blob(std::string s) : str_(s) { };

  const std::string& str() const {
    return str_;
  };

 private:
  std::string str_;
};
}

#endif
