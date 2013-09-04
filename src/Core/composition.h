#ifndef COMPOSITION_H
#define COMPOSITION_H

#include <map>
#include <boost/shared_ptr.hpp>

namespace cyclus {

typedef int Iso;

/// a raw definition of isotopes and corresponding (dimensionless quantities).
typedef std::map<Iso, double> CompMap;

/// An immutable object responsible for holding a nuclide composition. It tracks
/// decay lineages to prevent duplicate calculations and output recording and is
/// able to record its composition data to output when told.  Each composition
/// keeps a pointer to references to every other composition that is a result of
/// decaying this or a previously decayed-from composition.
///
/// Compositions are immutable and thus their state must be created/defined
/// entirely at their creation. Compositions are created by passing in a
/// CompMap (a map of nuclides to quantities). In general CompMaps are
/// not assumed to be normalized to any particular value.  A Uranium dioxide
/// composition can be created as follows:
///
/// @code
/// CompMap v;
/// v[92235] = 2.4;
/// v[8016] = 4.8;
/// Composition c = Composition::CreateFromAtom(v);
/// @endcode
///
class Composition {
 public:
  typedef boost::shared_ptr<Composition> Ptr;

  /// Creates a new composition from v with its components having appropriate
  /// atom-based ratios. v does not need to be normalized to any particular
  /// value.
  static Ptr CreateFromAtom(CompMap v);

  /// Creates a new composition from v with its components having appropriate
  /// mass-based ratios. v does not need to be normalized to any particular
  /// value.
  static Ptr CreateFromMass(CompMap v);

  /// Returns a unique id associated with this composition.  Note that multiple
  /// material objects can share the same composition. Also Note that the id is
  /// not the same for two compositions that were separately created from the
  /// same CompMap.
  int id();

  /// Returns the unnormalized atom composition.
  const CompMap& atom();

  /// Returns the unnormalized mass composition.
  const CompMap& mass();

  /// Returns a decayed version of this composition (decayed
  /// delta timesteps). This composition remains unchanged.
  Ptr Decay(int delta);

  /// Records the isotopic composition in output database Compositions table (if
  /// not done previously).
  void Record();

 protected:

  /// a chain containing compositions that are a result of decay from a common
  /// ancestor composition. The key is the total amount of time a composition
  /// has been decayed from its root parent.
  typedef std::map<int, Composition::Ptr> Chain;

  typedef boost::shared_ptr<Chain> ChainPtr;

  Composition();

  ChainPtr decay_line_;

 private:
  /// This constructor allows the creation of decayed versions of
  /// compositions while avoiding extra memory allocations.
  Composition(int prev_decay, ChainPtr decay_line);

  /// Performs a decay calculation and creates a new decayed composition.
  Ptr NewDecay(int delta);

  static int next_id_;
  int id_;
  bool recorded_;
  CompMap atom_;
  CompMap mass_;

  /// the total time delta this composition has been decayed from its root ancestor.
  int prev_decay_;
};

} // namespace cyclus

#endif
