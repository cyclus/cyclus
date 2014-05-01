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
  std::string cmd = "SELECT name FROM sqlite_master WHERE type='table';";
  std::vector<StrList> rows = db_.Query(cmd);
  for (int i = 0; i < rows.size(); ++i) {
    tbl_names_.insert(rows.at(i).at(0));
  }

  if (tbl_names_.count("FieldTypes") == 0) {
    cmd = "CREATE TABLE IF NOT EXISTS FieldTypes";
    cmd += "(TableName TEXT,Field TEXT,Type TEXT);";
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
      Bind(v, stmt, i+1);
    }
  }

  std::vector<StrList> rows = stmt->Query();
  for (int i = 0; i < rows.size(); ++i) {
    QueryRow r;
    for (int j = 0; j < q.fields.size(); ++j) {
      //FIXME r.push_back(StringAsVal(rows[i][j], q.types[j]));
    }
    q.rows.push_back(r);
  }
  return q;
}

QueryResult SqliteBack::GetTableInfo(std::string table) {
  std::string sql = "SELECT Field,Type FROM FieldTypes WHERE TableName = '" +
                    table + "';";
  std::vector<StrList> rows = db_.Query(sql);
  if (rows.size() == 0) {
    throw ValueError("Invalid table name " + table);
  }

  QueryResult info;
  for (int i = 0; i < rows.size(); ++i) {
    info.fields.push_back(rows[i][0]);
    //FIXME info.types.push_back(rows[i][1]);
  }
  return info;
}

std::string SqliteBack::Name() {
  return path_;
}

void SqliteBack::BuildStmt(Datum* d) {
  std::string name = d->title();
  Datum::Vals vals = d->vals();

  std::string insert = "INSERT INTO " + name + " VALUES (?";
  field_order_[name][vals[0].first] = 1;
  for (int i = 1; i < vals.size(); ++i) {
    field_order_[name][vals[i].first] = i + 1;
    insert += ", ?";
  }
  insert += ");";

  stmts_[name] = db_.Prepare(insert);
}

void SqliteBack::CreateTable(Datum* d) {
  std::string name = d->title();
  tbl_names_.insert(name);

  Datum::Vals vals = d->vals();
  Datum::Vals::iterator it = vals.begin();

  std::string types = "INSERT INTO FieldTypes VALUES ('" + name + "','"
                      + it->first + "','" + Type(it->second) + "');";
  db_.Execute(types);

  std::string cmd = "CREATE TABLE " + name + " (";
  cmd += std::string(it->first) + " " + SqlType(it->second);
  ++it;

  while (it != vals.end()) {
    cmd += ", " + std::string(it->first) + " " + SqlType(it->second);
    types = "INSERT INTO FieldTypes VALUES ('" + name + "','"
            + it->first + "','" + Type(it->second) + "');";
    db_.Execute(types);
    ++it;
  }

  cmd += ");";
  db_.Execute(cmd);
}

std::string SqliteBack::SqlType(boost::spirit::hold_any v) {
  if (v.type() == typeid(int)) {
    return "INTEGER";
  } else if (v.type() == typeid(float) || v.type() == typeid(double)) {
    return "REAL";
  } else if (v.type() == typeid(Blob)) {
    return "BLOB";
  } else if (v.type() == typeid(boost::uuids::uuid)) {
    return "BLOB";
  }
  return "TEXT";
}

std::string SqliteBack::Type(boost::spirit::hold_any v) {
  if (v.type() == typeid(int)) {
    return "int";
  } else if (v.type() == typeid(double)) {
    return "double";
  } else if (v.type() == typeid(float)) {
    return "float";
  } else if (v.type() == typeid(Blob)) {
    return "blob";
  } else if (v.type() == typeid(boost::uuids::uuid)) {
    return "uuid";
  } else if (v.type() == typeid(std::string)) {
    return "std::string";
  }
  return std::string("UNKNOWN_") + v.type().name();
}

void SqliteBack::Bind(boost::spirit::hold_any v, SqlStatement::Ptr stmt,
                      int index) {
  if (v.type() == typeid(int)) {
    stmt->BindInt(index, v.cast<int>());
  } else if (v.type() == typeid(double)) {
    stmt->BindDouble(index, v.cast<double>());
  } else if (v.type() == typeid(std::string)) {
    stmt->BindText(index, v.cast<std::string>().c_str());
  } else if (v.type() == typeid(boost::uuids::uuid)) {
    boost::uuids::uuid ui = v.cast<boost::uuids::uuid>();
    stmt->BindBlob(index, ui.data, 16);
  } else if (v.type() == typeid(float)) {
    stmt->BindDouble(index, v.cast<float>());
  } else if (v.type() == typeid(Blob)) {
    std::string s = v.cast<Blob>().str();
    stmt->BindBlob(index, s.c_str(), s.size());
  }
}

void SqliteBack::WriteDatum(Datum* d) {
  Datum::Vals vals = d->vals();
  SqlStatement::Ptr stmt = stmts_[d->title()];
  std::map<const char*, int> index = field_order_[d->title()];

  for (int i = 0; i < vals.size(); ++i) {
    boost::spirit::hold_any v = vals[i].second;
    Bind(v, stmt, index[vals[i].first]);
  }

  stmt->Exec();
}

boost::spirit::hold_any SqliteBack::StringAsVal(std::string s,
                                                std::string type) {
  boost::spirit::hold_any v;
  if (type == "int") {
    v = atoi(s.c_str());
  } else if (type == "double") {
    v = atof(s.c_str());
  } else if (type == "float") {
    v = (float)atof(s.c_str());
  } else if (type == "std::string") {
    v = s;
  } else if (type == "blob") {
    v = Blob(s);
  } else if (type == "uuid") {
    boost::uuids::uuid u;
    memcpy(&u, s.c_str(), 16);
    v = u;
  } else {
    CLOG(LEV_ERROR) << "attempted to retrieve unsupported type from backend "
                    << Name();
  }
  return v;
}

void SqliteBack::Flush() { }

} // namespace cyclus
