// SqliteBack.cpp
#include "SqliteBack.h"

#include "CycException.h"
#include "Logger.h"
#include "Event.h"

#include <fstream>
#include <boost/lexical_cast.hpp>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SqliteBack::SqliteBack(std::string path) {
  path_ = path;
  db_ = new SqliteDb(path_);
  db_->open();

  // cache pre-existing table names
  std::string cmd = "SELECT name FROM sqlite_master WHERE type='table';";
  std::vector<StrList> rows = db_->query(cmd);
  for (int i = 0; i < rows.size(); i++) {
    tbl_names_.push_back(rows.at(i).at(0));
  }

  short_sim_id_ = getShortId(EM->sim_id());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SqliteBack::~SqliteBack() {
  delete db_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::notify(EventList evts) {
  for (EventList::iterator it = evts.begin(); it != evts.end(); it++) {
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
  db_->close();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SqliteBack::name() {
  return path_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::createTable(event_ptr e) {
  std::string name = e->title();
  tbl_names_.push_back(name);

  std::string cmd = "CREATE TABLE " + name + " (" + kShortSimId + " INTEGER";
  ValMap vals = e->vals();
  ValMap::iterator it = vals.begin();
  for (it = vals.begin(); it != vals.end(); it++) {
    cmd += ", " + it->first + " " + valType(it->second);
  }

  cmd += ");";
  cmds_.push_back(cmd);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unsigned int SqliteBack::getShortId(std::string sim_id) {
  std::string cmd, retrieve;
  if (! tableExists(kSimIdTable)) {
    tbl_names_.push_back(kSimIdTable);

    cmd = "CREATE TABLE " + kSimIdTable + " ("
          + kShortSimId + " INTEGER PRIMARY KEY, "
          + kLongSimId + " INTEGER);";
    db_->execute(cmd);
  }

  retrieve = "SELECT " + kShortSimId + " FROM " + kSimIdTable + " WHERE "
             + kLongSimId + "='" + sim_id + "'";
  std::vector<StrList> rows = db_->query(retrieve);

  if (rows.size() == 1) {
    return boost::lexical_cast<unsigned int>(rows.at(0).at(0));
  }

  cmd = "INSERT INTO " + kSimIdTable + " (" + kLongSimId
        + ") VALUES ('" + sim_id + "');";
  db_->execute(cmd);

  rows = db_->query(retrieve);
  return boost::lexical_cast<unsigned int>(rows.at(0).at(0));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SqliteBack::valType(boost::any v) {
  if (v.type() == typeid(int)) {
    return "INTEGER";
  } else if (v.type() == typeid(float)) {
    return "REAL";
  } else if (v.type() == typeid(double)) {
    return "REAL";
  } else if (v.type() == typeid(std::string)) {
    return "VARCHAR(128)";
  }
  return "VARCHAR(128)";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SqliteBack::tableExists(std::string name) {
  return find(tbl_names_.begin(), tbl_names_.end(), name) != tbl_names_.end();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::writeEvent(event_ptr e) {
  std::stringstream colss, valss, cmd;
  ValMap vals = e->vals();
  ValMap::iterator it = vals.begin();

  colss << kShortSimId;
  valss << short_sim_id_;
  for (it = vals.begin(); it != vals.end(); it++) {
    colss << ", " << it->first;
    valss << ", " << valAsString(it->second);
  }

  cmd << "INSERT INTO " << e->title() << " (" << colss.str() << ") "
      << "VALUES (" << valss.str() << ");";
  cmds_.push_back(cmd.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string SqliteBack::valAsString(boost::any v) {
  // NOTE: the ugly structure of this if block is for performance reasons
  if (v.type() == typeid(int)) {
    std::stringstream ss;
    ss << boost::any_cast<int>(v);
    return ss.str();
  } else if (v.type() == typeid(double)) {
    std::stringstream ss;
    ss << boost::any_cast<double>(v);
    return ss.str();
  } else if (v.type() == typeid(std::string)) {
    return "\"" + boost::any_cast<std::string>(v) + "\"";
  } else if (v.type() == typeid(float)) {
    std::stringstream ss;
    ss << boost::any_cast<float>(v);
    return ss.str();
  }
  CLOG(LEV_ERROR) << "attempted to record unsupported type in backend "
                  << name();
  return "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SqliteBack::flush() {
  db_->execute("BEGIN TRANSACTION");
  for (StrList::iterator it = cmds_.begin(); it != cmds_.end(); it++) {
    try {
      db_->execute(*it);
    } catch (CycIOException err) {
      CLOG(LEV_ERROR) << "backend '" << path_ << "' failed write: "
                      << err.what();
    }
  }
  db_->execute("END TRANSACTION");
  cmds_.clear();
}

