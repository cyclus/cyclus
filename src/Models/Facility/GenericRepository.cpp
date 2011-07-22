// GenericRepository.cpp
// Implements the GenericRepository class
#include <iostream>

#include "GenericRepository.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"
#include "Timer.h"



/**
 * The GenericRepository class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility model is intended to calculate nuclide and heat metrics over
 * time in the repository. It will make appropriate requests for spent fuel 
 * which derive from heat- and perhaps dose- limited space availability. 
 *
 * BEGINNING OF SIMULATION
 * At the beginning of the simulation, this facility model loads the 
 * components within it and figures out its initial capacity for each heat or 
 * dose generating waste type it expects to accept. 
 *
 * TICK
 * Examining the stocks, materials recieved last month are emplaced.
 * The repository determines its current capacity for the first of the 
 * incommodities (waste classifications?) and requests as much 
 * of the incommodities that it can fit. The next incommodity is on the docket 
 * for next month. 
 *
 * TOCK
 * The repository passes the Tock radially outward through its 
 * components.
 *
 * (r = 0) -> -> -> -> -> -> -> ( r = R ) 
 * mat -> form -> package -> buffer -> barrier -> near -> far
 *
 * RECEIVE MATERIAL
 * Put the material in the stocks
 *
 * RECEIVE MESSAGE
 * reject it, I don't do messages.
 *
 *
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::init(xmlNodePtr cur)
{ 
  FacilityModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/GenericRepository");

  // initialize ordinary objects
  capacity = atof(XMLinput->get_xpath_content(cur,"capacity"));
  inventory_size = atof(XMLinput->get_xpath_content(cur,"inventorysize"));
  startOpYr = atoi(XMLinput->get_xpath_content(cur,"startOperYear"));
  startOpMo = atoi(XMLinput->get_xpath_content(cur,"startOperMonth"));

  // The repository accepts any commodities designated waste.
  // This will be a list
  string commod_name;
  Commodity* in_commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "incommodity");

  // there are potentially many in commodities, but no outcommodities
  for (int i=0;i<nodes->nodeNr;i++){
    xmlNodePtr ptr = nodes->nodeTab[i];

    // get in_commod
    commod_name = XMLinput->get_xpath_content(ptr,"text");
    in_commod = LI->getCommodity(commod_name);
    if (NULL == in_commod)
      throw GenException("Input commodity '" + commod_name 
          + "' does not exist for facility '" + getName() 
          + "'.");
    in_commods.push_back(in_commod);
  };

  stocks = deque<Material*>();
  inventory = deque< Material* >();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void GenericRepository::copy(GenericRepository* src)
{

  FacilityModel::copy(src);

  capacity = src->capacity;
  inventory_size = src->inventory_size;
  startOpYr = src->startOpYr;
  startOpMo = src->startOpMo;
  in_commods = src->in_commods;

  stocks = deque<Material*>();
  inventory = deque< Material*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::copyFreshModel(Model* src)
{
  copy((GenericRepository*)(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::print() 
{ 
  FacilityModel::print(); 
  cout << "stores commodity {"
    << in_commods.front()->getName()
      << "} among others."  << endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::receiveMessage(Message* msg)
{
  throw GenException("GenericRepository doesn't know what to do with a msg.");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::receiveMaterial(Transaction trans, vector<Material*> manifest)
{
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    cout<<"GenericRepository " << ID << " is receiving material with mass "
        << (*thisMat)->getTotMass() << endl;
    stocks.push_front(*thisMat);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::handleTick(int time)
{
  // EMPLACE WASTE
  emplaceWaste();

  // MAKE A REQUEST
  if(this->checkStocks() == 0){
    // It chooses the next incommodity in the preference lineup
    Commodity* in_commod;
    in_commod = in_commods.front();

    // It then moves that commodity from the front to the back of the preference lineup
    in_commods.push_back(in_commod);
    in_commods.pop_front();
  
    // It can accept amounts however small
    Mass requestAmt;
    Mass minAmt = 0;
    // The GenericRepository should ask for material unless it's full
    Mass inv = this->checkInventory();
    // including how much is already in its stocks
    Mass sto = this->checkStocks(); 
    // this will be a request for free stuff
    double commod_price = 0;
    // subtract inv and sto from inventory max size to get total empty space
    Mass space = inventory_size - inv - sto;
  
    if (space == 0){
      // don't request anything
    }
    else if (space <= capacity){
      Communicator* recipient = (Communicator*)(in_commod->getMarket());
      // if empty space is less than monthly acceptance capacity
      requestAmt = space;
      // recall that requests have a negative amount
      Message* request = new Message(up, in_commod, -requestAmt, minAmt, 
                                       commod_price, this, recipient);
        // pass the message up to the inst
        (request->getInst())->receiveMessage(request);
    }
    // otherwise
    else if (space >= capacity){
      Communicator* recipient = (Communicator*)(in_commod->getMarket());
      // the upper bound is the monthly acceptance capacity
      requestAmt = capacity;
      // recall that requests have a negative amount
      Message* request = new Message(up, in_commod, -requestAmt, minAmt, commod_price,
          this, recipient); 
      // send it
      sendMessage(request);
    };
  };
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::handleTock(int time)
{
  // send tock to the emplaced material module
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass GenericRepository::checkInventory(){
  Mass total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.
  for (deque< Material*>::iterator iter = inventory.begin(); 
       iter != inventory.end(); 
       iter ++){
    total += (*iter)->getTotMass();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass GenericRepository::checkStocks(){
  Mass total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.

  if(!stocks.empty()){
    for (deque< Material* >::iterator iter = stocks.begin(); 
         iter != stocks.end(); 
         iter ++){
        total += (*iter)->getTotMass();
    };
  };
  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void emplaceWaste(){
  // EMPLACE THE WASTE
}

