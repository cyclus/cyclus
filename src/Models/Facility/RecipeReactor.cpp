// RecipeReactor.cpp
// Implements the RecipeReactor class
#include <iostream>

#include "RecipeReactor.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"
#include "Timer.h"

/*
 * TICK
 * if stocks are empty, ask for a batch
 * offer anything in the inventory
 * if we're at the end of a cycle
 *    - begin the cycle
 *      - move currCore batch to inventory
 *      - move stocks batch to currCore
 *      - reset month_in_cycle clock
 *
 * TOCK
 * advance month_in_cycle
 * send appropriate materials to fill ordersWaiting.
 *
 * RECIEVE MATERIAL
 * put it in stocks
 *
 * SEND MATERIAL
 * pull it from inventory
 *
 * 
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::init(xmlNodePtr cur)
{ 
  FacilityModel::init(cur);

  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/RecipeReactor");

  // initialize ordinary objects
  capacity_ = atof(XMLinput->get_xpath_content(cur,"capacity"));
  setMemberVar("capacity_",&capacity_);
  //cycle_time_ = atoi(XMLinput->get_xpath_content(cur,"cycleTime"));
  //setMemberVar("cycle_time_",&cycle_time_);
  start_constr_yr_ = atoi(XMLinput->get_xpath_content(cur,"startConstrYear"));
  setMemberVar("start_constr_yr_",&start_constr_yr_);
  start_constr_mo_ = atoi(XMLinput->get_xpath_content(cur,"startConstrMonth"));
  setMemberVar("start_constr_mo_",&start_constr_mo_);
  start_op_yr_ = atoi(XMLinput->get_xpath_content(cur,"startOperYear"));
  setMemberVar("start_op_yr_",&start_op_yr_);
  start_op_mo_ = atoi(XMLinput->get_xpath_content(cur,"startOperMonth"));
  setMemberVar("start_op_mo_",&start_op_mo_);
  lic_exp_yr_ = atoi(XMLinput->get_xpath_content(cur,"licExpYear"));
  setMemberVar("lic_exp_yr_",&lic_exp_yr_);
  lic_exp_mo_ = atoi(XMLinput->get_xpath_content(cur,"licExpMonth"));
  setMemberVar("lic_exp_mo_",&lic_exp_mo_);
  state_ = XMLinput->get_xpath_content(cur,"state");
  setMemberVar("state_",&state_);
  type_reac_ = XMLinput->get_xpath_content(cur,"typeReac");
  setMemberVar("type_reac_",&type_reac_);
  CF_ = atof(XMLinput->get_xpath_content(cur,"elecCF"));
  setMemberVar("CF_",&CF_);

  // all facilities require commodities - possibly many
  string commod_name;
  string recipe_name;
  Commodity* in_commod;
  Commodity* out_commod;
  Material* in_recipe;
  Material* out_recipe;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "fuelpair");

  // for each fuel pair, there is an in and an out commodity
  for (int i=0;i<nodes->nodeNr;i++){
    xmlNodePtr pair_node = nodes->nodeTab[i];
    in_commod = out_commod = NULL;
    in_recipe = out_recipe = NULL; 

    // get in_commod
    commod_name = XMLinput->get_xpath_content(pair_node,"incommodity");
    in_commod = LI->getCommodity(commod_name);
    if (NULL == in_commod)
      throw GenException("Input commodity '" + commod_name 
          + "' does not exist for facility '" + getName() 
          + "'.");
    // get in_recipe
    recipe_name = XMLinput->get_xpath_content(pair_node,"inrecipe");
    in_recipe = LI->getRecipe(recipe_name);
    if (NULL == in_recipe)
      throw GenException("Recipe '" + recipe_name 
          + "' does not exist for facility '" + getName()
          + "'.");
    
    commod_name = XMLinput->get_xpath_content(pair_node,"outcommodity");
    out_commod = LI->getCommodity(commod_name);
    if (NULL == out_commod)
      throw GenException("Output commodity '" + commod_name 
          + "' does not exist for facility '" + getName() 
          + "'.");
    // get out_recipe
    recipe_name = XMLinput->get_xpath_content(pair_node,"outrecipe");
    out_recipe = LI->getRecipe(recipe_name);
    if (NULL == out_recipe)
      throw GenException("Recipe '" + recipe_name 
          + "' does not exist for facility '" + getName()
          + "'.");
    fuel_pairs_.push_back(make_pair(make_pair(in_commod,in_recipe),
          make_pair(out_commod, out_recipe)));
  };

  setMemberVar("fuel_pairs_",&fuel_pairs_);

  this->init(member_var_map_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::init(map<string, void*> member_var_map)
{ 
  // set the member variable map to this one
  member_var_map_ = member_var_map;

  // send the init signal upward
  FacilityModel::init(member_var_map);
  
  // set the current month in cycle to 1, it's the first month.
  month_in_cycle_ = 1;
  cycle_time_ = 3;

  // initialize ordinary objects
  capacity_ = getMapVar<double>("capacity_",member_var_map);
  lifetime_ = getMapVar<double>("lifetime_",member_var_map);
  start_constr_yr_ = getMapVar<int>("start_constr_yr_",member_var_map);
  start_constr_mo_ = getMapVar<int>("start_constr_mo_",member_var_map);
  start_op_yr_ = getMapVar<int>("start_op_yr_",member_var_map);
  start_op_mo_ = getMapVar<int>("start_op_mo_",member_var_map);
  lic_exp_yr_ = getMapVar<int>("lic_exp_yr_",member_var_map);
  lic_exp_mo_ = getMapVar<int>("lic_exp_mo_",member_var_map);
  state_ = getMapVar<string>("state_",member_var_map);
  type_reac_ = getMapVar<string>("type_reac_",member_var_map);
  CF_ = getMapVar<double>("CF_",member_var_map);
  fuel_pairs_ = getMapVar<deque< pair< pair< Commodity*, Material*> , pair< Commodity*, Material*> > > >("fuel_pairs_", member_var_map);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeReactor::copy(RecipeReactor* src)
{

  FacilityModel::copy(src);

  fuel_pairs_ = src->fuel_pairs_;
  capacity_ = src->capacity_;
  cycle_time_ = src->cycle_time_;
  lifetime_ = src->lifetime_;
  month_in_cycle_ = src->month_in_cycle_;
  start_constr_yr_ = src->start_constr_yr_;
  start_op_yr_ = src->start_op_yr_;
  start_op_mo_ = src->start_op_mo_;
  lic_exp_yr_ = src->lic_exp_yr_;
  lic_exp_mo_ = src->lic_exp_mo_;
  state_ = src->state_;
  type_reac_ = src->type_reac_;
  CF_ = src->CF_;


  stocks_ = deque<InFuel>();
  currCore_ = deque< pair<Commodity*, Material* > >();
  inventory_ = deque<OutFuel >();
  ordersWaiting_ = deque<Message*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::copyFreshModel(Model* src)
{
  copy(dynamic_cast<RecipeReactor*>(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::print() 
{ 
  FacilityModel::print(); 
  cout << "converts commodity {"
      << this->fuel_pairs_.front().first.first->getName()
      << "} into commodity {"
      << this->fuel_pairs_.front().second.first->getName()
      << "}."  << endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::beginCycle()
{
  if( !stocks_.empty() ){
    // move stocks batch to currCore
    Commodity* batchCommod = stocks_.front().first;
    Material* batchMat = stocks_.front().second;
    stocks_.pop_front();
    InFuel inBatch;
    inBatch = make_pair(batchCommod, batchMat);
    currCore_.push_back(inBatch);
    // reset month_in_cycle_ clock
    month_in_cycle_ = 1;
  }
  else{
    // wait for a successful transaction to fill the stocks.
    // reset the cycle month to zero 
    month_in_cycle_=0;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::endCycle()
{
  // move a batch out of the core 
  Commodity* batchCommod = currCore_.front().first;
  Material* batchMat = currCore_.front().second;
  currCore_.pop_front();

  // figure out the spent fuel commodity and material
  Commodity* outCommod;
  Material* outMat;

  bool found = false;
  while(!found){
    for(deque< pair< InFuel , OutFuel> >::iterator iter = fuel_pairs_.begin();
        iter != fuel_pairs_.end();
        iter++){
      if((*iter).first.first->getName() == batchCommod->getName()){
        outCommod = (*iter).second.first;
        outMat = (*iter).second.second;
        found=true;
      };
    };
  };

  // change the composition to the compositon of the spent fuel type
  batchMat->changeAtomComp(outMat->getAtomComp(), TI->getTime());

  // move converted material into Inventory
  OutFuel outBatch;
  outBatch = make_pair(outCommod, batchMat);
  inventory_.push_back(outBatch);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::receiveMessage(Message* msg)
{
  // is this a message from on high? 
  if(msg->getSupplierID()==this->getSN()){
    // file the order
    ordersWaiting_.push_front(msg);
  }
  else {
    throw GenException("RecipeReactor is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::sendMaterial(Message* msg, const Communicator* requester)
{
  Transaction trans = msg->getTrans();

  Mass newAmt = 0;


  // start with an empty manifest
  vector<Material*> toSend;

  // pull materials off of the inventory stack until you get the trans amount
  while(trans.amount > newAmt && !inventory_.empty() ){
    for (deque<OutFuel>::iterator iter = inventory_.begin(); 
        iter != inventory_.end(); 
        iter ++){
      // be sure to get the right commodity
      if(iter->first == trans.commod){
        Material* m = iter->second;
        // start with an empty material
        Material* newMat = new Material(CompMap(), 
            m->getUnits(),
            m->getName(), 
            0, ATOMBASED);
        // if the inventory obj isn't larger than the remaining need, send it as is.
        if(m->getTotMass() <= (trans.amount - newAmt)){
          newAmt += m->getTotMass();
          newMat->absorb(m);
          inventory_.pop_front();
        }
        else{ 
          // if the inventory obj is larger than the remaining need, split it.
          Material* toAbsorb = m->extractMass(trans.amount - newAmt);
          newAmt += toAbsorb->getTotMass();
          newMat->absorb(toAbsorb);
        }
        toSend.push_back(newMat);
        cout<<"RecipeReactor "<< getSN()
          <<"  is sending a mat with mass: "<< newMat->getTotMass()<< endl;
      }
    }
  }    
  FacilityModel::sendMaterial(msg, toSend);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::receiveMaterial(Transaction trans, vector<Material*> manifest)
{
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    cout<<"RecipeReactor " << getSN() << " is receiving material with mass "
        << (*thisMat)->getTotMass() << endl;
    stocks_.push_front(make_pair(trans.commod, *thisMat));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::handleTick(int time)
{
  // if at beginning of cycle, beginCycle()
  // if stocks are empty, ask for a batch
  // offer anything in the inventory
  
  // BEGIN CYCLE
  if(month_in_cycle_ == 1){
    this->beginCycle();
  };

  // MAKE A REQUEST
  if(this->checkStocks() == 0){
    // if(stocks.front().first == NULL){
    // It chooses the next in/out commodity pair in the preference lineup
    InFuel request_commod_pair;
    OutFuel offer_commod_pair;
    request_commod_pair = fuel_pairs_.front().first;
    offer_commod_pair = fuel_pairs_.front().second;
    Commodity* in_commod = request_commod_pair.first;
    Material* in_recipe = request_commod_pair.second;

    // It then moves that pair from the front to the back of the preference lineup
    fuel_pairs_.push_back(make_pair(request_commod_pair, offer_commod_pair));
    fuel_pairs_.pop_front();
  
    // It can accept only a whole batch
    Mass requestAmt;
    Mass minAmt = in_recipe->getTotMass();
    // The Recipe Reactor should ask for an batch if there isn't one in stock.
    Mass sto = this->checkStocks(); 
    // subtract sto from batch size to get total empty space. 
    // Hopefully the result is either 0 or the batch size 
    Mass space = minAmt - sto; // KDHFLAG get minAmt from the input ?
    // this will be a request for free stuff
    double commod_price = 0;
  
    if (space == 0){
      // don't request anything
    }
    else if (space <= minAmt){
      Communicator* recipient = dynamic_cast<Communicator*>(in_commod->getMarket());
      // if empty space is less than monthly acceptance capacity
      requestAmt = space;
      // recall that requests have a negative amount
      Message* request = new Message(UP_MSG, in_commod, -requestAmt, minAmt,
                                       commod_price, this, recipient);
        // pass the message up to the inst
        (request->getInst())->receiveMessage(request);
    }
    // otherwise, the upper bound is the batch size
    // minus the amount in stocks.
    else if (space >= minAmt){
      Communicator* recipient = dynamic_cast<Communicator*>(in_commod->getMarket());
      // if empty space is more than monthly acceptance capacity
      requestAmt = capacity_ - sto;
      // recall that requests have a negative amount
      Message* request = new Message(UP_MSG, in_commod, -requestAmt, minAmt,
                                      commod_price, this, recipient); 
      // pass the message up to the inst
      (request->getInst())->receiveMessage(request);
    }
  }

  // MAKE OFFERS
  // decide how much to offer

  // there is no minimum amount a null facility may send
  double min_amt = 0;
  // this will be an offer of free stuff
  double commod_price = 0;

  // there are potentially many types of batch in the inventory stack
  Mass inv = this->checkInventory();
  // send an offer for each material on the stack 
  Material* m;
  Commodity* commod;
  Communicator* recipient;
  Mass offer_amt;
  for (deque<pair<Commodity*, Material* > >::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter ++){
    // get commod
    commod = iter->first;
    // decide what market to offer to
    recipient = dynamic_cast<Communicator*>(commod->getMarket());
    // get amt
    offer_amt = iter->second->getTotMass();
    // create a message to go up to the market with these parameters
    Message* msg = new Message(UP_MSG, commod, offer_amt, min_amt, 
                                commod_price, this, recipient);
    // send it
    sendMessage(msg);
  };
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::handleTock(int time)
{
  // at the end of the cycle
  if (month_in_cycle_ > cycle_time_){
    this->endCycle();
  };

  // check what orders are waiting, 
  while(!ordersWaiting_.empty()){
    Message* order = ordersWaiting_.front();
    sendMaterial(order, dynamic_cast<Communicator*>(LI->getModelByID(order->getRequesterID(), FACILITY)));
    ordersWaiting_.pop_front();
  };
  month_in_cycle_++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass RecipeReactor::checkInventory(){
  Mass total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  for (deque< pair<Commodity*, Material*> >::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter ++){
    total += (*iter).second->getTotMass();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass RecipeReactor::checkStocks(){
  Mass total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.

  if(!stocks_.empty()){
    for (deque< pair<Commodity*, Material*> >::iterator iter = stocks_.begin(); 
         iter != stocks_.end(); 
         iter ++){
        total += (*iter).second->getTotMass();
    };
  };
  return total;
}


