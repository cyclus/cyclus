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
     @param comp the previous composition 
   */
  IsoVector(comp_t* comp); 

  /**
     constructor given some initial composition 
     basis assumed to be mass-wise
     @param initial_comp the initial composition 
   */
  IsoVector(CompMap* initial_comp); 

  /**
     constructor given some initial composition and some
     basis
     @param initial_comp the initial composition 
     @param atom true if atom-basis, false if mass basis
   */
  IsoVector(CompMap* initial_comp, bool atom); 

  /**
     default destructor
   */
  ~IsoVector();
  /* --- */

  /* --- Operators  --- */
  /**
     Adds like isotopes 
   */
  const IsoVector operator+ (const IsoVector& rhs_vector) const;

  /**
     Subtracts like isotopes 
      
     @exception CycRangeException thrown if subtraction results in a 
     negative quantity for any isotope. 
   */
  const IsoVector operator- (const IsoVector& rhs_vector) const;

  /**
     Compares quantities of like isotopes returning true only if 
     quantity for every isotope is equal. 
   */
  bool operator== (const IsoVector& rhs_vector) const;

  /**
     multiplication operators
   */
  friend const IsoVector operator* (const IsoVector &v, double factor);
  friend const IsoVector operator* (double factor, const IsoVector &v);
  friend const IsoVector operator* (const IsoVector &v, int factor);
  friend const IsoVector operator* (int factor, const IsoVector &v);

  /**
     division operators
   */
  friend const IsoVector operator/ (const IsoVector &v, double factor);
  friend const IsoVector operator/ (const IsoVector &v, int factor);
  /* --- */

  /* --- Instance Interaction  --- */ 
  /**
     the current state id 
   */
  int stateID();

  /**
     Return the mass-based composition 
   */
  CompMap* mass_comp();

  /**
     returns the decay time for the IsoVector's composition_
   */
  int decay_time();

  /**
     returns the parent of the IsoVector's composition_
   */
  comp_t* parent();

  /**
     whether or not this composition is logged as a recipe
  */
  bool logged();

  /**
     calls minimizeComposition() on composition_
   */    
  void minimizeComposition();

  /**
     multiply the mass_normalizer by a factor; used in multiplication
   */
  void multMassNormBy(double factor);

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
  /* --- */

  /* --- Isotope Wikipedia  --- */ 
  /**
     Returns the atomic number of the isotope with the given identifier. 
      
     @param tope the isotope whose atomic number is being returned 
     @return the atomic number 
   */
  static int getAtomicNum(Iso tope);

  /**
     Returns the mass number of the isotope with the given identifier. 
      
     @param tope the isotope whose mass number is being returned 
     @return the mass number 
   */
  static int getMassNum(Iso tope);
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
  void massify(CompMap* comp);

  /**
     Turns a list of mass-based compositions
     to atom-based
   */
  void atomify(CompMap* comp);

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
  void minimizeComposition(comp_t* c);

  /**
     multiply the mass_normalizer of c by a factor; 
     used in multiplication
   */
  void multMassNormBy(comp_t* c,double factor);
  /* --- */

  /* --- Instance Interaction  --- */ 
  /**
     a pointer to the isovector's composition
   */
  comp_t composition_;
  /* --- */

  /* --- Isotope Wikipedia  --- */
  /**
     calls validateIsotopeNumber() and validateFraction() 
     for each isotope and fraction value in this IsoVector's 
     composition
   */
  void validateComposition();

  /**
     throws an error if fraction < 0.0
   */
  static void validateFraction(double fraction);

  /**
     Used to determine validity of isotope defnition. 
     @param tope isotope identifier 
     @exception thrown if isotope identifier is invalid 
   */
  static void validateIsotopeNumber(Iso tope);
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
