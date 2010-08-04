// Logician.h
#if !defined(_LOGICIAN)
# define _LOGICIAN

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
 * A (singleton) simulation logician class. This class sends tick messages and 
 * collects and processes requests from simulation objects. 
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
	ModelList facilities, insts, regions, markets;

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
   * Handles all the duties of the time step (typically, a month)
   *
   * @param time is the simulation time step that is being handled.
   */
  void handleTimeStep(int time);
		
	/// generic routines to handle Model-based entities
  /**
   * sends the tick signal to all of the models
   *
   * @param time is the simulation time of the tick
   */
  void sendTick(int time);

  /**
   * sends the tock signal to all of the models
   *
   * @param time is the simulation time of the tock
   */
  void sendTock(int time);

  /**
   * sends the resolve signal to all of the market models
   * which in turn make matches and send orders
   *
   * @param time is the simulation time of the tock
   */
  void resolveMarkets();

	/* 
	 * Generic routine to add a Model-based entity to a specific list
	 *
	 * @param new_model pointer to model-based entity to be added
	 * @param list list to which this entity should be added
	 */
	void addModel(Model* new_model, ModelList &list);

	/*
	 * Generic routine to print a list of model-based entities
	 *
	 * @param list list to print
	 */
	void printModelList(ModelList list);	

	/* 
	 * Search a list of models for a particular name
	 *
	 * @param list list to be searched
	 * @param search_name name to search for
	 */
	Model* getModelByName(ModelList list, string search_name);

	/**
   * add a facility to the list
   *
   * @param new_facility the new facility to add to the list
   */
	void addFacility(Model* new_facility);

	/**
   * print list of facilities
   */
	void printFacilities();

	/** 
   * get number of facilities
   */
	int getNumFacilities(); 

	/**
   * get a pointer to the facility based on its ID number
   *
   * @param ID the ID number of the facility whose pointer to return.
   */
	Model* getFacilityByID(int ID);        

  /**
   * get a pointer to the facility based on its name
   *
   * @param name the name of the facility, a string
   */
	Model* getFacilityByName(string name); 
	
	/**
   * add a market to the list
   *
   * @param new_market the market to add to the list
   */
	void addMarket(Model* new_market);   

	/**
   * print list of markets
   */
	void printMarkets();

	/**
   * returns number of markets
   */
	int getNumMarkets();                 

	/**
   * get a pointer to a market based on its ID number
   *
   * @param ID the ID number of the market to return
   */
	Model* getMarketByID(int ID);        

	/**
   * get a pointer to a market based on its name
   *
   * @param name the name of the market whose pointer to return
   */
	Model* getMarketByName(string name); 

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
	
	/**
   * add a inst to the list
   *
   * @param new_inst
   */
	void addInst(Model* new_inst);   

	/** 
   * print list of insts
   */
	void printInsts();

	/**
   * get number of insts
   */
	int getNumInsts();                 

	/**
   * get a pointer to an inst based on its ID number
   *
   * @param ID the ID number of the inst for which to return a pointer
   */
	Model* getInstByID(int ID);        

	/**
   * get a pointer to an inst based on its name
   *
   * @param name the name of the inst for which to return a pointer
   */
	Model* getInstByName(string name); 

	/**
   * add a region to the list
   *
   * @param new_region
   */
	void addRegion(Model* new_region);   

	/** 
   * print list of regions
   */
	void printRegions();

	/**
   * get number of regions
   */
	int getNumRegions();                 

	/**
   * get a pointer to a region based on its ID number
   *
   * @param ID the ID number of the region for which to return a pointer
   */
	Model* getRegionByID(int ID);        

	/**
   * get a pointer to a region based on its name
   *
   * @param name the name of the region for which to return a pointer
   */
	Model* getRegionByName(string name); 

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
