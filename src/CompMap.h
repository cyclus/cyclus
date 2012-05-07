// CompMap.h
#if !defined(_COMPMAP_H)
#define _COMPMAP_H

/* -- Includes -- */
#include <map>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
/* -- */

/* -- Typedefs -- */
/**
   Isotope integer, ZZZAAA
 */
typedef int Iso;

/**
   map of isotope integer to value (mass or atom)
 */
typedef std::map<Iso,double> Map;  

/**
   a shared pointer to this class
 */
class CompMap;
typedef boost::shared_ptr<CompMap> CompMapPtr;
/* -- */

/* -- Enums -- */
/**
   Defines possible basis for this composition
 */
enum Basis {MASS, ATOM};
/* -- */

/* -- Sensitive Includes -- */
#include "IsoVector.h"
#include "RecipeLogger.h"
/* -- */

/** 
   @class CompMap

   @section Introduction
   The CompMap class provides an intelligent wrapper for the
   standard Cyclus Isotopic Composition map, a map of integer 
   values (ZZZAAA) to double values.

   This class forwards the relevant function calls to the actual
   map held as a member variable. Specifically, the array index
   operator, the begin iterator, and the end iterator can all be
   called by a CompMap.

   The CompMap also stores details about a Composition's lineage,
   specifically whether or not a CompMap is the child of another
   via decay, and if so how long the decay time between the two
   is.

   @section Internal Storage
   
*/
class CompMap : public boost::enable_shared_from_this<CompMap> {  
  /**
     masking Map
  */
  typedef Map::iterator iterator;
  typedef Map::const_iterator const_iterator;

 public:
  /* --- Constructors and Destructors --- */
  /**
     default constructor
   */
  CompMap(Basis b);

  /**
     copy constructor
   */
  CompMap(const CompMap& other);

  /**
     default destructor
   */
  ~CompMap();
  /* --- */
  
  /* --- Instance Interaction --- */    
  /**
     beginning iterator
   */
  CompMap::iterator begin();

  /**
     ending iterator
   */
  CompMap::iterator end();

  /**
     accesses the subscript operator of the map
   */
  double& operator[](int tope);

  /**
     the less-than operator to allowed compositions to be stored
     as keys in maps. compares IDs.
   */
  bool operator<(const CompMap& other) const;

  /**
     returns number of topes in map
   */
  int count(Iso tope);

  /**
     erases tope from map
   */
  void erase(Iso tope);

  /**
     returns true if the composition's id has been set
   */
  bool logged() const;

  /**
     returns the current basis
   */
  Basis basis() const;

  /**
     if b is different than basis_, map_ is converted to the new basis
   */
  void set_basis(Basis b);

  /**
     returns the current map
   */
  Map map() const;

  /**
     returns the mass to atoms ratio
   */
  double mass_to_atom_ratio() const;

  /**
     returns true if the composition has been normalized
   */
  bool normalized() const;

  /**
     Return the mass fraction of an isotope in the composition
   */
  double massFraction(Iso tope);

  /**
     returns the atom fraction of an isotope in the composition
   */
  double atomFraction(Iso tope);

  /**
     returns the composition's id
   */
  int ID() const;

  /**
     returns a shared pointer to this composition's parent
   */
  CompMapPtr parent() const;

  /**
     returns the time decayed between this CompMap and its parent
   */
  double decay_time() const;

  /**
     returns a shared pointer to this composition
   */
  CompMapPtr me();

  /**
     drills up the child-parent tree of comp, returning the root composition
     @return the root composition of this CompMap
   */
  CompMapPtr root_comp();

  /**
     drills up the child-parent tree of comp, returning sum of decay times to
     the root composition
     @return the total decay time between this CompMap and its root comp
   */
  double root_decay_time();
  
  /**
     calls validateEntry() on each entry in the map
  */
  void validate();
  /* --- */

  /* --- Transformations --- */
  /**
     alters comp, multiplying each entry by its molar weight (g/mol)
  */
  void massify();
  
  /**
     alters comp, deviding each entry by its molar weight (g/mol)
  */
  void atomify();
  
  /**
     alters comp, summing the total values and calling the normalize(sum)
     method 
  */
  void normalize();
  /* --- */

  /* --- Printing  --- */ 
  /**
     pipes the result of detail() into LEV_INFO3
   */
  void print();

  /**
     calls compStrings() and pipes each string in the result into
     LEV_INFO3
   */
  std::string detail();
  /* --- */

 private:  
  /* --- Instance Management --- */  
  /**
     the actual map of isotopes to valuse
   */
  Map map_;

  /**
     the CompMap's current basis
   */
  Basis basis_;

  /**
     true if the composition is normalized
   */
  bool normalized_;

  /**
     the ratio of the mass normalizer to atom normalizer
   */
  double mass_to_atom_ratio_;

  /**
     the CompMap's database ID, if it has one. default is 0.
   */
  int ID_;

  /**
     the overall elapsed decay time between this CompMap and its parent
   */
  double decay_time_;

  /**
     a shared pointer to the decay parent of this CompMap, if one exists
   */
  CompMapPtr parent_;

  /**
     initializes all relevant members
  */
  void init(Basis b);

  /**
   */
  void change_basis(Basis b);

  /**
     divides each entry in the map by a value labeled sum. it is assumed
     that sum is the total of all values in the map
     @param sum the value by which to normalize it
  */
  void normalize(double sum);

  /**
     populates a vector, each entry being a string describing an isotope and its
     % mass value
   */
  std::vector<std::string> compStrings();
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

  /* -- Friends -- */
  friend class IsoVector;
  friend class RecipeLogger;
  /* -- */
};

#endif
