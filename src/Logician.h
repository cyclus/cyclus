// Logician.h
#if !defined(_LOGICIAN)
#define _LOGICIAN

#include "FacilityModel.h"
#include "MarketModel.h"
#include "Model.h"
#include "Material.h"
#include "Commodity.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <list>

#define LI Logician::Instance()

using namespace std;

typedef vector<Model*> ModelList;
typedef map<string,Material*> RecipeList;
typedef map<string,Commodity*> CommodityList;

/**
 * A (singleton) simulation logician class. This class sends tick messages
 * and collects and processes requests from simulation objects. 
 */
class Logician
{
private:
  /**
   * A pointer to this Logician once it has been initialized.
   */
  static Logician* _instance;
  
  /**
   * The (protected) constructor for this class, which can only 
   * be called indirectly by the client.
   */
  Logician();
  
  /// lists of models
  ModelList facilities, insts, regions, markets, converters;

  /// map list names to model lists (used by addModel method)
  map<ModelType, ModelList*> model_lists;
  
  /// list of material templates
  RecipeList recipes;
  
  /// list of commodities
  CommodityList commodities;
  
  /// map commodities to markets
  map<Commodity*, Model*> commodity_market_map;
  
  /**
   * (Recursively) deletes this Logician (and the objects it oversees).
   */
  ~Logician() ;


public:
  /**
   * Gives all simulation objects global access to the Logician by 
   * returning a pointer to it.
   *
   * @return a pointer to the Logician
   */
  static Logician* Instance();
  
  /**
   * This handles all pre-history interactions between regions,
   * institutions, and facilities.
   */
  void handlePreHistory();
  
  /**
   * Handles all the duties of the time step (typically, a month)
   *
   * @param time is the simulation time step that is being handled.
   */
  void handleTimeStep(int time);
	
  /// generic routines to handle Model-based entities
  /**
   * sends the tick signal to all of the models
   * TODO: should be private (rcarlsen)
   *
   * @param time is the simulation time of the tick
   */
  void sendTick(int time);

  /**
   * sends the tock signal to all of the models
   * TODO: should be private (rcarlsen)
   *
   * @param time is the simulation time of the tock
   */
  void sendTock(int time);

  /**
   * sends the resolve signal to all of the market models
   * which in turn make matches and send orders
   * TODO: should be private (rcarlsen)
   *
   */
  void resolveMarkets();

  /* 
   * Generic routine to add a Model-based entity to a specific list
   * TODO: should be private (rcarlsen)
   *
   * @param new_model pointer to model-based entity to be added
   * @param model_type type of new_model as defined by ModelType enum
   */
  void addModel(Model* new_model, ModelType model_type);

  /**
   * get a pointer to a model based on its ID number
   *
   * @param ID the ID number of the model to return
   * @param model_type type of the model pointer to return (ModelType enum)
   */
  Model* getModelByID(int ID, ModelType model_type);        

  /**
   * get a pointer to a converter based on its name
   *
   * @param name the name of the converter whose pointer to return
   * @param model_type type of the model pointer to return (ModelType enum)
   */
  Model* getModelByName(string search_name, ModelType model_type); 

  /** 
   * get number of models of type model_type
   *
   * @param model_type type of model to return count for (ModelType enum)
   */
  int getNumModels(ModelType model_type); 
	
  /**
   * get a pointer to a market based on its commodity name
   *
   * @param commodity_name the name of the commodity
   */
  Model* getMarketByCommodity(string commodity_name);

  /**
   * get a pointer to a market based on its commodity pointer
   *
   * @param commod the pointer to a commodity
   */
	Model* getMarketByCommodity(Commodity* commod); 

  /**
   * register a commodity with a market
   * 
   * @param commod a pointer to the commodity to register
   * @param market a pointer to the market with which to register the commod.
   */
  void   registerCommodityMarket(Commodity* commod, Model* market);
	
  /*
   * Generic routine to print a list of model-based entities
   *
   * @param model_type type of models to print a list of (ModelType enum)
   */
  void printModelList(ModelType model_type);	

  /**
   * add a recipe to the list
   *
   * @param name the name of the recipe to add 
   * @param new_mat the material object to add to the recipes list
   */
	void addRecipe(string name, Material* new_mat);

  /**
   * print list of recipes
   */
  void printRecipes();

  /**
   * get number of recipes
   */
  int getNumRecipes();                                  

  /**
   * get a pointer to the recipe based on its name
   *
   * @param name the name of the recipe for which to return a material pointer.
   */
  Material* getRecipe(string name);                      
	
  /**
   * add a commodity to the list
   *
   * @param new_commod the new commodity to add to the list 
   */
  void addCommodity(Commodity* new_commod);

  /**
   * get number or commodities
   */
  int getNumCommodities(); 

  /**
   * get a pointer to the commodity based on its name
   *
   * @param name the name of the commodity for which to return a pointer.
   */
  Commodity* getCommodity(string name);

};
#endif

