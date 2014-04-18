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

std::vector<std::string> split(const std::string &s, char delim) {
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
  } catch(Error err) {
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
      if (v.type() == typeid(int)) {
        stmt->BindInt(i+1, v.cast<int>());
      } else if (v.type() == typeid(double)) {
        stmt->BindDouble(i+1, v.cast<double>());
      } else if (v.type() == typeid(std::string)) {
        stmt->BindText(i+1, v.cast<std::string>().c_str());
      } else if (v.type() == typeid(boost::uuids::uuid)) {
        boost::uuids::uuid ui = v.cast<boost::uuids::uuid>();
        stmt->BindBlob(i+1, ui.data, 16);
      } else if (v.type() == typeid(float)) {
        stmt->BindDouble(i+1, v.cast<float>());
      } else if (v.type() == typeid(Blob)) {
        std::string s = v.cast<Blob>().str();
        stmt->BindBlob(i+1, s.c_str(), s.size());
      }
    }
  }

  std::vector<StrList> rows = stmt->Query();
  for (int i = 0; i < rows.size(); ++i) {
    QueryRow r;
    for (int j = 0; j < q.fields.size(); ++j) {
      r.push_back(StringAsVal(rows[i][j], q.types[j]));
    }
    q.rows.push_back(r);
  }
  return q;
}

QueryResult SqliteBack::GetTableInfo(std::string table) {
  std::string sql = "SELECT sql FROM sqlite_master WHERE tbl_name = '" + table + "';";
  std::vector<StrList> rows = db_.Query(sql);
  if (rows.size() == 0) {
    throw ValueError("Invalid table name " + table);
  }

  std::string s = rows[0][0];
  size_t start = s.find("(") + 1;
  size_t n = s.find(")") - start;
  std::string type_data = s.substr(start, n);
  std::vector<std::string> fields = split(type_data, ',');
  QueryResult info;
  for (int i = 0; i < fields.size(); ++i) {
    std::string field = fields[i];
    boost::algorithm::trim(field);
    size_t pos = field.rfind(" ") + 1;
    std::string type = field.substr(pos);
    pos = field.find(" ");
    std::string name = field.substr(0, pos);
    if (type != "INTEGER" &&
        type != "REAL" &&
        type != "BLOB" &&
        type != "TEXT") {
      throw IOError("Unsupported type for querying " + type);
    }
    info.types.push_back(type);
    info.fields.push_back(name);
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
    field_order_[name][vals[i].first] = i+1;
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
  std::string cmd = "CREATE TABLE " + name + " (";
  cmd += std::string(it->first) + " " + ValType(it->second);
  ++it;

  while (it != vals.end()) {
    cmd += ", " + std::string(it->first) + " " + ValType(it->second);
    ++it;
  }

  cmd += ");";
  db_.Execute(cmd);
}

std::string SqliteBack::ValType(boost::spirit::hold_any v) {
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

void SqliteBack::WriteDatum(Datum* d) {
  Datum::Vals vals = d->vals();
  SqlStatement::Ptr stmt = stmts_[d->title()];
  std::map<const char*, int> index = field_order_[d->title()];

  for (int i = 0; i < vals.size(); ++i) {
    boost::spirit::hold_any v = vals[i].second;
    if (v.type() == typeid(int)) {
      stmt->BindInt(index[vals[i].first], v.cast<int>());
    } else if (v.type() == typeid(double)) {
      stmt->BindDouble(index[vals[i].first], v.cast<double>());
    } else if (v.type() == typeid(std::string)) {
      stmt->BindText(index[vals[i].first], v.cast<std::string>().c_str());
    } else if (v.type() == typeid(boost::uuids::uuid)) {
      boost::uuids::uuid ui = v.cast<boost::uuids::uuid>();
      stmt->BindBlob(index[vals[i].first], ui.data, 16);
    } else if (v.type() == typeid(float)) {
      stmt->BindDouble(index[vals[i].first], v.cast<float>());
    } else if (v.type() == typeid(Blob)) {
      std::string s = v.cast<Blob>().str();
      stmt->BindBlob(index[vals[i].first], s.c_str(), s.size());
    }
  }

  stmt->Exec();
}

std::string StringToHex(const std::string& s) {
  std::ostringstream ret;
  for (int i = 0; i < s.length(); ++i) {
    ret << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(s[i]);
  }
  return ret.str();
}

std::string StringFromHex(const std::string& in) {
    std::string output;

    if ((in.length() % 2) != 0) {
        throw std::runtime_error("String is not valid length ...");
    }

    size_t cnt = in.length() / 2;

    for (size_t i = 0; cnt > i; ++i) {
        uint32_t s = 0;
        std::stringstream ss;
        ss << std::hex << in.substr(i * 2, 2);
        ss >> s;

        output.push_back(static_cast<unsigned char>(s));
    }

    return output;
}

boost::spirit::hold_any SqliteBack::StringAsVal(std::string s, std::string type) {
  boost::spirit::hold_any v;
  if (type == "INTEGER") {
    v = atoi(s.c_str());
  } else if (type == "REAL") {
    v = atof(s.c_str());
  } else if (type == "TEXT") {
    v = s;
  } else if (type == "BLOB") {
    v = s;
  } else {
    CLOG(LEV_ERROR) << "attempted to retrieve unsupported type from backend "
                    << Name();
  }
  return v;
}

void SqliteBack::Flush() { }

} // namespace cyclus
