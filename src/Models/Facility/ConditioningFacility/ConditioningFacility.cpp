// ConditioningFacility.cpp
// Implements the ConditioningFacility class
#include <iostream>
#include <fstream>

#include "ConditioningFacility.h"

#include "CycException.h"
#include "Env.h"
#include "InputXML.h"
#include "hdf5.h"
#include "H5Cpp.h"
#include "H5Exception.h"
#include "Logger.h"
#include "Material.h"

using namespace H5;

/**
 * ConditioningFacility matches waste streams with 
 * waste form loading densities and waste form 
 * material definitions. 
 * 
 * It attaches these properties to the material objects 
 * as Material Properties.
 *
 * TICK
 * On the tick, the ConditioningFacility makes requests for each 
 * of the waste commodities (streams) that its table addresses
 * It also makes offers of any conditioned waste it contains
 *
 * TOCK
 * On the tock, the ConditioningFacility first prepares 
 * transactions by preparing and sending material from its stocks
 * all material in its stocks up to its monthly processing 
 * capacity.
 *
 * RECEIVE MATERIAL
 * Puts the material received in the stocks
 *
 * SEND MATERIAL
 * Sends material from inventory to fulfill transactions
 *
 */

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ConditioningFacility::ConditioningFacility() {
    // Initialize allowed formats map
    allowed_formats_.insert(make_pair("hdf5", &ConditioningFacility::loadHDF5File)); 
    allowed_formats_.insert(make_pair("sql", &ConditioningFacility::loadSQLFile)); 
    allowed_formats_.insert(make_pair("xml", &ConditioningFacility::loadXMLFile)); 
    allowed_formats_.insert(make_pair("csv", &ConditioningFacility::loadCSVFile)); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ConditioningFacility::~ConditioningFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConditioningFacility::init(xmlNodePtr cur)
{
    FacilityModel::init(cur);

    /// move XML pointer to current model
    cur = XMLinput->get_xpath_element(cur,"model/ConditioningFacility");

    /// initialize any ConditioningFacility-specific datamembers here
    string datafile = XMLinput->get_xpath_content(cur, "datafile");
    string fileformat = XMLinput->get_xpath_content(cur, "fileformat");

    // all facilities require commodities - possibly many
    std::string commod;
    int commod_id;
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "commodpair");

    // for each fuel pair, there is an in and an out commodity
    for (int i = 0; i < nodes->nodeNr; i++){
      xmlNodePtr pair_node = nodes->nodeTab[i];

      // get commods
      commod = XMLinput->get_xpath_content(pair_node,"commodity");

      // get in_recipe
      commod_id = strtol(XMLinput->get_xpath_content(pair_node,"id"), NULL, 10);

      commod_map_[commod] = commod_id ; // @todo check if commod is already registered
    };

    // for whatever format,
    // call loadTable
    loadTable(datafile, fileformat);
    file_is_open_ = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConditioningFacility::copy(ConditioningFacility* src)
{
    FacilityModel::copy(src);

    allowed_formats_ = src->allowed_formats_;
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
    LOG(LEV_DEBUG2) << " conditions waste streams into waste forms";
};


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::receiveMessage(Message* msg) {
  LOG(LEV_DEBUG2) << "Warning, the ConditioningFacility ignores messages.";

};


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<Resource*> ConditioningFacility::removeResource(Message* order) {
 // Send material from inventory to fulfill transactions

  Transaction trans = order->trans();

  double newAmt = 0;

  // pull materials off of the inventory stack until you get the transaction amount

  // start with an empty manifest
  vector<Resource*> toSend;

  while(trans.resource->quantity() > newAmt && !inventory_.empty() ) {
    Material* m = inventory_.front();

    // start with an empty material
    Material* newMat = new Material();

    // if the inventory obj isn't larger than the remaining need, send it as is.
    if(m->quantity() <= (trans.resource->quantity() - newAmt)) {
      newAmt += m->quantity();
      newMat->absorb(m);
      inventory_.pop_front();
    } else { 
      // if the inventory obj is larger than the remaining need, split it.
      Material* toAbsorb = m->extract(trans.resource->quantity() - newAmt);
      newMat->absorb(toAbsorb);
      newAmt += toAbsorb->quantity();
    }

    toSend.push_back(newMat);
    LOG(LEV_DEBUG2) <<"ConditioningFacility "<< ID()
      <<"  is sending a mat with mass: "<< newMat->quantity();
  }    
  return toSend;
};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::addResource(Message* msg, vector<Resource*> manifest) {
  // Put the material received in the stocks
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Resource*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++) {
    LOG(LEV_DEBUG2) <<"ConditioningFacility " << ID() << " is receiving material with mass "
        << (*thisMat)->quantity();
    stocks_.push_front(make_pair(msg->trans().commod, dynamic_cast<Material*>(*thisMat)));
  } 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::handleTick(int time){
  // TICK
  // On the tick, the ConditioningFacility makes requests for each 
  // of the waste commodities (streams) that its table addresses
  // It also makes offers of any conditioned waste it contains
  makeRequests();
  makeOffers();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::handleTock(int time){
  // TOCK
  // On the tock, the ConditioningFacility first prepares 
  // transactions by preparing and sending material from its stocks
  // all material in its stocks up to its monthly processing 
  // capacity.
  
  conditionMaterials();
  processOrders();

  // call the facility model's handle tock 
  // to check for decommissioning
  FacilityModel::handleTock(time);
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
void ConditioningFacility::loadHDF5File(string datafile){
  // add the current path to the file
  string file_path = ENV->getCyclusPath() + "/" + datafile; 

  // check if the file is an hdf5 file first.
  if (! H5File::isHdf5(file_path)) {
    string err = "The file at ";
    err += file_path;
    err += " is not an hdf5 file.";
    throw CycIOException(err);
  }

  // create strings that H5Cpp can use to access the data
  const H5std_string filename = file_path;
  const H5std_string groupname = "/loading";
  const H5std_string datasetname = "low";
  const H5std_string streamID_memb = "streamID";
  const H5std_string formID_memb = "formID";
  const H5std_string density_memb = "density";
  const H5std_string wfvol_memb = "wfvol";
  const H5std_string wfmass_memb = "wfmass";

  //try {
  /*
   * Turn off the auto-printing when failure occurs so that we can
   * handle the errors appropriately
   */
  //Exception::dontPrint();

  /*
   * Open the file and the dataset.
   */
  H5File* file;
  file = new H5File( filename, H5F_ACC_RDONLY );
  Group* group;
  group = new Group (file->openGroup( groupname ));
  DataSet* dataset;
  dataset = new DataSet (group->openDataSet( datasetname ));
  DataSpace* dataspace;
  dataspace = new DataSpace (dataset->getSpace( ));

  hsize_t dims_out[2];
  int ndims = dataspace->getSimpleExtentDims(dims_out, NULL);
  stream_len_ = dims_out[0];

  /*
   * Create a datatype for stream
   */
  CompType mtype( sizeof(stream_t) );
  mtype.insertMember( streamID_memb, HOFFSET(stream_t, streamID), PredType::NATIVE_INT); 
  mtype.insertMember( formID_memb, HOFFSET(stream_t, formID), PredType::NATIVE_INT); 
  mtype.insertMember( density_memb, HOFFSET(stream_t, density), PredType::NATIVE_DOUBLE);
  mtype.insertMember( wfvol_memb, HOFFSET(stream_t, wfvol), PredType::NATIVE_DOUBLE);
  mtype.insertMember( wfmass_memb, HOFFSET(stream_t, wfmass), PredType::NATIVE_DOUBLE);

  /*
   * Read two fields c and a from s1 dataset. Fields in the file
   * are found by their names "c_name" and "a_name".
   */
  stream_t streams[stream_len_];
  dataset->read( streams, mtype );

  stream_vec_.resize(stream_len_);
  std::copy(streams, streams + stream_len_, stream_vec_.begin() );

  stream_t stream;
  stream = stream_vec_[1];
  LOG(LEV_DEBUG2) <<  "streamID  = " <<  stream.streamID ;;
  LOG(LEV_DEBUG2) <<  "formID  = " <<  stream.formID ;;
  LOG(LEV_DEBUG2) <<  "density  = " <<  stream.density ;;
  LOG(LEV_DEBUG2) <<  "wfVol= " <<  stream.wfvol;;
  LOG(LEV_DEBUG2) <<  "wfMass  = " <<  stream.wfmass;;

  //  } catch (Exception error) {
  // error.printError();
  // }


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
  string file_path = ENV->getCyclusPath() + "/" + datafile; 

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
      LOG(LEV_DEBUG2) <<  "streamID  = " <<  stream.streamID ;;
      getline(file, buffer, ',');
      stream.formID = strtol(buffer.c_str() , NULL, 10);
      LOG(LEV_DEBUG2) <<  "formID  = " <<  stream.formID ;;
      getline(file, buffer, ',');
      stream.density = strtod(buffer.c_str() , NULL);
      LOG(LEV_DEBUG2) <<  "density  = " <<  stream.density ;;
      getline(file, buffer, ',');
      stream.wfvol = strtod(buffer.c_str() , NULL);
      LOG(LEV_DEBUG2) <<  "wfvol  = " <<  stream.wfvol ;;
      getline(file, buffer, '\n');
      stream.wfmass = strtod(buffer.c_str() , NULL);
      LOG(LEV_DEBUG2) <<  "wfmass  = " <<  stream.wfmass ;;
      // put the full struct into the vector of structs
      stream_vec_.push_back(stream);
    }
    file.close();
  }
  else {
    string err = "XML file, ";
    err += file_path;
    err += ", not found.";
    throw CycException(err);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::makeRequests(){
  // The ConditioningFacility should make requests of all of the things it's 
  // capable of conditioning
  // for each stream in the matrix
  // calculate your capacity to condition
  // make requests

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::makeOffers(){
  // Offer anything that has been conditioned or can be conditioned this month.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::conditionMaterials(){
  // Partition the in-stream according to loading density
  // for each stream object in stocks
  map<string, Material*> remainders;
  while( !stocks_.empty() ){
    // move stocks to currMat
    std::string currCommod = stocks_.front().first;
    Material* currMat = stocks_.front().second;
    if(remainders.find(currCommod)!=remainders.end()){
      (remainders[currCommod])->absorb(condition(currCommod, currMat));
    } else {
      remainders[currCommod] = condition(currCommod, currMat);
    }
    stocks_.pop_front();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* ConditioningFacility::condition(string commod, Material* mat){
  Material* mat_to_condition;
  stream_t stream = getStream(commod);
  double mass_to_condition = stream.wfmass;
  double mass_remaining = mat->quantity();
  while( mass_remaining > mass_to_condition) {
    mat_to_condition = mat->extract(mass_to_condition);
    // mat_to_condition->absorb(wf_iso_vec_[commod]);
    inventory_.push_back(mat_to_condition);
    mass_remaining = mat->quantity();
  }
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ConditioningFacility::stream_t ConditioningFacility::getStream(string commod){
  int stream_id =(commod_map_.find(commod))->second;
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
void ConditioningFacility::processOrders(){
}


/* --------------------
   output database info
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string ConditioningFacility::outputDir_ = "/conditioning";


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructConditioningFacility() {
  return new ConditioningFacility();
}

extern "C" void destructConditioningFacility(Model* p) {
  delete p;
}

/* ------------------- */ 

