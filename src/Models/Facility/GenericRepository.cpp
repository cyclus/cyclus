// GenericRepository.cpp
// Implements the GenericRepository class
#include <iostream>

#include "GenericRepository.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"
#include "Timer.h"
#include "GenericRepository/StubComponent.h"



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
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);
  lifetime_ = strtod(XMLinput->get_xpath_content(cur,"lifetime"), NULL);
  area_ = strtod(XMLinput->get_xpath_content(cur,"area"), NULL);
  inventory_size_ = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);
  start_op_yr_ = strtol(XMLinput->get_xpath_content(cur,"startOperYear"), NULL, 10);
  start_op_mo_ = strtol(XMLinput->get_xpath_content(cur,"startOperMonth"), NULL, 10);

  // The repository accepts any commodities designated waste.
  // This will be a list

  /// all facilities require commodities - possibly many
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


  // get components
  // get far field
  Component* new_comp;
  nodes = XMLinput->get_xpath_elements(cur, "farfield");
  xmlNodePtr comp_node = nodes->nodeTab[0];
  far_field_ = initComponent(comp_node);

  // get buffer
  nodes = XMLinput->get_xpath_elements(cur, "buffer");
  comp_node = nodes->nodeTab[0];
  buffers_ = deque< Component* >();
  buffer_templates_.push_back(initComponent(comp_node));
  buffers_.push_back(initComponent(comp_node));

  // for each waste form
  // (these are found before wp's in order to help with wf_wp_map creation)
  nodes = XMLinput->get_xpath_elements(cur,"wasteform");
  for (int i=0;i<nodes->nodeNr;i++)
  {
    xmlNodePtr wf_node = nodes->nodeTab[i];
    Component* wf = initComponent(wf_node);
    // // get allowed waste commodities
    xmlNodeSetPtr allowed_commod_nodes = XMLinput->get_xpath_elements(wf_node,"allowedcommod");
    for (int i=0;i<allowed_commod_nodes->nodeNr;i++) {
      Commodity* allowed_commod = LI->getCommodity((const char*)(allowed_commod_nodes->nodeTab[i]->children->content));
      commod_wf_map_.insert(make_pair(allowed_commod, wf));
    }
    wf_templates_.push_back(wf);
  }

  // for each waste package
  nodes = XMLinput->get_xpath_elements(cur,"wastepackage");
  for (int i=0;i<nodes->nodeNr;i++)
  {
    xmlNodePtr wp_node = nodes->nodeTab[i];
    Component* wp = initComponent(wp_node); 
    wp_templates_.push_back(wp);
    // // get allowed waste forms
    xmlNodeSetPtr allowed_wf_nodes = XMLinput->get_xpath_elements(wp_node,"allowedwf");
    for (int i=0;i<allowed_wf_nodes->nodeNr;i++) {
      string allowed_wf_name = (const char*)(allowed_wf_nodes->nodeTab[i]->children->content);
      //iterate through wf_templates_
      //for each wf_template_
      for (deque< Component* >::iterator iter = wf_templates_.begin(); 
           iter != wf_templates_.end(); 
           iter ++){
        if ((*iter)->getName() == allowed_wf_name){
          wf_wp_map_.insert(make_pair(allowed_wf_name, wp_templates_.back()));
        }
      }
    }
  }
  stocks_ = deque< WasteStream >();
  inventory_ = deque< WasteStream >();
  waste_packages_ = deque< Component* >();
  waste_forms_ = deque< Component* >();
  is_full_ = false;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Component* GenericRepository::initComponent(xmlNodePtr cur){
  string comp_name = XMLinput->get_xpath_content(cur,"name");
  string model_name = XMLinput->get_xpath_name(cur,"model/*");
  Component* toRet = getComponent(model_name);
  toRet->init(cur);
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void GenericRepository::copy(GenericRepository* src)
{

  FacilityModel::copy(src);

  capacity_ = src->capacity_;
  inventory_size_ = src->inventory_size_;
  start_op_yr_ = src->start_op_yr_;
  start_op_mo_ = src->start_op_mo_;
  in_commods_ = src->in_commods_;
  far_field_ = src->far_field_;
  buffer_templates_ = src->buffer_templates_;
  wp_templates_ = src->wp_templates_;
  wf_templates_ = src->wf_templates_;
  wf_wp_map_ = src->wf_wp_map_;
  commod_wf_map_ = src->commod_wf_map_;
  buffers_ = src->buffers_;

  stocks_ = deque< WasteStream >();
  inventory_ = deque< WasteStream >();
  waste_packages_ = deque< Component* >();
  waste_forms_ = deque< Component* >();
  is_full_ = false;
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
    stocks_.push_front(make_pair(*thisMat, trans.commod));
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
    } else {
      if (space <= capacity_){
        // if empty space is less than monthly acceptance capacity
        requestAmt = space;

      // otherwise
      } else if (space >= capacity_){
        // the upper bound is the monthly acceptance capacity
        requestAmt = capacity_;
      }

      Communicator* recipient = dynamic_cast<Communicator*>(in_commod->getMarket());
      // recall that requests have a negative amount
      Message* request = new Message(UP_MSG, in_commod, -requestAmt, minAmt, commod_price,
          this, recipient); 
      // send it
      request->setNextDest(getFacInst());
      request->sendOn();
    }
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::handleTock(int time) {

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
  for (deque< WasteStream >::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter ++){
    total += (*iter).first->getTotMass();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass GenericRepository::checkStocks(){
  Mass total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.

  if(!stocks_.empty()){
    for (deque< WasteStream >::iterator iter = stocks_.begin(); 
         iter != stocks_.end(); 
         iter ++){
        total += (*iter).first->getTotMass();
    };
  };
  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::emplaceWaste(){
  // if there's anything in the stocks, try to emplace it
  if(!stocks_.empty()){
    // for each waste stream in the stocks
    for (deque< WasteStream >::iterator iter = stocks_.begin(); 
        iter != stocks_.end(); 
        iter ++){
       // start by packing the commod in a waste form
       // -- what waste form does this type of waste go into?
       // -- what density?
       // -- associate the waste stream with the waste form
       conditionWaste((*iter));
    }
    for (deque< Component* >::iterator iter = current_waste_forms_.begin(); 
        iter != current_waste_forms_.end(); 
        iter ++){
      // put the waste form in a waste package
      // -- what waste package does this type of waste go into?
      // -- what density?
      // -- associate the waste form with the was package
      packageWaste((*iter));
      waste_forms_.push_back(current_waste_forms_.front());
      current_waste_forms_.pop_front();
    }
    for (deque< Component* >::iterator iter = current_waste_packages_.begin(); 
        iter != current_waste_packages_.end(); 
        iter ++){
      // try to load each package in the current buffer 
      Component* current_buffer = buffers_.front();
      if (NULL == current_buffer) {
        string err_msg = "Buffers not yet loaded into Generic Repository.";
        throw GenException(err_msg);
        // if the package is full
        if( (*iter)->isFull()
            // and not too hot
            && (*iter)->getPeakTemp(OUTER) <= current_buffer->getTempLim() 
            // or too toxic
            && (*iter)->getPeakTox() <= current_buffer->getToxLim()){
          // emplace it in the buffer
          loadBuffer((*iter));
          if( current_buffer->isFull() ) {
            buffers_.push_back(buffers_.front());
            buffers_.pop_front();
            if( buffers_.front()->isFull()){
              // all buffers are now full, capacity reached
              is_full_ = true;
            }
          }
          // take the waste package out of the current packagess
          waste_packages_.push_back(current_waste_packages_.front());
          current_waste_packages_.pop_front();
        }
        // if the waste package was either too hot or not full
        // push it back on the stack
        current_waste_packages_.push_back(current_waste_packages_.front());
        current_waste_packages_.pop_front();
        inventory_.push_back(stocks_.front());
        stocks_.pop_front();
        // once the waste is emplaced, is there anything else to do?
      }
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::conditionWaste(WasteStream waste_stream){
  // figure out what waste form to put the waste stream in
  Component* chosen_wf_template = NULL;
  chosen_wf_template = commod_wf_map_[waste_stream.second];
  if(chosen_wf_template == NULL){
    string err_msg = "The commodity '";
    err_msg += (waste_stream.second)->getName();
    err_msg +="' does not have a matching waste form in the GenericRepository.";
    throw GenException(err_msg);
  }
  // if there doesn't already exist a partially full one
  // @todo check for partially full wf's before creating new one (katyhuff)
  // create that waste form
  current_waste_forms_.push_back( getComponent(chosen_wf_template->getImpl()));
  current_waste_forms_.back()->copy(chosen_wf_template);
  // and load in the waste stream
  current_waste_forms_.back()->absorb(waste_stream.first);
  return current_waste_forms_.back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::packageWaste(Component* waste_form){
  // figure out what waste package to put the waste form in
  Component* chosen_wp_template = NULL;
  string name = waste_form->getName();
  chosen_wp_template = wf_wp_map_[name];
  if(chosen_wp_template == NULL){
    string err_msg = "The waste form '";
    err_msg += (waste_form)->getName();
    err_msg +="' does not have a matching waste package in the GenericRepository.";
    throw GenException(err_msg);
  }
  // if there doesn't already exist a partially full one
  // @todo check for partially full wp's before creating new one (katyhuff)
  // create that waste package
  current_waste_packages_.push_back(getComponent(chosen_wp_template->getImpl()));
  current_waste_packages_.back()->copy(chosen_wp_template);
  // and load in the waste form
  current_waste_packages_.back()->load(WP, waste_form); // what is this?
  return current_waste_packages_.back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::loadBuffer(Component* waste_package){
  // figure out what waste form to put the waste stream in
  Component* chosen_buffer = buffers_.front();
  // and load in the waste package
  buffers_.front()->load(BUFFER, waste_package);
  return buffers_.front();
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::getComponent(string model_name){
  Component* toRet;
  switch(getRepoComponentType(model_name))
  {
    case STUB:
      toRet = new StubComponent();
      break;
    default:
      throw GenException("Unknown component model enum value encountered."); 
  }
  return toRet;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
RepoComponent GenericRepository::getRepoComponentType(string model_name) {
  RepoComponent toRet;
  string repo_component_names[] = {"StubComponent"};
  for(int type = 0; type < LAST_COMPONENT; type++){
    if(repo_component_names[type] == model_name){
      toRet = (RepoComponent)type;
    } else {
      string err_msg ="'";
      err_msg += model_name;
      err_msg += "' does not name a valid RepoComponent.\n";
      throw GenException(err_msg);
    }
  }
  return toRet;
}
