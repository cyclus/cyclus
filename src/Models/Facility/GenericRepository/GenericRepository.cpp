// GenericRepository.cpp
// Implements the GenericRepository class
#include <iostream>
#include "Logger.h"

#include "GenericRepository.h"

#include "Logician.h"
#include "CycException.h"
#include "InputXML.h"
#include "Timer.h"

/**
 * The GenericRepository class inherits from the FacilityModel class and is 
 * dynamically
 * loaded by the Model class when requested.
 * 
 * This facility model is intended to calculate nuclide and heat metrics over
 * time in the repository. It will make appropriate requests for spent fuel 
 * which derive from heat- and perhaps dose- limited space availability. 
 *
 * BEGINNING OF SIMULATION
 * At the beginning of the simulation, this facility model loads the components 
 * within it, arranges them, and figures out its initial capacity for each 
 * heat or dose generating waste type it expects to accept. 
 *
 * TICK
 * Examining the stocks, materials recieved last month are emplaced.
 * The repository determines its current capacity for the first of the 
 * incommodities (waste classifications?) and requests as much of the 
 * incommodities that it can fit. The next incommodity is on the docket for next 
 * month. 
 *
 * TOCK
 * The repository passes the Tock radially outward through its components.
 *
 * (r = 0) -> -> -> -> -> -> -> ( r = R ) mat -> form -> package -> buffer -> 
 * barrier -> near -> far
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
{ FacilityModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/GenericRepository");

  // initialize ordinary objects
  x_ = strtod(XMLinput->get_xpath_content(cur,"x"), NULL);
  y_ = strtod(XMLinput->get_xpath_content(cur,"y"), NULL);
  z_ = strtod(XMLinput->get_xpath_content(cur,"z"), NULL);
  dx_ = strtod(XMLinput->get_xpath_content(cur,"dx"), NULL);
  dy_ = strtod(XMLinput->get_xpath_content(cur,"dy"), NULL);
  dz_ = strtod(XMLinput->get_xpath_content(cur,"dz"), NULL);
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);
  inventory_size_ = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), 
      NULL);
  lifetime_ = strtod(XMLinput->get_xpath_content(cur,"lifetime"), NULL);
  start_op_yr_ = strtol(XMLinput->get_xpath_content(cur,"startOperYear"), NULL, 
      10);
  start_op_mo_ = strtol(XMLinput->get_xpath_content(cur,"startOperMonth"), NULL, 
      10);

  // The repository accepts any commodities designated waste.
  // This will be a list

  /// all facilities require commodities - possibly many
  string commod_name;
  Commodity* new_commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"incommodity");

  for (int i=0;i<nodes->nodeNr;i++)
  {
    commod_name = (const char*)(nodes->nodeTab[i]->children->content);
    new_commod = Commodity::getCommodity(commod_name);
    in_commods_.push_back(new_commod);
  }


  // get components
  Component* new_comp;
  nodes = XMLinput->get_xpath_elements(cur,"component");
  // first, initialize the waste forms.
  for (int i=0;i<nodes->nodeNr;i++)
  {
    xmlNodePtr comp_node = nodes->nodeTab[i];
    if 
      (new_comp->getComponentType(XMLinput->get_xpath_content(comp_node,"componenttype")) 
       == WF){
      this->initComponent(comp_node);
    }
  }
  // now, initialize the rest 
  for (int i=0;i<nodes->nodeNr;i++)
  {
    xmlNodePtr comp_node = nodes->nodeTab[i];
    if 
      (new_comp->getComponentType(XMLinput->get_xpath_content(comp_node,"componenttype")) 
       != WF){
      this->initComponent(comp_node);
    }
  }

  // initialize things that don't depend on the input
  stocks_ = deque< WasteStream >();
  inventory_ = deque< WasteStream >();
  waste_packages_ = deque< Component* >();
  waste_forms_ = deque< Component* >();
  is_full_ = false;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Component* GenericRepository::initComponent(xmlNodePtr cur){
  Component* toRet = new Component();
  // the component class initialization function will pass down the xml pointer
  toRet->init(cur);

  // all components have a name and a type
  string comp_type = XMLinput->get_xpath_content(cur,"componenttype");

  // they will have allowed subcomponents (think russian doll)
  xmlNodeSetPtr allowed_sub_nodes;

  switch(toRet->getComponentType(comp_type)) {
    case BUFFER:
      buffer_template_ = toRet;
      // do the buffers have allowed waste package types?
      break;
    case FF:
      far_field_ = toRet;
      // does the far field have allowed buffer types?
      break;
    case WF:
      // get allowed waste commodities
      allowed_sub_nodes = XMLinput->get_xpath_elements(cur,"allowedcommod");
      for (int i=0;i<allowed_sub_nodes->nodeNr;i++) {
        Commodity* allowed_commod = NULL;
        allowed_commod = Commodity::getCommodity((const 
              char*)(allowed_sub_nodes->nodeTab[i]->children->content));
        commod_wf_map_.insert(make_pair(allowed_commod, toRet));
      }
      wf_templates_.push_back(toRet);
      break;
    case WP:
      wp_templates_.push_back(toRet);
      // // get allowed waste forms
      allowed_sub_nodes = XMLinput->get_xpath_elements(cur,"allowedwf");
      for (int i=0;i<allowed_sub_nodes->nodeNr;i++) {
        string allowed_wf_name = (const 
            char*)(allowed_sub_nodes->nodeTab[i]->children->content);
        //iterate through wf_templates_
        //for each wf_template_
        for (deque< Component* >::iterator iter = wf_templates_.begin(); iter != 
            wf_templates_.end(); iter ++){
          if ((*iter)->name() == allowed_wf_name){
            wf_wp_map_.insert(make_pair(allowed_wf_name, wp_templates_.back()));
          }
        }
      }
      break;
    default:
      throw CycException("Unknown ComponentType enum value encountered."); }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void GenericRepository::copy(GenericRepository* src)
{

  // copy facility level stuff
  FacilityModel::copy(src);

  // copy variables specific to this model
  capacity_ = src->capacity_;
  x_= src->x_;
  y_= src->y_;
  z_= src->z_;
  dx_= src->dx_;
  dy_= src->dy_;
  dz_= src->dz_;
  inventory_size_ = src->inventory_size_;
  start_op_yr_ = src->start_op_yr_;
  start_op_mo_ = src->start_op_mo_;
  in_commods_ = src->in_commods_;
  far_field_ = new Component();
  far_field_->copy(src->far_field_);
  buffer_template_ = src->buffer_template_;
  wp_templates_ = src->wp_templates_;
  wf_templates_ = src->wf_templates_;
  wf_wp_map_ = src->wf_wp_map_;
  commod_wf_map_ = src->commod_wf_map_;
  buffers_.push_front(new Component());
  buffers_.front()->copy(buffer_template_);
  setPlacement(buffers_.front());

  // don't copy things that should start out empty
  // initialize empty structures instead
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
void GenericRepository::print() { 
  // this should ultimately print all of the components loaded into this repository.
  FacilityModel::print(); LOG(LEV_DEBUG2) << "    with far_field_ {" 
    << far_field_->name()
    << "}, buffer {"
    << buffer_template_->name() 
    << "}, wp {"
    << wp_templates_.front()->name()
    << "}, wf {"
    << wf_templates_.front()->name()
    << "} which stores commodity {"
    << in_commods_.front()->name()
    << "} among others.";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::receiveMessage(Message* msg)
{
  throw CycException("GenericRepository doesn't know what to do with a msg.");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::receiveMaterial(Transaction trans, vector<Material*> 
    manifest)
{
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    LOG(LEV_DEBUG2) <<"GenericRepository " << ID() << " is receiving material with mass "
        << (*thisMat)->getTotMass();
    stocks_.push_front(make_pair(*thisMat, trans.commod));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::handleTick(int time)
{
  // if this is the first timestep, register the far field
  if (time==0){
    setPlacement(far_field_);
    far_field_->registerComponent();
  }

  // make requests
  makeRequests(time);
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
void GenericRepository::makeRequests(int time){

  // should this model make requests for all of the commodities it accepts?
  // there should be a section of the repository for each accepted commodity
 
  // right now it picks one commodity per month and asks for that.
  // It chooses the next incommodity in the preference lineup
  Commodity* in_commod;
  in_commod = in_commods_.front();

  // It then moves that commodity from the front to the back of the preference 
  // lineup
  in_commods_.push_back(in_commod);
  in_commods_.pop_front();

  // It can accept amounts however small
  Mass minAmt = 0;
  // this will be a request for free stuff
  double commod_price = 0;
  // It will need to figure out its capacity
  Mass requestAmt;
  // Perform the task of figuring out the capacity for this commod
  requestAmt = getCapacity(in_commod);
  
  // make requests
  if (requestAmt == 0){
    // don't request anything
  } else {
    Communicator* recipient = 
      dynamic_cast<Communicator*>(in_commod->getMarket());

    // build the transaction and message
    Transaction trans;
    trans.commod = in_commod;
    trans.min = minAmt;
    trans.price = commod_price;
    trans.amount = -requestAmt; // requests have a negative amount

    Message* request = new Message(this, recipient, trans); 
    request->setNextDest(getFacInst());
    request->sendOn();
  }
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass GenericRepository::getCapacity(Commodity* commod){
  Mass toRet;
  // if the overall repo has a legislative limit, report it
  // we need a heat model
  // The GenericRepository should ask for material unless it's full
  Mass inv = this->checkInventory();
  // including how much is already in its stocks
  Mass sto = this->checkStocks(); 
  // subtract inv and sto from inventory max size to get total empty space
  Mass space = inventory_size_- inv - sto;
  // if empty space is less than monthly acceptance capacity
  if (space <= capacity_){
    toRet = space;
    // otherwise
  } else if (space >= capacity_){
    // the upper bound is the monthly acceptance capacity
    toRet = capacity_;
  } return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass GenericRepository::checkInventory(){
  Mass total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.
  for (deque< WasteStream >::iterator iter = inventory_.begin(); iter != 
      inventory_.end(); iter ++){
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
    for (deque< WasteStream >::iterator iter = stocks_.begin(); iter != 
        stocks_.end(); iter ++){
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
    for (deque< WasteStream >::const_iterator iter = stocks_.begin(); iter != 
        stocks_.end(); iter ++){
      // -- put the waste stream in the waste form
      // -- associate the waste stream with the waste form
      conditionWaste((*iter));
    }
    for (deque< Component* >::const_iterator iter = 
        current_waste_forms_.begin(); iter != current_waste_forms_.end(); iter 
        ++){
      // -- put the waste form in a waste package
      // -- associate the waste form with the waste package
      packageWaste((*iter));
    }
    int nwf = current_waste_forms_.size();
    for(int i=0; i < nwf; i++){
      waste_forms_.push_back(current_waste_forms_.front());
      current_waste_forms_.pop_front();
    }
    int nwp = current_waste_packages_.size();
    for(int i=0; i < nwp; i++){
      Component* iter = current_waste_packages_.front();
      // try to load each package in the current buffer 
      Component* current_buffer = buffers_.front();
      if (NULL == current_buffer) {
        string err_msg = "Buffers not yet loaded into Generic Repository.";
        throw CycException(err_msg);
      }
      // if the package is full
      if( iter->isFull()
          // and not too hot
          //&& (*iter)->getPeakTemp(OUTER) <= current_buffer->getTempLim() or 
          //too toxic
          //&& (*iter)->getPeakTox() <= current_buffer->getToxLim()
          ) {
        // emplace it in the buffer
        loadBuffer(iter);
        if( current_buffer->isFull() ) {
          buffers_.push_back(buffers_.front());
          buffers_.pop_front();
          if( buffers_.front()->isFull()){
            // all buffers are now full, capacity reached
            is_full_ = true;
          } else {
            setPlacement(buffers_.front());
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::conditionWaste(WasteStream waste_stream){
  // figure out what waste form to put the waste stream in
  Component* chosen_wf_template = NULL;
  chosen_wf_template = commod_wf_map_[waste_stream.second];
  if(chosen_wf_template == NULL){
    string err_msg = "The commodity '";
    err_msg += (waste_stream.second)->name();
    err_msg +="' does not have a matching WF in the GenericRepository.";
    throw CycException(err_msg);
  }
  // if there doesn't already exist a partially full one
  // @todo check for partially full wf's before creating new one (katyhuff)
  // create that waste form
  current_waste_forms_.push_back( new Component() );
  current_waste_forms_.back()->copy(chosen_wf_template);
  // and load in the waste stream
  current_waste_forms_.back()->absorb(waste_stream.first);
  return current_waste_forms_.back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::packageWaste(Component* waste_form){
  // figure out what waste package to put the waste form in
  bool loaded = false;
  Component* chosen_wp_template = NULL;
  string name = waste_form->name();
  chosen_wp_template = wf_wp_map_[name];
  if(chosen_wp_template == NULL){
    string err_msg = "The waste form '";
    err_msg += (waste_form)->name();
    err_msg +="' does not have a matching WP in the GenericRepository.";
    throw CycException(err_msg);
  }
  Component* toRet;
  // until the waste form has been loaded into a package
  while (!loaded){
    // look through the current waste packages 
    for (deque<Component*>::const_iterator iter= 
        current_waste_packages_.begin();
        iter != current_waste_packages_.end();
        iter++){
      // if there already exists an only partially full one of the right kind
      if( !(*iter)->isFull() && (*iter)->name() == 
          chosen_wp_template->name()){
        // fill it
        (*iter)->load(WP, waste_form);
        toRet = (*iter);
        loaded = true;
      } }
    // if no currently unfilled waste packages match, create a new waste package
    current_waste_packages_.push_back( new Component() );
    current_waste_packages_.back()->copy(chosen_wp_template);
    // and load in the waste form
    toRet = current_waste_packages_.back()->load(WP, waste_form); 
    loaded = true;
  }
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::loadBuffer(Component* waste_package){
  // figure out what buffer to put the waste package in
  Component* chosen_buffer = buffers_.front();
  // and load in the waste package
  buffers_.front()->load(BUFFER, waste_package);
  // put this on the stack of waste packages that have been emplaced
  emplaced_waste_packages_.push_back(waste_package);
  // set the location of the waste package 
  setPlacement(waste_package);
  waste_package->registerComponent();
  // set the location of the waste forms within the waste package
  std::vector<Component*> daughters = waste_package->getDaughters();
  for (std::vector<Component*>::iterator iter = daughters.begin();  
      iter != daughters.end(); 
      iter ++){
    setPlacement(*iter);
    (*iter)->registerComponent();
  }
  return buffers_.front();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::setPlacement(Component* comp){
  double x,y,z;
  // figure out what type of component it is
  switch(comp->getComponentType()) 
  {
    case FF :
      x = x_/2;
      y = y_/2;
      z = z_/2;
      break;
    case BUFFER :
      x = (buffers_.size()- .5)*dx_ ;
      y = y_/2 ; 
      z = dz_ ; 
      break;
    case WP :
      x = (comp->getParent())->getX();
      y = (emplaced_waste_packages_.size()*dy_ - dy_/2) ; 
      z = dz_ ; 
      break;
    case WF :
      x = (comp->getParent())->getX();
      y = (comp->getParent())->getY();
      z = (comp->getParent())->getZ();
      break;
    default :
      string err = "ComponentType, '";
      err += comp->getComponentType();
      err +="' is not a valid type for Component ";
      err += comp->name();
      err += ".";
      throw CycException(err);
  }
  // figure out what buffer to put the waste package in
  comp->setPlacement(x,y,z);
  return comp; 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::transportHeat(){
  // update the thermal BCs everywhere
  // pass the transport heat signal through the components, inner -> outer
  for( deque< Component* >::const_iterator iter = waste_forms_.begin();
      iter != waste_forms_.end();
      iter++){
    (*iter)->transportHeat();
  }
  for( deque< Component* >::const_iterator iter = waste_packages_.begin();
      iter != waste_packages_.end();
      iter++){
    (*iter)->transportHeat();
  }
  for( deque< Component* >::const_iterator iter = buffers_.begin();
      iter != buffers_.end();
      iter++){
    (*iter)->transportHeat();
  }
  far_field_->transportHeat();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::transportNuclides(){
  // update the nuclide transport BCs everywhere
  // pass the transport nuclides signal through the components, inner -> outer
  for( deque< Component* >::const_iterator iter = waste_forms_.begin();
      iter != waste_forms_.end();
      iter++){
    (*iter)->transportNuclides();
  }
  for( deque< Component* >::const_iterator iter = waste_packages_.begin();
      iter != waste_packages_.end();
      iter++){
    (*iter)->transportNuclides();
  }
  for( deque< Component* >::const_iterator iter = buffers_.begin();
      iter != buffers_.end();
      iter++){
    (*iter)->transportNuclides();
  }
  far_field_->transportNuclides();
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new GenericRepository();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

