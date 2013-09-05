#ifndef CYCLUS_CORE_RESOURCES_RESTRACKER_H_
#define CYCLUS_CORE_RESOURCES_RESTRACKER_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "resource.h"

namespace cyclus {

/// Tracks and records the state and parent-child relationships of resources as
/// they are changed. Resource implementations embed this as a member variable
/// and invoke its methods appropriately to have their state changes tracked in
/// the canonical core resource table.
///
/// Invocations to Create, Extract, Absorb, and Modify result in one or more
/// entries in the output db Resource table and also call the Record method of
/// the tracker's tracked resource.  A zero parent id indicates a resource id
/// has no parent; if both are zeros the resource was newly created.
class ResTracker {
 public:
  /// Create a new tracker following r.
  ResTracker(Resource* r);

  /// Prevents a resource's heritage from being tracked and recorded.
  void DontTrack();

  /// Should be called when a resource instance is newly created.
  void Create();

  /// Should be called when a resource has some quantity removed from it (e.g.
  /// resource is split).
  /// @param removed the tracker of the extracted/removed resource.
  void Extract(ResTracker* removed);

  /// Should be called when a resource is combined with another.
  /// @param absorbed the tracker of the resource being absorbed.
  void Absorb(ResTracker* absorbed);

  /// Should be called when the state of a resource changes (e.g. radioactive
  /// decay).
  void Modify();

 private:
  void Record();

  int parent1_;
  int parent2_;
  bool tracked_;
  Resource* res_;
};

} // namespace cyclus

#endif
