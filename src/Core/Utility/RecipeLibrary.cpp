// RecipeLibrary.cpp
// Implements the RecipeLibrary class

#include "RecipeLibrary.h"

#include "CompMap.h"
#include "MassTable.h"
#include "CycException.h"
#include "EventManager.h"

#include <map>
#include <sstream>

using namespace std;

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
  int nRecipes = qe->nElementsMatchingQuery("recipe");
  CLOG(LEV_DEBUG2) << "loading recipes {";
  for (int i = 0; i < nRecipes; i++) {
    QueryEngine* recipe = qe->queryElement("recipe",i);
    string name = recipe->getElementContent("name");
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
  string basis_str = qe->getElementContent("basis");
  if (basis_str == "atom") {
    atom = true;
    basis = ATOM;
  }
  else if (basis_str == "mass") {
    atom = false;
    basis = MASS;
  }
  else {
    throw CycIOException(basis + " basis is not 'mass' or 'atom'.");
  }

  string name = qe->getElementContent("name");
  CLOG(LEV_DEBUG3) << "loading recipe: " << name 
                   << " with basis: " << basis_str;

  // make a new composition
  CompMapPtr recipe(new CompMap(basis));

  // get values needed for composition
  double value;
  int key;
  string query = "isotope";
  int nIsos = qe->nElementsMatchingQuery(query);
  
  for (int i = 0; i < nIsos; i++) 
    {
      QueryEngine* isotope = qe->queryElement(query,i);
      key = strtol(isotope->getElementContent("id").c_str(), NULL, 10);
      value = strtod(isotope->getElementContent("comp").c_str(), NULL);
      // update our mass-related values
      (*recipe)[key] = value;
      CLOG(LEV_DEBUG3) << "  Isotope: " << key << " Value: " << value;
    }
  
  recipe->massify();
  // record this composition (static members and database)
  recordRecipe(name,recipe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::recordRecipe(std::string name, CompMapPtr recipe) {
  if ( !recipeRecorded(name) ) {
    recordRecipe(recipe); // record this with the database, assigns id
    recipes_[name] = recipe; // store this as a named recipe, copies recipe
    storeDecayableRecipe(Recipe(name)); // store this as a decayable recipe
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool RecipeLibrary::recipeRecorded(std::string name) {
  int count = recipes_.count(name);
  return (count != 0); // true iff name in recipes_
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::recordRecipe(CompMapPtr recipe) {
  if (!recipe->recorded()) {
    recipe->ID_ = nextStateID_++;
    addToTable(recipe);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr RecipeLibrary::Recipe(std::string name) {
  checkRecipe(name);
  return recipes_[name];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::storeDecayableRecipe(CompMapPtr recipe) {
  // initialize containers
  decay_times times;
  ChildMap childs;
  // assign containers
  decay_times_.insert( pair<CompMapPtr,decay_times>(recipe,times) );
  decay_hist_.insert( pair<CompMapPtr,ChildMap>(recipe,childs) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::recordRecipeDecay(CompMapPtr parent, CompMapPtr child, int t_f) {
  addDecayTime(parent,t_f);
  addChild(parent,child,t_f);
  recordRecipe(child);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int RecipeLibrary::recipeCount() { 
  return RecipeLibrary::recipes_.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::checkRecipe(std::string name) {
  if (!recipeRecorded(name)) {
    stringstream err;
    err << "RecipeLibrary has not recorded recipe with name: " << name << ".";
    throw CycIndexException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::checkDecayable(CompMapPtr parent) {
  if (!compositionDecayable(parent)) {
    stringstream err;
    err << "RecipeLibrary has not recorded recipe with id:" << parent->ID_
        << " as decayable.";
    throw CycIndexException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::checkChild(CompMapPtr parent, int time) {
  if (!childRecorded(parent,time)) {
    stringstream err;
    err << "RecipeLibrary has not recorded a decayed recipe for the parent " 
        << "recipe with id:" << parent->ID_ << " and decay time:" << time 
        << ".";
    throw CycIndexException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::addDecayTime(CompMapPtr parent, int time) {
  checkDecayable(parent);
  decayTimes(parent).insert(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
decay_times& RecipeLibrary::decayTimes(CompMapPtr parent) {
  checkDecayable(parent);
  return decay_times_[parent];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
ChildMap& RecipeLibrary::Children(CompMapPtr parent) {
  checkDecayable(parent);
  return decay_hist_[parent];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr& RecipeLibrary::Child(CompMapPtr parent, int time) {
  checkChild(parent,time);
  return Children(parent)[time];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool RecipeLibrary::childRecorded(CompMapPtr parent, int time) {
  int count = Children(parent).count(time);
  return (count != 0); // true iff name in recipes_
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLibrary::addChild(CompMapPtr parent, CompMapPtr child, int time) {
  child->parent_ = parent;
  child->decay_time_ = time;
  Child(parent,time) = child; // child is copied
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool RecipeLibrary::compositionDecayable(CompMapPtr comp) {
  int count1 = decay_times_.count(comp);
  int count2 = decay_hist_.count(comp);
  return (count1 != 0 && count2 != 0); // true iff comp in both 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RecipeLibrary::addToTable(CompMapPtr recipe){
  for (CompMap::iterator item = recipe->begin();
       item != recipe->end(); item++) {
    EM->newEvent(NULL, "IsotopicStates")
      ->addVal("ID", recipe->ID())
      ->addVal("IsoID", item->first)
      ->addVal("Value", item->second)
      ->record();
  }
}
