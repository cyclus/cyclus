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

// Useful Definitions
#define AVOGADRO 6.02e23 // avagadro's number
#define EPS_KG 1e-6 // smallest kilogram value

// Useful Typedefs
// Isotope integer, ZZZAAA
typedef int Iso; 
// a map of isotopes to composition values 
typedef std::map<Iso, double> CompMap; 
// a pointer to an isovector
class IsoVector;
typedef IsoVector* pIsoVector;

/** 
   @class IsoVector 
    
   The IsoVector Class is an intelligent container, holding a mass-
   based isotopic composition container. It reports normalized 
   mass or atomic fractions, as requested.
  
   @section recipes Recipes 
   Recipes are pre-defined IsoVectors. Recipes will be used and decay
   frequently, so their evolution is tracked. Original recipes are
   the parent of their series of daughters arising from decay. All
   IsoVectors using recipes from this process point to this class'
   static container of recipes. A copy is only made if the composition 
   deviates from the parent-daughter chain through a + or - operation.

   Input file recipes can be either mass or atom (mole) based. 
   Recipes can be accessed through the constructor. For example:  
   @code 
    
   std::string recipe_name = "my-recipe-name"; 
   IsoVector* my_recipe = new IsoVector(recipe_name); 
   
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
     constructor given some initial composition 
     @param initial_comp the initial composition 
   */
  IsoVector(CompMap initial_comp); 

  /**
     constructor given some initial composition and some
     basis
     @param initial_comp the initial composition 
     @param atom true if atom-basis, false if mass basis
   */
  IsoVector(CompMap initial_comp, bool atom); 

  /**
     Used for reading in and initizliaing material recipes. 
   */
  IsoVector(xmlNodePtr cur);

  /**
     constructor given some recipe 
     @param recipe a recipe loaded in the simulation
   */
  IsoVector(std::string recipe); 

  /**
     default destructor 
   */
  ~IsoVector() {};
  /* --- */

  /* --- Pre-Defined Recipe Interaction  --- */
  /**
     loads the recipes from the input file 
   */
  static void load_recipes();

  /**
     the total number of recipes 
   */
  static int recipeCount();        

  /**
     a container of recipes 
   */
  static std::map<std::string, IsoVector*> recipes_;
  /* --- */

  /* --- Operators  --- */
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
  /* --- */

  /* --- Instance Interaction  --- */ 
  /**
     Return the mass-based composition 
   */
  CompMap mass_comp();
  
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
  /* --- */

 private:
  /* --- Instance Interaction  --- */ 
  /**
     Stores the next available state ID 
   */
  static int nextStateID_;

  /**
     Unique identifier. 
   */
  int stateID_;

  /**
     Core isotope composition information stored here. 
   */
  CompMap mass_fractions_;

  /**
     mass-based normalization constant
  */
  double mass_normalizer_;

  /**
     atom-based normalization constant
  */
  double atom_normalizer_;

  /**
     a pointer to this iso vector's parent's pointer
   */
  pIsoVector* parent_;

  /**
     total decay time between this isovector and its parent
  */
  int decayTime_;
  /* --- */

  /* --- Isotope Wikipedia  --- */
  void validateComposition();

  /**
     Used to determine validity of isotope defnition. 
     @param tope isotope identifier 
     @exception thrown if isotope identifier is invalid 
   */
  static void validateIsotopeNumber(Iso tope);
  /* --- */
 
  /* --- Printing Functionality  --- */
  /**
     used by print() to 'hide' print code when logging is not desired 
   */
  std::string detail();
  /* --- */
 

 /* -- Output Database Interaction  -- */ 

 public:
  /**
     the isotopics output database Table 
   */
  static table_ptr iso_table;
  
  /**
     return the agent table's primary key 
   */
  primary_key_ref pkref() { return pkref_;}
  
  /**
     the current state id 
   */
  int stateID() {return stateID_;}

  /**
     returns true if a new state was recorded, false if already in db
  */
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

 /* -- */ 
};

#endif
