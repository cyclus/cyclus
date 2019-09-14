#include "sqlite_back.h"

#include <iomanip>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/tmpdir.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/assume_abstract.hpp>


#include "blob.h"
#include "datum.h"
#include "error.h"
#include "logger.h"

namespace cyclus {

std::vector<std::string> split(const std::string& s, char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

SqliteBack::~SqliteBack() {
  try {
    Flush();
    db_.close();
  } catch (Error err) {
    CLOG(LEV_ERROR) << "Error in SqliteBack destructor: " << err.what();
  }
}

SqliteBack::SqliteBack(std::string path) : db_(path) {
  path_ = path;
  db_.open();

  db_.Execute("PRAGMA synchronous=OFF;");
  db_.Execute("PRAGMA journal_mode=MEMORY;");
  db_.Execute("PRAGMA temp_store=MEMORY;");

  // cache pre-existing table names
  SqlStatement::Ptr stmt;
  stmt = db_.Prepare("SELECT name FROM sqlite_master WHERE type='table';");

  for (int i = 0; stmt->Step(); ++i) {
    tbl_names_.insert(stmt->GetText(0, NULL));
  }

  if (tbl_names_.count("FieldTypes") == 0) {
    std::string cmd = "CREATE TABLE IF NOT EXISTS FieldTypes";
    cmd += "(TableName TEXT,Field TEXT,Type INTEGER);";
    db_.Execute(cmd);
  }
}

void SqliteBack::Notify(DatumList data) {
  db_.Execute("BEGIN TRANSACTION;");
  try {
    for (DatumList::iterator it = data.begin(); it != data.end(); ++it) {
      std::string tbl = (*it)->title();
      if (tbl_names_.count(tbl) == 0) {
        CreateTable(*it);
      }
      if (stmts_.count(tbl) == 0) {
        BuildStmt(*it);
      }
      WriteDatum(*it);
    }
  } catch (ValueError err) {
    db_.Execute("END TRANSACTION;");
    throw ValueError(err.what());
  }
  db_.Execute("END TRANSACTION;");
  Flush();
}

void SqliteBack::Flush() { }

std::list<ColumnInfo> SqliteBack::Schema(std::string table) {
  std::list<ColumnInfo> schema;
  QueryResult qr = GetTableInfo(table);
  for (int i = 0; i < qr.fields.size(); ++i) {
    ColumnInfo info = ColumnInfo(table, qr.fields[i], i, qr.types[i], std::vector<int>());
    schema.push_back(info);
  }
  return schema;
}

QueryResult SqliteBack::Query(std::string table, std::vector<Cond>* conds) {
  QueryResult q = GetTableInfo(table);

  std::stringstream sql;
  sql << "SELECT * FROM " << table;
  if (conds != NULL) {
    sql << " WHERE ";
    for (int i = 0; i < conds->size(); ++i) {
      if (i > 0) {
        sql << " AND ";
      }
      Cond c = (*conds)[i];
      sql << c.field << " " << c.op << " ?";
    }
  }
  sql << ";";

  SqlStatement::Ptr stmt = db_.Prepare(sql.str());

  if (conds != NULL) {
    for (int i = 0; i < conds->size(); ++i) {
      boost::spirit::hold_any v = (*conds)[i].val;
      Bind(v, Type(v), stmt, i+1);
    }
  }

  for (int i = 0; stmt->Step(); ++i) {
    QueryRow r;
    for (int j = 0; j < q.fields.size(); ++j) {
      r.push_back(ColAsVal(stmt, j, q.types[j]));
    }
    q.rows.push_back(r);
  }
  return q;
}

std::map<std::string, DbTypes> SqliteBack::ColumnTypes(std::string table) {
  QueryResult qr = GetTableInfo(table);
  std::map<std::string, DbTypes> rtn;
  for (int i = 0; i < qr.fields.size(); ++i)
    rtn[qr.fields[i]] = qr.types[i];
  return rtn;
}

std::set<std::string> SqliteBack::Tables() {
  using std::set;
  using std::string;
  set<string> rtn;
  std::string sql = "SELECT name FROM sqlite_master WHERE type='table';";
  SqlStatement::Ptr stmt;
  stmt = db_.Prepare(sql);
  while (stmt->Step()) {
    rtn.insert(stmt->GetText(0, NULL));
  }
  rtn.erase("FieldTypes");
  return rtn;
}

SqliteDb& SqliteBack::db() {
  return db_;
}

QueryResult SqliteBack::GetTableInfo(std::string table) {
  std::string sql = "SELECT Field,Type FROM FieldTypes WHERE TableName = '" +
                    table + "';";
  SqlStatement::Ptr stmt;
  stmt = db_.Prepare(sql);

  int i = 0;
  QueryResult info;
  for (i = 0; stmt->Step(); ++i) {
    info.fields.push_back(stmt->GetText(0, NULL));
    info.types.push_back((DbTypes)stmt->GetInt(1));
  }
  if (i == 0) {
    throw ValueError("Invalid table name " + table);
  }
  return info;
}

std::string SqliteBack::Name() {
  return path_;
}

void SqliteBack::BuildStmt(Datum* d) {
  std::string name = d->title();
  Datum::Vals vals = d->vals();
  std::vector<DbTypes> schema;

  schema.push_back(Type(vals[0].second));
  std::string insert = "INSERT INTO " + name + " VALUES (?";
  for (int i = 1; i < vals.size(); ++i) {
    schema.push_back(Type(vals[i].second));
    insert += ", ?";
  }
  insert += ");";

  schemas_[name] = schema;
  stmts_[name] = db_.Prepare(insert);
}

void SqliteBack::CreateTable(Datum* d) {
  std::string name = d->title();
  tbl_names_.insert(name);

  Datum::Vals vals = d->vals();
  Datum::Vals::iterator it = vals.begin();

  std::stringstream types;
  types << "INSERT INTO FieldTypes VALUES ('"
        << name << "','" << it->first << "','"
        << Type(it->second) << "');";
  db_.Execute(types.str());

  std::string cmd = "CREATE TABLE " + name + " (";
  cmd += std::string(it->first) + " " + SqlType(it->second);
  ++it;

  while (it != vals.end()) {
    cmd += ", " + std::string(it->first) + " " + SqlType(it->second);
    std::stringstream types;
    types << "INSERT INTO FieldTypes VALUES ('"
          << name << "','" << it->first << "','"
          << Type(it->second) << "');";
    db_.Execute(types.str());
    ++it;
  }

  cmd += ");";
  db_.Execute(cmd);
}

void SqliteBack::WriteDatum(Datum* d) {
  Datum::Vals vals = d->vals();
  SqlStatement::Ptr stmt = stmts_[d->title()];
  std::vector<DbTypes> schema = schemas_[d->title()];

  for (int i = 0; i < vals.size(); ++i) {
    boost::spirit::hold_any v = vals[i].second;
    Bind(v, schema[i], stmt, i+1);
  }

  stmt->Exec();
}

void SqliteBack::Bind(boost::spirit::hold_any v, DbTypes type, SqlStatement::Ptr stmt,
                      int index) {

// serializes the value v of type T and DBType D and binds it to stmt (inside
// a case statement.
// NOTE: Since we are archiving to a stringstream, the archive must be closed before
// the stringstream, so we put it in its own scope. This first became an issue in
// Boost v1.66.0.  For more information, see http://boost.2283326.n4.nabble.com/the-boost-xml-serialization-to-a-stringstream-does-not-have-an-end-tag-tp2580772p2580773.html
#define CYCLUS_COMMA ,
#define CYCLUS_BINDVAL(D, T) \
    case D: { \
    T vect = v.cast<T>(); \
    std::stringstream ss; \
    { \
      boost::archive::xml_oarchive ar(ss); \
      ar & BOOST_SERIALIZATION_NVP(vect); \
    } \
    v = vect; \
    std::string s = ss.str(); \
    stmt->BindBlob(index, s.c_str(), s.size()); \
    break; \
    }

  switch (type) {
  case INT: {
    stmt->BindInt(index, v.cast<int>());
    break;
  }
  case BOOL: {
    stmt->BindInt(index, v.cast<bool>());
    break;
  }
  case DOUBLE: {
    stmt->BindDouble(index, v.cast<double>());
    break;
  }
  case FLOAT: {
    stmt->BindDouble(index, v.cast<float>());
    break;
  }
  case BLOB: {
      std::string s = v.cast<Blob>().str();
      stmt->BindBlob(index, s.c_str(), s.size());
      break;
    }
  case STRING: {
    stmt->BindText(index, v.cast<std::string>().c_str());
    break;
  }
  case UUID: {
    boost::uuids::uuid ui = v.cast<boost::uuids::uuid>();
    stmt->BindBlob(index, ui.data, 16);
    break;
  }
  CYCLUS_BINDVAL(SET_INT, std::set<int>);
  CYCLUS_BINDVAL(SET_STRING, std::set<std::string>);
  CYCLUS_BINDVAL(LIST_INT, std::list<int>);
  CYCLUS_BINDVAL(LIST_STRING, std::list<std::string>);
  CYCLUS_BINDVAL(VECTOR_INT, std::vector<int>);
  CYCLUS_BINDVAL(VECTOR_DOUBLE, std::vector<double>);
  CYCLUS_BINDVAL(VECTOR_STRING, std::vector<std::string>);
  CYCLUS_BINDVAL(MAP_INT_DOUBLE, std::map<int CYCLUS_COMMA double>);
  CYCLUS_BINDVAL(MAP_INT_INT, std::map<int CYCLUS_COMMA int>);
  CYCLUS_BINDVAL(MAP_INT_STRING, std::map<int CYCLUS_COMMA std::string>);
  CYCLUS_BINDVAL(MAP_STRING_INT, std::map<std::string CYCLUS_COMMA int>);
  CYCLUS_BINDVAL(MAP_STRING_DOUBLE, std::map<std::string CYCLUS_COMMA double>);
  CYCLUS_BINDVAL(MAP_STRING_STRING,
                 std::map<std::string CYCLUS_COMMA std::string>);
  CYCLUS_BINDVAL(MAP_STRING_VECTOR_DOUBLE,
                 std::map<std::string CYCLUS_COMMA std::vector<double> >);
  CYCLUS_BINDVAL(
      MAP_STRING_MAP_INT_DOUBLE,
      std::map<std::string CYCLUS_COMMA std::map<int CYCLUS_COMMA double> >);
  CYCLUS_BINDVAL(MAP_STRING_PAIR_DOUBLE_MAP_INT_DOUBLE,
                 std::map<std::string CYCLUS_COMMA std::pair<
                     double CYCLUS_COMMA std::map<int CYCLUS_COMMA double> > >);
  CYCLUS_BINDVAL(MAP_INT_MAP_STRING_DOUBLE,
                 std::map<int CYCLUS_COMMA
                          std::map<std::string CYCLUS_COMMA double> >);
  CYCLUS_BINDVAL(
      MAP_STRING_VECTOR_PAIR_INT_PAIR_STRING_STRING,
      std::map<std::string CYCLUS_COMMA
      std::vector<std::pair<int CYCLUS_COMMA
      std::pair<std::string CYCLUS_COMMA std::string> > > >);

  CYCLUS_BINDVAL(
      MAP_STRING_PAIR_STRING_VECTOR_DOUBLE,
      std::map<std::string CYCLUS_COMMA
      std::pair<std::string CYCLUS_COMMA std::vector<double> > > );

  CYCLUS_BINDVAL(LIST_PAIR_INT_INT, std::list< std::pair<int CYCLUS_COMMA int> >);

  CYCLUS_BINDVAL(
      MAP_STRING_MAP_STRING_INT,
      std::map<std::string CYCLUS_COMMA std::map<std::string CYCLUS_COMMA int> >);

  CYCLUS_BINDVAL(
      VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_MAP_STRING_DOUBLE,
      std::vector<std::pair<
              std::pair<double CYCLUS_COMMA double> CYCLUS_COMMA
                  std::map<std::string CYCLUS_COMMA double> > > );

  CYCLUS_BINDVAL(
      MAP_PAIR_STRING_STRING_INT,
      std::map<std::pair<std::string CYCLUS_COMMA std::string> CYCLUS_COMMA int > );

  CYCLUS_BINDVAL(
      MAP_STRING_MAP_STRING_DOUBLE,
      std::map<std::string CYCLUS_COMMA std::map<std::string CYCLUS_COMMA double> >);

  default: {
    throw ValueError("attempted to retrieve unsupported sqlite backend type");
  }
  }
#undef CYCLUS_BINDVAL
#undef CYCLUS_COMMA
}

boost::spirit::hold_any SqliteBack::ColAsVal(SqlStatement::Ptr stmt,
                                             int col,
                                             DbTypes type) {

  boost::spirit::hold_any v;

// reconstructs from a serialization in stmt of type T and DbType D and
// store it in v.
#define CYCLUS_COMMA ,
#define CYCLUS_LOADVAL(D, T) \
      case D: { \
      char* data =  stmt->GetText(col, NULL); \
      std::stringstream ss; \
      ss << data; \
      boost::archive::xml_iarchive ar(ss); \
      T vect; \
      ar & BOOST_SERIALIZATION_NVP(vect); \
      v = vect; \
      break; \
      }

  switch (type) {
  case INT: {
    v = stmt->GetInt(col);
    break;
  } case BOOL: {
    v = static_cast<bool>(stmt->GetInt(col));
    break;
  } case DOUBLE: {
    v = stmt->GetDouble(col);
    break;
  } case FLOAT: {
    v = static_cast<float>(stmt->GetDouble(col));
    break;
  } case STRING: {
    v = std::string(stmt->GetText(col, NULL));
    break;
  } case BLOB: {
    int n;
    char* s = stmt->GetText(col, &n);
    v = Blob(std::string(s, n));
    break;
  } case UUID: {
    boost::uuids::uuid u;
    memcpy(&u, stmt->GetText(col, NULL), 16);
    v = u;
    break;
  }
  CYCLUS_LOADVAL(SET_INT, std::set<int>);
  CYCLUS_LOADVAL(SET_STRING, std::set<std::string>);
  CYCLUS_LOADVAL(LIST_INT, std::list<int>);
  CYCLUS_LOADVAL(LIST_STRING, std::list<std::string>);
  CYCLUS_LOADVAL(VECTOR_INT, std::vector<int>);
  CYCLUS_LOADVAL(VECTOR_DOUBLE, std::vector<double>);
  CYCLUS_LOADVAL(VECTOR_STRING, std::vector<std::string>);
  CYCLUS_LOADVAL(MAP_INT_DOUBLE, std::map<int CYCLUS_COMMA double>);
  CYCLUS_LOADVAL(MAP_INT_INT, std::map<int CYCLUS_COMMA int>);
  CYCLUS_LOADVAL(MAP_INT_STRING, std::map<int CYCLUS_COMMA std::string>);
  CYCLUS_LOADVAL(MAP_STRING_DOUBLE, std::map<std::string CYCLUS_COMMA double>);
  CYCLUS_LOADVAL(MAP_STRING_INT, std::map<std::string CYCLUS_COMMA int>);
  CYCLUS_LOADVAL(MAP_STRING_STRING,
                 std::map<std::string CYCLUS_COMMA std::string>);
  CYCLUS_LOADVAL(MAP_STRING_VECTOR_DOUBLE,
                 std::map<std::string CYCLUS_COMMA std::vector<double> >);
  CYCLUS_LOADVAL(
      MAP_STRING_MAP_INT_DOUBLE,
      std::map<std::string CYCLUS_COMMA std::map<int CYCLUS_COMMA double> >);
  CYCLUS_LOADVAL(MAP_STRING_PAIR_DOUBLE_MAP_INT_DOUBLE,
                 std::map<std::string CYCLUS_COMMA std::pair<
                     double CYCLUS_COMMA std::map<int CYCLUS_COMMA double> > >);
  CYCLUS_LOADVAL(MAP_INT_MAP_STRING_DOUBLE,
                 std::map<int CYCLUS_COMMA
                          std::map<std::string CYCLUS_COMMA double> >);
  CYCLUS_LOADVAL(
      MAP_STRING_VECTOR_PAIR_INT_PAIR_STRING_STRING,
      std::map<std::string CYCLUS_COMMA
      std::vector<std::pair<int CYCLUS_COMMA
      std::pair<std::string CYCLUS_COMMA std::string> > > >);

  CYCLUS_LOADVAL(
      MAP_STRING_PAIR_STRING_VECTOR_DOUBLE,
      std::map<std::string CYCLUS_COMMA
      std::pair<std::string CYCLUS_COMMA std::vector<double> > > );

  CYCLUS_LOADVAL(LIST_PAIR_INT_INT, std::list< std::pair<int CYCLUS_COMMA int> >);

  CYCLUS_LOADVAL(
      MAP_STRING_MAP_STRING_INT,
      std::map<std::string CYCLUS_COMMA std::map<std::string CYCLUS_COMMA int> >);

  CYCLUS_LOADVAL(
      VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_MAP_STRING_DOUBLE,
      std::vector<std::pair<
              std::pair<double CYCLUS_COMMA double> CYCLUS_COMMA
                  std::map<std::string CYCLUS_COMMA double> > > );

  CYCLUS_LOADVAL(
      MAP_PAIR_STRING_STRING_INT,
      std::map<std::pair<std::string CYCLUS_COMMA std::string> CYCLUS_COMMA int > );

  CYCLUS_LOADVAL(
      MAP_STRING_MAP_STRING_DOUBLE,
      std::map<std::string CYCLUS_COMMA std::map<std::string CYCLUS_COMMA double> >);

  default: {
    throw ValueError("Attempted to retrieve unsupported backend type");
  }}

#undef CYCLUS_LOADVAL
#undef CYCLUS_COMMA

  return v;
}

std::string SqliteBack::SqlType(boost::spirit::hold_any v) {
  switch (Type(v)) {
  case INT:  // fallthrough
  case BOOL:
    return "INTEGER";
  case DOUBLE:  // fallthrough
  case FLOAT:
    return "REAL";
  case STRING:
    return "TEXT";
  case BLOB:  // fallthrough
  case UUID:  // fallthrough
  default:  // all templated types
    return "BLOB";
  }
}

struct compare {
  bool operator()(const std::type_info* a, const std::type_info* b) const {
    return a->before(*b);
  }
};

static std::map<const std::type_info*, DbTypes, compare> type_map;

DbTypes SqliteBack::Type(boost::spirit::hold_any v) {
  if (type_map.size() == 0) {
    type_map[&typeid(int)] = INT;
    type_map[&typeid(double)] = DOUBLE;
    type_map[&typeid(float)] = FLOAT;
    type_map[&typeid(bool)] = BOOL;
    type_map[&typeid(Blob)] = BLOB;
    type_map[&typeid(boost::uuids::uuid)] = UUID;
    type_map[&typeid(std::string)] = STRING;
    type_map[&typeid(std::set<int>)] = SET_INT;
    type_map[&typeid(std::set<std::string>)] = SET_STRING;
    type_map[&typeid(std::vector<int>)] = VECTOR_INT;
    type_map[&typeid(std::vector<double>)] = VECTOR_DOUBLE;
    type_map[&typeid(std::vector<std::string>)] = VECTOR_STRING;
    type_map[&typeid(std::list<int>)] = LIST_INT;
    type_map[&typeid(std::list<std::string>)] = LIST_STRING;
    type_map[&typeid(std::map<int, int>)] = MAP_INT_INT;
    type_map[&typeid(std::map<int, double>)] = MAP_INT_DOUBLE;
    type_map[&typeid(std::map<int, std::string>)] = MAP_INT_STRING;
    type_map[&typeid(std::map<std::string, int>)] = MAP_STRING_INT;
    type_map[&typeid(std::map<std::string, double>)] = MAP_STRING_DOUBLE;
    type_map[&typeid(std::map<std::string, std::string>)] = MAP_STRING_STRING;
    type_map[&typeid(std::map<std::string, std::vector<double> >)] =
        MAP_STRING_VECTOR_DOUBLE;
    type_map[&typeid(std::map<std::string, std::map<int, double> >)] =
        MAP_STRING_MAP_INT_DOUBLE;
    type_map[&typeid(std::map<std::string,
                              std::pair<double, std::map<int, double> > >)] =
        MAP_STRING_PAIR_DOUBLE_MAP_INT_DOUBLE;
    type_map[&typeid(std::map<int, std::map<std::string, double> >)] =
        MAP_INT_MAP_STRING_DOUBLE;
    type_map[&typeid(
        std::map<std::string,
                 std::vector<std::pair<int, std::pair<std::string,
                                                      std::string> > > >)] =
        MAP_STRING_VECTOR_PAIR_INT_PAIR_STRING_STRING;

    type_map[&typeid(
        std::map<std::string,
                  std::pair<std::string,
                            std::vector<double> > >)] =
        MAP_STRING_PAIR_STRING_VECTOR_DOUBLE;

    type_map[&typeid(std::map<std::string, std::map<std::string,int> >)] =
        MAP_STRING_MAP_STRING_INT;

    type_map[&typeid(std::list<std::pair<int, int> >)] = LIST_PAIR_INT_INT;

    type_map[&typeid(
        std::vector<std::pair<std::pair<double, double>,
                              std::map<std::string, double> > > )] =
        VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_MAP_STRING_DOUBLE;

    type_map[&typeid(
        std::map<std::pair<std::string, std::string>, int > )] =
        MAP_PAIR_STRING_STRING_INT;

    type_map[&typeid(std::map<std::string, std::map<std::string,double> >)] =
        MAP_STRING_MAP_STRING_DOUBLE;
  }

  const std::type_info* ti = &v.type();
  if (type_map.count(ti) == 0) {
    throw ValueError(std::string("unsupported backend type ") + ti->name());
  }
  return type_map[ti];
}

}  // namespace cyclus
