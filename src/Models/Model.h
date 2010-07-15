// Model.h
#if !defined(_MODEL_H)
#define _MODEL_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <libxml/tree.h>

using namespace std;

class Model;

typedef Model* mdl_ctor();
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

    /// Create a model for use in the simulation
    static Model* create(string model_type, xmlNodePtr cur);

    /// Create a new mode object based on an existing one
    static Model* create(Model* src);

    /// Destroy a model cleanly
    static void* destroy(Model* model);
    
    /// every model needs an init method
    virtual void init(xmlNodePtr cur);

    /// every model needs a copy method
    virtual void copy(Model* src);

    /// Constructor for the Model Class
    Model() {};

    /// Models should not be indestructible.
    virtual ~Model() {};

    /// get model instance name
    const string getName() const { return name; };

    /// get model instance SN
    const int getSN() const { return ID; };

    /// get model implementation name
    const string getModelName() { return modelImpl; };

    /// every model should be able to print a verbose description
    virtual void print();

    static void load_markets();
    static void load_facilities();
    static void load_facilitycatalog(string filename, string ns, string format);
    static void load_regions();
    static void load_institutions();

protected:
    /// every instance of a model should have a name
    string name;

    /// every instance of a model should know its type
    string model_type;

    /// every instance of a model should know its implementation
    string modelImpl;

    /// every instance of a model will have a serialized ID
    int ID;

private:    
    /// map of constructor methods for each loaded model
    static map<string, mdl_ctor*> create_map;
    /// map of destructor methods for each loaded model
    static map<string, mdl_dtor*> destroy_map;
    
};


#endif



