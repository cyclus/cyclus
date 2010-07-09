// Commodity.h
#if !defined(_COMMODITY_H)
#define _COMMODITY_H
#include <string>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "Model.h"

using namespace std;

class Commodity
{
public:
    /// construct new commodities from the input stream
    Commodity(xmlNodePtr cur);
    
    ~Commodity();
    
    static void load_commodities();

    /// get the commodity name
    const string getName() const { return name;};
    
    /// get the ID number
    int getSN() const {return ID;};
    
    /// get the Model pointer
    Model* getMarket() {return market;};
    
    /// set the Model pointer
    void setMarket(Model* new_market) { market = new_market; };
    
private:
    /// unique ID space for serialization
    static int nextID;
    
    /**
     * This is the name of the commodity, indicative of the market on 
     * which it's to be traded.
     */
    string name;
    
    /**
     * The ID number is to be used for serialization of the commodity types.
     */
    int ID;
    
    /**
     * Each commodity should be associated with a market on which it will be
     * traded between offering and requesting facilities at each timestep.
     */
    Model* market;
    
    
};
#endif
