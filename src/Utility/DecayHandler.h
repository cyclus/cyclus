
// DecayHandler.h
#if !defined(_DECAYHANDLER_H)
#define _DECAYHANDLER_H

#include "UseMatrixLib.h"
#include "IsoVector.h"

/*!
 A map type to represent all of the parent isotopes tracked.  The key for
 this map type is the parent's Iso number, and the value is a pair that
 contains the corresponding decay matrix column and decay constant
 associated with that parent.
 */
typedef std::map< int, std::pair<int, double> > ParentMap;

/*!
 A map type to represent all of the daughter isotopes tracked.  The key for
 this map type is the decay matrix column associated with the parent, and the
 value is a vector of pairs of all the daughters for that parent. Each of the
 daughters are represented by a pair that contains the daughter's Iso number
 and its branching ratio.
 */
typedef std::map<int, std::vector<std::pair<int, double> > > DaughtersMap;

class DecayHandler {
  private:

    /*!
     Builds the decay matrix needed for the decay calculations from the parent
     and daughters map variables.  The resulting matrix is stored in the static
     variable decayMatrix.
     */
    static void buildDecayMatrix();

    /*!
     Reads the decay information found in the 'decayInfo.dat' file into the
     parent and daughters maps.Uses these maps to create the decay matrix.
     */
    static void loadDecayInfo();

    static ParentMap parent_; 
    
    static DaughtersMap daughters_; 
    
    static Matrix decayMatrix_; 

    CompMap atom_comp_;

    static bool decay_info_loaded_;

  public:

    DecayHandler();

    void setComp(CompMap comp);

    void setComp(Vector comp);

    Vector compAsVector();

    CompMap compAsCompMap();

    void decay(double years);

};

#endif

