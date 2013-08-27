// recipe_library.h
#if !defined(_RECIPELIBRARY_H)
#define _RECIPELIBRARY_H

#include "composition.h"
#include "query_engine.h"
#include "context.h"

namespace cyclus {

/// The RecipeLibrary loads recipes from imput files.
class RecipeLibrary {
 public:
  /// loads the recipes from the input file
  static void LoadRecipes(Context* ctx, QueryEngine* qe);

  /// loads a specific recipe
  static void LoadRecipe(Context* ctx, QueryEngine* qe);
};

} // namespace cyclus

#endif

