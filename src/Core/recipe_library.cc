// recipe_library.cpp
// Implements the RecipeLibrary class

#include "recipe_library.h"

#include <map>
#include <sstream>

#include "error.h"
#include "logger.h"
#include "mass_table.h"

namespace cyclus {

// initialize singleton member
RecipeLibrary* RecipeLibrary::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RecipeLibrary* RecipeLibrary::Instance() {
  // If we haven't created a RecipeLibrary yet, create and return it.
  if (0 == instance_) {
    instance_ = new RecipeLibrary();
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RecipeLibrary::RecipeLibrary() {
  AddRecipe("blank", Composition::CreateFromAtom(Composition::Vect()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RecipeLibrary::LoadRecipes(QueryEngine* qe) {
  // load recipes from file
  int nRecipes = qe->NElementsMatchingQuery("recipe");
  CLOG(LEV_DEBUG2) << "loading recipes {";
  for (int i = 0; i < nRecipes; i++) {
    QueryEngine* recipe = qe->QueryElement("recipe", i);
    std::string name = recipe->GetElementContent("name");
    CLOG(LEV_DEBUG2) << "Adding recipe '" << name << "'.";
    LoadRecipe(recipe); // load recipe
  }

  CLOG(LEV_DEBUG2) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RecipeLibrary::LoadRecipe(QueryEngine* qe) {
  bool atomBasis;
  std::string basis_str = qe->GetElementContent("basis");
  if (basis_str == "atom") {
    atomBasis = true;
  } else if (basis_str == "mass") {
    atomBasis = false;
  } else {
    throw IOError(basis_str + " basis is not 'mass' or 'atom'.");
  }

  std::string name = qe->GetElementContent("name");
  CLOG(LEV_DEBUG3) << "loading recipe: " << name
                   << " with basis: " << basis_str;

  double value;
  int key;
  std::string query = "isotope";
  int nIsos = qe->NElementsMatchingQuery(query);
  Composition::Vect v;
  for (int i = 0; i < nIsos; i++) {
    QueryEngine* isotope = qe->QueryElement(query, i);
    key = strtol(isotope->GetElementContent("id").c_str(), NULL, 10);
    value = strtod(isotope->GetElementContent("comp").c_str(), NULL);
    v[key] = value;
    CLOG(LEV_DEBUG3) << "  Isotope: " << key << " Value: " << v[key];
  }

  if (atomBasis) {
    AddRecipe(name, Composition::CreateFromAtom(v));
  } else {
    AddRecipe(name, Composition::CreateFromMass(v));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RecipeLibrary::AddRecipe(std::string name, Composition::Ptr recipe) {
  recipes_[name] = recipe;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Composition::Ptr RecipeLibrary::GetRecipe(std::string name) {
  if (recipes_.count(name) == 0) {
    std::stringstream err;
    err << "RecipeLibrary has not recorded recipe with name: " << name << ".";
    throw KeyError(err.str());
  }
  return recipes_[name];
}

} // namespace cyclus

