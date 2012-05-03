// IsoVector.h
#if !defined(_ISOVECTOR_H)
#define _ISOVECTOR_H


#include <map>
#include <vector>
#include <string>
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
   shared pointer to another isovector
 */
class IsoVector;
typedef boost::shared_ptr<IsoVector> IsoVectorPtr;
/* -- */

#include "RecipeLogger.h"

/** 
   @class IsoVector

   @section Introduction
   The IsoVector class in Cyclus is a wrapper class for
   isotopic compositions so that they may be logged with the 
   BookKeeper.
*/
class IsoVector : public boost::enable_shared_from_this<IsoVector> {  
 public:
  /* --- Constructors and Destructors --- */
  /**
     most general constructor. constructors a composition given atom or
     mass basis.
     calls the specialized constuctor IsoVector().
     @param comp the composition to be copied into composition_
     @param atom true if the given composition is atom based
   */
  IsoVector(CompMap& comp, bool atom);

  /**
     specialized constructor, assuming comp is already mass based.
     @param comp the composition to be copied into composition_
   */
  IsoVector(CompMap& comp);

  /**
     default destructor, releases mass_fractions_ and parent_
   */
  ~IsoVector();
  /* --- */

  /* --- Operators --- */
  /**
     assignment operator
   */
  IsoVector& operator=(const IsoVector& rhs);

  /**
     adds two compositions
     uses the mix() function with a ratio of 1
   */
  IsoVector& operator+=(const IsoVector& rhs);

  /**
     subtracts two compositions
   */
  IsoVector& operator-=(const IsoVector& rhs);

  /**
     adds two compositions
     uses the mix() function with a ratio of 1
   */
  const IsoVector operator+(const IsoVector& rhs) const;

  /**
     subtracts two compositions
   */
  const IsoVector operator-(const IsoVector& rhs) const;

  /**
     the less-than operator to allowed compositions to be stored
     as keys in maps. compares IDs.
   */
  bool operator<(const IsoVector& other) const;

  /**
     compares composition_ to rhs.comp()
   */
  bool operator==(const IsoVector& rhs) const;

  /**
     calls the == operator preceded by not (!)
   */
  bool operator!=(const IsoVector& rhs) const;
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
  IsoVectorPtr parent() const;

  /**
     returns the time decayed between this IsoVector and its parent
   */
  double decay_time() const;

  /**
     returns the mass to atoms ratio
   */
  double mass_to_atoms() const;

  /**
     returns a shared pointer to this composition
   */
  IsoVectorPtr me();

  /**
     drills up the child-parent tree of comp, returning the root composition
     @return the root composition of comp
   */
  static IsoVectorPtr root_comp(IsoVectorPtr comp);

  /**
     drills up the child-parent tree of comp, returning sum of decay times to
     the root composition
     @return the total decay time between comp and its root comp
   */
  static double root_decay_time(IsoVectorPtr comp);
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
  static IsoVectorPtr decay(IsoVectorPtr comp, double time);

  /**
     mixes two IsoVectors with a given ratio of c1:c2
     @param c1 the first IsoVector
     @param c2 the second IsoVector
     @param ratio the amount of c1 compared to c2
     @return a shared pointer to the resulting composition
   */
  static IsoVectorPtr mix(const IsoVector& c1, const IsoVector& c2, double ratio);

  /**
     calls mix() on values of p_c1 and p_c2
   */
  static IsoVectorPtr mix(const IsoVectorPtr& p_c1, const IsoVectorPtr& p_c2, double ratio);

  /**
     separates one IsoVector from another at a given efficiency
     @param c1 the base IsoVector
     @param c2 the IsoVector to extract from c1
     @param efficiency the effiency of the separation
     @return a shared pointer to the resulting composition
   */
  static IsoVectorPtr separate(const IsoVector& c1, const IsoVector& c2, double efficiency);

  /**
     calls separate() on values of p_c1 and p_c2
   */
  static IsoVectorPtr separate(const IsoVectorPtr& p_c1, const IsoVectorPtr& p_c2, double efficiency);
  /* --- */
  
 private:
  /* --- Instance Management --- */
  /**
     a pointer to the CompMap managed by this IsoVector
     this is a unique pointer to a CompMap
   */
  CompMapPtr composition_;

  /**
     the ratio of the mass normalizer to atom normalizer
   */
  double mass_to_atoms_;

  /**
     the overall elapsed decay time between this IsoVector and its parent
   */
  double decay_time_;

  /**
     the IsoVector's database ID, if it has one. default is 0.
   */
  int ID_;

  /**
     a pointer to the decay parent of this IsoVector, if one exists
     this is a shared pointer to a IsoVector
   */
  IsoVectorPtr parent_;

  /**
     initializes the IsoVector given some CompMap
     - sets the composition_ member
     - calls normalize()
     - sets ID_ and decay_time_ to 0
     @param comp the CompMap copied into the composition_ data member
   */
  void init(CompMap& comp);

  /**
     resets all members to their initialized values
   */
  void reset();

  /**
     calls normalize() and validateIsoVector() on composition_.
     mass_to_atoms_ is also calculated.
   */
  void checkCompMap();
  
  /**
     loops through the a mass-based CompMap, multiplying their fraction
     by their gram/mol value.
     @param comp is a normalized CompMap of mass basis
   */
  double calculateMassAtomRatio(CompMap& comp);

  /**
     sets the composition's id, to be used for logging
   */
  void setID(int ID);

  /**
     sets parent_ to some other parent, p
     @param p a predefined composition pointer to p
   */
  void setParent(IsoVectorPtr p);

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
  static IsoVectorPtr executeDecay(IsoVectorPtr comp, double time);
  /* --- */

 public:  
  /* --- Printing  --- */ 
  /**
     pipes the result of detail() into LEV_INFO3
   */
  void print();

  /**
     calls compStrings() and pipes each string in the result into
     LEV_INFO3
   */
  static std::string detail(CompMapPtr c);

  /**
     populates a vector, each entry being a string describing an isotope and its
     % mass value
   */
  static std::vector<std::string> compStrings(CompMapPtr c);
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
  static void validateIsoVector(const CompMapPtr& comp);
  
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

  friend class RecipeLogger;
};

#endif
