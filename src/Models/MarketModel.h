// MarketModel.h
#if !defined(_MARKETMODEL_H)
#define _MARKETMODEL_H
#include <string>


#include "Model.h"

using namespace std;

/// forward declaration to resolve recursion
class Commodity;

//-----------------------------------------------------------------------------
/*
 * The MarketModel class is the abstract class/interface used by all market
 * models
 * 
 * This is all that is known externally about Markets
 */
//-----------------------------------------------------------------------------
class MarketModel : public Model
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    /// Default constructor for the MarketModel Class
    MarketModel() { ID = nextID++; };
    
    /// MarketModels should not be indestructible.
    virtual ~MarketModel() {};
    
    // every model should be able to print a verbose description
    virtual void print() = 0;

    /// get model implementation name
    virtual const string getModelName() = 0;

protected:
    /// Stores next available market ID
    static int nextID;
/* ------------------- */ 


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */
public:
    /// every market should provide its commodity
    Commodity* getCommidity() { return commodity; } ;

protected: 
    /// every market has a commodity
    Commodity* commodity;
/* ------------------- */ 
    
};

#endif



