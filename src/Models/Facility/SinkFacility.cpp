/// SinkFacility.cpp
// Implements the SinkFacility class
#include <iostream>

#include "SinkFacility.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SinkFacility::SinkFacility(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SinkFacility::~SinkFacility(){
  // Delete all the Material in the inventory.
  while (!inventory.empty()) {
    Material* m = inventory.front();
    inventory.pop_front();
    delete m;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::init(xmlNodePtr cur)
{
  FacilityModel::init(cur);

  /** 
   *  Allow a Sink Facility to have many input/output commodities
   */

  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SinkFacility");

  /// all facilities require commodities - possibly many
  string commod_name;
  Commodity* new_commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"incommodity");

  for (int i=0;i<nodes->nodeNr;i++)
  {
    commod_name = (const char*)nodes->nodeTab[i]->children->content;
    new_commod = LI->getCommodity(commod_name);
    if (NULL == new_commod)
      throw GenException("Input commodity '" + commod_name 
          + "' does not exist for facility '" + getName() 
			    + "'.");
    in_commods.push_back(new_commod);
  }

  // get monthly capacity
  Mass capacity = atof(XMLinput->get_xpath_content(cur,"capacity"));

  // get inventory_size
  Mass inventory_size = atof(XMLinput->get_xpath_content(cur,"inventorysize"));

  // get commodity price
  double commod_price = atof(XMLinput->get_xpath_content(cur,"commodprice"));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::copy(SinkFacility* src)
{
  FacilityModel::copy(src);

  in_commods = src->in_commods;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::print() 
{ 
  FacilityModel::print();

  cout << "accepts commodities ";
  for (vector<Commodity*>::iterator commod=in_commods.begin();
       commod != in_commods.end();
       commod++)
  {
    cout << (commod == in_commods.begin() ? "{" : ", " );
    cout << (*commod)->getName();
  }
  cout << "} until its inventory is full at ";
  cout << inventory_size << " [UNITS?]." << endl ;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::handleTick(int time){

  // The sink facility should ask for as much stuff as it can reasonably receive.
  Mass requestAmt;
  // And it can accept amounts no matter how small
  Mass minAmt = 0;
  // check how full its inventory is
  Mass fullness = this->checkInventory();
  // subtract from max size to get total empty space
  Mass emptiness = inventory_size - fullness;

  if (emptiness == 0){
    // don't request anything
  }
  else if (emptiness < capacity){
  // if empty space is less than monthly acceptance capacity, request it,
    // for each commodity, request emptiness/(noCommodities)
    for (vector<Commodity*>::iterator commod=in_commods.begin();
       commod != in_commods.end();
       commod++) 
    {
      Communicator* recipient = (Communicator*)((*commod)->getMarket());
      requestAmt = emptiness/in_commods.size();
      Message* request = new Message(up, *commod, requestAmt, minAmt, 
                                     commod_price, this, recipient);
    // pass the message up to the inst
    (request->getInst())->receiveMessage(request);
    }
  }
  // otherwise, the upper bound is the monthly acceptance capacity, request cap.
  else if (emptiness >= capacity){
    // for each commodity, request capacity/(noCommodities)
    for (vector<Commodity*>::iterator commod=in_commods.begin();
       commod != in_commods.end();
       commod++) 
    {
      Communicator* recipient = (Communicator*)((*commod)->getMarket());
      requestAmt = capacity/in_commods.size();
      Message* request = new Message(up, *commod, requestAmt, minAmt, commod_price,
                          this, recipient); 
    // pass the message up to the inst
    (request->getInst())->receiveMessage(request);
    }
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::handleTock(int time){

  // On the tock, the sink facility doesn't really do much. 
  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  cout << "SinkFacility " << this->getSN();
  cout << " is holding " << this->checkInventory();
  cout << " material at month" << time; 
  cout << "." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::receiveMaterial(Transaction trans, vector<Material*> manifest){

  // grab each material object off of the manifest
  // and move it into the inventory.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
  inventory.push_back(*thisMat);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass SinkFacility::checkInventory(){
  Mass total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  deque<Material*>::iterator iter;

  for (iter = inventory.begin(); iter != inventory.end(); iter ++)
    total += (*iter)->getTotalMass();

  return total;
}

