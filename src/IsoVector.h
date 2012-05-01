// IsoVector.h
#if !defined(_ISOVECTOR_H)
#define _ISOVECTOR_H

#include <map>
#include <set>
#include <utility>
#include <math.h>
#include <vector>
#include <string>
#include <libxml/tree.h>

#include "IsotopicDefinitions.h"
#include "UseMatrixLib.h"

/** 
   @class IsoVector 
    
   The IsoVector Class is an intelligent, mass-based isotopic composition 
   container. It reports normalized mass or atomic fractions, as requested.
  
   @section recipes Recipes 
   Recipes are pre-defined IsoVectors. Recipes will be used and decay
   frequently, so their evolution is tracked. The decay of recipe-based 
   IsoVectors is tracked through a parent-daughter relationship. Each parent
   and daughter is stored in a static container, and all IsoVectors in the 
   simulation using one of these recipes points to this static container. A 
   copy is only made if the composition deviates from the parent-daughter 
   chain through a + or - operation.

   Input file recipes can be either mass or atom (mole) based. 
   Recipes can be accessed a static map. For example:  
   @code 
    
   std::string recipe_name = "my-recipe-name"; 
   IsoVector* my_recipe = IsoVector::recipe(recipe_name); 
   
   @endcode 
 */
class IsoVector {
public:
  /* --- Constructors and Destructors --- */
  /**
     default constructor 
   */
  IsoVector(); 

  /**
     constructor given some previous composition 
     basis assumed to be mass-wise
     @param pcomp a pointer to a composition
   */
  IsoVector(CompMapPtr pcomp); 

  /**
     constructor given some initial composition and some
     basis
     @param pcomp a pointer to a composition
     @param atom true if atom-basis, false if mass basis
   */
  IsoVector(CompMapPtr pcomp, bool atom); 

  /**
     default destructor
   */
  ~IsoVector();
  /* --- */

  /* --- Operators  --- */
  /**
     adds two compositions
     uses the mix() function with a ratio of 1
   */
  const IsoVector operator+ (const IsoVector& rhs_vector) const;

  /**
     subtracts two compositions
   */
  const IsoVector operator- (const IsoVector& rhs_vector) const;

  /**
     Compares quantities of like isotopes returning true only if 
     quantity for every isotope is equal. 
   */
  bool operator== (const IsoVector& rhs_vector) const;
  /* --- */

  /* --- Instance Interaction  --- */ 
  /**
     the current state id 
   */
  int stateID();

  /**
     Return the mass-based composition 
   */
  CompMapPtr comp();

  /**
     returns the decay time for the IsoVector's composition_
   */
  int decay_time();

  /**
     returns the parent of the IsoVector's composition_
   */
  IsoVectorPtr parent();

  /**
     whether or not this composition is logged as a recipe
  */
  bool logged();

  /**
     Return the mass fraction of an isotope in the composition
   */
  double massFraction(Iso tope);

  /**
     returns the atom fraction of an isotope in the composition
   */
  double atomFraction(Iso tope);

  /**
     Returns true if the given isotope's number density is less than the 
     conservation of mass tolerance. 
      
     @param tope the isotope in question 
     @return true iff nd(tope) == 0 
   */
  bool isZero(Iso tope);

  /**
     Decays this Material object for the given change in time and 
     updates its composition map with the new number densities. 
      
     @param time_change the number of months to decay 
   */
  void executeDecay(double time_change);

  /**
     separates a composition of isovector v from this isovector's composition
     at a given effiency
     @param v the isovector defining the composition to be separated
     @param efficiency the efficiency by which to perform that separation
   */
  void separate(const IsoVector& v, double efficiency);

  /**
     mixes two IsoVectors with a given ratio of v1:v2
     @param v1 the first isovector
     @param v2 the second isovector
     @param ratio the amount of v1 compared to v2
     @return a shared pointer to the resulting composition
   */
  static IsoVectorPtr mix(const IsoVector& v1, const IsoVector& v2, double ratio);
  /* --- */

  /* --- Printing Functionality  --- */
  /**
     print the details of this IsoVector 
   */
  void print();
  /* --- */

 private:
  /* --- Initializations  --- */ 
  /**
     initialize any required members
   */
  void init();

  /**
     Turns a list of atom-based compositions
     to mass-based
   */
  void massify(CompMapPtr comp);

  /**
     Turns a list of mass-based compositions
     to atom-based
   */
  void atomify(CompMapPtr comp);

  /**
     set's the composition for this isovector
   */    
  void setComposition(comp_t* c);

  /**
     constructs a composition out of comp
     and calls setComposition()
   */    
  void setComposition(CompMap* comp);

  /**
     sets the composition for a mass or atom based
     composition. if atom == true, then massify()
     is called. setComposition() for the mass-based
     case is then called.
   */    
  void setComposition(CompMap* comp, bool atom);

  /**
     determines the mass/atom normalizers for a composition
   */    
  std::pair<double,double> getNormalizers(CompMap* comp);

  /**
     adjusts the composition such that the mass normalizer
     is equal to unity
   */    
  void normalize();
  /* --- */

  /* --- Instance Interaction  --- */ 
  /**
     a pointer to the isovector's composition
   */
  CompMapPtr composition_;

  /**
     a pointer to this isovector's parent, if it has one
  */
  IsoVectorPtr parent_;
  
  /* --- */
 
  /* --- Printing Functionality  --- */
  /**
     return a pointer to a vector of the composition as strings 
     @return the composition string vector 
   */
  static std::vector<std::string>* compStrings(comp_t* c);

  /**
     used by print() to 'hide' print code when logging is not desired 
   */
  static std::string detail(comp_t* c);
  /* --- */
};

#endif
