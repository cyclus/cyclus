// RecipeLogger.h
#if !defined(_RECIPELOGGER_H)
#define _RECIPELOGGER_H

#include "IsotopicDefinitions.h" // defines comp_t, comp_p
#include "Table.h"
#include "InputXML.h"

#include <set>
#include <map>

#define RL RecipeLogger::Instance()

/**
   map of recipe name to composition
 */
typedef std::map<std::string,comp_p> RecipeMap; 

/**
   set of decay times
 */
typedef std::set<int> decay_times;

/**
   map of composition times decayed
 */
typedef std::map<comp_p,decay_times> DecayTimesMap; 

/**
   map of decay time to composition
 */
typedef std::map<int,comp_p> DaughterMap; 

/**
   map of recipe composition to its decayed daughters
 */
typedef std::map<comp_p,DaughterMap> DecayChainMap; 

/**
   The RecipeLogger manages the list of recipes held in memory
   during a simulation. It works in conjunction with the IsoVector
   class to efficiently manage isotopic-related memory.
 */
class RecipeLogger {
  /* --- Singleton Members and Methods --- */
 public:
  /**
     Gives all simulation objects global access to the RecipeLogger by 
     returning a pointer to it. 
     Like the Highlander, there can be only one. 
      
     @return a pointer to the RecipeLogger 
   */
  static RecipeLogger* Instance();

 protected:
  /**
     The (protected) constructor for this class, which can only be 
     called indirectly by the client. 
   */
  RecipeLogger();

 private:
  /**
     A pointer to this RecipeLogger once it has been initialized. 
   */
  static RecipeLogger* instance_;
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
  static void logRecipe(comp_p recipe);

  /**
     logs a new named recipe with the simulation
     - adds recipe to IsoVector's static containers
     - calls the logRecipe() method
   */
  static void logRecipe(std::string name, comp_p recipe);

  /**
     logs a new named recipe with the simulation
     - sets the parent of and decay time child
     - calls the logRecipe() method
     @param t_f -> total time decayed from parent to child
   */
  static void logRecipeDecay(comp_p parent, comp_p child, int t_f);
  
  /**
     checks if the recipe has been logged (i.e. it exists in the simulation)
   */
  static bool recipeLogged(std::string name);

  /**
     the total number of recipes 
   */
  static int recipeCount();   

  /**
     print all recipes 
   */
  static void printRecipes();

  /**
     checks if the composition is logged
   */
  static bool compositionDecayable(comp_p comp);

  /**
     checks if the parent has already been decayed by this time
   */
  static bool daughterLogged(comp_p parent, int time);

  /**
     accessing a specific daughter 
   */
  static comp_p Daughter(comp_p parent, int time);

 private:
  /**
     adds recipe to containers tracking decayed recipes
   */
  static void storeDecayableRecipe(comp_p recipe);

  /**
     accessing a recipe 
   */
  static comp_p Recipe(std::string name);

  /**
     accessing a set of decay times 
   */
  static decay_times& decayTimes(comp_p parent);

  /**
     accessing the daughters of a parent
   */
  static DaughterMap& Daughters(comp_p parent);

  /**
     add a daughter to a map of daughters
   */
  static void addDaughter(comp_p parent, comp_p child, int time);

  /**
     calls recipeLogged() and throws an error if false
   */
  static void checkRecipe(std::string name);

  /**
     calls compositionDecayable() and throws an error if false
   */
  static void checkDecayable(comp_p parent);

  /**
     calls daughterLogged() and throws an error if false
   */
  static void checkDaughter(comp_p parent, int time);

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
  static void addToTable(comp_t& comp);

  /* /\** */
  /*    Store information about the transactions's primary key  */
  /*  *\/ */
  /* primary_key_ref pkref_; */
 /* -- */ 
};

#endif

