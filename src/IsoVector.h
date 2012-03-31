// IsoVector.h
#if !defined(_ISOVECTOR_H)
#define _ISOVECTOR_H

#include <map>
#include <utility>
#include <math.h>
#include <vector>
#include <string>
#include <libxml/tree.h>

#include "Table.h"
#include "UseMatrixLib.h"

#define AVOGADRO 6.02e23

/**
   simulation wide numerical threshold for mass significance. 
   Its units are kg. 
 */
#define EPS_KG 1e-6

/**
   An isotope's identification number 
   The isotope ZAID number (Z A IDentiﬁcation) contains six digits 
   ZZZAAA ZZZ is the atomic number Z and AAA is the atomic mass number 
   A. Thus  235 U has a ZAID number 092235 or simply 92235. 
 */
typedef int Iso;

/**
   A map type to represent all of the parent isotopes tracked.  The key 
   for this map type is the parent's Iso number, and the value is a pair 
   that contains the corresponding decay matrix column and decay 
   constant associated with that parent. 
 */
typedef std::map< Iso, std::pair<int, double> > ParentMap;

/**
   A map type to represent all of the daughter isotopes tracked.  The 
   key for this map type is the decay matrix column associated with the 
   parent, and the value is a vector of pairs of all the daughters for 
   that parent. Each of the daughters are represented by a pair that 
   contains the daughter's Iso number and its branching ratio. 
 */
typedef std::map<int, std::vector<std::pair<Iso, double> > > DaughtersMap;

/**
   map isotope (int) to atoms/mass (double) 
 */
typedef std::map<Iso, double> CompMap;

/**
   map Composition Map Pointer (CompMap*) to state (int) 
 */
typedef std::map<CompMap*, int> StateMap;

/** 
   @class IsoVector 
    
    
   This class is the object used to transact material objects around the 
   system.  
   @section intro Introduction 
   This class keeps track of the isotopic composition of a material 
   using both the atomic fractions and the mass fractions, combined with 
   the total number of atoms and total mass. The default mass unit is 
   kilograms. Access to nuclide data is also provided through the 
   IsoVector class.  
   @section recipes Recipes 
   Input file recipes can be either mass or atom (mole) based and define 
   an often-referenced composition vector. Recipes can be retrieved at 
   will by any agent during the simulation via the static 
   `IsoVector::recipe(std::string name)` method.  For example:  
   @code 
    
   std::string recipe_name = "my-recipe-name"; 
   IsoVector my_recipe = IsoVector::recipe(recipe_name); 
    
   @endcode 
 */

class IsoVector {

public:
  /**
     default constructor 
   */
  IsoVector(); 

  /**
     constructor given some initial composition 
     @param initial_comp the initial composition 
   */
  IsoVector(CompMap initial_comp); 

  /**
     Used for reading in and initizliaing material recipes. 
   */
  IsoVector(xmlNodePtr cur);

  /**
     default destructor 
   */
  ~IsoVector() {};

  /**
     loads the recipes from the input file 
   */
  static void load_recipes();

  /**
     get a pointer to the recipe based on its name 
      
     @param name the name of the recipe for which to return a material 
   */
  static IsoVector recipe(std::string name);                      

  /**
     print all recipes 
   */
  static void printRecipes();

  /**
     print the details of this IsoVector 
   */
  void print();

  /**
     return a vector of the composition as strings 
     @return the composition string vector 
   */
  std::vector<std::string> compStrings();

  /**
     the total number of recipes 
   */
  static int recipeCount();        

  /**
     a container of recipes 
   */
  static std::map<std::string, IsoVector*> recipes_;
  
  /**
     Adds like isotopes 
   */
  IsoVector operator+ (IsoVector rhs_vector);

  /**
     Subtracts like isotopes 
      
     @exception CycRangeException thrown if subtraction results in a 
     negative quantity for any isotope. 
   */
  IsoVector operator- (IsoVector rhs_vector);

  /**
     Compares quantities of like isotopes returning true only if 
     quantity for every isotope is equal. 
   */
  bool operator== (IsoVector rhs_vector);

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
     get material ID 
      
     @return ID 
   */
  int id() {return ID_;};

  /**
     returns the total mass of this material object PER UNIT 
   */
  double mass();

  /**
     Returns the current mass of the given isotope, or zero if 
     that isotope isn't present. 
      
     @param tope the isotope whose mass in the material will be returned 
     @return the mass of the given isotope within the material, or zero 
   */
  double mass(Iso tope);

