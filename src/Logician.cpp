// Logician.cpp
// Implements the Logician class.
#include <iostream> 
#include <math.h>
#include "Logician.h"
#include "GenException.h"

Logician* Logician::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician::Logician() {

  /*
   * @warning This loop requires the ModelType enum to have its default
   *          consecutive integer values.
   */
  for ( int i = 0; i != END_MODEL_TYPES; i++ ) {
    createModelList((ModelType)i);
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::createModelList(ModelType model_type) {
  ModelList new_list;
  model_lists_[model_type] = new_list;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician::~Logician() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician* Logician::Instance() {
  // If we haven't created a Logician yet, create and return it.
  if (0 == instance_)
    instance_ = new Logician();
  
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::handlePreHistory() {
  // tell all of the region models to handle the tick
  ModelList* region_list;
  region_list = &(model_lists_[REGION]);

  for(ModelList::iterator reg = region_list->begin();
       reg != region_list->end(); 
       reg++) {
    dynamic_cast<RegionModel*>(reg->second)->handlePreHistory();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::handleTimeStep(int time) {
  sendTick(time);
  resolveMarkets();
  sendTock(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::sendTick(int time) {
  // tell all of the region models to handle the tick
  ModelList* region_list;
  region_list = &(model_lists_[REGION]);

  for(ModelList::iterator reg=region_list->begin();
       reg != region_list->end(); 
       reg++) {
    dynamic_cast<RegionModel*>(reg->second)->handleTick(time);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::sendTock(int time) {
  // tell all of the region models to handle the tock
  ModelList* region_list;
  region_list = &(model_lists_[REGION]);

  for(ModelList::iterator reg=region_list->begin();
       reg != region_list->end(); 
       reg++) {
    dynamic_cast<RegionModel*>(reg->second)->handleTock(time);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::resolveMarkets() {
  // tell each market model to make matches and send out the orders
  ModelList* market_list;
  market_list = &(model_lists_[MARKET]);

  for(ModelList::iterator mkt=market_list->begin();
      mkt != market_list->end();
      mkt++){
    (dynamic_cast<MarketModel*>(mkt->second))->resolve();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addModel(Model* new_model, ModelType model_type) { 
  int model_id = new_model->getSN();
  model_lists_[model_type][model_id] = new_model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getModelByID(int ID, ModelType model_type) { 
  return model_lists_[model_type][ID];
}

ModelList::iterator Logician::begin(ModelType model_type) {
  ModelList* list;
  list = &model_lists_[model_type];

  ModelList::iterator my_iter = list->begin();
  return my_iter;
}

ModelList::iterator Logician::end(ModelType model_type) {
  ModelList* list;
  list = &model_lists_[model_type];

  ModelList::iterator my_iter = list->end();
  return my_iter;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getModelByName(string name, ModelType model_type) {
  Model* found_model = NULL;

  ModelList* list;
  list = &model_lists_[model_type];
  
  for(ModelList::iterator model=list->begin();
      model != list->end(); model++) {
    if (name == model->second->getName()) {
      found_model = model->second;
    }
  }
  
  return found_model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumModels(ModelType model_type) { 
  return model_lists_[model_type].size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getMarketByCommodity(string commodity_name) 
{ 
  return commodity_market_map_[getCommodity(commodity_name)]; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getMarketByCommodity(Commodity* commod) { 
  return commodity_market_map_[commod]; 
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::  registerCommodityMarket(Commodity* commod, Model* market) { 
  commodity_market_map_[commod] = market; 
} 
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::printModelList(ModelType model_type) {
  ModelList* list;
  list = &model_lists_[model_type];
  
  for (ModelList::iterator model = list->begin();
       model != list->end();
       model++) {
    model->second->print();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addRecipe(string name, Material* new_mat) { 
  recipes_[name] = new_mat; 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::printRecipes() {
  for (RecipeList::iterator recipe=recipes_.begin();
      recipe != recipes_.end();
      recipe++){
    cout << "Recipe " << (*recipe).first << endl;
    recipe->second->print();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumRecipes() { 
  return recipes_.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Logician::getRecipe(string name) { 
  return recipes_[name]; 
} 
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addCommodity(Commodity* new_commod) { 
  commodities_[new_commod->getName()] = new_commod; 
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumCommodities() {
  return commodities_.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Logician::getCommodity(string name) { 
  return commodities_[name]; 
} 

