// sqlite_back.cc
#include "sqlite_back.h"

#include <iomanip>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "blob.h"
#include "error.h"
#include "event.h"
#include "logger.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SqliteBack::SqliteBack(std::string path) : db_(path) {
  path_ = path;
  db_.open();

  // cache pre-existing table names
  std::string cmd = "SELECT name FROM sqlite_master WHERE type='table';";
  std::vector<StrList> rows = db_.Query(cmd);
  for (int i = 0; i < rows.size(); ++i) {
    tbl_names_.push_back(rows.at(i).at(0));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::Notify(EventList evts) {
  for (EventList::iterator it = evts.begin(); it != evts.end(); ++it) {
    if (! TableExists((*it)->title())) {
      CreateTable(*it);
    }
    WriteEvent(*it);
  }
  Flush();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::Close() {
  Flush();
  db_.close();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SqliteBack::Name() {
  return path_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::CreateTable(Event* e) {
  std::string name = e->title();
  tbl_names_.push_back(name);

  Event::Vals vals = e->vals();
  Event::Vals::iterator it = vals.begin();
  std::string cmd = "CREATE TABLE " + name + " (";
  cmd += std::string(it->first) + " " + ValType(it->second);
  ++it;
  while (it != vals.end()) {
    cmd += ", " + std::string(it->first) + " " + ValType(it->second);
    ++it;
  }

  cmd += ");";
  cmds_.push_back(cmd);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SqliteBack::ValType(boost::spirit::hold_any v) {
  if (v.type() == typeid(int)) {
    return "INTEGER";
  } else if (v.type() == typeid(float) || v.type() == typeid(double)) {
    return "REAL";
  } else if (v.type() == typeid(cyclus::Blob)) {
    return "BLOB";
  }
  return "TEXT";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SqliteBack::TableExists(std::string name) {
  return find(tbl_names_.begin(), tbl_names_.end(), name) != tbl_names_.end();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::WriteEvent(Event* e) {
  std::stringstream colss, valss, cmd;
  Event::Vals vals = e->vals();

  Event::Vals::iterator it = vals.begin();
  colss << it->first;
  valss << ValAsString(it->second);
  ++it;
  while (it != vals.end()) {
    colss << ", " << it->first;
    valss << ", " << ValAsString(it->second);
    ++it;
  }

  cmd << "INSERT INTO " << e->title() << " (" << colss.str() << ") "
      << "VALUES (" << valss.str() << ");";
  cmds_.push_back(cmd.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string toHex(const std::string& s) {
  std::ostringstream ret;
  for (int i = 0; i < s.length(); ++i) {
    ret << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(s[i]);
  }
  return ret.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SqliteBack::ValAsString(boost::spirit::hold_any v) {
  // NOTE: the ugly structure of this if block is for performance reasons
  if (v.type() == typeid(int)) {
    std::stringstream ss;
    ss << v.cast<int>();
    return ss.str();
  } else if (v.type() == typeid(double)) {
    std::stringstream ss;
    ss << v.cast<double>();
    return ss.str();
  } else if (v.type() == typeid(std::string)) {
    return "'" + v.cast<std::string>() + "'";
  } else if (v.type() == typeid(boost::uuids::uuid)) {
    char data[16];
    boost::uuids::uuid ui = v.cast<boost::uuids::uuid>();
    return "'" + boost::lexical_cast<std::string>(ui) + "'";
  } else if (v.type() == typeid(float)) {
    std::stringstream ss;
    ss << v.cast<float>();
    return ss.str();
  } else if (v.type() == typeid(cyclus::Blob)) {
    std::string s = v.cast<cyclus::Blob>().str();
    return "X'" + toHex(s) + "'";
  }
  CLOG(LEV_ERROR) << "attempted to record unsupported type in backend "
                  << Name();
  return "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::Flush() {
  db_.Execute("BEGIN TRANSACTION");
  for (StrList::iterator it = cmds_.begin(); it != cmds_.end(); ++it) {
    db_.Execute(*it);
  }
  db_.Execute("END TRANSACTION");
  cmds_.clear();
}

} // namespace cyclus
