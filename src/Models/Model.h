// Model.h
#if !defined(_MODEL_H)
#define _MODEL_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class Model;

typedef Model* mdl_ctor(string);
typedef void mdl_dtor(Model*);


/*!
 * The Model class is the abstract class used by all types of models
 * that will be available for dynamic loading.  This common interface
 * means that the basic process of loading and registering models can
 * be implemented in a single place.
 *
 * To allow serialization of different types of models in separate
 * ID space, this interface is inherited by type-specific abstract
 * classes, such as MarketModel, that has its own static integer
 * to keep track of the next available ID.
 */
class Model
{
public:
    /// Add a model to the simulation
    static mdl_ctor* load(string model_type,string model_name);
    
    /// Constructor for the Model Class
    Model() {};

    /// Models should not be indestructible.
    virtual ~Model() {};

    /// get model instance name
    const string getName() const { return name; };

    /// get model instance SN
    const int getSN() const { return ID; };

    /// every model should be able to print a verbose description
    virtual void print() = 0;
    
protected:
    /// every instance of a model should have a name
    string name;

    /// every instance of a model will have a serialized ID
    int ID;

private:    
    /// map of constructor methods for each loaded model
    static map<string, mdl_ctor*> create_map;
    /// map of destructor methods for each loaded model
    static map<string, mdl_dtor*> destroy_map;
    
};


#endif



