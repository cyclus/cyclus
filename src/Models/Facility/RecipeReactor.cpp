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
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::init(xmlNodePtr cur)
{ 
  FacilityModel::init(cur);
  
  // set the current month in cycle to 1, it's the first month.
  month_in_cycle = 1;

  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/RecipeReactor");

  // initialize ordinary objects
  capacity = atof(XMLinput->get_xpath_content(cur,"capacity"));
  cycle_time = atof(XMLinput->get_xpath_content(cur,"cycletime"));
  lifetime = atoi(XMLinput->get_xpath_content(cur,"lifetime"));
  startConstrYr = atoi(XMLinput->get_xpath_content(cur,"startConstrYear"));
  startConstrMo = atoi(XMLinput->get_xpath_content(cur,"startConstrMonth"));
  startOpYr = atoi(XMLinput->get_xpath_content(cur,"startOperYear"));
  startOpMo = atoi(XMLinput->get_xpath_content(cur,"startOperMonth"));
  licExpYr = atoi(XMLinput->get_xpath_content(cur,"licExpYear"));
  licExpMo = atoi(XMLinput->get_xpath_content(cur,"licExpMonth"));
  state = XMLinput->get_xpath_content(cur,"state");
  typeReac = XMLinput->get_xpath_content(cur,"typeReac");
  CF = atof(XMLinput->get_xpath_content(cur,"elecCF"));

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
    commod_name = XMLinput->get_xpath_content(cur,"incommodity");
    in_commod = LI->getCommodity(commod_name);
    if (NULL == in_commod)
      throw GenException("Input commodity '" + commod_name 
          + "' does not exist for facility '" + getName() 
          + "'.");
    // get in_recipe
    recipe_name = XMLinput->get_xpath_content(cur,"inrecipe");
    in_recipe = LI->getRecipe(recipe_name);
    if (NULL == in_recipe)
      throw GenException("Recipe '" + recipe_name 
          + "' does not exist for facility '" + getName()
          + "'.");
    
    commod_name = XMLinput->get_xpath_content(cur,"outcommodity");
    out_commod = LI->getCommodity(commod_name);
    if (NULL == out_commod)
      throw GenException("Output commodity '" + commod_name 
          + "' does not exist for facility '" + getName() 
          + "'.");
    // get out_recipe
    recipe_name = XMLinput->get_xpath_content(cur,"outrecipe");
    out_recipe = LI->getRecipe(recipe_name);
    if (NULL == out_recipe)
      throw GenException("Recipe '" + recipe_name 
          + "' does not exist for facility '" + getName()
          + "'.");
    fuelPairs.push_back(make_pair(make_pair(in_commod,in_recipe),
          make_pair(out_commod, out_recipe)));
  };

  InFuel stocks;
  currCore = deque< pair<Commodity*, Material* > >();
  inventory = deque< pair<Commodity*, Material*> >();
  ordersWaiting = deque< Message*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeReactor::copy(RecipeReactor* src)
{

  FacilityModel::copy(src);

  fuelPairs = src->fuelPairs;
  capacity = src->capacity;
  cycle_time = src->cycle_time;
  lifetime = src->lifetime;
  month_in_cycle = src->month_in_cycle;
  startConstrYr = src->startConstrYr;
  startOpYr = src->startOpYr;
  startOpMo = src->startOpMo;
  licExpYr = src->licExpYr;
  licExpMo = src->licExpMo;
  state = src->state;
  typeReac = src->typeReac;
  CF = src->CF;


  stocks = deque<InFuel>();
  currCore = deque< pair<Commodity*, Material* > >();
  inventory = deque<OutFuel >();
  ordersWaiting = deque<Message*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::copyFreshModel(Model* src)
{
  copy((RecipeReactor*)(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::print() 
{ 
  FacilityModel::print(); 
  cout << "converts commodity {"
      << fuelPairs.front().first.first->getName()
      << "} into commodity {"
      << fuelPairs.front().first.second->getName()
      << "}, and has an inventory that holds " 
      << inventory_size << " materials"
      << endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::endCycle()
{
  // move a batch out of the core 
  Commodity* batchCommod = currCore.front().first;
  Material* batchMat = currCore.front().second;
  currCore.pop_front();

  // figure out the spent fuel commodity and material
  Commodity* outCommod;
  Material* outMat;

  bool found = false;
  while(!found){
    for(deque< pair<InFuel, OutFuel> >::iterator iter = fuelPairs.begin();
        iter != fuelPairs.end();
        iter++){
      if((*iter).first.first == batchCommod){
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
  inventory.push_back(outBatch);
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::beginCycle()
{
  // move stocks batch to currCore
  Commodity* batchCommod = stocks.front().first;
  Material* batchMat = stocks.front().second;
  stocks.pop_front();

  // move converted material into Inventory
  InFuel inBatch;
  inBatch = make_pair(batchCommod, batchMat);
  currCore.push_back(inBatch);

  // reset month_in_cycle clock
  month_in_cycle = 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::receiveMessage(Message* msg)
{
  // is this a message from on high? 
  if(msg->getSupplierID()==this->getSN()){
    // file the order
    ordersWaiting.push_front(msg);
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
  while(trans.amount > newAmt && !inventory.empty() ){
    for (deque<OutFuel>::iterator iter = inventory.begin(); 
        iter != inventory.end(); 
        iter ++){
      // be sure to get the right commodity
      if(iter->first == trans.commod){
        Material* m = iter->second;
        // start with an empty material
        Material* newMat = new Material(CompMap(), 
            m->getUnits(),
            m->getName(), 
            0, atomBased);
        // if the inventory obj isn't larger than the remaining need, send it as is.
        if(m->getTotMass() <= (trans.amount - newAmt)){
          newAmt += m->getTotMass();
          newMat->absorb(m);
          inventory.pop_front();
        }
        else{ 
          // if the inventory obj is larger than the remaining need, split it.
          Material* toAbsorb = m->extractMass(trans.amount - newAmt);
          newAmt += toAbsorb->getTotMass();
          newMat->absorb(toAbsorb);
        }
        toSend.push_back(newMat);
        cout<<"RecipeReactor "<< ID
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
    cout<<"RecipeReactor " << ID << " is receiving material with mass "
        << (*thisMat)->getTotMass() << endl;
    stocks.push_front(make_pair(trans.commod, *thisMat));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::handleTick(int time)
{
  // if at beginning of cycle, beginCycle()
  // if stocks are empty, ask for a batch
  // offer anything in the inventory
  
  // BEGIN CYCLE
  if(month_in_cycle == 1){
    this->beginCycle();
  };

  // MAKE A REQUEST
  if(stocks.front().first == NULL){
    // It chooses the next in/out commodity pair in the preference lineup
    InFuel request_commod_pair;
    OutFuel offer_commod_pair;
    request_commod_pair = fuelPairs.front().first;
    offer_commod_pair = fuelPairs.front().second;
    Commodity* in_commod = request_commod_pair.first;
    Material* in_recipe = request_commod_pair.second;

    // It then moves that pair from the front to the back of the preference lineup
    fuelPairs.push_back(make_pair(request_commod_pair, offer_commod_pair));
    fuelPairs.pop_front();
  
    // It can accept only a whole batch
    Mass requestAmt;
    Mass minAmt = 0; // KDHFLAG does this come from the facility or batch definition?
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
    else if (space < minAmt){
      Communicator* recipient = (Communicator*)(in_commod->getMarket());
      // if empty space is less than monthly acceptance capacity
      requestAmt = space;
      // recall that requests have a negative amount
      Message* request = new Message(up, in_commod, -requestAmt, minAmt, 
                                       commod_price, this, recipient);
        // pass the message up to the inst
        (request->getInst())->receiveMessage(request);
    }
    // otherwise, the upper bound is the batch size
    // minus the amount in stocks.
    else if (space >= capacity){
      Communicator* recipient = (Communicator*)(in_commod->getMarket());
      // if empty space is more than monthly acceptance capacity
      requestAmt = capacity - sto;
      // recall that requests have a negative amount
      Message* request = new Message(up, in_commod, -requestAmt, minAmt, commod_price,
                                     this, recipient); 
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
  for (deque<pair<Commodity*, Material* > >::iterator iter = inventory.begin(); 
       iter != inventory.end(); 
       iter ++){
    // get commod
    commod = iter->first;
    // decide what market to offer to
    recipient = (Communicator*)(commod->getMarket());
    // get amt
    offer_amt = iter->second->getTotMass();
    // create a message to go up to the market with these parameters
    Message* msg = new Message(up, commod, offer_amt, min_amt, commod_price, 
        this, recipient);
    // send it
    sendMessage(msg);
  };
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::handleTock(int time)
{
  // at the end of the cycle
  if (month_in_cycle > cycle_time){
    this->endCycle();
  };

  // check what orders are waiting, 
  while(!ordersWaiting.empty()){
    Message* order = ordersWaiting.front();
    sendMaterial(order, ((Communicator*)LI->getFacilityByID(order->getRequesterID())));
    ordersWaiting.pop_front();
  };
  month_in_cycle++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass RecipeReactor::checkInventory(){
  Mass total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  for (deque< pair<Commodity*, Material*> >::iterator iter = inventory.begin(); 
       iter != inventory.end(); 
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


  for (deque< pair<Commodity*, Material*> >::iterator iter = stocks.begin(); 
       iter != stocks.end(); 
       iter ++){
    total += (*iter).second->getTotMass();
  }

  return total;
}


