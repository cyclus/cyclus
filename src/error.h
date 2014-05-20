// error.h
#ifndef CYCLUS_SRC_ERROR_H_
#define CYCLUS_SRC_ERROR_H_

#include <exception>
#include <iostream>
#include <map>
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

enum Warnings {
  WARNING = 0,
  VALUE_WARNING,
  KEY_WARNING,
  STATE_WARNING,
  IO_WARNING,
  CAST_WARNING,
  VALIDATION_WARNING,
  DEPRECATION_WARNING,
  PENDING_DEPRECATION_WARNING,
};


/// This is maximum number of times to issue a warning of each kind.
extern unsigned int warn_limit;

/// Flag for throwing an error when a warning happens
extern bool warn_as_error;

/// The number of warnings issues for each kind.
extern std::map<Warnings, unsigned int> warn_count;

/// Creates the warning prefixes mapping.
std::map<Warnings, std::string> warn_prefixes();

/// The number of warnings issues for each kind.
extern std::map<Warnings, std::string> warn_prefix;

/// Issue a warning with the approriate message, accoring to the current 
/// warning settings.
template <Warnings T>
void Warn(const std::string& msg) {
  if (warn_as_error) {
    switch (T) {
      case VALUE_WARNING:
        throw ValueError(msg);
      case KEY_WARNING:
        throw KeyError(msg);
      case STATE_WARNING:
        throw StateError(msg);
      case IO_WARNING:
        throw IOError(msg);
      case CAST_WARNING:
        throw CastError(msg);
      case VALIDATION_WARNING:
        throw ValidationError(msg);
      default:
        throw Error(msg);
    }
  }
  unsigned int cnt = warn_count[T]++;
  if (cnt >= warn_limit)
    return;
  std::cerr << warn_prefix[T] << ": " << msg << "\n";
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_ERROR_H_
