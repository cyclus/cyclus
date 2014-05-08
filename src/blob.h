// blob.h
#ifndef CYCLUS_SRC_BLOB_H_
#define CYCLUS_SRC_BLOB_H_

#include <string>

namespace cyclus {
/// A type to represent variable-length array of bytes for dumping to a cyclus
/// output database.
class Blob {
 public:
  explicit Blob(std::string s) : str_(s) {}

  const std::string& str() const {
    return str_;
  }

 private:
  std::string str_;
};
}  // namespace cyclus

inline std::ostream& operator<<(std::ostream& out, const cyclus::Blob& b) {
   return out << b.str();
}

inline bool operator< (const cyclus::Blob& lhs, const cyclus::Blob& rhs) {
  return lhs.str() < rhs.str();
}

inline bool operator> (const cyclus::Blob& lhs, const cyclus::Blob& rhs) {
  return rhs < lhs;
}

inline bool operator<=(const cyclus::Blob& lhs, const cyclus::Blob& rhs) {
  return !(lhs > rhs);
}

inline bool operator>=(const cyclus::Blob& lhs, const cyclus::Blob& rhs) {
  return !(lhs < rhs);
}

inline bool operator==(const cyclus::Blob& lhs, const cyclus::Blob& rhs) {
  return lhs.str() == rhs.str();
}

inline bool operator!=(const cyclus::Blob& lhs, const cyclus::Blob& rhs) {
  return !(lhs == rhs);
}

#endif  // CYCLUS_SRC_BLOB_H_
