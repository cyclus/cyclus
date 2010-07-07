// Logician.h
#if !defined(_LOGICIAN)
# define _LOGICIAN

#include "Model.h"
#include "Material.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <list>

#define LI Logician::Instance()

using namespace std;

typedef vector<Model*> ModelList;
typedef vector<Material*> MaterialList;

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
	Logician() { };

	/// lists of models
	ModelList facilities, markets, regions;

	/// list of material templates
	MaterialList recipes;

	/**
	 * (Recursively) deletes this Logician (and the objects it manages).
	 */
	~Logician() {};


public:
	/**
	 * Gives all simulation objects global access to the Logician by 
	 * returning a pointer to it.
	 *
	 * @return a pointer to the Logician
	 */
	static Logician* Instance();
		
	/// generic routines to handle Model-based entities
	/* 
	 * Generic routine to add a Model-based entity to a specific list
	 *
	 * @param new_model pointer to model-based entity to be added
	 * @param list list to which this entity should be added
	 */
	void addModel(Model* new_model, ModelList &list)
    	      { list.push_back(new_model);};
	/*
	 * Generic routine to print a list of model-based entities
	 *
	 * @param list list to print
	 */
	void printModelList(ModelList list)
	{for (ModelList::iterator model = list.begin();
	      model != list.end();
	      model++) (*model)->print();
	}
	/* 
	 * Search a list of models for a particular name
	 *
	 * @param list list to be searched
	 * @param search_name name to search for
	 */
	Model* getModelByName(ModelList list, string search_name);

	/// add a facility to the list
	void addFacility(Model* new_facility) { facilities.push_back(new_facility); }
	/// print list of facilities
	void printFacilities()                { printModelList(facilities); };
	/// get number of facilities
	int getNumFacilities()                { return facilities.size(); }
	/// get a pointer to the facility based on its ID number
	Model* getFacilityByID(int ID)        { return facilities[ID]; }
	Model* getFacilityByName(string name) { return getModelByName(facilities,name); }
	
	/// add a market to the list
	void addMarket(Model* new_market)   { markets.push_back(new_market); }
	/// print list of markets
	void printMarkets()                 { printModelList(markets); };
	/// get number of markets
	int getNumMarkets()                 { return markets.size(); }
	/// get a pointer to the market based on its ID number
	Model* getMarketByID(int ID)        { return markets[ID]; }
	Model* getMarketByName(string name) { return getModelByName(markets,name); }
	
	/// add a region to the list
	void addRegion(Model* new_region)   { regions.push_back(new_region); }
	/// print list of regions
	void printRegions()                 { printModelList(regions); };
	/// get number of regions
	int getNumRegions()                 { return regions.size(); }
	/// get a pointer to the region based on its ID number
	Model* getRegionByID(int ID)        { return regions[ID]; }
	Model* getRegionByName(string name) { return getModelByName(regions,name); }

	/// add a material to the list
	void addRecipe(Material* new_mat) { recipes.push_back(new_mat); };


};
#endif
