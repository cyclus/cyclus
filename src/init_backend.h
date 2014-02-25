#ifndef CYCLUS_SRC_QUERY_BACKEND_H_
#define CYCLUS_SRC_QUERY_BACKEND_H_

#include "rec_backend.h"

namespace cyclus {

class QueryBackend : public RecBackend {
 public:
  typedef std::map<std::string, boost::spirit::hold_any> Row;

  virtual ~QueryBackend() {};

  virtual std::vector<Row> QueryFull(Model* m, std::string table) = 0;
};
} // namespace cyclus
#endif
