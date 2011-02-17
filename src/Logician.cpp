// Logician.cpp
// Implements the Logician class.
#include <iostream> 
#include <math.h>
#include "Logician.h"
#include "GenException.h"

Logician* Logician::_instance = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician::Logician() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician::~Logician() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician* Logician::Instance() {
  // If we haven't created a Logician yet, create and return it.
  if (0 == _instance)
    _instance = new Logician();
  
  return _instance;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::handleTimeStep(int time)
{
  sendTick(time);
  resolveMarkets();
  sendTock(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::sendTick(int time)
{
  // tell all of the region models to handle the tick
  for(ModelList::iterator reg=regions.begin();
    reg != regions.end(); 
    reg++){
    ((RegionModel*)(*reg))->handleTick(time);
  }
  // tell all of the facility models to handle the tick
  for(ModelList::iterator fac=facilities.begin();
    fac != facilities.end(); 
    fac++){
    ((FacilityModel*)(*fac))->handleTick(time);
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::sendTock(int time)
{
  // tell all of the region models to handle the tock
  for(ModelList::iterator reg=regions.begin();
    reg != regions.end(); 
    reg++){
    ((RegionModel*)(*reg))->handleTock(time);
  }
  // tell all of the facility models to handle the tock
  for(ModelList::iterator fac=facilities.begin();
    fac != facilities.end(); 
    fac++){
    ((FacilityModel*)(*fac))->handleTock(time);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::resolveMarkets()
{
  // tell each market model to make matches and send out the orders
  for(ModelList::iterator mkt=markets.begin();
      mkt != markets.end();
      mkt++){
    ((MarketModel*)(*mkt))-> resolve();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addModel(Model* new_model, ModelList &list)
{ 
  list.push_back(new_model);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::printModelList(ModelList list) 
{
  for (ModelList::iterator model = list.begin();
       model != list.end();
       model++) (*model)->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getModelByName(ModelList list, string search_name)
{
  Model* found_model = NULL;
  
  for(ModelList::iterator model=list.begin();
      model != list.end(); model++)
    if (search_name == (*model)->getName())
      found_model = *model;
  
  return found_model;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addFacility(Model* new_facility) 
{ 
  facilities.push_back(new_facility); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::printFacilities()                
{
  printModelList(facilities); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumFacilities()                
{ 
  return facilities.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getFacilityByID(int ID)        
{ 
  return facilities[ID]; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getFacilityByName(string name) 
{ 
  return getModelByName(facilities,name); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addMarket(Model* new_market)   
{ 
  markets.push_back(new_market); 

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::printMarkets()                 
{ 
  printModelList(markets); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumMarkets()                 
{ 
  return markets.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getMarketByID(int ID)        
{ 
  return markets[ID]; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getMarketByName(string name) 
{ 
  return getModelByName(markets,name); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getMarketByCommodity(string commodity_name) 
{ 
  return commodity_market_map[getCommodity(commodity_name)]; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getMarketByCommodity(Commodity* commod) 
{ 
  return commodity_market_map[commod]; 
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::  registerCommodityMarket(Commodity* commod, Model* market)
{ 
  commodity_market_map[commod] = market; 
} 
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addConverter(Model* new_converter)   
{ 
  converters.push_back(new_converter); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::printConverters()                 
{ 
  printModelList(converters); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumConverters()                 
{ 
  return converters.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getConverterByID(int ID)        
{ 
  return converters[ID]; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getConverterByName(string name) 
{ 
  return getModelByName(converters,name); 
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addInst(Model* new_inst)   
{ 
  insts.push_back(new_inst); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::printInsts()                 
{ 
  printModelList(insts); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumInsts()                 
{ 
  return insts.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getInstByID(int ID)        
{ 
  return insts[ID]; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getInstByName(string name) 
{ 
  return getModelByName(insts,name); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addRegion(Model* new_region)   
{ 
  regions.push_back(new_region); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::printRegions()                 
{ 
  printModelList(regions); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumRegions()                 
{ 
  return regions.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getRegionByID(int ID)        
{ 
  return regions[ID]; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Logician::getRegionByName(string name) 
{ 
  return getModelByName(regions,name); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addRecipe(string name, Material* new_mat) 
{ 
  recipes[name] = new_mat; 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::printRecipes()
{
  for (RecipeList::iterator recipe=recipes.begin();
      recipe != recipes.end();
      recipe++){
    cout << "Recipe " << (*recipe).first << endl;
    (*recipe).second->print();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumRecipes()                                  
{ 
  return recipes.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Logician::getRecipe(string name)                     
{ 
  return recipes[name]; 
} 
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addCommodity(Commodity* new_commod) 
{ 
  commodities[new_commod->getName()] = new_commod; 
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumCommodities()                              
{
  return commodities.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Logician::getCommodity(string name)                 
{ 
  return commodities[name]; 
} 

