// Logician.cpp
// Implements the Logician class.
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


