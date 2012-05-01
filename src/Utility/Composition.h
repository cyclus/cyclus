// Composition.h
#if !defined(_COMPOSITION_H)
#define _COMPOSITION_H

#include <map>
#include <boost/unique_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

/* -- Useful Definitions -- */
/**
   avagadro's number
*/
#define AVOGADRO 6.02e23

/**
   smallest kilogram value
*/
#define EPS_KG 1e-6

/**
   smallest percent
*/
#define EPS_PERCENT 1e-14
/* -- */

/* -- Useful Typedefs -- */
/**
   Isotope integer, ZZZAAA
 */
typedef int Iso;

/**
   map of isotope integer to value (mass or atom)
   and a pointer to said map
 */
typedef std::map<Iso, double> CompMap;
typedef boost::unique_ptr<CompMap> CompMapPtr;

/**
   pointer to another composition
 */
class Composition;
typedef boost::shared_ptr<Composition> CompositionPtr;
/* -- */

/** 
   @class Composition

   @section Introduction
   The Composition class in Cyclus is a wrapper class for
   isotopic compositions so that they may be logged with the 
   BookKeeper.
*/
class Composition : public enable_shared_from_this<Composition> {
 public:
  /* --- Constructors and Destructors --- */
  /**
     constructor given a composition. the ID_ and decaytime_
     are initially set to 0.
     @param comp the composition to be copied into composition_
   */
  Composition(CompMap& comp);

  /**
     default destructor, releases mass_fractions_ and parent_
   */
  ~Composition();
  /* --- */

  /* --- Operators --- */
  /**
     the less-than operator to allowed compositions to be stored
     as keys in maps
   */
  bool operator<(const Composition& other) const;
  /* --- */

  /* --- Instance Access --- */
  /**
     returns true if the composition's id has been set
   */
  bool logged();

  /**
     returns a shared pointer to this composition
   */
  CompositionPtr me();

  /**
     returns a shared pointer to this composition's parent
   */
  CompositionPtr parent();

  /**
     returns the time decayed between this Composition and its parent
   */
  double decay_time();
  /* --- */

  /* --- Transformations --- */
  /**
     alters comp, multiplying each entry by its molar weight (g/mol)
     @param comp the initial atom-based composition
   */
  static void massify(CompMap& comp);

  /**
     alters comp, summing the total values to 1
     @param comp the composition to normalize
   */
  static void normalize(CompMap& comp);

  /**
     divides each entry in a CompMap by a value labeled sum. it is assumed
     that sum is the total of all values in the CompMap.
     @param comp the composition to normalize
     @param sum the value by which to normalize it
   */
  static void normalize(CompMap& comp, double sum);
  /* --- */
  
 private:
  /* --- Instance Management --- */
  /**
     a pointer to the CompMap managed by this Composition
     this is a unique pointer to a CompMap
   */
  CompMapPtr composition_;

  /**
     the overall elapsed decay time between this Composition and its parent
   */
  int decay_time_;

  /**
     the Composition's database ID, if it has one. default is 0.
   */
  int ID_;

  /**
     a pointer to the decay parent of this Composition, if one exists
     this is a shared pointer to a Composition
   */
  CompositionPtr parent_;
  
  /**
     initializes the Composition given some CompMap
     - sets the composition_ member
     - calls normalize()
     - sets ID_ and decay_time_ to 0
     @param comp the CompMap copied into the composition_ data member
   */
  void init(CompMap& comp);

  /**
     sets parent_ to a shared pointer to p
     @param p a pointer to this Composition's parent
   */
  void setParent(Composition* p);

  /**
     sets parent_ to some other parent, p
     @param p a predefined composition pointer to p
   */
  void setParent(CompositionPtr p);

  /**
     sets decay_time_ to a value
     @param time the time to set it to
   */
  void setDecayTime(int time);
  /* --- */
  
 public:
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
  
  /**
     calls validateEntry() on each entry in comp
     @ comp the CompMapPtr to validate
  */
  static void validateComposition(const CompMapPtr& comp);
  
  /**
     calls validateIsotopeNumber() and validateFraction() 
     @param tope - isotope number to check
     @param value - value to check
  */
  static void validateEntry(const Iso& tope, const double& value);

  /**
     Used to determine validity of isotope defnition. 
     @param tope isotope identifier 
     @exception thrown if isotope identifier is invalid 
  */
  static void validateIsotopeNumber(const Iso& tope);
  
  /**
     Used to determine validity of isotope's value
     @param tope isotope identifier 
     @exception throws an error if fraction < 0.0
  */
  static void validateValue(const double& value);  
  /* --- */
};

#endif
