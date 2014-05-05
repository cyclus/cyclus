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
/// datum value types: int, float, double, std::string, cyclus::Blob,
/// boost::uuids::uuid.
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

  virtual void Flush() {H5Fflush(file_, H5F_SCOPE_GLOBAL);};

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds);

 private:
  /// Creates a QueryResult from a table description.
  QueryResult GetTableInfo(std::string title, hid_t dset, hid_t dt);

  /// Reads a table's column types into tbl_types_ if they aren't already there
  /// {
  void LoadTableTypes(std::string title, hsize_t ncols);
  void LoadTableTypes(std::string title, hid_t dset, hsize_t ncols);
  /// }

  /// creates and initializes an hdf5 table with schema defined by d.
  void CreateTable(Datum* d);

  /// writes a group of Datum objects with the same title to their
  /// corresponding hdf5 dataset.
  void WriteGroup(DatumList& group);

  /// fill a contiguous memory buffer with data from group for writing to an
  /// hdf5 dataset.
  void FillBuf(std::string title, char* buf, DatumList& group, size_t* sizes, 
               size_t rowsize);

  /// An reference to a database.
  hid_t file_;
  hid_t string_type_;
  hid_t blob_type_;

  /// Stores the database's path+name, declared during construction.
  std::string path_;

  std::map<std::string, size_t*> tbl_offset_;
  std::map<std::string, size_t*> tbl_sizes_;
  std::map<std::string, size_t> tbl_size_;
  std::map<std::string, DbTypes*> tbl_types_;
};

/// Compares a condiontion for a value 
template <typename T>
inline bool CmpCond(T* x, Cond* cond) {
  bool rtn;
  switch (cond->opcode) {
    case LT: {
      rtn = (*x) < cond->val.cast<T>() ? true : false;
      break;
    }
    case GT: {
      rtn = (*x) > cond->val.cast<T>() ? true : false;
      break;
    }
    case LE: {
      rtn = (*x) <= cond->val.cast<T>() ? true : false;
      break;
    }
    case GE: {
      rtn = (*x) >= cond->val.cast<T>() ? true : false;
      break;
    }
    case EQ: {
      rtn = (*x) == cond->val.cast<T>() ? true : false;
      break;
    }
    case NE: {
      rtn = (*x) != cond->val.cast<T>() ? true : false;
      break;
    }
  }
  return rtn;
};

/// Compares all condiontions for a value 
template <typename T>
inline bool CmpConds(T* x, std::vector<Cond*>* conds) {
  int i;
  for (i = 0; i < conds->size(); ++i)
    if (!CmpCond<T>(&(*x), (*conds)[i]))
      return false;
  return true;
};

} // namespace cyclus
#endif
