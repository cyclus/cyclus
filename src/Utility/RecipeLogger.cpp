// RecipeLogger.cpp
// Implements the RecipeLogger class

#include "RecipeLogger.h"

#include "IsotopicDefinitions.h"
#include "MassTable.h"
#include "CycException.h"
#include "InputXML.h"

#include <map>
#include <sstream>

using namespace std;

// initialize singleton member
RecipeLogger* RecipeLogger::instance_ = 0;
// initialize logging members
int RecipeLogger::nextStateID_ = 0;
RecipeMap RecipeLogger::recipes_;
DecayChainMap RecipeLogger::decay_chains_;
DecayTimesMap RecipeLogger::decay_times_;
// initialize table member
table_ptr RecipeLogger::iso_table = new Table("IsotopicStates"); 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
RecipeLogger* RecipeLogger::Instance() {
  // If we haven't created a RecipeLogger yet, create and return it.
  if (0 == instance_){
    instance_ = new RecipeLogger();  
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RecipeLogger::RecipeLogger() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::load_recipes() {
  // load recipes from file
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/recipe");
  string name;
  CLOG(LEV_DEBUG2) << "loading recipes {";
  for (int i = 0; i < nodes->nodeNr; i++) {
    name = XMLinput->getCurNS() + 
                  XMLinput->get_xpath_content(nodes->nodeTab[i], "name");
    CLOG(LEV_DEBUG2) << "Adding recipe '" << name << "'.";
    load_recipe(nodes->nodeTab[i]); // load recipe
  }

  // load recipes from databases
  nodes = XMLinput->get_xpath_elements("/*/recipebook");
  string filename, ns, format;
  for (int i = 0; i < nodes->nodeNr; i++) {
    filename = XMLinput->get_xpath_content(nodes->nodeTab[i], "filename");
    ns = XMLinput->get_xpath_content(nodes->nodeTab[i], "namespace");
    format = XMLinput->get_xpath_content(nodes->nodeTab[i], "format");
    XMLinput->extendCurNS(ns);

    if ("xml" == format) {
      CLOG(LEV_DEBUG2) << "going into a recipe book...";
      XMLinput->load_recipebook(filename);  // load recipe book
    } 
    else {
      throw 
        CycRangeException(format + "is not a supported recipebook format.");
    }
    XMLinput->stripCurNS();
  }
  CLOG(LEV_DEBUG2) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::load_recipe(xmlNodePtr cur) {
  // initialize comp map
  CompMap values;

  // get general values from xml
  string name = XMLinput->get_xpath_content(cur,"name");
  string basis = XMLinput->get_xpath_content(cur,"basis");
  bool atom = (basis == "atom");
  bool mass = (basis == "mass");
  if (!atom && !mass) {
    throw CycIOException(basis + " basis is not 'mass' or 'atom'.");
  }
  xmlNodeSetPtr isotopes = XMLinput->get_xpath_elements(cur,"isotope");

  // get values needed for composition
  double value;
  int key;
  xmlNodePtr iso_node;
  for (int i = 0; i < isotopes->nodeNr; i++) {
    iso_node = isotopes->nodeTab[i];
    key = strtol(XMLinput->get_xpath_content(iso_node,"id"), NULL, 10);
    value = strtod(XMLinput->get_xpath_content(iso_node,"comp"), NULL);
    // update our mass-related values
    values[key] = value;
  }
  
  // make a new composition
  CompositionPtr recipe(new Composition(values,atom));

  // log this composition (static members and database)
  logRecipe(name,recipe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::logRecipe(std::string name, CompositionPtr recipe) {
  if ( !recipeLogged(name) ) {
    logRecipe(recipe); // log this with the database, assigns id
    recipes_[name] = recipe; // store this as a named recipe, copies recipe
    storeDecayableRecipe(Recipe(name)); // store this as a decayable recipe
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool RecipeLogger::recipeLogged(std::string name) {
  int count = recipes_.count(name);
  return (count != 0); // true iff name in recipes_
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::logRecipe(CompositionPtr recipe) {
  if (!recipe->logged()) {
    recipe->ID_ = nextStateID_++;
    addToTable(*recipe);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr RecipeLogger::Recipe(std::string name) {
  checkRecipe(name);
  return recipes_[name];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::storeDecayableRecipe(CompositionPtr recipe) {
  // initialize containers
  decay_times times;
  DaughterMap daughters;
  // assign containers
  decay_times_.insert( pair<CompositionPtr,decay_times>(recipe,times) );
  decay_chains_.insert( pair<CompositionPtr,DaughterMap>(recipe,daughters) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::logRecipeDecay(CompositionPtr parent, CompositionPtr child, int t_f) {
  addDecayTime(parent,t_f);
  addDaughter(parent,child,t_f);
  logRecipe(child);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int RecipeLogger::recipeCount() { 
  return RecipeLogger::recipes_.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::checkRecipe(std::string name) {
  if (!recipeLogged(name)) {
    stringstream err;
    err << "RecipeLogger has not logged recipe with name: " << name << ".";
    throw CycIndexException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::checkDecayable(CompositionPtr parent) {
  if (!compositionDecayable(parent)) {
    stringstream err;
    err << "RecipeLogger has not logged recipe with id:" << parent->ID
        << " as decayable.";
    throw CycIndexException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::checkDaughter(CompositionPtr parent, int time) {
  if (!daughterLogged(parent,time)) {
    stringstream err;
    err << "RecipeLogger has not logged a decayed recipe for the parent " 
        << "recipe with id:" << parent->ID << " and decay time:" << time 
        << ".";
    throw CycIndexException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::addDecayTime(CompositionPtr parent, int time) {
  checkDecayable(parent);
  decayTimes(parent).insert(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
decay_times& RecipeLogger::decayTimes(CompositionPtr parent) {
  checkDecayable(parent);
  return decay_times_[parent];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
DaughterMap& RecipeLogger::Daughters(CompositionPtr parent) {
  checkDecayable(parent);
  return decay_chains_[parent];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr& RecipeLogger::Daughter(CompositionPtr parent, int time) {
  checkDaughter(parent,time);
  return Daughters(parent)[time];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool RecipeLogger::daughterLogged(CompositionPtr parent, int time) {
  int count = Daughters(parent).count(time);
  return (count != 0); // true iff name in recipes_
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeLogger::addDaughter(CompositionPtr parent, CompositionPtr child, int time) {
  child->setParent(parent);
  child->setDecayTime(time);
  Daughter(parent,time) = child; // child is copied
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool RecipeLogger::compositionDecayable(CompositionPtr comp) {
  int count1 = decay_times_.count(comp);
  int count2 = decay_chains_.count(comp);
  return (count1 != 0 && count2 != 0); // true iff comp in both 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RecipeLogger::define_table() {
  // declare the state id columns and add it to the table
  column state_id("ID","INTEGER");
  column iso_id("IsoID","INTEGER");
  column iso_value("Value","REAL");
  iso_table->addColumn(state_id);
  iso_table->addColumn(iso_id);
  iso_table->addColumn(iso_value);
  // declare the table's primary key
  primary_key pk;
  pk.push_back("ID"), pk.push_back("IsoID");
  iso_table->setPrimaryKey(pk);
  // we've now defined the table
  iso_table->tableDefined();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RecipeLogger::addToTable(Composition& recipe){
  // if we haven't logged a composition yet, define the table
  if ( !iso_table->defined() ) {
    RecipeLogger::define_table();
  }

  // make a row - stateid first then isotopics
  // declare data
  data an_id(recipe.ID);
  // declare entries
  entry id("ID",an_id);

  // now for the composition isotopics
  CompMap_p comp = recipe.mass_fractions;
  int i = 0;
  for (CompMap::iterator item = comp->begin();
       item != comp->end(); item++){
    CLOG(LEV_DEBUG2) << "isotope " << i++ << " of " << comp->size();
    // declare row
    // decalre data
    data an_iso_id(item->first), an_iso_value(item->second);
    // declare entries
    entry iso_id("IsoID",an_iso_id), iso_value("Value",an_iso_value);
    // construct row
    row aRow;
    aRow.push_back(id), aRow.push_back(iso_id), aRow.push_back(iso_value);
    // add the row
    iso_table->addRow(aRow);
    // // record this primary key
    // pkref_.push_back(id);
    // pkref_.push_back(iso_id);
  }
}

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// primary_key_ref RecipeLogger::pkref() {
//   return pkref_;
// }
