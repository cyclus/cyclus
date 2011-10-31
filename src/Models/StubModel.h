// StubModel.h
#if !defined(_STUBMODEL_H)
#define _STUBMODEL_H

#include "Model.h"
#include <vector>

//-----------------------------------------------------------------------------
/*
 * The StubModel class is the abstract class/interface used by all stub models
 * 
 * This StubModel is intended as a skeleton to guide the implementation of new
 * Models.
 */
//-----------------------------------------------------------------------------
class StubModel : public Model {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor for StubModel Class
   */
  StubModel();

  /**
   * every model should be destructable
   */
  virtual ~StubModel();
    
  /**
   * every model needs a method to initialize from XML
   *
   * @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
   * every model needs a method to copy one object to another
   *
   * @param src is the StubModel to copy
   */
  virtual void copy(StubModel* src) ;

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src){};

  /**
   * every model should be able to print a verbose description
   */
   virtual void print();

  /**
   * return this model's parent
   */
  Model* parent(){return parent_;};

  /**
   * set this model's parent
   */
  void setParent(Model* parent){parent_ = parent;};

 protected:
  /**
   * container of children
   */
  std::vector<Model*> children_;
  
 private:
  /**
   * the model's parent
   */
  Model* parent_;


};

#endif



