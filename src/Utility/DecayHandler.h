// DecayHandler.h
#if !defined(_DECAYHANDLER_H)
#define _DECAYHANDLER_H

#include "UseMatrixLib.h"
#include "IsoVector.h"

/**
 *  A map type to represent all of the parent isotopes tracked.  The key for
 * this map type is the parent's Iso number, and the value is a pair that
 * contains the corresponding decay matrix column and decay constant
 * associated with that parent.
 */
typedef std::map< int, std::pair<int, double> > ParentMap;

/**
 *  A map type to represent all of the daughter isotopes tracked.  The key for
 * this map type is the decay matrix column associated with the parent, and the
 * value is a vector of pairs of all the daughters for that parent. Each of the
 * daughters are represented by a pair that contains the daughter's Iso number
 * and its branching ratio.
 */
typedef std::map<int, std::vector<std::pair<int, double> > > DaughtersMap;

typedef std::vector<int> IsoList;

class DecayHandler {
  private:
    /**
     *  Builds the decay matrix needed for the decay calculations from the parent
     * and daughters map variables.  The resulting matrix is stored in the static
     * variable decayMatrix.
     */
    static void buildDecayMatrix();

    /**
     *  Reads the decay information found in the 'decayInfo.dat' file into the
     * parent and daughters maps.Uses these maps to create the decay matrix.
     */
    static void loadDecayInfo();

    /**
     *  The IsoVector's parent
     */
    static ParentMap parent_; 
    
    /**
     *  The IsoVector's daughters
     */
    static DaughtersMap daughters_; 

    /**
     *  The decay matrix
     */
    static Matrix decayMatrix_; 

    /**
     *  The atomic composition map
     */
    CompMap atom_comp_;

    /**
     *  whether the decay information is loaded
     */
    static bool decay_info_loaded_;

    /**
     *  the list of tracked isotopes
     */
    static IsoList IsotopesTracked_;
    
    /**
     *  Add the Isotope to our list of tracked isotopes IFF it is not already in that list
     */
    static void addIsoToList(int iso);

  public:    
    /**
     *  default constructor
     */
    DecayHandler();
    
    /**
     *  set the composition from a CompMap
     */
    void setComp(CompMap comp);

    /**
     *  set the composition from a composition vector
     */
    void setComp(Vector comp);

    /**
     *  return the composition as a composition vector
     */ 
    Vector compAsVector();

    /**
     *  return the composition as a composition map
     */ 
    CompMap compAsCompMap();

    /**
     *  decay the material
     * @param years the number of years to decay
     */ 
    void decay(double years);
    
    /**
     *  the number of tracked isotopes
     */
    int nTrackedIsotopes(){return IsotopesTracked_.size();}

    /**
     *  the tracked isotope at position i
     */
    int trackedIsotope(int i){return IsotopesTracked_.at(i);}
};

#endif

