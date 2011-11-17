// Logician.h
#if !defined(_LOGICIAN)
#define _LOGICIAN

#include "Model.h"
#include "Material.h"

#include <vector>
#include <string>
#include <map>
#include <list>

#define LI Logician::Instance()

typedef std::map<int, Model*> ModelList;
typedef std::map<std::string, Material*> RecipeList;

/**
 * A (singleton) simulation logician class. This class sends tick messages
 * and collects and processes requests from simulation objects. 
 */
class Logician {

  private:
    /**
     * A pointer to this Logician once it has been initialized.
     */
    static Logician* instance_;
    
    /**
     * The (protected) constructor for this class, which can only 
     * be called indirectly by the client.
     */
    Logician();

    /// true if decay should occur, false if not.
    bool decay_wanted_;

    /// how many months between decay calculations
    int decay_interval_;

    /**
     * Used by the constructor to create a ModelList for every ModelType.
     */
    void createModelList(ModelType model_type);
    
    /// map of model types to model lists. Used by [add/get]Model methods)
    std::map<ModelType, ModelList> model_lists_;
    
    /// list of material templates
    RecipeList recipes_;
    
    /// list of materialss
    std::vector<Material*> materials_;
    
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
     * Decays all of the materials if decay is on
     *
     * @todo should be private (khuff/rcarlsen)
     *
     * @param time is the simulation time of the tick
     */
    void decayMaterials(int time);
    
    /*
     * sets the decay boolean and the interval
     */
    void setDecay(int dec);

    /* 
     * Generic routine to add a Model-based entity to a specific list
     *
     * @param new_model pointer to model-based entity to be added
     * @param model_type type of new_model as defined by ModelType enum
     */
    void addModel(Model* new_model, ModelType model_type);

    /**
     * @brief Get an iterator to models of particular type.
     *
     * @param model_type type of models to iterate over.
     */
    ModelList::iterator begin(ModelType model_type);

    /**
     * @brief Get an iterator to models of particular type.
     * 
     * @param model_type type of models to iterate over.
     *
     */
    ModelList::iterator end(ModelType model_type);

    /**
     * get a pointer to a converter based on its name
     *
     * @param search_name the name of the converter whose pointer to return
     * @param model_type type of the model pointer to return (ModelType enum)
     */
    Model* getModelByName(std::string search_name, ModelType model_type); 

    /** 
     * get number of models of type model_type
     *
     * @param model_type type of model to return count for (ModelType enum)
     */
    int getNumModels(ModelType model_type); 
    
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
    void addRecipe(std::string name, Material* new_mat);

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
    Material* getRecipe(std::string name);                      
    
};

#endif

