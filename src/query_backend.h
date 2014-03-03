#ifndef CYCLUS_SRC_QUERY_BACKEND_H_
#define CYCLUS_SRC_QUERY_BACKEND_H_

#include "rec_backend.h"

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

  /// value supported by backends
  boost::spirit::hold_any val;
};

typedef std::vector<boost::spirit::hold_any> QueryRow;

struct QueryResult {
  std::vector<std::string> fields;
  std::vector<std::string> types;
  std::vector<QueryRow> rows;

  template <class T>
  T GetVal(int row, std::string field) {
    if (row > rows.size()) {
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

class QueryBackend {
 public:
  virtual ~QueryBackend() {};

  /// Return a set of rows from the specificed table that match all given
  /// conditions.  Conditions are AND'd together.
  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) = 0;
};

class FullBackend: public QueryBackend, public RecBackend {
 public:
  virtual ~FullBackend() {};
};

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

} // namespace cyclus
#endif

