// recipe_library.cc
// Implements the RecipeLibrary class

#include "recipe_library.h"

#include "comp_map.h"
#include "mass_table.h"
#include "error.h"
#include "event_manager.h"

#include <map>
#include <sstream>

namespace cyclus {

// initialize singleton member
RecipeLibrary* RecipeLibrary::instance_ = 0;
// initialize recordging members
int RecipeLibrary::nextStateID_ = 0;
RecipeMap RecipeLibrary::recipes_;
DecayHistMap RecipeLibrary::decay_hist_;
DecayTimesMap RecipeLibrary::decay_times_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
RecipeLibrary* RecipeLibrary::Instance() {
  // If we haven't created a RecipeLibrary yet, create and return it.
  if (0 == instance_){
    instance_ = new RecipeLibrary();  
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RecipeLibrary::RecipeLibrary() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::load_recipes(QueryEngine* qe) {
  // load recipes from file
  int nRecipes = qe->NElementsMatchingQuery("recipe");
  CLOG(LEV_DEBUG2) << "loading recipes {";
  for (int i = 0; i < nRecipes; i++) {
    QueryEngine* recipe = qe->QueryElement("recipe",i);
    std::string name = recipe->GetElementContent("name");
    CLOG(LEV_DEBUG2) << "Adding recipe '" << name << "'.";
    load_recipe(recipe); // load recipe
  }
  CLOG(LEV_DEBUG2) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::load_recipe(QueryEngine* qe) {
  // set basis
  bool atom;
  Basis basis;
  std::string basis_str = qe->GetElementContent("basis");
  if (basis_str == "atom") {
    atom = true;
    basis = ATOM;
  }
  else if (basis_str == "mass") {
    atom = false;
    basis = MASS;
  }
  else {
    throw ValidationError(basis + " basis is not 'mass' or 'atom'.");
  }

  std::string name = qe->GetElementContent("name");
  CLOG(LEV_DEBUG3) << "loading recipe: " << name 
                   << " with basis: " << basis_str;

  // make a new composition
  CompMapPtr recipe(new CompMap(basis));

  // get values needed for composition
  double value;
  int key;
  std::string query = "isotope";
  int nIsos = qe->NElementsMatchingQuery(query);
  
  for (int i = 0; i < nIsos; i++) 
    {
      QueryEngine* isotope = qe->QueryElement(query,i);
      key = strtol(isotope->GetElementContent("id").c_str(), NULL, 10);
      value = strtod(isotope->GetElementContent("comp").c_str(), NULL);
      // update our mass-related values
      (*recipe)[key] = value;
      CLOG(LEV_DEBUG3) << "  Isotope: " << key << " Value: " << value;
    }
  
  recipe->Massify();
  // record this composition (static members and database)
  RecordRecipe(name,recipe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::RecordRecipe(std::string name, CompMapPtr recipe) {
  if ( !RecipeRecorded(name) ) {
    RecordRecipe(recipe); // record this with the database, assigns id
    recipes_[name] = recipe; // store this as a named recipe, copies recipe
    StoreDecayableRecipe(Recipe(name)); // store this as a decayable recipe
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool RecipeLibrary::RecipeRecorded(std::string name) {
  int count = recipes_.count(name);
  return (count != 0); // true iff name in recipes_
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::RecordRecipe(CompMapPtr recipe) {
  if (!recipe->Recorded()) {
    recipe->ID_ = nextStateID_++;
    AddToTable(recipe);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr RecipeLibrary::Recipe(std::string name) {
  CheckRecipe(name);
  return recipes_[name];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::StoreDecayableRecipe(CompMapPtr recipe) {
  // initialize containers
  DecayTimes times;
  ChildMap childs;
  // assign containers
  decay_times_.insert(std::pair<CompMapPtr,DecayTimes>(recipe,times) );
  decay_hist_.insert(std::pair<CompMapPtr,ChildMap>(recipe,childs) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::RecordRecipeDecay(CompMapPtr parent, CompMapPtr child, int t_f) {
  AddDecayTime(parent,t_f);
  AddChild(parent,child,t_f);
  RecordRecipe(child);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int RecipeLibrary::RecipeCount() { 
  return RecipeLibrary::recipes_.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::CheckRecipe(std::string name) {
  if (!RecipeRecorded(name)) {
    std::stringstream err;
    err << "recipe_library.has not recorded recipe with name: " << name << ".";
    throw ValueError(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::CheckDecayable(CompMapPtr parent) {
  if (!CompositionDecayable(parent)) {
    std::stringstream err;
    err << "recipe_library.has not recorded recipe with id:" << parent->ID_
        << " as decayable.";
    throw ValueError(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::CheckChild(CompMapPtr parent, int time) {
  if (!ChildRecorded(parent,time)) {
    std::stringstream err;
    err << "recipe_library.has not recorded a decayed recipe for the parent " 
        << "recipe with id:" << parent->ID_ << " and decay time:" << time 
        << ".";
    throw ValueError(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::AddDecayTime(CompMapPtr parent, int time) {
  CheckDecayable(parent);
  decay_times(parent).insert(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
DecayTimes& RecipeLibrary::decay_times(CompMapPtr parent) {
  CheckDecayable(parent);
  return decay_times_[parent];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
ChildMap& RecipeLibrary::Children(CompMapPtr parent) {
  CheckDecayable(parent);
  return decay_hist_[parent];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr& RecipeLibrary::Child(CompMapPtr parent, int time) {
  CheckChild(parent,time);
  return Children(parent)[time];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool RecipeLibrary::ChildRecorded(CompMapPtr parent, int time) {
  int count = Children(parent).count(time);
  return (count != 0); // true iff name in recipes_
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::AddChild(CompMapPtr parent, CompMapPtr child, int time) {
  child->parent_ = parent;
  child->decay_time_ = time;
  Child(parent,time) = child; // child is copied
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool RecipeLibrary::CompositionDecayable(CompMapPtr comp) {
  int count1 = decay_times_.count(comp);
  int count2 = decay_hist_.count(comp);
  return (count1 != 0 && count2 != 0); // true iff comp in both 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RecipeLibrary::AddToTable(CompMapPtr recipe){
  for (CompMap::Iterator item = recipe->begin();
       item != recipe->end(); item++) {
    EM->NewEvent("IsotopicStates")
      ->AddVal("ID", recipe->ID())
      ->AddVal("IsoID", item->first)
      ->AddVal("Value", item->second)
      ->Record();
  }
}
} // namespace cyclus
