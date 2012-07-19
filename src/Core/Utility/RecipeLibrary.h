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
typedef std::map<double,CompMapPtr> ChildMap; 

/**
   map of recipe composition to its decayed children
 */
typedef std::map<CompMapPtr,ChildMap> DecayHistMap; 

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

  /* --- Recipe Record --- */
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
     records a new recipe in the simulation
     - records the recipe in the BookKeeper

     @param recipe the recipe to be recorded, a CompMapPtr
   */
  static void recordRecipe(CompMapPtr recipe);

  /**
     records a new named recipe in the simulation
     - adds recipe to CompMap's static containers
     - calls the recordRecipe(recipe) method

     @param name the recipe name to be used as a key in the recipes_ map, a string
     @param recipe the recipe to be recorded, a CompMapPtr
   */
  static void recordRecipe(std::string name, CompMapPtr recipe);

  /**
     records a new named recipe with the simulation
     - sets the parent of and decay time child
     - calls the recordRecipe() method

     @param parent the initial recipe in a recipe map 
     @param t_f -> total time decayed from parent to child
   */
  static void recordRecipeDecay(CompMapPtr parent, CompMapPtr child, double t_f);
  
  /**
     checks if the named recipe has been recorded (i.e. it exists in the simulation)

     @param name the name of the parent recipe, a key in the recipes_ map
   */
  static bool recipeRecorded(std::string name);

  /**
     the total number of recipes in the recipes_ map  
   */
  static int recipeCount();   

  /**
     This returns a CompMapPtr to the named recipe in the recipes_ map 

     @param name the name of the parent recipe, a key in the recipes_ map
   */
  static CompMapPtr Recipe(std::string name);

  /**
     print all recipes 
   */
  static void printRecipes();

  /**
     checks if the composition is recorded. Returns true if the composition 
     appears in the decay_hist_ map. 

     @param comp a pointer to the composition to check
   */
  static bool compositionDecayable(CompMapPtr comp);

  /**
     checks if the parent has already been decayed by this time

     @param parent a pointer to the composition that might have a child
     @param time the time at which a child might exist for a parent
   */
  static bool childRecorded(CompMapPtr parent, double time);

 private:
  /**
     adds recipe to containers tracking decayed recipes

     @param recipe the recipe to store in the decay_hist_ and decay_times_ maps
   */
  static void storeDecayableRecipe(CompMapPtr recipe);

  /**
     add a new decay time for a parent composition

     @param parent the recipe with which a new decay_time should be associated
     @param time the time to be added to the keys of the decay_times_ map 
   */
  static void addDecayTime(CompMapPtr parent, double time);

  /**
     accesses a set of decay times that have been recorded (associated with this parent)

     @param parent the recipe whose decay times are to be retrieved
   */
  static decay_times& decayTimes(CompMapPtr parent);

  /**
     accesses the child recipes of a parent recipe

     @param parent the recipe whose composition evolution is to be retrieved
   */
  static ChildMap& Children(CompMapPtr parent);

  /**
     accesses a specific child recipe 

     @param parent the recipe whose composition evolution is to be retrieved
     @param time the time at which the child recipe of interest is indexed 
   */
  static CompMapPtr& Child(CompMapPtr parent, double time);

  /**
     add a child to a map of children

     @param parent the recipe with which a new child should be associated
     @param child the child recipe to be added to the evolution of the parent recipe 
     @param time the time at which the child recipe of interest is indexed 
   */
  static void addChild(CompMapPtr parent, CompMapPtr child, double time);

  /**
     calls recipeRecorded() and throws an error if false

     @param name the name of the recipe to be found
   */
  static void checkRecipe(std::string name);

  /**
     calls compositionDecayable() and throws an error if false

     @param parent the recipe whose composition record is to be queried
   */
  static void checkDecayable(CompMapPtr parent);

  /**
     calls childRecorded() and throws an error if false

     @param parent the recipe whose composition record is to be queried
     @param time the time at which the child recipe of interest is indexed 
   */
  static void checkChild(CompMapPtr parent, double time);

  /**
     Stores the next available state ID 
   */
  static int nextStateID_;

  /**
     a container of recipes 
   */
  static RecipeMap recipes_;

  /**
     a container of recipes for each parent recipe, representing its decay history
   */
  static DecayHistMap decay_hist_;

  /**
     a container of decay times that recipes have gone through, the time keys in the ChildMap
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

