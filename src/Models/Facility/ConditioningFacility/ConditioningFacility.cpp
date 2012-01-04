// ConditioningFacility.cpp
// Implements the ConditioningFacility class
#include <iostream>
#include <fstream>
#include "boost/multi_array.hpp"

#include "ConditioningFacility.h"

#include "CycException.h"
#include "Env.h"
#include "InputXML.h"
#include "hdf5.h"
#include "H5Cpp.h"
#include "H5Exception.h"
#include "Logician.h"
#include "Logger.h"

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
ConditioningFacility::ConditioningFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ConditioningFacility::~ConditioningFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConditioningFacility::init(xmlNodePtr cur)
{
    FacilityModel::init(cur);

    /// move XML pointer to current model
    cur = XMLinput->get_xpath_element(cur,"model/ConditioningFacility");

    // Initialize allowed formats map
    allowed_formats_.insert(make_pair("hdf5", &ConditioningFacility::loadHDF5File)); 
    allowed_formats_.insert(make_pair("sql", &ConditioningFacility::loadSQLFile)); 
    allowed_formats_.insert(make_pair("xml", &ConditioningFacility::loadXMLFile)); 
    allowed_formats_.insert(make_pair("csv", &ConditioningFacility::loadCSVFile)); 

    /// initialize any ConditioningFacility-specific datamembers here
    string datafile = XMLinput->get_xpath_content(cur, "datafile");
    string fileformat = XMLinput->get_xpath_content(cur, "fileformat");
    // for whatever format,
    // call loadTable
    loadTable(datafile, fileformat);
    // commodities will be loaded by loadTable
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
void ConditioningFacility::sendMaterial(Message* order, const Communicator* receiver){
 // Send material from inventory to fulfill transactions

  Transaction trans = order->getTrans();

  Mass newAmt = 0;

  // pull materials off of the inventory stack until you get the transaction amount

  // start with an empty manifest
  vector<Material*> toSend;

  while(trans.resource->getQuantity() > newAmt && !inventory_.empty() ){
    Material* m = inventory_.front();

    // start with an empty material
    Material* newMat = new Material(CompMap(), 
                                  m->getUnits(),
                                  m->name(), 
                                  0, 
                                  ATOMBASED,
                                  false);

    // if the inventory obj isn't larger than the remaining need, send it as is.
    if(m->getTotMass() <= (trans.resource->getQuantity() - newAmt)){
      newAmt += m->getTotMass();
      newMat->absorb(m);
      inventory_.pop_front();
    }
    else{ 
      // if the inventory obj is larger than the remaining need, split it.
      Material* toAbsorb = m->extractMass(trans.resource->getQuantity() - newAmt);
      newAmt += toAbsorb->getTotMass();
      newMat->absorb(toAbsorb);
    }

    toSend.push_back(newMat);
    LOG(LEV_DEBUG2) <<"NullFacility "<< ID()
      <<"  is sending a mat with mass: "<< newMat->getTotMass();
  }    
  FacilityModel::sendMaterial( order, toSend );
};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::receiveMaterial(Transaction trans, vector<Material*> manifest){
  // Put the material received in the stocks
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    LOG(LEV_DEBUG2) <<"RecipeReactor " << ID() << " is receiving material with mass "
        << (*thisMat)->getTotMass();
    stocks_.push_front(make_pair(trans.commod, *thisMat));
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
  mtype.insertMember( streamID_memb, HOFFSET(stream_t, streamID), PredType::NATIVE_INT); // Change this to int when you can make a table where this is an int. 
  mtype.insertMember( formID_memb, HOFFSET(stream_t, formID), PredType::NATIVE_INT); // Change this to int when you can make a table where this is an int. 
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

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::loadSQLFile(string datafile){
  // get dimensions
  // // how many rows
  // // how many columns
  // create array

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

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::makeOffers(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::conditionMaterials(){
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConditioningFacility::processOrders(){
}



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

