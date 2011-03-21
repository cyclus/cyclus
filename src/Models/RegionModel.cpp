// RegionModel.cpp
// Implements the RegionModel class

#include "RegionModel.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initialize the RegionModel nextID to zero.
int RegionModel::nextID = 0;

#include "GenException.h"
#include "Logician.h"
#include "InputXML.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::init(xmlNodePtr cur)
{
 
  Model::init(cur);

  /** 
   *  Specific initialization for RegionModels
   */

  /// all regions require allowed facilities - possibly many
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"allowedfacility");

  string fac_name;
  Model* new_fac;
  
  for (int i=0;i<nodes->nodeNr;i++){
    fac_name = (const char*)nodes->nodeTab[i]->children->content;
    new_fac = LI->getFacilityByName(fac_name);
    if (NULL == new_fac){
      throw GenException("Facility " + fac_name 
          + " is not defined in this simulation.");
    }
    allowedFacilities.insert(new_fac);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::copy(RegionModel* src)
{
  Model::copy(src);
  Communicator::copy(src);

  /** 
   *  Specific initialization for RegionModels
   */

  allowedFacilities = src->allowedFacilities;
  
  // don't copy institutions!
  LI->addRegion(this);

}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::print()
{

  Model::print();

  cout << "allows facilities " ;

  for(set<Model*>::iterator fac=allowedFacilities.begin();
      fac != allowedFacilities.end();
      fac++){
    cout << (fac == allowedFacilities.begin() ? "{" : ", " )
        << (*fac)->getName();
  }
  
  cout << "} and has the following institutions:" << endl;
  
  for(vector<Model*>::iterator inst=institutions.begin();
      inst != institutions.end();
      inst++){
    (*inst)->print();
  }
}

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::receiveMessage(Message* msg){
  // Default regions aren't insterested in fooling with messages.
  // Just pass them along. 
  // If it's going up, send it to the market.
  // If it's going down, send it to the inst.
  MessageDir dir = msg->getDir();
  if (dir == up){
    Communicator* nextRecipient = msg->getMkt();
    nextRecipient->receiveMessage(msg);
  }
  else if (dir == down){
    Communicator* nextRecipient = msg->getInst();
    nextRecipient->receiveMessage(msg);
  }
}

void RegionModel::handleTick(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=institutions.begin();
      inst != institutions.end();
      inst++){
    ((InstModel*)(*inst))->handleTick(time);
  }
}

void RegionModel::handleTock(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=institutions.begin();
      inst != institutions.end();
      inst++){
    ((InstModel*)(*inst))->handleTock(time);
  }
}

/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */
