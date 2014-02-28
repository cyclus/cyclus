// csv_back.cc
#include "csv_back.h"

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "blob.h"
#include "datum.h"
#include "error.h"
#include "logger.h"

namespace fs = boost::filesystem;

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CsvBack::CsvBack(std::string path, bool overwrite) : path_(path) {
  if (overwrite) {
    fs::remove_all(path);
  }
  fs::create_directory(path);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CsvBack::Notify(DatumList data) {
  for (DatumList::iterator it = data.begin(); it != data.end(); it++) {
    WriteDatum(*it);
  }
  Flush();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CsvBack::Close() {
  Flush();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string CsvBack::Name() {
  return path_.string();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CsvBack::WriteDatum(Datum* d) {
  std::stringstream line;
  std::stringstream header;
  Datum::Vals vals = d->vals();
  fs::path fname(d->title() + ".csv");
  std::string path = (path_ / fname).string();

  // create header if first datum with this title
  Datum::Vals::iterator it = vals.begin();
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
    line << ValAsString(it->second);
    ++it;
    if (it == vals.end()) {
      break;
    }
    line << ", ";
  }

  file_data_[path].push_back(line.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string CsvBack::ValAsString(boost::spirit::hold_any& v) {
  std::stringstream ss;
  if (v.type() == typeid(int)) {
    ss << v.cast<int>();
  } else if (v.type() == typeid(float)) {
    ss << v.cast<float>();
  } else if (v.type() == typeid(double)) {
    ss << v.cast<double>();
  } else if (v.type() == typeid(std::string)) {
    ss << "\"" << v.cast<std::string>() << "\"";
  } else if (v.type() == typeid(Blob)) {
    boost::uuids::uuid u = boost::uuids::random_generator()();
    std::string fname = boost::lexical_cast<std::string>(u) + ".blob";
    std::string path = (path_ / fname).string();

    std::string s = v.cast<Blob>().str();
    std::ofstream file;
    file.open(path.c_str(), std::fstream::in | std::fstream::app);
    file << s;
    file.close();

    ss << "\"" << fname << "\"";
  } else if (v.type() == typeid(boost::uuids::uuid)) {
    boost::uuids::uuid u = v.cast<boost::uuids::uuid>();
    ss << "\"" << boost::lexical_cast<std::string>(u) << "\"";
  } else {
    ss << "\"unsupported-type: " << v.type().name() << "\"";
    CLOG(LEV_ERROR) << "attempted to record unsupported type '"
                    << v.type().name() << "' in backend "
                    << Name();
  }
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CsvBack::Flush() {
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

}  // namespace cyclus
