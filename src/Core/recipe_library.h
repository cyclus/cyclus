// recipe_library.h
#if !defined(_RECIPELIBRARY_H)
#define _RECIPELIBRARY_H

#include <map>
#include <set>

#include "composition.h"
#include "query_engine.h"

#define RL RecipeLibrary::Instance()

namespace cyclus {

/**
   map of recipe name to composition
 */
typedef std::map<std::string, Composition::Ptr> RecipeMap;

/**
   The RecipeLibrary manages the list of recipes held in memory
   during a simulation. It works in conjunction with the CompMap
   class to efficiently manage isotopic-related memory.
 */
class RecipeLibrary {
 public:
  /**
     Gives all simulation objects global access to the RecipeLibrary by
     returning a pointer to it.
     Like the Highlander, there can be only one.

     @return a pointer to the RecipeLibrary
   */
  static RecipeLibrary* Instance();

  /**
     loads the recipes from the input file
   */
  void LoadRecipes(QueryEngine* qe);

  /**
     loads a specific recipe
   */
  void LoadRecipe(QueryEngine* qe);

  /**
     records a new recipe in the simulation
     - records the recipe in the BookKeeper

     @param recipe the recipe to be recorded, a CompMapPtr
   */
  void AddRecipe(std::string name, Composition::Ptr c);

  /**
     This returns a CompMapPtr to the named recipe in the recipes_ map

     @param name the name of the parent recipe, a key in the recipes_ map
   */
  Composition::Ptr GetRecipe(std::string name);

 private:
  RecipeLibrary();

  /// A pointer to this RecipeLibrary once it has been initialized.
  static RecipeLibrary* instance_;

  RecipeMap recipes_;
};

} // namespace cyclus

#endif