  /**
     Sets the total mass of the entire IsoVector maintaining isotopic 
   */
  void setMass(double new_mass);

  /**
     Sets the mass of the specified isotope. 
   */
  void setMass(Iso tope, double new_mass);

  /**
     Multiplies the total mass of the entire IsoVector by 'factor' 
     maintaining isotopic ratios. 
   */
  void multBy(double factor);

  /**
     returns the total atoms in this material object 
   */
  double atomCount();

  /**
     Returns the current number density of the given isotope, or zero if 
     that isotope isn't present. 
      
     @param tope the isotope whose number density will be returned 
     @return the number density of the given isotope, or zero 
   */
  double atomCount(Iso tope);

  /**
     Sets the total number of atoms for the entire IsoVector maintaining 
     isotopic ratios. 
   */
  void setAtomCount(double new_count);

  /**
     Sets the total number of atoms for the entire IsoVector maintaining 
     isotopic ratios. 
   */
  void setAtomCount(Iso tope, double new_count);

  /**
     Returns the mass of the given element in this Material. 
      
     @param elt the element 
     @return the mass of the element (in tons) 
   */
  double eltMass(int elt);

  /**
     Decays this Material object for the given change in time and 
     updates its composition map with the new number densities. 
      
     @param time_change the number of months to decay 
   */
  void executeDecay(double time_change);

  /**
     Return the atomic (in moles) composition as a std::map<std::string, 
   */
  CompMap comp();
  
  /**
     Returns true if the given isotope's number density is less than the 
     conservation of mass tolerance. 
      
     @param tope the isotope in question 
     @return true iff nd(tope) == 0 
   */
  bool isZero(Iso tope);

protected:
  /**
     Builds the decay matrix needed for the decay calculations from the 
     parent and daughters map variables.  The resulting matrix is stored 
     in the static variable decayMatrix. 
   */
  static void buildDecayMatrix();

  /**
     Returns a mathematical Vector representation of the Material's 
     current composition map. 
      
     @return the mathematical Vector 
   */
  Vector compositionAsVector();

  /**
     Overwrites composition with data from the given Vector. 
      
     @param compVector Vector of data that constitutes the new 
   */
  void copyVectorIntoComp(const Vector & compVector);

  /**
     the IsoVector Class' parent 
   */
  static ParentMap parent_; 
  
  /**
     the IsoVector Class' daughters 
   */
  static DaughtersMap daughters_; 
  
  /**
     the matrix used for decay functionality 
   */
  static Matrix decayMatrix_; 

private:
  /**
     allows calculated mass to be reused if no changes 
     have been made since last calculated. 
   */
  double total_mass_;

  /**
     aids in mass recalc prevention - used in conjunction with 
   */
  bool mass_out_of_date_;

  /**
     used by print() to 'hide' print code when logging is not desired 
   */
  std::string detail();

  /**
     Stores the next available material ID 
   */
  static int nextID_;

  void validateComposition();

  /**
     Used to determine validity of isotope defnition. 
     @param tope isotope identifier 
     @exception thrown if isotope identifier is invalid 
   */
  static void validateIsotopeNumber(Iso tope);

  /**
     Unique identifier. 
   */
  int ID_;

  /**
     Core isotope composition information stored here. 
   */
  CompMap atom_comp_;
    
// -------- output database related members  -------- 
 public:
  /**
     the isotopics output database Table 
   */
  static table_ptr iso_table;
  
  /**
     return the agent table's primary key 
   */
  primary_key_ref pkref(){ return pkref_;}
  
  /**
     the current state id 
   */
  int stateID(){return stateID_;}

  void recordState();
  
 private:
  /**
     Define the database table on the first Message's init 
   */
  static void define_table();

  /**
     Add an isotopic state to the table 
   */
  void addToTable();

  /**
     Store information about the transactions's primary key 
   */
  primary_key_ref pkref_;
  
  /**
     this IsoVector's state 
   */
  int stateID_;

  /**
     the IsoVector class' state incrementor 
   */
  static int nextStateID_;

  /**
     a mapping of already-defined compositions to states 
   */
  static StateMap predefinedStates_;

  /**
     returns the state id of the current composition if it is already 
   */
  int compositionIsTracked();
// -------- output database related members  -------- 
};

#endif
