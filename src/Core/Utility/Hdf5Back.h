// Hdf5Back.h
#if !defined(_HDF5BACK_H)
#define _HDF5BACK_H

#include "EventBackend.h"
#include "hdf5.h"
#include "hdf5_hl.h"

#include <list>
#include <string>

/// Records simulation data in HDF5 format.
class Hdf5Back : public EventBackend {

  public:

    // Creates a new backend writing data to the specified file.
    //
    // @param path the file to write to. If it exists, it will be overwritten.
    Hdf5Back(const char* path);

    virtual ~Hdf5Back() {};

    virtual void notify(EventList events);

    virtual std::string name();

    // cleans up resources and closes the file.
    virtual void close();

  private:

    /// creates and initializes an hdf5 table
    void createTable(event_ptr ev);

    /// write a set of events with the same title to their corresponding hdf5 dataset
    void writeSet(EventList& set);

    /// fill a contiguous memory buffer with data from set for writing to an hdf5 dataset.
    void fillBuf(char* buf, EventList& set, size_t* sizes, size_t rowsize);

    /// An interface to a sqlite db managed by the SqliteBack class.
    hid_t file_;

    hid_t string_type_;

    /// Stores the database's path+name, declared during construction.
    const char* path_;

    std::map<std::string, size_t*> tbl_offset_;
    std::map<std::string, size_t*> tbl_sizes_;
    std::map<std::string, size_t> tbl_size_;
};

#endif

