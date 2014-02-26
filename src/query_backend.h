#ifndef CYCLUS_SRC_QUERY_BACKEND_H_
#define CYCLUS_SRC_QUERY_BACKEND_H_

#include "rec_backend.h"

namespace cyclus {

class Cond {
 public:
  Cond() {};
  Cond(std::string field, std::string op, boost::spirit::hold_any val)
    : field(field), op(op), val(val) {}

  std::string field;
  std::string op;
  boost::spirit::hold_any val;
};

typedef std::vector<boost::spirit::hold_any> QueryRow;

struct QueryResult {
  std::vector<std::string> fields;
  std::vector<std::string> types;
  std::vector<QueryRow> rows;
};

class QueryBackend : public RecBackend {
 public:
  typedef std::vector<boost::spirit::hold_any> Row;

  virtual ~QueryBackend() {};

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) = 0;
};
} // namespace cyclus
#endif
