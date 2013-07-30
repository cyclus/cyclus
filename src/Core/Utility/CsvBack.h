// CsvBack.h
#pragma once

#include "EventBackend.h"
#include "any.hpp"

#include <string>
#include <boost/filesystem.hpp>

typedef std::vector<std::string> LineList;

/*!
An EventManager backend that writes data to a collection of csv files.
Identically named events have their data placed in the same file.  Handles the
following event value types: int, float, double, std::string, cyclus::Blob.
*/
class CsvBack: public EventBackend {
  public:
    /*!
    Creates a new csv backend that will write to a set of csv files in path

    @param path directory to place csv file into
    @param overwrite true to overwrite existing csv files with same path/name.
    */
    CsvBack(std::string path, bool overwrite = false);

    /*!
    Collect events to be written to csv files.

    @param events group of events to write to the csv file collection.
    */
    void notify(EventList events);

    std::string name();

    /// Writes any remaining csv lines to files.
    void close();

  protected: // for testing

    /// Write all buffered csv lines.
    virtual void flush();

    /// maps filenames to pending csv lines
    std::map<std::string, LineList> file_data_;

  private:

    /// converts the value to a valid csv value string.
    std::string valAsString(boost::spirit::hold_any& v);

    /// constructs a valid csv line for the event and queues it for writing
    void writeEvent(Event* e);

    /// Stores the database's path, declared during construction.
    boost::filesystem::path path_;
};

