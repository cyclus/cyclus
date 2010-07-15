// Commodity.h
#if !defined(_COMMODITY_H)
#define _COMMODITY_H
#include <string>

#include <libxml/tree.h>

using namespace std;

// foward declaration avoids including entire class header here
class Model;

/**
 *  A class to defining a particular type of material.  Each Commodity will
 *  have a single MarketModel market that is repsonsible for managing the
 *  trade of that Commodity.
 */
class Commodity
{
public:
    /**
     *  @brief Primary constructor uses an existing XML Node Pointer to
     *  extract the name of the Commodity.
     *
     * @param cur a pointer to an XML node as defined by the libxml2 interface
     */
    Commodity(xmlNodePtr cur);
    
    /// cleanly delete and free memory used by the Commodity object
    ~Commodity() {};
    
    /// get the Commodity name
    const string getName() const { return name;};
    
    /// get the ID number
    int getSN() const {return ID;};
    
    /// get the Model pointer
    Model* getMarket() {return market;};
    
    /**
     *  @brief set the Model pointer
     *
     * @param new_market pointer to Model that represents MarketModel
     */
    void setMarket(Model* new_market) { market = new_market; };
    
    /**
     *  @brief Load the Commodity objects from the XML file
     *
     *  This method extracts the list of XML Nodes that refer to commodities,
     *  creates each one, loading its parameters from the XML node, and 
     *  registers it with the Logican.
     *
     *  Despite needing no special knowledge of a Commodity object, this
     *  method is a member of Commodity in order to localize this access in
     *  case it does need special knowledge in the future.
     */
    static void load_commodities();

private:
    /// unique ID space for serialization
    static int nextID;
    
    /**
     * @brief the name of this Commodity as defined by the user
     */
    string name;
    
    /**
     * @brief the unique ID number of this Commodity as serialized by the
     * constructor
     *
     * Although the pointer to this object already serves as a unique
     * reference, this ID is provided as reference in a contiguous ID space.
     */
    int ID;
    
    /**
     * @brief a pointer to the MarketModel model that is trading this
     * Commodity
     * 
     * Each commodity should be associated with a market on which it will be
     * traded between offering and requesting facilities at each timestep.
     */
    Model* market;
    
};
#endif
