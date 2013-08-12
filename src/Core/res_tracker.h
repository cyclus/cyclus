#ifndef CYCLUS_CORE_RESOURCES_RESTRACKER_H_
#define CYCLUS_CORE_RESOURCES_RESTRACKER_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "resource.h"

namespace cyclus {

class ResTracker {
 public:
  /// Create a new tracker following r.
  ResTracker(Resource* r);

  /// Prevents a resource's heritage from being tracked and recorded.
  void DontTrack();

  /// Should be called when a resource instance is newly created
  void Create();

  /// Should be called when a resource has some quantity removed from it (e.g.
  /// resource is split).
  void Extract(ResTracker* removed);

  /// Should be called when a resource is combined with another.
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
