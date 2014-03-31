// recorder.h
#ifndef CYCLUS_RECORDER_H_
#define CYCLUS_RECORDER_H_

#include <list>
#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>

#include "error.h"

namespace cyclus {

class Datum;
class Recorder;
class RecBackend;

typedef std::vector<Datum*> DatumList;

/// default number of Datum objects to collect before flushing to backends.
static unsigned int const kDefaultDumpCount = 500;

/// Collects and manages output data generation for the cyclus core and agents
/// during a simulation.  By default, datum managers are auto-initialized with a
/// unique uuid simulation id.
///
/// Example usage:
///
/// @code
///
/// Recorder* manager = new Recorder();
/// CustomBackend* myback = new CustomBackend();
/// manager->RegisterBackend(myback);
/// ...
/// ...
/// manager->NewDatum("CapacityFactor");
///        ->AddVal("Name", aname)
///        ->AddVal("Capacity", cap)
///        ->Record();
/// ...
/// ...
/// manager->Close();
///
/// @endcode
class Recorder {
  friend class Datum;

 public:
  /// create a new recorder with default dump frequency and random
  /// simulation id.
  Recorder();

  /// create a new recorder with default dump frequency and the specified
  /// simulation id.
  Recorder(boost::uuids::uuid simid);

  ~Recorder();

  /// Return the dump frequency, # Datum objects buffered between flushes to
  /// backends.
  unsigned int dump_count();

  /// set the Recorder to flush its collected Datum objects to registered
  /// backends every [count] Datum objects. If count == 0 then Datum objects
  /// will be flushed immediately as they come.
  ///
  /// @param count # Datum objects to buffer before flushing to backends.
  /// @warning this deletes all buffered data from the recorder.
  void set_dump_count(unsigned int count);

  /// returns the unique id associated with this cyclus simulation.
  boost::uuids::uuid sim_id();

  /// Creates a new datum namespaced under the specified title.
  ///
  /// @warning choose title carefully to not conflict with Datum objects from other
  /// agents. Also note that a static title (e.g. an unchanging string) will
  /// result in multiple instances of this agent storing datum data together
  /// (e.g. the same table).
  Datum* NewDatum(std::string title);

  /// Registers b to receive Datum notifications for all Datum objects collected
  /// by the Recorder and to receive a flush notification when there
  /// are no more Datum objects.
  ///
  /// @param b backend to receive Datum objects
  void RegisterBackend(RecBackend* b);

  /// Flushes all buffered Datum objects and flushes all registered backends.
  void Flush();

  /// Flushes all buffered Datum objects and flushes all registered backends.
  /// Unregisters all backends and resets.
  void Close();

 private:
  void NotifyBackends();
  void AddDatum(Datum* d);

  DatumList data_;
  int index_;
  std::list<RecBackend*> backs_;
  unsigned int dump_count_;
  boost::uuids::uuid uuid_;
};
} // namespace cyclus

// this allows files to use Datum objects without having to explicitly include
// both recorder.h and datum.h, while avoiding a circular include
// dependency.
#include "datum.h"

#endif
