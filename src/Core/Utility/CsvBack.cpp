// CsvBack.cpp
#include "CsvBack.h"

#include "CycException.h"
#include "Logger.h"
#include "Event.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace fs = boost::filesystem;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CsvBack::CsvBack(std::string path, bool overwrite) : path_(path) {
  if (overwrite) {
    fs::remove_all(path);
  }
  fs::create_directory(path);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CsvBack::notify(EventList evts) {
  for (EventList::iterator it = evts.begin(); it != evts.end(); it++) {
    writeEvent(*it);
  }
  flush();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CsvBack::close() {
  flush();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string CsvBack::name() {
  return path_.string();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CsvBack::writeEvent(Event* e) {
  std::stringstream line;
  std::stringstream header;
  Event::Vals vals = e->vals();
  fs::path fname(e->title() + ".csv");
  std::string path = (path_ / fname).string();

  // create header if first event with this title
  Event::Vals::iterator it = vals.begin();
  if (file_data_.find(path) == file_data_.end() && !fs::exists(path)) {
    it = vals.begin();
    while (true) {
      header << it->first;
      ++it;
      if (it == vals.end()) {
        break;
      }
      header << ", ";
    }
    file_data_[path].push_back(header.str());
  }

  it = vals.begin();
  while (true) {
    line << valAsString(it->second);
    ++it;
    if (it == vals.end()) {
      break;
    }
    line << ", ";
  }

  file_data_[path].push_back(line.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string CsvBack::valAsString(boost::spirit::hold_any& v) {
  std::stringstream ss;
  if (v.type() == typeid(int)) {
    ss << v.cast<int>();
  } else if (v.type() == typeid(float)) {
    ss << v.cast<float>();
  } else if (v.type() == typeid(double)) {
    ss << v.cast<double>();
  } else if (v.type() == typeid(std::string)) {
    ss << "\"" << v.cast<std::string>() << "\"";
  } else if (v.type() == typeid(boost::uuids::uuid)) {
    boost::uuids::uuid u = v.cast<boost::uuids::uuid>();
    ss << "\"" << boost::lexical_cast<std::string>(u) << "\"";
  } else {
    ss << "\"unsupported-type: " << v.type().name() << "\"";
    CLOG(LEV_ERROR) << "attempted to record unsupported type '"
                    << v.type().name() << "' in backend "
                    << name();
  }
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CsvBack::flush() {
  std::map<std::string, LineList>::iterator it;
  for (it = file_data_.begin(); it != file_data_.end(); it++) {
    LineList lines = it->second;

    std::ofstream file;
    file.open(it->first.c_str(), std::fstream::in | std::fstream::app);
    for (int i = 0; i < lines.size(); ++i) {
      file << lines[i] << std::endl;
    }
    file.close();
    file_data_[it->first].clear();
  }
}

