#ifndef CYCLUS_SRC_QUERY_BACKEND_H_
#define CYCLUS_SRC_QUERY_BACKEND_H_

#include "rec_backend.h"
#include "any.hpp"

namespace cyclus {

/// This is the master list of all supported database types.  All types must 
/// have a constant length unless they begin with the prefix VL_, which stand
/// for "variable length" or are implicitly variable length, such as blob.
/// Changing the order here may invalidate previously created databases.
enum DbTypes {
  BOOL = 0,
  INT,
  FLOAT,
  DOUBLE,
  STRING,
  VL_STRING,
  BLOB,
  UUID,
};

/// Represents a condition used to filter rows returned by a query.
class Cond {
 public:
  Cond() {};

  Cond(std::string field, std::string op, boost::spirit::hold_any val)
    : field(field), op(op), val(val) {}

  /// table column name
  std::string field;

  /// One of: "<", ">", "<=", ">=", "==", "!="
  std::string op;

  /// value supported by backend(s) in use
  boost::spirit::hold_any val;
};

typedef std::vector<boost::spirit::hold_any> QueryRow;

/// Meta data and results of a query.
class QueryResult {
 public:
  /// names of each field returned by a query
  std::vector<std::string> fields;

  /// types of each field returned by a query.
  std::vector<DbTypes> types;

  /// ordered results of a query
  std::vector<QueryRow> rows;

  void Reset() {
    fields.clear();
    types.clear();
    rows.clear();
  };

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

    return rows[row][field_idx].cast<T>();
  };
};

/// Interface implemented by backends that support rudimentary querying.
class QueryableBackend {
 public:
  virtual ~QueryableBackend() {};

  /// Return a set of rows from the specificed table that match all given
  /// conditions.  Conditions are AND'd together.  conds may be NULL.
  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) = 0;
};

/// Interface implemented by backends that support recording and querying.
class FullBackend: public QueryableBackend, public RecBackend {
 public:
  virtual ~FullBackend() {};
};

/// Wrapper class for QueryableBackends that injects a set of Cond's into every
/// query before being executed.
class CondInjector: public QueryableBackend {
 public:
  CondInjector(QueryableBackend* b, std::vector<Cond> to_inject)
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
  QueryableBackend* b_;
  std::vector<Cond> to_inject_;
};

/// Wrapper class for QueryableBackends that injects prefix in front of the
/// title/table for every query before being executed.  A query to the
/// "MyAgentTable" table will actually be passed to the wrapped backend as
/// [prefix] + "MyAgentTable".
class PrefixInjector: public QueryableBackend {
 public:
  PrefixInjector(QueryableBackend* b, std::string prefix)
    : b_(b), prefix_(prefix) {};

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) {
    return b_->Query(prefix_ + table, conds);
  };

 private:
  QueryableBackend* b_;
  std::string prefix_;
};

} // namespace cyclus
#endif

