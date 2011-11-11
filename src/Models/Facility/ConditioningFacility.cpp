// ConditioningFacility.cpp
// Implements the ConditioningFacility class
#include <iostream>
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

using namespace std;
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

  // pull materials off of the inventory stack until you get the trans amount

  // start with an empty manifest
  vector<Material*> toSend;

  while(trans.amount > newAmt && !inventory_.empty() ){
    Material* m = inventory_.front();

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
    LOG(LEV_DEBUG2) <<"NullFacility "<< getSN()
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
    LOG(LEV_DEBUG2) <<"RecipeReactor " << getSN() << " is receiving material with mass "
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
};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" Model* construct() {
  return new ConditioningFacility();
}

extern "C" void destruct(Model* p) {
  delete p;
}

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

  // get dimensions
  // // how many rows
  // // how many columns
  // create array
  string file_path = ENV->getCyclusPath() + datafile; 

  // check that the file is valid
  if(!H5File::isHdf5(file_path))
  {
    string err = file_path;
    err += " is not a valid HDF5 file";
    throw CycException(err);
  }

  // open the file
  H5File* db;
  if(file_is_open_==false){
    //If the database is already open, throw an exception. 
    try{ 
      db  = new H5File(file_path, H5F_ACC_RDWR);
      file_is_open_ = true;
    }
    catch( FileIException error )
    {
      error.printError();
    }
  }

  const H5std_string filename = file_path;
  const H5std_string groupname = "/";
  const H5std_string datasetname = "conditioning";
  const H5std_string ws_memb = "ws";
  const H5std_string wf_memb = "wf";
  const H5std_string load_memb = "load";
  
  //check if the file is an hdf5 file first.
  if (! H5File::isHdf5(file_path)) {
    throw CycIOException("The waste conditioning file is not an hdf5 file.");
  }

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = db->openDataSet(datasetname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its a double
    FloatType dbltype;
    if( type_class == H5T_FLOAT ) 
    {
      // oh good, it's a double. Now figure out what kind.
      dbltype = dataset.getFloatType();
    }
    else{
      throw CycTypeException("The dataset " + datasetname + " is not of float type");
    }
  
    // get the file dataspace
    DataSpace filespace = dataset.getSpace();
  
    // find the rank
    int rank = filespace.getSimpleExtentNdims();
  
    // find what the dataspace dimensions are
    hsize_t dims[2];
    int ndims = filespace.getSimpleExtentDims(dims, NULL);
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
  
    // select everything in each dataspace
    memspace.selectAll();
    filespace.selectAll();
  
    // initializes the memory space for the data
    double out_array[dims[0]][dims[1]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        out_array[i][j]=1.0;
      }
    }
  
    // This is basically a memcopy from the memspace into out_array
    dataset.read( out_array, dbltype, memspace , filespace );

    // Now the data is a multi_array of doubles
    // (which is much easier to deal with... thanks boost)
    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        loading_densities_[row][col] = out_array[row][col];
      }
    }

    // assume that the rows are waste streams
    // get their names
    // the columns are waste forms
    // get their names
    // assume there will be an attribute called UNIT
    // assume there will be a dataspace of waste form masses?
    // assume there will be a dataspace of waste form volumes?

  } catch (Exception error) {
    error.printError();
  }

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
void ConditioningFacility::makeRequests(){

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


