// hdf5_back.h
#ifndef CYCLUS_CORE_UTILITY_HDF5_BACK_H_
#define CYCLUS_CORE_UTILITY_HDF5_BACK_H_

#include <map>
#include <set>
#include <string>

#include "boost/filesystem.hpp"

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

  virtual inline void Flush() {H5Fflush(file_, H5F_SCOPE_GLOBAL);};

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds);

 private:
  /// Creates a QueryResult from a table description.
  QueryResult GetTableInfo(std::string title, hid_t dset, hid_t dt);

  /// Reads a table's column types into tbl_types_ if they aren't already there
  /// \{
  void LoadTableTypes(std::string title, hsize_t ncols);
  void LoadTableTypes(std::string title, hid_t dset, hsize_t ncols);
  /// \}

  /// Creates and initializes an hdf5 table with schema defined by d.
  void CreateTable(Datum* d);

  /// Writes a group of Datum objects with the same title to their
  /// corresponding hdf5 dataset.
  void WriteGroup(DatumList& group);

  /// Fill a contiguous memory buffer with data from group for writing to an
  /// hdf5 dataset.
  void FillBuf(std::string title, char* buf, DatumList& group, size_t* sizes, 
               size_t rowsize);

  /// Read variable length data from the database.
  /// @param rawkey the SHA1 digest key as a byte array.
  /// @return the value indicated by this type at this location.
  template <typename T, DbTypes U>
  T VLRead(const char* rawkey);

  /// Writes a variable length data to its on-disk bidirectional hash map.
  /// @param x the data to write.
  /// @param dbtype the data type of x.
  /// @return the key of x, which is a SHA1 hash as len-5 an array of ints.
  /// \{
  template <typename T, DbTypes U>
  Digest VLWrite(const T& x);

  template <typename T, DbTypes U>
  inline Digest VLWrite(const boost::spirit::hold_any* x) {
    return VLWrite<T, U>(x->cast<T>());
  };
  /// \}

  /// Gets an HDF5 reference dataset for a variable length datatype
  /// If the dataset does not exist in the database, it will create it.
  ///
  /// @param dbtype the datatype to retrive
  ///
  /// @param forkeys specifies whether to retrieve the keys (true) or 
  /// values (false) dataset, optional
  ///
  /// @return the dataset identifier
  hid_t VLDataset(DbTypes dbtype, bool forkeys=true);

  /// Appends a key to a variable lengeth key dataset
  ///
  /// @param dset an open HDF5 dataset
  /// @param dbtype the variable length data type
  /// @param key the SHA1 digest to append
  void AppendVLKey(hid_t dset, DbTypes dbtype, const Digest& key);


  /// Inserts a variable length data into it value dataset
  ///
  /// @param dset an open HDF5 dataset
  /// @param dbtype the variable length data type
  /// @param key the SHA1 digest to append
  /// @param buf the value or buffer to insert
  /// \{
  void InsertVLVal(hid_t dset, DbTypes dbtype, const Digest& key, 
                   const std::string& val);
  void InsertVLVal(hid_t dset, DbTypes dbtype, const Digest& key, 
                   hvl_t buf);
  /// \}

  hvl_t VLValToBuf(const std::vector<int>& x);

  std::vector<int> VLBufToVal(const hvl_t& buf);

  /// A class to help with hashing variable length datatypes
  Sha1 hasher_;

  /// A reference to a database.
  hid_t file_;
  /// The HDF5 UUID type, 16 byte char string.
  hid_t uuid_type_;
  /// The HDF5 SHA1 type, len-5 int array.
  hid_t sha1_type_;
  /// The HDF5 variable length string type.
  hid_t vlstr_type_;
  /// The HDF5 Blob type, variable length string.
  hid_t blob_type_;

  /// Variable length value chunk size and extent
  static const hsize_t vlchunk_[CYCLUS_SHA1_NINT];

  /// Listing of types opened here so that we may close them.
  std::set<hid_t> opened_types_;

  /// Stores the database's path+name, declared during construction.
  std::string path_;

  std::map<std::string, size_t*> tbl_offset_;
  std::map<std::string, size_t*> tbl_sizes_;
  std::map<std::string, size_t> tbl_size_;
  std::map<std::string, DbTypes*> tbl_types_;
  std::map<std::string, hid_t> vldatasets_;
  std::map<DbTypes, hid_t> vldts_;
  std::map<DbTypes, std::set<Digest> > vlkeys_;
};

} // namespace cyclus
#endif
