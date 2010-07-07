// Model.h
#if !defined(_MODEL_H)
#define _MODEL_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class Model;
//-----------------------------------------------------------------------------
/* The Market Class.
 * The Market class is the abstract class used by all objects that our 
 * factory will be responsible for creating, or manufacturing. 
 *
 * This is all our factory really knows about the products it's creating
 */
//-----------------------------------------------------------------------------
typedef Model* mdl_ctor(string);
typedef void mdl_dtor(Model*);

class Model
{
public:
    /// Add a model to the simulation
    static mdl_ctor* load(string model_type,string model_name);
    
    /// Constructor for the Model Class
    Model() {};

    /// Models should not be indestructible.
    virtual ~Model() {};

    // get model instance name
    const string getName() const { return name; };

    // get model instance SN
    const int getSN() const { return ID; };

    // every model should be able to print a verbose description
    virtual void print() = 0;
    
protected:
    /// every instance of a model should have a name
    string name;

    /// every instance of a model will have a serialized ID
    int ID;

private:    
    /// maps of constructor & destructor methods for each 
    static map<string, mdl_ctor*> create_map;
    static map<string, mdl_dtor*> destroy_map;
    
};


#endif



