// hdf5_back.h
#ifndef CYCLUS_CORE_UTILITY_HDF5_BACK_H_
#define CYCLUS_CORE_UTILITY_HDF5_BACK_H_

#include <map>
#include <set>
#include <string>

#include "boost/filesystem.hpp"

#include "hdf5.h"
#include "hdf5_hl.h"

#include "query_backend.h"

namespace cyclus {

/// An Recorder backend that writes data to an hdf5 file.  Identically named
/// Datum objects have their data placed as rows in a single table.  
///
/// The HDF5 backend ensures that every column in its tables is represented 
/// in the schema with a fixed size.  This in turn ensures that the schema itself
/// is of a fixed size. This fixed size constraint applies even to variable length 
/// (VL) data types (string, blob, vector, etc).  
///
/// Variable length data is handled in a special way to ensure a fixed length 
/// column.  The naive approach would be to set a maximum size based on the data
/// available. However, this is not truly a fixed length data type. Instead, the
/// HDF5 backend serves as an on-disk bidriectional hash map for each VL data type.
///
/// A regular hash table applies a hash function to keys and stores the values based
/// on this hash. Keys are unique and values may be repeated for many keys. In a
/// bidirectional hash map the keys and values are both one-to-one and onto. This 
/// makes storing a seperate hash redunant and since the key and hash are the same.
///
/// The HDF5 backend uses the well-known SHA1 hash function as its keys for VL data.
/// This is because SHA1 is 5x the size (20 bytes) of standard 32-bit unsigned ints.
/// This provides a gigantic address space in which to store variable length data.
/// HDF5 provides two critical features that make an address space of this size 
/// possible: multidiemnsional arrays and chunking.
///
/// HDF5 easily supports 5D arrays. This allows us to use the SHA1 hash not only as 
/// a key, but also a we can cast it to an index (len-5 array of unsigned ints) for 
/// a 5D array.  Furthermore, for such an array we can set the chunksize to a single 
/// element ([1, 1, 1, 1, 1]). This allows us to have the full space available 
/// ([UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX]) while only storing the 
/// data that actually exists!
///
/// In the table columns for VL data, the HDF5 backend stores the SHA1 as a length-5
/// array of unsigned ints. Looking up the associated value is simply a matter of using
/// this array as an index into a special data type array as above.
/// This has the added advantage of de-duplicating storage for identical entries.
///
/// On disk the keys and values for a data type are stored as arrays named with 
/// base data type and the string "Keys" and "Vals" appended respectively.  For 
/// instance, BLOB is stored in the arrays BlobKeys and BlobVals while VL_VECTOR_INT
/// is stored in the arrays VectorIntKeys and VectorIntVals.
///
/// In memory, all active keys are stored in vlkeys_ private member of this class.
/// This maps the DbType to a set of the SHA1 digests. This is used to prevent 
/// excessive writing of values to disk that already exist.
///
/// The cost of the bidirectional hash map strategy is that the values need to be 
/// looked up in a separate read() from that of the table itself.  However, by 
/// using VL data types users should expect a performance hit and this is one of 
/// the more effiecient strategies.
///
/// Another implicit problem with all hash mappings is the possibility of collision.
/// However, this is in practice impossible here.  For SHA1, there is a 3.4e-13 chance
/// of having a single collission with 1e18 (a billion billion) entries.  
///
/// Still, if the address space of SHA1 ever becomes insufficient for some reason, 
/// please  move to a larger SHA value such as SHA224 or SHA256 or higher. Such a 
/// migration is not anticipated but would be straighforward.
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

  /// Converts a value to a variable length buffer for HDF5.
  /// \{
  hvl_t VLValToBuf(const std::vector<int>& x);
  hvl_t VLValToBuf(const std::set<int>& x);
  hvl_t VLValToBuf(const std::list<int>& x);
  /// \}

  /// Converts a variable length buffer to a value for HDF5.
  /// \{
  template <typename T> 
  T VLBufToVal(const hvl_t& buf);
  /// \}

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
