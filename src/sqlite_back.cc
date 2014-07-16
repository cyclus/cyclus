#include "sqlite_back.h"

#include <iomanip>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

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
  hasher_ = Sha1();

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

  // initialize template type table statements
  stmt = db_.Prepare("CREATE TABLE IF NOT EXISTS VectorInt (Sum BLOB,Val INTEGER);");
  stmt->Exec();
  vect_int_ins_ = db_.Prepare("INSERT INTO VectorInt VALUES (?,?);");
  vect_int_get_ = db_.Prepare("SELECT Val FROM VectorInt WHERE Sum = ?;");
  stmt = db_.Prepare("SELECT Sum FROM VectorInt;");
  while (stmt->Step()) {
    Digest d;
    int n;
    char* data = stmt->GetText(0, &n);
    memcpy(d.val, data, n);
    vect_int_keys_.insert(d);
  }

  stmt = db_.Prepare("CREATE TABLE IF NOT EXISTS VectorDbl (Sum BLOB,Val REAL);");
  stmt->Exec();
  vect_dbl_ins_ = db_.Prepare("INSERT INTO VectorDbl VALUES (?,?);");
  vect_dbl_get_ = db_.Prepare("SELECT Val FROM VectorDbl WHERE Sum = ?;");
  stmt = db_.Prepare("SELECT Sum FROM VectorDbl;");
  while (stmt->Step()) {
    Digest d;
    int n;
    char* data = stmt->GetText(0, &n);
    memcpy(d.val, data, n);
    vect_dbl_keys_.insert(d);
  }

  stmt = db_.Prepare("CREATE TABLE IF NOT EXISTS VectorStr (Sum BLOB,Val TEXT);");
  stmt->Exec();
  vect_str_ins_ = db_.Prepare("INSERT INTO VectorStr VALUES (?,?);");
  vect_str_get_ = db_.Prepare("SELECT Val FROM VectorStr WHERE Sum = ?;");
  stmt = db_.Prepare("SELECT Sum FROM VectorStr;");
  while (stmt->Step()) {
    Digest d;
    int n;
    char* data = stmt->GetText(0, &n);
    memcpy(d.val, data, n);
    vect_str_keys_.insert(d);
  }

  stmt = db_.Prepare("CREATE TABLE IF NOT EXISTS MapIntDouble (Sum BLOB,Key INTEGER,Val REAL);");
  stmt->Exec();
  map_int_double_ins_ = db_.Prepare("INSERT INTO MapIntDouble VALUES (?,?,?);");
  map_int_double_get_ = db_.Prepare("SELECT Key,Val FROM MapIntDouble WHERE Sum = ?;");
  stmt = db_.Prepare("SELECT Sum FROM MapIntDouble;");
  while (stmt->Step()) {
    Digest d;
    int n;
    char* data = stmt->GetText(0, &n);
    memcpy(d.val, data, n);
    map_int_double_keys_.insert(d);
  }

  stmt = db_.Prepare("CREATE TABLE IF NOT EXISTS MapIntInt (Sum BLOB,Key INTEGER,Val INTEGER);");
  stmt->Exec();
  map_int_int_ins_ = db_.Prepare("INSERT INTO MapIntInt VALUES (?,?,?);");
  map_int_int_get_ = db_.Prepare("SELECT Key,Val FROM MapIntInt WHERE Sum = ?;");
  stmt = db_.Prepare("SELECT Sum FROM MapIntInt;");
  while (stmt->Step()) {
    Digest d;
    int n;
    char* data = stmt->GetText(0, &n);
    memcpy(d.val, data, n);
    map_int_int_keys_.insert(d);
  }

  stmt = db_.Prepare("CREATE TABLE IF NOT EXISTS MapIntStr (Sum BLOB,Key INTEGER,Val TEXT);");
  stmt->Exec();
  map_int_str_ins_ = db_.Prepare("INSERT INTO MapIntStr VALUES (?,?,?);");
  map_int_str_get_ = db_.Prepare("SELECT Key,Val FROM MapIntStr WHERE Sum = ?;");
  stmt = db_.Prepare("SELECT Sum FROM MapIntStr;");
  while (stmt->Step()) {
    Digest d;
    int n;
    char* data = stmt->GetText(0, &n);
    memcpy(d.val, data, n);
    map_int_str_keys_.insert(d);
  }

  stmt = db_.Prepare("CREATE TABLE IF NOT EXISTS MapStrInt (Sum BLOB,Key TEXT,Val INTEGER);");
  stmt->Exec();
  map_str_int_ins_ = db_.Prepare("INSERT INTO MapStrInt VALUES (?,?,?);");
  map_str_int_get_ = db_.Prepare("SELECT Key,Val FROM MapStrInt WHERE Sum = ?;");
  stmt = db_.Prepare("SELECT Sum FROM MapStrInt;");
  while (stmt->Step()) {
    Digest d;
    int n;
    char* data = stmt->GetText(0, &n);
    memcpy(d.val, data, n);
    map_str_int_keys_.insert(d);
  }

  stmt = db_.Prepare("CREATE TABLE IF NOT EXISTS MapStrDouble (Sum BLOB,Key TEXT,Val REAL);");
  stmt->Exec();
  map_str_double_ins_ = db_.Prepare("INSERT INTO MapStrDouble VALUES (?,?,?);");
  map_str_double_get_ = db_.Prepare("SELECT Key,Val FROM MapStrDouble WHERE Sum = ?;");
  stmt = db_.Prepare("SELECT Sum FROM MapStrDouble;");
  while (stmt->Step()) {
    Digest d;
    int n;
    char* data = stmt->GetText(0, &n);
    memcpy(d.val, data, n);
    map_str_double_keys_.insert(d);
  }

  stmt = db_.Prepare("CREATE TABLE IF NOT EXISTS MapStrStr (Sum BLOB,Key TEXT,Val TEXT);");
  stmt->Exec();
  map_str_str_ins_ = db_.Prepare("INSERT INTO MapStrStr VALUES (?,?,?);");
  map_str_str_get_ = db_.Prepare("SELECT Key,Val FROM MapStrStr WHERE Sum = ?;");
  stmt = db_.Prepare("SELECT Sum FROM MapStrStr;");
  while (stmt->Step()) {
    Digest d;
    int n;
    char* data = stmt->GetText(0, &n);
    memcpy(d.val, data, n);
    map_str_str_keys_.insert(d);
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
  case SET_INT: {
    std::set<int> vect = v.cast<std::set<int> >();
    hasher_.Clear();
    hasher_.Update(vect);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (vect_int_keys_.count(d) == 0) {
      std::set<int>::iterator it;
      for (it = vect.begin(); it != vect.end(); ++it) {
        vect_int_ins_->BindBlob(1, d.val, nbytes);
        vect_int_ins_->BindInt(2, *it);
        vect_int_ins_->Exec();
      }
      vect_int_keys_.insert(d);
    }
    break;
  }
  case SET_STRING: {
    std::set<std::string> vect = v.cast<std::set<std::string> >();
    hasher_.Clear();
    hasher_.Update(vect);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (vect_str_keys_.count(d) == 0) {
      std::set<std::string>::iterator it;
      for (it = vect.begin(); it != vect.end(); ++it) {
        vect_str_ins_->BindBlob(1, d.val, nbytes);
        vect_str_ins_->BindText(2, it->c_str());
        vect_str_ins_->Exec();
      }
      vect_str_keys_.insert(d);
    }
    break;
  }
  case LIST_INT: {
    std::list<int> vect = v.cast<std::list<int> >();
    hasher_.Clear();
    hasher_.Update(vect);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (vect_int_keys_.count(d) == 0) {
      std::list<int>::iterator it;
      for (it = vect.begin(); it != vect.end(); ++it) {
        vect_int_ins_->BindBlob(1, d.val, nbytes);
        vect_int_ins_->BindInt(2, *it);
        vect_int_ins_->Exec();
      }
      vect_int_keys_.insert(d);
    }
    break;
  }
  case LIST_STRING: {
    std::list<std::string> vect = v.cast<std::list<std::string> >();
    hasher_.Clear();
    hasher_.Update(vect);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (vect_str_keys_.count(d) == 0) {
      std::list<std::string>::iterator it;
      for (it = vect.begin(); it != vect.end(); ++it) {
        vect_str_ins_->BindBlob(1, d.val, nbytes);
        vect_str_ins_->BindText(2, it->c_str());
        vect_str_ins_->Exec();
      }
      vect_str_keys_.insert(d);
    }
    break;
  }
  case VECTOR_INT: {
    std::vector<int> vect = v.cast<std::vector<int> >();
    hasher_.Clear();
    hasher_.Update(vect);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (vect_int_keys_.count(d) == 0) {
      for (int i = 0; i < vect.size(); ++i) {
        vect_int_ins_->BindBlob(1, d.val, nbytes);
        vect_int_ins_->BindInt(2, vect[i]);
        vect_int_ins_->Exec();
      }
      vect_int_keys_.insert(d);
    }
    break;
  }
  case VECTOR_DOUBLE: {
    std::vector<double> vect = v.cast<std::vector<double> >();
    hasher_.Clear();
    hasher_.Update(vect);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (vect_dbl_keys_.count(d) == 0) {
      for (int i = 0; i < vect.size(); ++i) {
        vect_dbl_ins_->BindBlob(1, d.val, nbytes);
        vect_dbl_ins_->BindDouble(2, vect[i]);
        vect_dbl_ins_->Exec();
      }
      vect_dbl_keys_.insert(d);
    }
    break;
  }
  case VECTOR_STRING: {
    std::vector<std::string> vect = v.cast<std::vector<std::string> >();
    hasher_.Clear();
    hasher_.Update(vect);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (vect_str_keys_.count(d) == 0) {
      for (int i = 0; i < vect.size(); ++i) {
        vect_str_ins_->BindBlob(1, d.val, nbytes);
        vect_str_ins_->BindText(2, vect[i].c_str());
        vect_str_ins_->Exec();
      }
      vect_str_keys_.insert(d);
    }
    break;
  }
  case MAP_INT_DOUBLE: {
    std::map<int, double> m = v.cast<std::map<int, double> >();
    hasher_.Clear();
    hasher_.Update(m);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (map_int_double_keys_.count(d) == 0) {
      std::map<int, double>::iterator it;
      for (it = m.begin(); it != m.end(); ++it) {
        map_int_double_ins_->BindBlob(1, d.val, nbytes);
        map_int_double_ins_->BindInt(2, it->first);
        map_int_double_ins_->BindDouble(3, it->second);
        map_int_double_ins_->Exec();
      }
      map_int_double_keys_.insert(d);
    }
    break;
  }
  case MAP_INT_INT: {
    std::map<int, int> m = v.cast<std::map<int, int> >();
    hasher_.Clear();
    hasher_.Update(m);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (map_int_int_keys_.count(d) == 0) {
      std::map<int, int>::iterator it;
      for (it = m.begin(); it != m.end(); ++it) {
        map_int_int_ins_->BindBlob(1, d.val, nbytes);
        map_int_int_ins_->BindInt(2, it->first);
        map_int_int_ins_->BindInt(3, it->second);
        map_int_int_ins_->Exec();
      }
      map_int_int_keys_.insert(d);
    }
    break;
  }
  case MAP_INT_STRING: {
    std::map<int, std::string> m = v.cast<std::map<int, std::string> >();
    hasher_.Clear();
    hasher_.Update(m);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (map_int_str_keys_.count(d) == 0) {
      std::map<int, std::string>::iterator it;
      for (it = m.begin(); it != m.end(); ++it) {
        map_int_str_ins_->BindBlob(1, d.val, nbytes);
        map_int_str_ins_->BindInt(2, it->first);
        map_int_str_ins_->BindText(3, it->second.c_str());
        map_int_str_ins_->Exec();
      }
      map_int_str_keys_.insert(d);
    }
    break;
  }
  case MAP_STRING_INT: {
    std::map<std::string, int> m = v.cast<std::map<std::string, int> >();
    hasher_.Clear();
    hasher_.Update(m);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (map_str_int_keys_.count(d) == 0) {
      std::map<std::string, int>::iterator it;
      for (it = m.begin(); it != m.end(); ++it) {
        map_str_int_ins_->BindBlob(1, d.val, nbytes);
        map_str_int_ins_->BindText(2, it->first.c_str());
        map_str_int_ins_->BindInt(3, it->second);
        map_str_int_ins_->Exec();
      }
      map_str_int_keys_.insert(d);
    }
    break;
  }
  case MAP_STRING_DOUBLE: {
    std::map<std::string, double> m = v.cast<std::map<std::string, double> >();
    hasher_.Clear();
    hasher_.Update(m);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (map_str_double_keys_.count(d) == 0) {
      std::map<std::string, double>::iterator it;
      for (it = m.begin(); it != m.end(); ++it) {
        map_str_double_ins_->BindBlob(1, d.val, nbytes);
        map_str_double_ins_->BindText(2, it->first.c_str());
        map_str_double_ins_->BindDouble(3, it->second);
        map_str_double_ins_->Exec();
      }
      map_str_double_keys_.insert(d);
    }
    break;
  }
  case MAP_STRING_STRING: {
    std::map<std::string, std::string> m =
        v.cast<std::map<std::string, std::string> >();
    hasher_.Clear();
    hasher_.Update(m);
    Digest d = hasher_.digest();
    int nbytes = CYCLUS_SHA1_NINT*4;
    stmt->BindBlob(index, d.val, nbytes);

    if (map_str_str_keys_.count(d) == 0) {
      std::map<std::string, std::string>::iterator it;
      for (it = m.begin(); it != m.end(); ++it) {
        map_str_str_ins_->BindBlob(1, d.val, nbytes);
        map_str_str_ins_->BindText(2, it->first.c_str());
        map_str_str_ins_->BindText(3, it->second.c_str());
        map_str_str_ins_->Exec();
      }
      map_str_str_keys_.insert(d);
    }
    break;
  }
  default: {
    throw ValueError("attempted to retrieve unsupported sqlite backend type");
  }
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
  } case SET_INT: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::set<int> vect;
    vect_int_get_->BindBlob(1, data, n);
    while (vect_int_get_->Step()) {
      vect.insert(vect_int_get_->GetInt(0));
    }
    vect_int_get_->Reset();
    v = vect;
    break;
  } case SET_STRING: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::set<std::string> vect;
    vect_str_get_->BindBlob(1, data, n);
    while (vect_str_get_->Step()) {
      vect.insert(vect_str_get_->GetText(0, NULL));
    }
    vect_str_get_->Reset();
    v = vect;
    break;
  } case LIST_INT: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::list<int> vect;
    vect_int_get_->BindBlob(1, data, n);
    while (vect_int_get_->Step()) {
      vect.push_back(vect_int_get_->GetInt(0));
    }
    vect_int_get_->Reset();
    v = vect;
    break;
  } case LIST_STRING: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::list<std::string> vect;
    vect_str_get_->BindBlob(1, data, n);
    while (vect_str_get_->Step()) {
      vect.push_back(vect_str_get_->GetText(0, NULL));
    }
    vect_str_get_->Reset();
    v = vect;
    break;
  } case VECTOR_INT: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::vector<int> vect;
    vect_int_get_->BindBlob(1, data, n);
    while (vect_int_get_->Step()) {
      vect.push_back(vect_int_get_->GetInt(0));
    }
    vect_int_get_->Reset();
    v = vect;
    break;
  } case VECTOR_DOUBLE: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::vector<double> vect;
    vect_dbl_get_->BindBlob(1, data, n);
    while (vect_dbl_get_->Step()) {
      vect.push_back(vect_dbl_get_->GetDouble(0));
    }
    vect_dbl_get_->Reset();
    v = vect;
    break;
  } case VECTOR_STRING: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::vector<std::string> vect;
    vect_str_get_->BindBlob(1, data, n);
    while (vect_str_get_->Step()) {
      vect.push_back(vect_str_get_->GetText(0, NULL));
    }
    vect_str_get_->Reset();
    v = vect;
    break;
  } case MAP_INT_DOUBLE: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::map<int, double> m;
    map_int_double_get_->BindBlob(1, data, n);
    while (map_int_double_get_->Step()) {
      int key = map_int_double_get_->GetInt(0);
      double val = map_int_double_get_->GetDouble(1);
      m[key] = val;
    }
    map_int_double_get_->Reset();
    v = m;
    break;
  } case MAP_INT_INT: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::map<int, int> m;
    map_int_int_get_->BindBlob(1, data, n);
    while (map_int_int_get_->Step()) {
      int key = map_int_int_get_->GetInt(0);
      int val = map_int_int_get_->GetInt(1);
      m[key] = val;
    }
    map_int_int_get_->Reset();
    v = m;
    break;
  } case MAP_INT_STRING: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::map<int, std::string> m;
    map_int_str_get_->BindBlob(1, data, n);
    while (map_int_str_get_->Step()) {
      int key = map_int_str_get_->GetInt(0);
      std::string val = map_int_str_get_->GetText(1, NULL);
      m[key] = val;
    }
    map_int_str_get_->Reset();
    v = m;
    break;
  } case MAP_STRING_DOUBLE: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::map<std::string, double> m;
    map_str_double_get_->BindBlob(1, data, n);
    while (map_str_double_get_->Step()) {
      std::string key = map_str_double_get_->GetText(0, NULL);
      double val = map_str_double_get_->GetDouble(1);
      m[key] = val;
    }
    map_str_double_get_->Reset();
    v = m;
    break;
  } case MAP_STRING_INT: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::map<std::string, int> m;
    map_str_int_get_->BindBlob(1, data, n);
    while (map_str_int_get_->Step()) {
      std::string key = map_str_int_get_->GetText(0, NULL);
      int val = map_str_int_get_->GetInt(1);
      m[key] = val;
    }
    map_str_int_get_->Reset();
    v = m;
    break;
  } case MAP_STRING_STRING: {
    int n;
    char* data = stmt->GetText(col, &n);

    std::map<std::string, std::string> m;
    map_str_str_get_->BindBlob(1, data, n);
    while (map_str_str_get_->Step()) {
      std::string key = map_str_str_get_->GetText(0, NULL);
      std::string val = map_str_str_get_->GetText(1, NULL);
      m[key] = val;
    }
    map_str_str_get_->Reset();
    v = m;
    break;
  } default: {
    throw ValueError("Attempted to retrieve unsupported backend type");
  }}
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
    // type_map[&typeid(std::set<double>)] = SET_DOUBLE;
    // type_map[&typeid(std::set<float>)] = SET_FLOAT;
    // type_map[&typeid(std::set<Blob>)] = SET_BLOB;
    // type_map[&typeid(std::set<boost::uuids::uuid>)] = SET_UUID;
    type_map[&typeid(std::set<std::string>)] = SET_STRING;

    type_map[&typeid(std::vector<int>)] = VECTOR_INT;
    type_map[&typeid(std::vector<double>)] = VECTOR_DOUBLE;
    // type_map[&typeid(std::vector<float>)] = VECTOR_FLOAT;
    // type_map[&typeid(std::vector<Blob>)] = VECTOR_BLOB;
    // type_map[&typeid(std::vector<boost::uuids::uuid>)] = VECTOR_UUID;
    type_map[&typeid(std::vector<std::string>)] = VECTOR_STRING;

    type_map[&typeid(std::list<int>)] = LIST_INT;
    // type_map[&typeid(std::list<double>)] = LIST_DOUBLE;
    // type_map[&typeid(std::list<float>)] = LIST_FLOAT;
    // type_map[&typeid(std::list<Blob>)] = LIST_BLOB;
    // type_map[&typeid(std::list<boost::uuids::uuid>)] = LIST_UUID;
    type_map[&typeid(std::list<std::string>)] = LIST_STRING;

    type_map[&typeid(std::map<int, int>)] = MAP_INT_INT;
    type_map[&typeid(std::map<int, double>)] = MAP_INT_DOUBLE;
    // type_map[&typeid(std::map<int, float>)] = MAP_INT_FLOAT;
    // type_map[&typeid(std::map<int, Blob>)] = MAP_INT_BLOB;
    // type_map[&typeid(std::map<int, boost::uuids::uuid>)] = MAP_INT_UUID;
    type_map[&typeid(std::map<int, std::string>)] = MAP_INT_STRING;

    type_map[&typeid(std::map<std::string, int>)] = MAP_STRING_INT;
    type_map[&typeid(std::map<std::string, double>)] = MAP_STRING_DOUBLE;
    // type_map[&typeid(std::map<std::string, float>)] = MAP_STRING_FLOAT;
    // type_map[&typeid(std::map<std::string, Blob>)] = MAP_STRING_BLOB;
    // type_map[&typeid(std::map<std::string, boost::uuids::uuid>)] = MAP_STRING_UUID;
    type_map[&typeid(std::map<std::string, std::string>)] = MAP_STRING_STRING;
  }

  const std::type_info* ti = &v.type();
  if (type_map.count(ti) == 0) {
    throw ValueError(std::string("unsupported backend type ") + ti->name());
  }
  return type_map[ti];
}

}  // namespace cyclus
