// SqliteBack.cpp
#include "SqliteBack.h"

#include "CycException.h"
#include "Logger.h"
#include "Event.h"

#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SqliteBack::SqliteBack(std::string path)
    : db_(path) {
  path_ = path;
  db_.open();

  // cache pre-existing table names
  std::string cmd = "SELECT name FROM sqlite_master WHERE type='table';";
  std::vector<StrList> rows = db_.query(cmd);
  for (int i = 0; i < rows.size(); ++i) {
    tbl_names_.push_back(rows.at(i).at(0));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::notify(EventList evts) {
  for (EventList::iterator it = evts.begin(); it != evts.end(); ++it) {
    if (! tableExists((*it)->title())) {
      createTable(*it);
    }
    writeEvent(*it);
  }
  flush();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::close() {
  flush();
  db_.close();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SqliteBack::name() {
  return path_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::createTable(Event* e) {
  std::string name = e->title();
  tbl_names_.push_back(name);

  Event::Vals vals = e->vals();
  Event::Vals::iterator it = vals.begin();
  std::string cmd = "CREATE TABLE " + name + " (";
  cmd += std::string(it->first) + " " + valType(it->second);
  ++it;
  while (it != vals.end()) {
    cmd += ", " + std::string(it->first) + " " + valType(it->second);
    ++it;
  }

  cmd += ");";
  cmds_.push_back(cmd);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SqliteBack::valType(boost::spirit::hold_any v) {
  if (v.type() == typeid(int)) {
    return "INTEGER";
  } else if (v.type() == typeid(float) || v.type() == typeid(double)) {
    return "REAL";
  }
  return "TEXT";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SqliteBack::tableExists(std::string name) {
  return find(tbl_names_.begin(), tbl_names_.end(), name) != tbl_names_.end();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::writeEvent(Event* e) {
  std::stringstream colss, valss, cmd;
  Event::Vals vals = e->vals();

  Event::Vals::iterator it = vals.begin();
  colss << it->first;
  valss << valAsString(it->second);
  ++it;
  while (it != vals.end()) {
    colss << ", " << it->first;
    valss << ", " << valAsString(it->second);
    ++it;
  }

  cmd << "INSERT INTO " << e->title() << " (" << colss.str() << ") "
      << "VALUES (" << valss.str() << ");";
  cmds_.push_back(cmd.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SqliteBack::valAsString(boost::spirit::hold_any v) {
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
    return "\"" + v.cast<std::string>() + "\"";
  } else if (v.type() == typeid(boost::uuids::uuid)) {
    char data[16];
    boost::uuids::uuid ui = v.cast<boost::uuids::uuid>();
    return "\"" + boost::lexical_cast<std::string>(ui) + "\"";
  } else if (v.type() == typeid(float)) {
    std::stringstream ss;
    ss << v.cast<float>();
    return ss.str();
  }
  CLOG(LEV_ERROR) << "attempted to record unsupported type in backend "
                  << name();
  return "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::flush() {
  db_.execute("BEGIN TRANSACTION");
  for (StrList::iterator it = cmds_.begin(); it != cmds_.end(); ++it) {
    try {
      db_.execute(*it);
    } catch (CycIOException err) {
      CLOG(LEV_ERROR) << "backend '" << path_ << "' failed write: "
                      << err.what();
    }
  }
  db_.execute("END TRANSACTION");
  cmds_.clear();
}

