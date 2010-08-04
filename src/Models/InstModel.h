// InstModel.h
#if !defined(_INSTMODEL_H)
#define _INSTMODEL_H
#include <string>


#include "Model.h"
#include "Communicator.h"
#include "RegionModel.h"

using namespace std;




//-----------------------------------------------------------------------------
/*
 * The InstModel class is the abstract class/interface used by all institution models
 * 
 * This InstModel is intended as a skeleton to guide the implementation of new
 * Models.
 */
//-----------------------------------------------------------------------------
class InstModel : public Model, public Communicator
{

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
public:
  /// Default constructor for InstModel Class
  InstModel() { ID = nextID++; model_type="Inst"; commType=InstComm; };

  /// every model should be destructable
  virtual ~InstModel() {};
  
  // every model needs a method to initialize from XML
  virtual void init(xmlNodePtr cur);
  // every model needs a method to copy one object to another
  virtual void copy(InstModel* src);

  // every model should be able to print a verbose description
  virtual void print();

protected: 
  /// Stores the next available institution ID
  static int nextID;
/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
  /// default InstModel receiver has no default behavior
  virtual void receiveMessage(Message* msg) = 0;

protected:


/* ------------------- */ 
/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

public:
  /// sets this institution's region
  void setRegion(Model* my_region) { region = my_region; };

  /// returns this institution's region
  RegionModel* getRegion() { return ((RegionModel*)(region)); };

  /// adds a facility to this model
  void addFacility(Model* new_fac){ facilities.push_back(new_fac);};

protected:
  /**
   * Each institution is a member of exactly one region
   */
  Model* region;

  /**
   * Each institution keeps a list of its facilities;
   */
  vector<Model*> facilities;

/* ------------------- */ 
  
};

#endif



