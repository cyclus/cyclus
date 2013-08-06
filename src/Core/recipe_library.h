// recipe_library.h
#if !defined(_RECIPELIBRARY_H)
#define _RECIPELIBRARY_H

#include <set>
#include <map>

#include "query_engine.h"
#include "comp_map.h"

#define RL RecipeLibrary::Instance()

namespace cyclus {

/**
   map of recipe name to composition
 */
typedef std::map<std::string, CompMapPtr> RecipeMap; 

/**
   set of decay times
 */
typedef std::set<int> DecayTimes;

/**
   map of composition times decayed
 */
typedef std::map<CompMapPtr,DecayTimes> DecayTimesMap; 

/**
   map of decay time to composition
 */
typedef std::map<int,CompMapPtr> ChildMap; 

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
  static void load_recipes(QueryEngine* qe);

  /**
     loads a specific recipe 
   */
  static void load_recipe(QueryEngine* qe);
  
  /**
     records a new recipe in the simulation
     - records the recipe in the BookKeeper

     @param recipe the recipe to be recorded, a CompMapPtr
   */
  static void RecordRecipe(CompMapPtr recipe);

  /**
     records a new named recipe in the simulation
     - adds recipe to CompMap's static containers
     - calls the RecordRecipe(recipe) method

     @param name the recipe name to be used as a key in the recipes_ map, a string
     @param recipe the recipe to be recorded, a CompMapPtr
   */
  static void RecordRecipe(std::string name, CompMapPtr recipe);

  /**
     records a new named recipe with the simulation
     - sets the parent of and decay time child
     - calls the RecordRecipe() method

     @param parent the initial recipe in a recipe map 
     @param child the child recipe
     @param t_f -> total time decayed from parent to child
   */
  static void RecordRecipeDecay(CompMapPtr parent, CompMapPtr child, int t_f);
  
  /**
     checks if the named recipe has been recorded (i.e. it exists in the simulation)

     @param name the name of the parent recipe, a key in the recipes_ map
   */
  static bool RecipeRecorded(std::string name);

  /**
     the total number of recipes in the recipes_ map  
   */
  static int RecipeCount();   

  /**
     This returns a CompMapPtr to the named recipe in the recipes_ map 

     @param name the name of the parent recipe, a key in the recipes_ map
   */
  static CompMapPtr Recipe(std::string name);

  /**
     print all recipes 
   */
  static void PrintRecipes();

  /**
     checks if the composition is recorded. Returns true if the composition 
     appears in the decay_hist_ map. 

     @param comp a pointer to the composition to check
   */
  static bool CompositionDecayable(CompMapPtr comp);

  /**
     checks if the parent has already been decayed by this time

     @param parent a pointer to the composition that might have a child
     @param time the time at which a child might exist for a parent
   */
  static bool ChildRecorded(CompMapPtr parent, int time);

 private:
  /**
     adds recipe to containers tracking decayed recipes

     @param recipe the recipe to store in the decay_hist_ and decay_times_ maps
   */
  static void StoreDecayableRecipe(CompMapPtr recipe);

  /**
     add a new decay time for a parent composition

     @param parent the recipe with which a new decay_time should be associated
     @param time the time to be added to the keys of the decay_times_ map 
   */
  static void AddDecayTime(CompMapPtr parent, int time);

  /**
     accesses a set of decay times that have been recorded (associated with this parent)

     @param parent the recipe whose decay times are to be retrieved
   */
  static DecayTimes& decay_times(CompMapPtr parent);

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
  static CompMapPtr& Child(CompMapPtr parent, int time);

  /**
     add a child to a map of children

     @param parent the recipe with which a new child should be associated
     @param child the child recipe to be added to the evolution of the parent recipe 
     @param time the time at which the child recipe of interest is indexed 
   */
  static void AddChild(CompMapPtr parent, CompMapPtr child, int time);

  /**
     calls RecipeRecorded() and throws an error if false

     @param name the name of the recipe to be found
   */
  static void CheckRecipe(std::string name);

  /**
     calls CompositionDecayable() and throws an error if false

     @param parent the recipe whose composition record is to be queried
   */
  static void CheckDecayable(CompMapPtr parent);

  /**
     calls ChildRecorded() and throws an error if false

     @param parent the recipe whose composition record is to be queried
     @param time the time at which the child recipe of interest is indexed 
   */
  static void CheckChild(CompMapPtr parent, int time);

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
  
  /* /\** */
  /*    return the agent table's primary key  */
  /*  *\/ */
  /* primary_key_ref pkref(); */
  
  /**
     returns true if a new state was recorded, false if already in db
  */
  void RecordState();
  
 private:

  /**
     Add an isotopic state to the table 
   */
  static void AddToTable(CompMapPtr recipe);

  //  friend class CompMap;
  friend class IsoVector;
};
} // namespace cyclus
#endif

