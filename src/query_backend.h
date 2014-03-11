#ifndef CYCLUS_SRC_QUERY_BACKEND_H_
#define CYCLUS_SRC_QUERY_BACKEND_H_

#include "rec_backend.h"
#include "any.hpp"
#include <boost/any.hpp>

namespace cyclus {

/// Represents a condition used to filter rows returned by a query.
class Cond {
 public:
  Cond() {};

  Cond(std::string field, std::string op, boost::spirit::hold_any val)
    : field(field), op(op), val(val) {}

  /// table column name
  std::string field;

  /// One of: "<", ">", "<=", ">=", "=="
  std::string op;

  /// value supported by backend(s) in use
  boost::spirit::hold_any val;
};

typedef std::vector<boost::any> QueryRow;

/// Meta data and results of a query.
class QueryResult {
 public:
  /// names of each field returned by a query
  std::vector<std::string> fields;

  /// types of each field returned by a query. Possible values: INTEGER, REAL, BLOB, TEXT
  std::vector<std::string> types;

  /// ordered results of a query
  std::vector<QueryRow> rows;

  /// Convenience method for retrieving a value from a specific row and named
  /// field (column). The caller is responsible for specifying a valid templated
  /// type to cast to. Example use:
  ///
  /// @code
  ///
  /// QueryResult qr = ...
  ///
  /// for (int i = 0; i < qr.rows.size(); ++i) {
  ///   std::cout << qr.GetVal<int>("field1", i) << "\n";
  ///   std::cout << qr.GetVal<double>("field2", i) << "\n";
  ///   std::cout << qr.GetVal<std::string>("field3", i) << "\n";
  /// }
  ///
  /// @endcode
  template <class T>
  T GetVal(std::string field, int row = 0) {
    if (row >= rows.size()) {
      throw KeyError("index larger than number of query rows");
    }

    int field_idx = -1;
    for (int i = 0; i < fields.size(); ++i) {
      if (fields[i] == field) {
        field_idx = i;
        break;
      }
    }
    if (field_idx == -1) {
      throw KeyError("query result has no such field " + field);
    }

    return boost::any_cast<T>(rows[row][field_idx]);
  };
};

/// Interface implemented by backends that support rudimentary querying.
class QueryBackend {
 public:
  virtual ~QueryBackend() {};

  /// Return a set of rows from the specificed table that match all given
  /// conditions.  Conditions are AND'd together.  conds may be NULL.
  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) = 0;
};

/// Interface implemented by backends that support recording and querying.
class FullBackend: public QueryBackend, public RecBackend {
 public:
  virtual ~FullBackend() {};
};

/// Wrapper class for QueryBackends that injects a set of Cond's into every
/// query before being executed.
class CondInjector: public QueryBackend {
 public:
  CondInjector(QueryBackend* b, std::vector<Cond> to_inject)
    : b_(b), to_inject_(to_inject) {};

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) {
    if (conds == NULL) {
      return b_->Query(table, &to_inject_);
    }

    std::vector<Cond> c = *conds;
    for (int i = 0; i < to_inject_.size(); ++i) {
      c.push_back(to_inject_[i]);
    }
    return b_->Query(table, &c);
  };

 private:
  QueryBackend* b_;
  std::vector<Cond> to_inject_;
};

/// Wrapper class for QueryBackends that injects prefix in front of the
/// title/table for every query before being executed.  A query to the
/// "MyAgentTable" table will actually be passed to the wrapped backend as
/// [prefix] + "MyAgentTable".
class PrefixInjector: public QueryBackend {
 public:
  PrefixInjector(QueryBackend* b, std::string prefix)
    : b_(b), prefix_(prefix) {};

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) {
    return b_->Query(prefix_ + table, conds);
  };

 private:
  QueryBackend* b_;
  std::string prefix_;
};

} // namespace cyclus
#endif

