// sqlite_back.cc
#include "sqlite_back.h"

#include <iomanip>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

#include "blob.h"
#include "error.h"
#include "datum.h"
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

  // cache pre-existing table names
  SqlStatement::Ptr stmt;
  stmt = db_.Prepare("SELECT name FROM sqlite_master WHERE type='table';");

  for(int i = 0; stmt->Step(); ++i) {
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
  db_.Execute("END TRANSACTION;");
  Flush();
}

void SqliteBack::Flush() { }

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
  switch (type) {
  case INT:
    stmt->BindInt(index, v.cast<int>());
    break;
  case DOUBLE:
    stmt->BindDouble(index, v.cast<double>());
    break;
  case FLOAT:
    stmt->BindDouble(index, v.cast<float>());
    break;
  case BLOB: {
      std::string s = v.cast<Blob>().str();
      stmt->BindBlob(index, s.c_str(), s.size());
      break;
    }
  case VL_STRING:
    stmt->BindText(index, v.cast<std::string>().c_str());
    break;
  case UUID:
    boost::uuids::uuid ui = v.cast<boost::uuids::uuid>();
    stmt->BindBlob(index, ui.data, 16);
    break;
  default: // templated type
  }
}

boost::spirit::hold_any SqliteBack::ColAsVal(SqlStatement::Ptr stmt,
                                                int col,
                                                DbTypes type) {
  boost::spirit::hold_any v;
  switch (type) {
  case INT: {
    v = stmt->GetInt(col);
    break;
  } case DOUBLE: {
    v = stmt->GetDouble(col);
    break;
  } case FLOAT: {
    v = (float)stmt->GetDouble(col);
    break;
  } case VL_STRING: {
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
  } default: {
    throw ValueError("Attempted to retrieve unsupported backend type");
  }}
  return v;
}

std::string SqliteBack::SqlType(boost::spirit::hold_any v) {
  switch (Type(v)) {
  case INT: // fallthrough
  case BOOL:
    return "INTEGER";
  case DOUBLE: // fallthrough
  case FLOAT:
    return "REAL";
  case STRING: // fallthrough
  case VL_STRING: // fallthrough
    return "TEXT";
  case BLOB: // fallthrough
  case UUID: // fallthrough
  default: // all templated types
    return "BLOB";
  }
}

struct compare {
  bool operator ()(const std::type_info* a, const std::type_info* b) const {
    return a->before(*b);
  }
};

static std::map<const std::type_info*, DbTypes, compare> type_map;

DbTypes SqliteBack::Type(boost::spirit::hold_any v) {
  if (type_map.size() == 0) {
    type_map[&typeid(int)] = INT;
    type_map[&typeid(double)] = DOUBLE;
    type_map[&typeid(float)] = FLOAT;
    type_map[&typeid(Blob)] = BLOB;
    type_map[&typeid(boost::uuids::uuid)] = UUID;
    type_map[&typeid(std::string)] = VL_STRING;

    type_map[&typeid(std::set<int>)] = SET_INT;
    type_map[&typeid(std::set<double>)] = SET_DOUBLE;
    type_map[&typeid(std::set<float>)] = SET_FLOAT;
    type_map[&typeid(std::set<Blob>)] = SET_BLOB;
    type_map[&typeid(std::set<boost::uuids::uuid>)] = SET_UUID;
    type_map[&typeid(std::set<std::string>)] = SET_VL_STRING;

    type_map[&typeid(std::vector<int>)] = VECTOR_INT;
    type_map[&typeid(std::vector<double>)] = VECTOR_DOUBLE;
    type_map[&typeid(std::vector<float>)] = VECTOR_FLOAT;
    type_map[&typeid(std::vector<Blob>)] = VECTOR_BLOB;
    type_map[&typeid(std::vector<boost::uuids::uuid>)] = VECTOR_UUID;
    type_map[&typeid(std::vector<std::string>)] = VECTOR_VL_STRING;

    type_map[&typeid(std::list<int>)] = LIST_INT;
    type_map[&typeid(std::list<double>)] = LIST_DOUBLE;
    type_map[&typeid(std::list<float>)] = LIST_FLOAT;
    type_map[&typeid(std::list<Blob>)] = LIST_BLOB;
    type_map[&typeid(std::list<boost::uuids::uuid>)] = LIST_UUID;
    type_map[&typeid(std::list<std::string>)] = LIST_VL_STRING;

    type_map[&typeid(std::map<int, int>)] = MAP_INT_INT;
    type_map[&typeid(std::map<int, double>)] = MAP_INT_DOUBLE;
    type_map[&typeid(std::map<int, float>)] = MAP_INT_FLOAT;
    type_map[&typeid(std::map<int, Blob>)] = MAP_INT_BLOB;
    type_map[&typeid(std::map<int, boost::uuids::uuid>)] = MAP_INT_UUID;
    type_map[&typeid(std::map<int, std::string>)] = MAP_INT_VL_STRING;

    type_map[&typeid(std::map<std::string, int>)] = MAP_VL_STRING_INT;
    type_map[&typeid(std::map<std::string, double>)] = MAP_VL_STRING_DOUBLE;
    type_map[&typeid(std::map<std::string, float>)] = MAP_VL_STRING_FLOAT;
    type_map[&typeid(std::map<std::string, Blob>)] = MAP_VL_STRING_BLOB;
    type_map[&typeid(std::map<std::string, boost::uuids::uuid>)] = MAP_VL_STRING_UUID;
    type_map[&typeid(std::map<std::string, std::string>)] = MAP_VL_STRING_VL_STRING;
  }

  const std::type_info* ti = &v.type();
  if (type_map.count(ti) == 0) {
    throw ValueError(std::string("Unsupported backend type ") + ti->name());
  }
  return type_map[ti];
};

} // namespace cyclus
