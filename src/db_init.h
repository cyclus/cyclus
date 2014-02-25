#ifndef CYCLUS_SRC_DB_INIT_H_
#define CYCLUS_SRC_DB_INIT_H_

namespace cyclus {

class DbInit {
 public:
  Datum* NewDatum(Model* m, std::string title);
};

} // namespace cyclus
#endif

