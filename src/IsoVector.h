// IsoVector.h
#if !defined(_ISOVECTOR_H)
#define _ISOVECTOR_H

#include <map>
#include <utility>
#include <math.h>
#include <vector>
#include <string>
#include <libxml/tree.h>

#include "UseMatrixLib.h"

/*!
 we will always need Avogadro's number somewhere
 */
#define AVOGADRO 6.02e23

/*!
 we should define this numerical threshold as a simulation parameter
 its units are kg.
 */
#define EPS_KG 1e-6

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

/*!
 map isotope (int) to atoms/mass (double)
 */
typedef std::map<int, double> CompMap;

/*! 
 Class Material the object used to transact material objects around the system.
 
 This class keeps track of the isotopic composition of a material using both
 the atomic fractions and the mass fractions, combined with the total number of
 atoms and total mass.
 
 It is an important goal (requirement) that all material objects maintain an
 account of the atoms that is consistent with the account of the mass.
 
 The default mass unit is kilograms.
 */
class IsoVector {

public:
  
  IsoVector(); 

  IsoVector(CompMap initial_comp); 

  /*!
  Used for reading in and initizliaing material recipes.
  */
  IsoVector(xmlNodePtr cur);

  ~IsoVector() {};

  /*!
   loads the recipes from the input file
   */
  static void load_recipes();

  /**
   * get a pointer to the recipe based on its name
   *
   * @param name the name of the recipe for which to return a material pointer.
   */
  IsoVector* recipe(std::string name);                      

  static void printRecipes();

  void print();

  static int recipeCount();                                  

  static std::map<std::string, IsoVector*> recipes_;
  
  IsoVector operator+ (IsoVector rhs_vector);

  IsoVector operator- (IsoVector rhs_vector);

  /*!
   Returns the atomic number of the isotope with the given identifier.
   
   @param tope the isotope whose atomic number is being returned
   @return the atomic number
   */
  static int getAtomicNum(int tope);

  /*!
   Returns the mass number of the isotope with the given identifier.
   
   @param tope the isotope whose mass number is being returned
   @return the mass number
   */
  static int getMassNum(int tope);

  /*!
   Reads the decay information found in the 'decayInfo.dat' file into the
   parent and daughters maps.Uses these maps to create the decay matrix.
   */
  static void loadDecayInfo();

  /*!
   get material ID
   
   @return ID
   */
  int id() {return ID_;};

  /*!
   returns the total mass of this material object PER UNIT
   */
  double mass();

  /*!
   Returns the current mass of the given isotope, or zero if 
   that isotope isn't present.
   
   @param tope the isotope whose mass in the material will be returned
   @return the mass of the given isotope within the material, or zero
   */
  double mass(int tope);

  /*!
   Sets the mass of the entire IsoVector
   */
  void setMass(double new_mass);

  /*!
   returns the total atoms in this material object 
   */
  double atomCount() {return total_atoms_;};

  /*!
   Returns the current number density of the given isotope, or zero if 
   that isotope isn't present.
   
   @param tope the isotope whose number density will be returned
   @return the number density of the given isotope, or zero
   */
  double atomCount(int tope);

  /*!
   Returns the mass of the given element in this Material.
   
   @param elt the element
   @return the mass of the element (in tons)
   */
  double eltMass(int elt);

  /*!
   Decays this Material object for the given change in time and updates its
   composition map with the new number densities.
   
   @param time_change the number of months to decay
   */
  void executeDecay(double time_change);

  CompMap comp() {return atom_comp_;}
  
private:
  /*!
   Builds the decay matrix needed for the decay calculations from the parent
   and daughters map variables.  The resulting matrix is stored in the static
   variable decayMatrix.
   */
  static void buildDecayMatrix();

  /*!
   Stores the next available material ID
   */
  static int nextID_;

  /*!
   
   */
  static ParentMap parent_; 
  
  /*!
   
   */
  static DaughtersMap daughters_; 
  
  /*!
   
   */
  static Matrix decayMatrix_; 

  /*!
   Returns a mathematical Vector representation of the Material's current
   composition map.
   
   @return the mathematical Vector 
   */
  Vector compositionAsVector();

  /*!
   Overwrites composition with data from the given Vector.
   
   @param compVector Vector of data that constitutes the new composition
   
   */
  void copyVectorIntoComp(const Vector & compVector);

  void validateComposition();

  /*!
   @brief Used to determine validity of isotope defnition.
   @param tope isotope identifier
   @exception thrown if isotope identifier is invalid
   */
  static void validateAtomicNumber(int tope);

  /*!
   Returns true if the given isotope's number density is less than the 
   conservation of mass tolerance.
   
   @param tope the isotope in question
   @return true iff nd(tope) == 0
   */
  bool isZero(int tope);

  /*!
   Normalizes the composition vector it is provided.
   
   @return composition normalized to 1 mole total
   */
  CompMap normalized();

  /*! 
   Unique identifier.
   */
  int ID_;

  /*!
   total number of atoms in this material object PER UNIT
   */
  double total_atoms_;

  /*
   Core isotope composition information stored here.
   */
  CompMap atom_comp_;

};

#endif
