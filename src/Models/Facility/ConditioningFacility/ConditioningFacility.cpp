// ConditioningFacility.cpp
// Implements the ConditioningFacility class
#include <iostream>
#include <fstream>

#include "ConditioningFacility.h"

#include "CycException.h"
#include "Env.h"
#include "InputXML.h"
#include "Logger.h"
#include "Material.h"
#include "GenericResource.h"
#include "MarketModel.h"
#include "Timer.h"

using namespace std;

/**
  ConditioningFacility matches waste streams with 
  waste form loading densities and waste form 
  material definitions. 
  
  It attaches these properties to the material objects 
  as Material Properties.
 
  TICK
  On the tick, the ConditioningFacility makes requests for each 
  of the waste commodities (streams) that its table addresses
  It also makes offers of any conditioned waste it contains
 
  TOCK
  On the tock, the ConditioningFacility first prepares 
  transactions by preparing and sending material from its stocks
  all material in its stocks up to its monthly processing 
  capacity.
 
  RECEIVE MATERIAL
  Puts the material received in the stocks
 
  SEND MATERIAL
  Sends material from inventory to fulfill transactions
 
 */

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Database table for conditioned materials
table_ptr ConditioningFacility::cond_fac_table = new Table("ConditionedResources"); 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ConditioningFacility::ConditioningFacility() {
    // Initialize allowed formats map
    allowed_formats_.insert(make_pair("sql", &ConditioningFacility::loadSQLFile)); 
    allowed_formats_.insert(make_pair("xml", &ConditioningFacility::loadXMLFile)); 
    allowed_formats_.insert(make_pair("csv", &ConditioningFacility::loadCSVFile)); 
    stocks_ = deque<pair<string, mat_rsrc_ptr> >();
    inventory_ = deque<pair<string, mat_rsrc_ptr> >();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ConditioningFacility::~ConditioningFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConditioningFacility::init(xmlNodePtr cur)
{
    FacilityModel::init(cur);

    /// move XML pointer to current model
    cur = XMLinput->get_xpath_element(cur,"model/ConditioningFacility");

    capacity_ = strtod(XMLinput->get_xpath_content(cur, "capacity"), NULL);
    remaining_capacity_= capacity_;

    /// initialize any ConditioningFacility-specific datamembers here
    string datafile = XMLinput->get_xpath_content(cur, "datafile");
    string fileformat = XMLinput->get_xpath_content(cur, "fileformat");

    // all facilities require commodities - possibly many
    string in_commod, out_commod;
    int commod_id;
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "commodset");

    // for each commod pair
    for (int i = 0; i < nodes->nodeNr; i++){
      xmlNodePtr pair_node = nodes->nodeTab[i];

      // get name and id
      in_commod = XMLinput->get_xpath_content(pair_node,"incommodity");
      out_commod = XMLinput->get_xpath_content(pair_node,"outcommodity");
      commod_id = strtol(XMLinput->get_xpath_content(pair_node,"id"), NULL, 10);
      commod_map_.insert(make_pair( in_commod, make_pair(commod_id, out_commod)) ); 
    };

    loadTable(datafile, fileformat);
    file_is_open_ = false;

    // we haven't conditioned anything yet
    current_cond_rsrc_id_ = -1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConditioningFacility::copy(ConditioningFacility* src)
{
    FacilityModel::copy(src);

    allowed_formats_ = src->allowed_formats_;
    capacity_ = src->capacity_;
    remaining_capacity_ = capacity_;
    file_is_open_ = src->file_is_open_;
    commod_map_ = src->commod_map_;
    stream_vec_ = src->stream_vec_;
    loading_densities_ = src->loading_densities_;
    current_cond_rsrc_id_ = -1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConditioningFacility::copyFreshModel(Model* src)
{
  copy(dynamic_cast<ConditioningFacility*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConditioningFacility::print() 
{ 
    FacilityModel::print();
    string incommods, outcommods;
    map<string, pair<int, string> >::const_iterator it;
    for(it = commod_map_.begin(); it != commod_map_.end(); it++){
      incommods += (*it).first;
      incommods += ", ";
      outcommods += (*it).second.second;
      outcommods += ", ";
    }
    LOG(LEV_DEBUG2, "none!") << " conditions {" 
      << incommods
      <<"} into forms { "
      << outcommods
      << " }.";
};

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::receiveMessage(msg_ptr msg) {
  LOG(LEV_DEBUG2, "none!") << "Warning, the ConditioningFacility ignores messages.";
};

/* ------------------- */ 


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<rsrc_ptr> ConditioningFacility::removeResource(msg_ptr order) {
  vector<rsrc_ptr> toRet = vector<rsrc_ptr>() ;
  Transaction trans = order->trans();
  double order_amount = trans.resource->quantity()*trans.minfrac;
  if (remaining_capacity_ >= order_amount){
    toRet = processOrder(order);
  } else { 
    string msg;
    msg += "The ConditioningFacility has run out of processing capacity. ";
    msg += "The order requested by ";
    msg += order->requester()->name();
    msg += " will not be sent.";
    LOG(LEV_DEBUG2, "none!") << msg;
    gen_rsrc_ptr empty = gen_rsrc_ptr(new GenericResource("kg","kg",0));
    toRet.push_back(empty);
  }
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<rsrc_ptr> ConditioningFacility::processOrder(msg_ptr order) {
 // Send material from inventory to fulfill transactions

  Transaction trans = order->trans();

  double newAmt = 0;

  // pull materials off of the inventory stack until you get the transaction amount

  // start with an empty manifest
  vector<rsrc_ptr> toSend;

  while(trans.resource->quantity() > newAmt && !inventory_.empty() ) {
    mat_rsrc_ptr m = inventory_.front().second;

    // start with an empty material
    mat_rsrc_ptr newMat = mat_rsrc_ptr(new Material());

    // if the inventory obj isn't larger than the remaining need, send it as is.
    if(m->quantity() <= (trans.resource->quantity() - newAmt)) {
      newAmt += m->quantity();
      newMat->absorb(m);
      inventory_.pop_front();
      remaining_capacity_ = remaining_capacity_ - newAmt;
    } else { 
      // if the inventory obj is larger than the remaining need, split it.
      mat_rsrc_ptr toAbsorb = m->extract(trans.resource->quantity() - newAmt);
      newMat->absorb(toAbsorb);
      newAmt += toAbsorb->quantity();
      remaining_capacity_ = remaining_capacity_ - newAmt;
    }

    toSend.push_back(newMat);
    LOG(LEV_DEBUG2, "none!") <<"ConditioningFacility "<< ID()
      <<"  is sending a mat with mass: "<< newMat->quantity();
  }    
  return toSend;
};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::addResource(msg_ptr msg, vector<rsrc_ptr> manifest) {
  // Put the material received in the stocks
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<rsrc_ptr>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++) {
    LOG(LEV_DEBUG2, "none!") <<"ConditiondingFacility " << ID() << " is receiving material with mass "
        << (*thisMat)->quantity();

    mat_rsrc_ptr mat = boost::dynamic_pointer_cast<Material>(*thisMat);
    stocks_.push_front(make_pair(msg->trans().commod, mat));
  } 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::handleTick(int time){
  // TICK
  // On the tick, the ConditioningFacility makes requests for each 
  // of the waste commodities (streams) that its table addresses
  // It also makes offers of any conditioned waste it contains
  remaining_capacity_ = capacity_;
  makeRequests();
  makeOffers();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::handleTock(int time){
  // TOCK
  // On the tock, the ConditioningFacility first prepares 
  // all material in its stocks up to its monthly processing 
  // capacity.
  conditionMaterials();
  printStatus(time);
};


/* --------------------
 * this FACILITYMODEL class has these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::loadTable(string datafile, string fileformat){
  // check that the format is supported
  // if not, throw an exception
  bool table_okay = verifyTable(datafile, fileformat);

  // it will be a matrix
  // rows/records = waste stream commodities
  // columns/fields = waste forms
  // data = loading, grams per m^3
  // fill the data table
  map<string, void(ConditioningFacility::*)(string)>::iterator format_found;
  format_found = allowed_formats_.find(fileformat);
  (this->*format_found->second)(datafile);

  // match it with commodities
  // verify number of commodities matches number of streams
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ConditioningFacility::verifyTable(string datafile, string fileformat){
  // start by assuming the table is bad
  bool okay = false;
  // if the fileformat is supported
  map<string, void(ConditioningFacility::*)(string)>::iterator found = allowed_formats_.find(fileformat);
  if (found != allowed_formats_.end()){
    okay = true;
  } else {
    string err = "The file format, ";
    err += fileformat;
    err += ", is not supported by the Conditioning Facility.\n";
    throw CycException(err);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::loadXMLFile(string datafile){
  // get dimensions
  // // how many rows
  // // how many columns
  // create array
  throw CycException("XML-based ConditioningFacility input is not yet supported.");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::loadSQLFile(string datafile){
  // get dimensions
  // // how many rows
  // // how many columns
  // create array
  throw CycException("SQL-based ConditioningFacility input is not yet supported.");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::loadCSVFile(string datafile){
  string file_path = Env::getCyclusPath() + "/" + datafile; 

  // create an ifstream for the file
  ifstream file(file_path.c_str());

  // and data structures to read the data into
  string buffer;
  stream_t stream;

  if (file.is_open()){
    while(file.good()){
      // set the buffer string equal to everything up to the next comma 
      getline(file, buffer, ',');
      // copy the data to the typed member of the stream struct
      stream.streamID = strtol(buffer.c_str() , NULL, 10);
      LOG(LEV_DEBUG2, "none!") <<  "streamID  = " <<  stream.streamID ;;
      getline(file, buffer, ',');
      stream.formID = strtol(buffer.c_str() , NULL, 10);
      LOG(LEV_DEBUG2, "none!") <<  "formID  = " <<  stream.formID ;;
      getline(file, buffer, ',');
      stream.density = strtod(buffer.c_str() , NULL);
      LOG(LEV_DEBUG2, "none!") <<  "density  = " <<  stream.density ;;
      getline(file, buffer, ',');
      stream.wfvol = strtod(buffer.c_str() , NULL);
      LOG(LEV_DEBUG2, "none!") <<  "wfvol  = " <<  stream.wfvol ;;
      getline(file, buffer, '\n');
      stream.wfmass = strtod(buffer.c_str() , NULL);
      LOG(LEV_DEBUG2, "none!") <<  "wfmass  = " <<  stream.wfmass ;;
      // put the full struct into the vector of structs
      stream_vec_.push_back(stream);
    }
    file.close();
  }
  else {
    string err = "CSV file, ";
    err += file_path;
    err += ", not found.";
    throw CycException(err);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::makeRequests(){
  // The ConditioningFacility should make requests of all of the things it's 
  // capable of conditioning for each stream in the matrix
  // calculate your capacity to condition
  // MAKE A REQUEST
  if(this->checkStocks() <= remaining_capacity_){
    // It chooses the next in/out commodity pair in the preference lineup
    map<string, pair<int, string> >::const_iterator it; 
    for(it = commod_map_.begin(); it != commod_map_.end(); it++){
      string in_commod = (*it).first;
      int in_id = (*it).second.first;
      double requestAmt;
      double minAmt = 0;

      // The ConditioningFacility should ask for whatever it can process .
      double sto = this->checkStocks(); 
      // subtract sto from remaining_capacity_ to get total empty space. 
      double space = remaining_capacity_ - sto; 

      // this will be a request for free stuff
      double commod_price = 0;

      MarketModel* market = MarketModel::marketForCommod(in_commod);
      Communicator* recipient = dynamic_cast<Communicator*>(market);

      // if empty space is less than monthly acceptance capacity
      requestAmt = space;

      // request a generic resource
      gen_rsrc_ptr request_res = gen_rsrc_ptr(new GenericResource(in_commod, "kg", requestAmt));

      // build the transaction and message
      Transaction trans;
      trans.commod = in_commod;
      trans.minfrac = minAmt/requestAmt;
      trans.is_offer = false;
      trans.price = commod_price;
      trans.resource = request_res;

      sendMessage(recipient, trans);
      LOG(LEV_DEBUG2, "none!") << " The ConditioningFacility has requested "
        << requestAmt 
        << " kg of "
        << in_commod 
        << ".";
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::makeOffers(){
  // Offer anything that has been conditioned or can be conditioned this month.
  // this will be an offer of free stuff
  double commod_price = 0;

  // there are potentially many types of material in the inventory stack
  double inv = this->checkInventory();
  // send an offer for each material on the stack 
  string outcommod, offers;
  Communicator* recipient;
  double offer_amt;
  for (deque< pair<string, mat_rsrc_ptr > >::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter++){
    // get out commod
    outcommod = iter->first;
    MarketModel* market = MarketModel::marketForCommod(outcommod);
    // decide what market to offer to
    recipient = dynamic_cast<Communicator*>(market);
    // get amt
    offer_amt = iter->second->quantity();

    // make a material to offer
    mat_rsrc_ptr offer_mat = iter->second;
    offer_mat->setQuantity(offer_amt);

    // build the transaction and message
    Transaction trans;
    trans.commod = outcommod;
    trans.minfrac = 1;
    trans.is_offer = true;
    trans.price = commod_price;
    trans.resource = offer_mat;

    sendMessage(recipient, trans);

    offers += offer_mat->quantity();
    offers += " kg ";
    offers += outcommod;
    if(inventory_.end()!=iter){ 
      offers += " , ";
    }
  }
  LOG(LEV_DEBUG2, "none!") << " The ConditioningFacility has offered "
    << offers 
    << ".";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConditioningFacility::sendMessage(Communicator* recipient, Transaction trans){
      msg_ptr msg(new Message(this, recipient, trans)); 
      msg->setNextDest(facInst());
      msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double ConditioningFacility::checkInventory(){
  double total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  for (deque< pair<string, mat_rsrc_ptr> >::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter ++){
    total += iter->second->quantity();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double ConditioningFacility::checkStocks(){
  double total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.

  if(!stocks_.empty()){
    for (deque< pair<string, mat_rsrc_ptr> >::iterator iter = stocks_.begin(); 
         iter != stocks_.end(); 
         iter ++){
        total += iter->second->quantity();
    };
  };
  return total;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::conditionMaterials(){

  LOG(LEV_INFO3, "CondFac ") << facName() << " is conditioning {";
  // Partition the in-stream according to loading density
  // for each stream object in stocks
  map<string, mat_rsrc_ptr> remainders;
  while( !stocks_.empty() && remaining_capacity_ > 0){
    // move stocks to currMat
    string currCommod = stocks_.front().first;
    mat_rsrc_ptr currMat = stocks_.front().second;
    if(remainders.find(currCommod)!=remainders.end()){
      (remainders[currCommod])->absorb(condition(currCommod, currMat));
    } else {
      remainders.insert(make_pair(currCommod, condition(currCommod, currMat)));
    }
    stocks_.pop_front();
  }
  map<string, mat_rsrc_ptr>::const_iterator rem;
  for(rem=remainders.begin(); rem!=remainders.end(); rem++){
      stocks_.push_back(*rem);
  }

  // log the fact that we just conditioned stuff
  current_cond_rsrc_id_ = TI->time();
  addToTable();
 
  LOG(LEV_INFO3, "none!c ") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr ConditioningFacility::condition(string commod, mat_rsrc_ptr mat){
  stream_t stream = getStream(commod);
  double mass_to_condition = stream.wfmass;
  double mass_remaining = mat->quantity();
  string out_commod;
  while( mass_remaining > mass_to_condition && remaining_capacity_ > mass_to_condition) {
    out_commod = commod_map_.find(commod)->second.second;
    inventory_.push_back(make_pair(out_commod, mat->extract(mass_to_condition)));
    // mat_to_condition->absorb(wf_iso_vec_[commod]); ^^
    remaining_capacity_ = remaining_capacity_ - mass_to_condition;
    mass_remaining = mat->quantity();
  }
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ConditioningFacility::stream_t ConditioningFacility::getStream(string commod){
  int stream_id =(commod_map_.find(commod))->second.first;
  vector<stream_t>::const_iterator it = stream_vec_.begin();
  bool found = false;
  stream_t toRet;
  while(!found){
    if((*it).streamID == stream_id){
      toRet = (*it);
      found = true;
    } else if (it == stream_vec_.end() ) {
      string msg;
      msg += "The stream for id ";
      msg += stream_id;
      msg += " was not found in the ConditioningFacility stream map.";
      throw CycException(msg);
    } else {
      it++;
    }
  }
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::printStatus(int time){
  // For now, lets just print out what we have at each timestep.
  LOG(LEV_DEBUG2, "none!") << "ConditioningFacility " << this->ID()
                  << " is holding " << this->checkInventory()
                  << " units of material in inventory, and  "
                  << this->checkStocks() 
                  << " in stocks at the close of month " 
                  << time
                  << ".";
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::define_table() {
  // declare the table columns
  vector<column> columns;
  columns.push_back(make_pair("ID","INTEGER"));
  columns.push_back(make_pair("Time","INTEGER"));
  columns.push_back(make_pair("ConditionedRsrcID","INTEGER"));
  // declare the table's primary key
  primary_key pk;
  pk.push_back("ID"), pk.push_back("ConditionedRsrcID");
  cond_fac_table->defineTable(columns,pk);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::addToTable(){
  // if we haven't logged some conditioned material yet, define the table
  if ( !cond_fac_table->defined() ) {
    ConditioningFacility::define_table();
  }
  // make a row
  // declare data
  data an_id( this->ID() ), a_time( TI->time() ), 
    a_rsrc_id( this->current_cond_rsrc_id_ );
  // declare entries
  entry id("ID",an_id), time("Time",a_time), rid("ConditionedRsrcID",a_rsrc_id);
  // declare row
  row aRow;
  aRow.push_back(id), aRow.push_back(time), aRow.push_back(rid);
  // add the row
  cond_fac_table->addRow(aRow);
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructConditioningFacility() {
  return new ConditioningFacility();
}

/* ------------------- */ 

