// StubInst.h
#if !defined(_STUBINST_H)
#define _STUBINST_H
#include <iostream>
#include "Logger.h"

#include "InstModel.h"

/**
 * The StubInst class inherits from the InstModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This model will do nothing. This InstModel is intended as a skeleton to guide
 * the implementation of new InstModel models. 
 *
 */
class StubInst : public InstModel  
{

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor for StubInst Class
   */
  StubInst();

  /**
   * every model should be destructable
   */
  virtual ~StubInst();
    
  /**
   * every model needs a method to initialize from XML
   *
   * @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
   * every model needs a method to copy one object to another
   *
   * @param src is the StubStub to copy
   */
  virtual void copy(StubInst* src) ;

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /**
   * every model should be able to print a verbose description
   */
   virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:

    /**
     * The StubInst should ignore incoming messages
     */
    virtual void receiveMessage(msg_ptr msg);

protected:


/* -------------------- */

/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

/* --------------------
 * This INSTMODEL class has these members
 * --------------------
 */

protected:

/* --------------------
   output directory info
 * --------------------
 */
 public:
  /**
     The getter function for the this inst model output dir
  */
  static std::string outputDir(){ 
    return InstModel::outputDir().append(outputDir_);}
  
 private:
  /**
     Every specific inst model writes to the output database
     location: InstModel::OutputDir_ + /inst_model_name
  */
  static std::string outputDir_;


};

#endif
