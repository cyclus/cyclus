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

    /// returns true if the table name already exists.
    bool tableExists(std::string name);

    /// creates and stores a valid hdf5 composite type set definition for e.
    void createMemType(event_ptr e);

    /// constructs an SQL INSERT command for e and queues it for db insertion.
    void writeSet(EventList set);

    /// An interface to a sqlite db managed by the SqliteBack class.
    H5::H5File* file_;

    /// Stores the database's path+name, declared during construction.
    std::string path_;

    std::map<std::string, H5::CompType*> types_;
    std::map<std::string, H5::DataSet*> datasets_;

    /// table names already existing (created) in the hdf5 file.
    std::map<std::string, size_t> table_sizes_;
};

#endif

