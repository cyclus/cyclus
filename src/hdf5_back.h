// hdf5_back.h
#ifndef CYCLUS_CORE_UTILITY_HDF5_BACK_H_
#define CYCLUS_CORE_UTILITY_HDF5_BACK_H_

#include <map>
#include <string>

#include "query_backend.h"
#include "hdf5.h"
#include "hdf5_hl.h"

namespace cyclus {
/// An Recorder backend that writes data to an hdf5 file.  Identically named
/// Datum objects have their data placed as rows in a single table.  Handles the following
/// datum value types: int, float, double, std::string, cyclus::Blob
class Hdf5Back : public FullBackend {
 public:
  /// Creates a new backend writing data to the specified file.
  ///
  /// @param path the file to write to. If it exists, it will be overwritten.
  Hdf5Back(std::string path);

  /// cleans up resources and closes the file.
  virtual ~Hdf5Back();

  virtual void Notify(DatumList data);

  virtual std::string Name();

  virtual void Flush() {};

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds);

 private:
  /// creates and initializes an hdf5 table
  void CreateTable(Datum* d);

  /// write a group of Datum objects with the same title to their corresponding hdf5 dataset
  void WriteGroup(DatumList& group);

  /// fill a contiguous memory buffer with data from group for writing to an hdf5 dataset.
  void FillBuf(char* buf, DatumList& group, size_t* sizes, size_t rowsize);

  /// An interface to a sqlite db managed by the SqliteBack class.
  hid_t file_;

  hid_t string_type_;
  hid_t blob_type_;

  /// Stores the database's path+name, declared during construction.
  std::string path_;

  std::map<std::string, size_t*> tbl_offset_;
  std::map<std::string, size_t*> tbl_sizes_;
  std::map<std::string, size_t> tbl_size_;
};
} // namespace cyclus
#endif
