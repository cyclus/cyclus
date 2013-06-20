// Hdf5Back.h
#if !defined(_HDF5BACK_H)
#define _HDF5BACK_H

#include "EventBackend.h"
#include "H5Cpp.h"

#include <list>
#include <string>

class Hdf5Back : public EventBackend {

  public:

    Hdf5Back(std::string path);

    virtual void notify(EventList events);

    virtual std::string name();

    virtual void close();

  private:

    /// creates and stores a valid hdf5 composite type set definition for ev.
    H5::CompType* createMemType(event_ptr ev);

    /// creates and stores a valid hdf5 dataset definition for ev.
    H5::DataSet* createDataSet(std::string title, H5::CompType* mtype);

    /// write a set of events with the same title to their corresponding hdf5 dataset
    void writeSet(EventList& set);

    /// fill a contiguous memory buffer with data from set for writing to an hdf5 dataset.
    void fillBuf(char* buf, EventList& set, H5::CompType* mtype, size_t rowsize);

    /// An interface to a sqlite db managed by the SqliteBack class.
    H5::H5File* file_;

    /// Stores the database's path+name, declared during construction.
    std::string path_;

    /// hdf5 dataset composite rowtype information
    std::map<std::string, H5::CompType*> mtypes_;

    /// already created datasets kept available for extension as new events arrive.
    std::map<std::string, H5::DataSet*> datasets_;

    /// names and memory sizes for already created datasets in the hdf5 file.
    std::map<std::string, size_t> dataset_sizes_;
};

#endif

