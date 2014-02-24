// error.h
#ifndef CYCLUS_SRC_ERROR_H_
#define CYCLUS_SRC_ERROR_H_

#include <exception>
#include <string>

namespace cyclus {

/// A generic mechanism to manually manage exceptions
class Error : public std::exception {
 public:
  /// Constructs a new Error with the default message.
  Error();

  /// Constructs a new Error with a provided message
  Error(std::string  msg);

  /// Returns the error message associated with this Error.
  virtual const char* what() const throw();

  /// Returns the error message associated with this Error.
  std::string msg() const {
    return msg_;
  }

  /// sets the error message
  void msg(std::string msg) {
    msg_ = msg;
  }

  virtual ~Error() throw() {}

 protected:
  /// The message associated with this exception.
  std::string msg_;

 private:
  static const std::string kPrefix;
};

/// For values that are too big, too small, etc.
class ValueError : public Error {
 public:
  ValueError(std::string msg) : Error(msg) {}
};

/// For failed retrieval/insertion of key-based data into/from data structures
class KeyError : public Error {
 public:
  KeyError(std::string msg) : Error(msg) {}
};

/// For failed object state expectations
class StateError : public Error {
 public:
  StateError(std::string msg) : Error(msg) {}
};

/// for failed reading/writing to files, network connections, etc..
class IOError : public Error {
 public:
  IOError(std::string msg) : Error(msg) {}
};

/// For failed casts that shouldn't
class CastError : public Error {
 public:
  CastError(std::string msg) : Error(msg) {}
};

/// For validating files received via IO.
class ValidationError : public Error {
 public:
  ValidationError(std::string msg) : Error(msg) {}
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_ERROR_H_
