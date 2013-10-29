// decayer.h
#ifndef DECAYER_H_
#define DECAYER_H_

#include <map>

#include "use_matrix_lib.h"
#include "composition.h"

namespace cyclus {

/**
   A map type to represent all of the parent isotopes tracked.  The key
   for this map type is the parent's Iso number, and the value is a pair
   that contains the corresponding decay matrix column and decay
   constant associated with that parent.
 */
typedef std::map< int, std::pair<int, double> > ParentMap;

/**
   A map type to represent all of the daughter isotopes tracked.  The
   key for this map type is the decay matrix column associated with the
   parent, and the value is a vector of pairs of all the daughters for
   that parent. Each of the daughters are represented by a pair that
   contains the daughter's Iso number and its branching ratio.
 */
typedef std::map<int, std::vector<std::pair<int, double> > > DaughtersMap;

typedef std::vector<int> IsoList;

class Decayer {
 private:
  /**
     Builds the decay matrix needed for the decay calculations from
     the parent and daughters map variables.  The resulting matrix is
     stored in the static variable decayMatrix.
   */
  static void BuildDecayMatrix();

  /**
     Reads the decay information found in the 'decayInfo.dat' file
     into the parent and daughters maps.Uses these maps to create the
   */
  static void LoadDecayInfo();

  /**
     The CompMap's parent
   */
  static ParentMap parent_;

  /**
     The CompMap's daughters
   */
  static DaughtersMap daughters_;

  /**
     The decay matrix
   */
  static Matrix decay_matrix_;

  /**
     The atomic composition map
   */
  Vector pre_vect_;
  Vector post_vect_;

  /**
     whether the decay information is loaded
   */
  static bool decay_info_loaded_;

  /**
     the list of tracked isotopes
   */
  static IsoList isotopes_tracked_;

  /**
     Add the Isotope to our list of tracked isotopes IFF it is not
   */
  static void AddIsoToList(int iso);

 public:
  /// Returns the decay constant for specified isotope in inverse years.
  static double DecayConstant(int iso);

  /**
     default constructor
   */
  Decayer(const CompMap& comp);

  /**
     set the composition from a CompMap
   */
  void GetResult(CompMap& comp);

  /**
     decay the material
     @param years the number of years to decay
   */
  void Decay(double years);

  /**
     the number of tracked isotopes
   */
  int n_tracked_isotopes() {
    return isotopes_tracked_.size();
  }

  /**
     the tracked isotope at position i
   */
  int TrackedIsotope(int i) {
    return isotopes_tracked_.at(i);
  }
};

} // namespace cyclus

#endif

