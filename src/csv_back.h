// csv_back.h
#ifndef CYCLUS_CORE_UTILITY_CSV_BACK_H_
#define CYCLUS_CORE_UTILITY_CSV_BACK_H_

#include <map>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "any.hpp"
#include "rec_backend.h"

namespace cyclus {

typedef std::vector<std::string> LineList;

/// An Recorder backend that writes data to a collection of csv files.
/// Identically named Datum objects have their data placed in the same file.  Handles the
/// following datum value types: int, float, double, std::string, cyclus::Blob.
class CsvBack: public RecBackend {
 public:
  /// Creates a new csv backend that will write to a set of csv files in path
  /// @param path directory to place csv file into
  /// @param overwrite true to overwrite existing csv files with same path/name.
  CsvBack(std::string path, bool overwrite = false);

  /// Collects Datum objects to be written to csv files.
  /// @param data group of Datum objects to write to the csv file collection.
  void Notify(DatumList data);

  std::string Name();

  /// Writes any remaining csv lines to files.
  void Close();

 protected: // for testing
  /// Write all buffered csv lines.
  virtual void Flush();

  /// maps filenames to pending csv lines
  std::map<std::string, LineList> file_data_;

 private:
  /// converts the value to a valid csv value string.
  std::string ValAsString(boost::spirit::hold_any& v);

  /// constructs a valid csv line for the datum and queues it for writing
  void WriteDatum(Datum* d);

  /// Stores the database's path, declared during construction.
  boost::filesystem::path path_;
};
}  // namespace cyclus
#endif
