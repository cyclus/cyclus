// RecipeLibrary.h
#if !defined(_RECIPELOGGER_H)
#define _RECIPELOGGER_H

#include "CompMap.h"
#include "Table.h"
#include "InputXML.h"

#include <set>
#include <map>

#define RL RecipeLibrary::Instance()

/**
   map of recipe name to composition
 */
typedef std::map<std::string,CompMapPtr> RecipeMap; 

/**
   set of decay times
 */
typedef std::set<double> decay_times;

/**
   map of composition times decayed
 */
typedef std::map<CompMapPtr,decay_times> DecayTimesMap; 

/**
   map of decay time to composition
 */
typedef std::map<double,CompMapPtr> DaughterMap; 

/**
   map of recipe composition to its decayed daughters
 */
typedef std::map<CompMapPtr,DaughterMap> DecayChainMap; 

/**
   The RecipeLibrary manages the list of recipes held in memory
   during a simulation. It works in conjunction with the CompMap
   class to efficiently manage isotopic-related memory.
 */
class RecipeLibrary {
  /* --- Singleton Members and Methods --- */
 public: 
  /**
     Gives all simulation objects global access to the RecipeLibrary by 
     returning a pointer to it. 
     Like the Highlander, there can be only one. 
      
     @return a pointer to the RecipeLibrary 
   */
  static RecipeLibrary* Instance();

 protected:
  /**
     The (protected) constructor for this class, which can only be 
     called indirectly by the client. 
   */
  RecipeLibrary();

 private:
  /**
     A pointer to this RecipeLibrary once it has been initialized. 
   */
  static RecipeLibrary* instance_;
  /* --- */

  /* --- Recipe Logging --- */
 public:
  /**
     loads the recipes from the input file 
   */
  static void load_recipes();

  /**
     loads a specific recipe 
   */
  static void load_recipe(xmlNodePtr cur);
  
  /**
     logs a new recipe with the simulation
     - logs recipe with BookKeeper
   */
  static void logRecipe(CompMapPtr recipe);

  /**
     logs a new named recipe with the simulation
     - adds recipe to CompMap's static containers
     - calls the logRecipe() method
   */
  static void logRecipe(std::string name, CompMapPtr recipe);

  /**
     logs a new named recipe with the simulation
     - sets the parent of and decay time child
     - calls the logRecipe() method
     @param t_f -> total time decayed from parent to child
   */
  static void logRecipeDecay(CompMapPtr parent, CompMapPtr child, double t_f);
  
  /**
     checks if the recipe has been logged (i.e. it exists in the simulation)
   */
  static bool recipeLogged(std::string name);

  /**
     the total number of recipes 
   */
  static int recipeCount();   

  /**
     accessing a recipe 
   */
  static CompMapPtr Recipe(std::string name);

  /**
     print all recipes 
   */
  static void printRecipes();

  /**
     checks if the composition is logged
   */
  static bool compositionDecayable(CompMapPtr comp);

  /**
     checks if the parent has already been decayed by this time
   */
  static bool daughterLogged(CompMapPtr parent, double time);

 private:
  /**
     adds recipe to containers tracking decayed recipes
   */
  static void storeDecayableRecipe(CompMapPtr recipe);

  /**
     add a new decay time for a parent composition
   */
  static void addDecayTime(CompMapPtr parent, double time);

  /**
     accessing a set of decay times 
   */
  static decay_times& decayTimes(CompMapPtr parent);

  /**
     accessing the daughters of a parent
   */
  static DaughterMap& Daughters(CompMapPtr parent);

  /**
     accessing a specific daughter 
   */
  static CompMapPtr& Daughter(CompMapPtr parent, double time);

  /**
     add a daughter to a map of daughters
   */
  static void addDaughter(CompMapPtr parent, CompMapPtr child, double time);

  /**
     calls recipeLogged() and throws an error if false
   */
  static void checkRecipe(std::string name);

  /**
     calls compositionDecayable() and throws an error if false
   */
  static void checkDecayable(CompMapPtr parent);

  /**
     calls daughterLogged() and throws an error if false
   */
  static void checkDaughter(CompMapPtr parent, double time);

  /**
     Stores the next available state ID 
   */
  static int nextStateID_;

  /**
     a container of recipes 
   */
  static RecipeMap recipes_;

  /**
     a container of recipes in each decay chain
   */
  static DecayChainMap decay_chains_;

  /**
     a container of decay times that recipes have gone through
   */
  static DecayTimesMap decay_times_;

 /* -- Output Database Interaction  -- */ 
 public:
  /**
     the isotopics output database Table 
   */
  static table_ptr iso_table;
  
  /* /\** */
  /*    return the agent table's primary key  */
  /*  *\/ */
  /* primary_key_ref pkref(); */
  
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
  static void addToTable(CompMapPtr recipe);

  /* /\** */
  /*    Store information about the transactions's primary key  */
  /*  *\/ */
  /* primary_key_ref pkref_; */
 /* -- */ 
  
  //  friend class CompMap;
  friend class IsoVector;
};

#endif

