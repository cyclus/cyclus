// Composition.h
#if !defined(_COMPOSITION_H)
#define _COMPOSITION_H

#include <map>
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
typedef std::map<Iso,double> CompMap;
typedef boost::shared_ptr<CompMap> CompMapPtr;

/**
   shared pointer to another composition
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
class Composition : public boost::enable_shared_from_this<Composition> {
 public:
  /* --- Constructors and Destructors --- */
  /**
     most general constructor. constructors a composition given atom or
     mass basis.
     calls the specialized constuctor Composition().
     @param comp the composition to be copied into composition_
     @param atom true if the given composition is atom based
   */
  Composition(CompMap& comp, bool atom);

  /**
     specialized constructor, assuming comp is already mass based.
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
     return the composition map
   */
  CompMapPtr comp() const;

  /**
     returns true if the composition's id has been set
   */
  bool logged() const;

  /**
     returns the composition's id
   */
  int ID() const;

  /**
     Return the mass fraction of an isotope in the composition
   */
  double massFraction(const Iso& tope) const;

  /**
     returns the atom fraction of an isotope in the composition
   */
  double atomFraction(const Iso& tope) const;

  /**
     returns a shared pointer to this composition's parent
   */
  CompositionPtr parent() const;

  /**
     returns the time decayed between this Composition and its parent
   */
  double decay_time() const;

  /**
     returns a shared pointer to this composition
   */
  CompositionPtr me();

  /**
     drills up the child-parent tree of comp, returning the root composition
     @return the root composition of comp
   */
  static CompositionPtr root_comp(CompositionPtr comp);

  /**
     drills up the child-parent tree of comp, returning sum of decay times to
     the root composition
     @return the total decay time between comp and its root comp
   */
  static double root_decay_time(CompositionPtr comp);
  /* --- */

  /* --- Transformations --- */
  /**
     alters comp, multiplying each entry by its molar weight (g/mol)
     @param comp the initial atom-based composition
   */
  static void massify(CompMap& comp);

  /**
     alters comp, deviding each entry by its molar weight (g/mol)
     @param comp the initial mass-based composition
   */
  static void atomify(CompMap& comp);

  /**
     alters comp, summing the total values and calling the normalize()
     method
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

  /**
     decays a composition for a given time, assumed to be in months
     
     this public function checks with the RecipeLogger to see if comp is
     logged as a decayable parent. if so, it will intelligently decay comp
     if a daughter has not already been decayed. if one has, a copy will be
     returned.
     @param comp the composition to be decayed
     @param time the decay time, in months
     @return a pointer to the result of this decay
   */
  static CompositionPtr decay(CompositionPtr comp, double time);

  /**
     mixes two Compositions with a given ratio of c1:c2
     @param c1 the first Composition
     @param c2 the second Composition
     @param ratio the amount of c1 compared to c2
     @return a shared pointer to the resulting composition
   */
  static CompositionPtr mix(const Composition& c1, const Composition& c2, double ratio);

  /**
     calls mix() on values of p_c1 and p_c2
   */
  static CompositionPtr mix(const CompositionPtr& p_c1, const CompositionPtr& p_c2, double ratio);

  /**
     separates one Composition from another at a given efficiency
     @param c1 the base Composition
     @param c2 the Composition to extract from c1
     @param efficiency the effiency of the separation
     @return a shared pointer to the resulting composition
   */
  static CompositionPtr separate(const Composition& c1, const Composition& c2, double efficiency);

  /**
     calls separate() on values of p_c1 and p_c2
   */
  static CompositionPtr separate(const CompositionPtr& p_c1, const CompositionPtr& p_c2, double efficiency);
  /* --- */
  
 private:
  /* --- Instance Management --- */
  /**
     a pointer to the CompMap managed by this Composition
     this is a unique pointer to a CompMap
   */
  CompMapPtr composition_;

  /**
     the ratio of the mass normalizer to atom normalizer
   */
  double mass_to_atoms_;

  /**
     the overall elapsed decay time between this Composition and its parent
   */
  double decay_time_;

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
     loops through the a mass-based CompMap, multiplying their fraction
     by their gram/mol value.
     @param comp is a normalized CompMap of mass basis
   */
  double calculateMassAtomRatio(CompMap& comp);

  /**
     sets parent_ to some other parent, p
     @param p a predefined composition pointer to p
   */
  void setParent(CompositionPtr p);

  /**
     sets decay_time_ to a value
     @param time the time to set it to
   */
  void setDecayTime(double time);

  /**
     this private function uses the DecayHandler to decay a composition
     by a given time
     @param comp the composition to be decayed
     @param time the decay time, in months
     @return a pointer to the result of this decay
   */
  static CompositionPtr executeDecay(CompositionPtr comp, double time);
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
