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
  capacity_ = atof(XMLinput->get_xpath_content(cur,"capacity"));
  setMapVar("capacity_",&capacity_);
  lifetime_ = atof(XMLinput->get_xpath_content(cur,"lifetime"));
  setMapVar("lifetime_",&lifetime_);
  area_ = atof(XMLinput->get_xpath_content(cur,"area"));
  setMapVar("area_",&area_);
  inventory_size_ = atof(XMLinput->get_xpath_content(cur,"inventorysize"));
  setMapVar("inventory_size_",&inventory_size_);
  start_op_mo_ = atoi(XMLinput->get_xpath_content(cur,"startOperMonth"));
  setMapVar("start_op_mo_",&start_op_mo_);
  start_op_yr_ = atoi(XMLinput->get_xpath_content(cur,"startOperYear"));
  setMapVar("start_op_yr_",&start_op_yr_);

  // this facility requires input commodities
  string commod_name;
  Commodity* new_commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"incommodity");

  for (int i=0;i<nodes->nodeNr;i++)
  {
    commod_name = (const char*)(nodes->nodeTab[i]->children->content);
    new_commod = LI->getCommodity(commod_name);
    if (NULL == new_commod)
      throw GenException("Input commodity '" + commod_name 
          + "' does not exist for facility '" + getName() 
                            + "'.");
    in_commods_.push_back(new_commod);
  }

  setMapVar("in_commods_",&in_commods_);

  this->init(member_var_map_);

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::init(map<string, void*> member_var_map)
{ 
  // set the member variable map accross the board
  member_var_map_ = member_var_map;

  // send the init signal upward
  FacilityModel::init(member_var_map);

  // this facility starts out empty 
  is_full_ = false;
  
  // initialize ordinary objects
  // get capacity_
  capacity_ = getMapVar<double>("capacity_",member_var_map); 
  // get lifetime_
  lifetime_ = getMapVar<double>("lifetime_",member_var_map); 
  // get area_
  area_ = getMapVar<double>("area_",member_var_map); 
  // get inventory_size_
  inventory_size_ = getMapVar<double>("inventory_size_",member_var_map); 
  // get start_op_yr_
  start_op_yr_ = getMapVar<int>("start_op_yr_",member_var_map); 
  // get start_op_mo_
  start_op_mo_ = getMapVar<int>("start_op_mo_",member_var_map); 

  // this takes commodity names as commodity* objects
  // it assumes that the commodity* provided exists within the simulation.
  in_commods_ = getMapVar< deque<Commodity*> > ("in_commods_", member_var_map);

  stocks_ = deque<Material*>();
  inventory_ = deque< Material* >();

  // create far field component
  // inititalize far field component
  // create buffer template
  // copy template and initialize buffers
  // create waste package templates
  // create waste form templates
  buffers_ = deque< Component* >();
  waste_packages_ = deque< Component* >();
  waste_forms_ = deque< Component* >();
  is_full_ = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void GenericRepository::copy(GenericRepository* src)
{

  FacilityModel::copy(src);

  // are these accessing the right stuff?
  capacity_ = src->capacity_;
  inventory_size_ = src->inventory_size_;
  start_op_yr_ = src->start_op_yr_;
  start_op_mo_ = src->start_op_mo_;
  in_commods_ = src->in_commods_;

  stocks_ = deque<Material*>();
  inventory_ = deque< Material*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::copyFreshModel(Model* src)
{
  copy(dynamic_cast<GenericRepository*>(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::print() 
{ 
  FacilityModel::print(); 
  cout << "stores commodity {"
    << in_commods_.front()->getName()
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
    cout<<"GenericRepository " << getSN() << " is receiving material with mass "
        << (*thisMat)->getTotMass() << endl;
    stocks_.push_front(*thisMat);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::handleTick(int time)
{
  // MAKE A REQUEST
  if(this->checkStocks() == 0){
    // It chooses the next incommodity in the preference lineup
    Commodity* in_commod;
    in_commod = in_commods_.front();

    // It then moves that commodity from the front to the back of the preference lineup
    in_commods_.push_back(in_commod);
    in_commods_.pop_front();
  
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
    Mass space = inventory_size_- inv - sto;
  
    if (space == 0){
      // don't request anything
    }
    else if (space <= capacity_){
      Communicator* recipient = dynamic_cast<Communicator*>(in_commod->getMarket());
      // if empty space is less than monthly acceptance capacity
      requestAmt = space;
      // recall that requests have a negative amount
      Message* request = new Message(UP_MSG, in_commod, -requestAmt, minAmt,
                                       commod_price, this, recipient);
        // pass the message up to the inst
        (request->getInst())->receiveMessage(request);
    }
    // otherwise
    else if (space >= capacity_){
      Communicator* recipient = dynamic_cast<Communicator*>(in_commod->getMarket());
      // the upper bound is the monthly acceptance capacity
      requestAmt = capacity_;
      // recall that requests have a negative amount
      Message* request = new Message(UP_MSG, in_commod, -requestAmt, minAmt, commod_price,
          this, recipient); 
      // send it
      sendMessage(request);
    };
  };
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::handleTock(int time)
{

  // emplace the waste that's ready
  emplaceWaste();

  // calculate the heat
  transportHeat();
  
  // calculate the nuclide transport
  transportNuclides();
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass GenericRepository::checkInventory(){
  Mass total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.
  for (deque< Material*>::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
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

  if(!stocks_.empty()){
    for (deque< Material* >::iterator iter = stocks_.begin(); 
         iter != stocks_.end(); 
         iter ++){
        total += (*iter)->getTotMass();
    };
  };
  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::emplaceWaste(){
  // if there's anything in the stocks, try to emplace it
  if(!stocks_.empty()){
    // for each waste stream in the stocks
    for (deque< Material* >::iterator iter = stocks_.begin(); 
        iter != stocks_.end(); 
        iter ++){
      while( !is_full_ ){
        // start by packing it in a waste form
        // -- what waste form does this type of waste go into?
        // -- what density?
        // -- associate the waste stream with the waste form
        Component* waste_form = conditionWaste((*iter));
      
        // put the in a waste package
        // -- what waste package does this type of waste go into?
        // -- what density?
        // -- associate the waste form with the was package
        Component* waste_package = packageWaste(waste_form);
      
        // try to load it in the current buffer 
        Component* current_buffer = buffers_.front();
        if (NULL == current_buffer) {
          string err_msg = "Buffers not yet loaded into Generic Repository.";
          throw GenException(err_msg);
        }
        // if the package is full
        if( waste_package->isFull()
            // and not too hot
            && waste_package->getPeakTemp(OUTER) <= current_buffer->getTempLim() 
            // or too toxic
            && waste_package->getPeakTox() <= current_buffer->getToxLim()){
          // take the stream out of the stocks
          // emplace it in the buffer
          loadBuffer(waste_package);
          if( current_buffer->isFull() ) {
            buffers_.push_back(buffers_.front());
            buffers_.pop_front();
            if( buffers_.front()->isFull()){
              // all buffers are now full, capacity reached
              is_full_ = true;
            }
          }
        }
        // once the waste is emplaced, is there anything else to do?
      }
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::conditionWaste(Material* waste_stream){
  Component* toRet = new Component();
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::packageWaste(Component* waste_form){
  Component* toRet = new Component();
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::loadBuffer(Component* waste_package){
  // find the current buffer and load it
  Component* toRet = new Component();
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::transportHeat(){
  // update the thermal BCs everywhere
  // pass the transport heat signal through the components, inner -> outer
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::transportNuclides(){
  // update the nuclide transport BCs everywhere
  // pass the transport nuclides signal through the components, inner -> outer
}




