// BookKeeper.cpp
// Implements the BookKeeper class

#include "BookKeeper.h"

#include <string>
#include <cstring>
#include <iostream>

#include "boost/multi_array.hpp"
#include "hdf5.h"
#include "H5Cpp.h"
#include "H5Exception.h"
#include "h5wrap.h"

#include "Teuchos_ParameterList.hpp"
#include "Teuchos_Version.hpp"

#include "Timer.h"
#include "CycException.h"
#include "Material.h"
#include "GenericResource.h"
#include "TimeAgent.h"
#include "Message.h"
#include "Model.h"
#include "Logger.h"

#include <boost/algorithm/string.hpp>


BookKeeper* BookKeeper::instance_ = 0;
int BookKeeper::next_comp_entry_id_ = 0;
  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BookKeeper* BookKeeper::Instance() {
  // If we haven't created a BookKeeper yet, create and return it.
  if (0 == instance_){
    instance_ = new BookKeeper();  
  }
  return instance_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
BookKeeper::BookKeeper() {
  dbIsOpen_ = false;
  dbExists_ = false;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BookKeeper::createDB() {
  /// @todo make the output database name optional (khuff)
  createDB("cyclus.h5");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::createDB(std::string name) {
  dbName_ = name;

  try {
    // create database. If it already exits, H5F_ACC_TRUNC erases all 
    // data previously stored in the file.
    myDB_ = new H5File( name , H5F_ACC_TRUNC );
    dbIsOpen_ = true; // use the H5 function for this.. !!! KDHFLAG
    dbExists_ = true;

    // create groups for the input and output data, respectively
    Group* outGroup = new Group( myDB_->createGroup("/output"));
    Group* inGroup = new Group( myDB_->createGroup("/input"));

    delete outGroup;
    delete inGroup;
  } catch( Exception error ) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
H5File* BookKeeper::getDB() {
  return myDB_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::openDB() {
  // see if the file has already been opened
  if(! dbIsOpen_) {
    try {
      // check to make sure it's an hdf5 file
      bool isH5 = H5::H5File::isHdf5(dbName_);
      if (!isH5) 
        throw h5wrap::FileNotHDF5(dbName_);
      // if it is, lets open it
      myDB_ = new H5File(dbName_, H5F_ACC_RDWR);
      dbIsOpen_ = true;
    } catch ( FileIException error ) {
      error.printError();
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::closeDB()
{
  try{
    // Turn off the auto-printing when failure occurs so that we can
    // handle the errors appropriately
    Exception::dontPrint();
    if(dbIsOpen_==true){
      this->getDB()->close();
      dbIsOpen_ = false;
    }
    else
      throw CycIOException("Tried to close a database that was not open."); 
  }
  catch( FileIException error )
  {
    error.printError();
  } 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::pair <std::string, std::string> 
BookKeeper::getGroupNamePair(std::string output_dir)
{
  // split the output directory dir into two strings
  // ex:: /output/group/dir to:
  //      /output/group and dir
  std::string str_to_split = output_dir;
  std::vector<std::string> split_str;
  boost::split(split_str,str_to_split,boost::is_any_of("/"));
  std::string output_name = "", subgroup_name = "";
  for (int i = 0; i < split_str.size(); i++){
    if (i == split_str.size()-1)
      subgroup_name.append(split_str.at(i));
    else {
      output_name.append("/");
      output_name.append(split_str.at(i));
    }
  }  
  std::pair <std::string,std::string> retPair (output_name,subgroup_name);
  return retPair;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::registerTransaction(int id, msg_ptr msg, std::vector<rsrc_ptr> manifest){
  trans_t toRegister;
  toRegister.transID = id;
  toRegister.requesterID = msg->requester()->ID();
  toRegister.supplierID = msg->supplier()->ID();
  toRegister.timestamp = TI->time();
  toRegister.price = msg->price();
   
  strcpy(toRegister.commodName, msg->commod().c_str());
  transactions_.push_back(toRegister);

  // grab each material object off of the manifest
  // and add its state to the mat state list
  for (vector<rsrc_ptr>::iterator thisResource = manifest.begin();
       thisResource != manifest.end();
       thisResource++) {
    registerResourceState(id, *thisResource);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::printTrans(trans_t trans){
  std::cout << "Transaction info (via BookKeeper):" << std::endl <<
    "    Transaction ID: " << trans.transID << std::endl <<
    "    Requester ID: " << trans.requesterID << std::endl <<
    "    Supplier ID: " << trans.supplierID << std::endl <<
    "    Timestamp: " << trans.timestamp << std::endl <<
    "    Price: "  << trans.price << std::endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::registerResourceState(int trans_id, rsrc_ptr resource){
  mat_hist_t toRegister;
  fill_n(toRegister.iso, NUMISOS, 0);
  fill_n(toRegister.comp, NUMISOS, 0.0);

  toRegister.materialID = resource->ID(); 
  toRegister.transID = trans_id; 
  toRegister.timestamp = TI->time();
  toRegister.quantity = resource->quantity();
  strcpy(toRegister.units, resource->units().c_str());

  if (resource->type() == GENERIC_RES) {
    strcpy(toRegister.name, boost::dynamic_pointer_cast<GenericResource>(resource)->quality().c_str());
  } else if (resource->type() == MATERIAL_RES) {
    mat_rsrc_ptr mat = boost::dynamic_pointer_cast<Material>(resource);
    strcpy(toRegister.name, "Material");
    CompMap comp = (mat->isoVector()).comp();
    int i = 0;
    for(CompMap::const_iterator it = comp.begin(); it != comp.end(); it++){
      toRegister.iso[i] = it->first;
      toRegister.comp[i] = it->second;
      i++;
    }
  }

  // if it's registered in some other timestamp, register the material 
  materials_.push_back(toRegister);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeDataSet(const void *data, const DataType &data_desc, 
                              int data_rank, const hsize_t *data_dims,
                              std::string dataset_name, std::string output_dir){
  try{ 
    // Open the file
    this->openDB();

    // get the correct group names
    std::pair <std::string,std::string> groupNames = getGroupNamePair(output_dir);
    std::string output_name = groupNames.first;
    std::string subgroup_name = groupNames.second;

    // set up the data set
    H5::Group* outputgroup = new Group(this->getDB()->openGroup(output_name));
    // MJGFLAG add functionality: if subgroup exists, open it. if not create.
    H5::Group* subgroup = new Group(outputgroup->createGroup(subgroup_name));
    DataSpace* dataspace = new DataSpace(data_rank, data_dims);
    DataSet* dataset = 
      new DataSet(subgroup->createDataSet(dataset_name, data_desc, *dataspace ));

    // write it, finally 
    dataset->write(data, data_desc);

    // clean up the mess we made
    delete outputgroup;
    delete subgroup;
    delete dataspace;
    delete dataset;

  } catch (Exception error) {
    error.printError();
  }  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeAgentList() {
  try {
    // Turn off the auto-printing when failure occurs so that we can
    // handle the errors appropriately
    Exception::dontPrint();

    // set output and dataset names
    std::string output_dir = TimeAgent::outputDir();
    std::string dataset_name = "agentList";
    
    // describe the data type to fill this table
    size_t agent_struct_size = sizeof(agent_t);
    H5::CompType data_desc(agent_struct_size);
    data_desc.insertMember("id", HOFFSET(agent_t, ID), 
                           H5::PredType::NATIVE_INT);
    data_desc.insertMember("name", HOFFSET(agent_t, name), 
                           H5::StrType(0,64));
    data_desc.insertMember("model impl.",HOFFSET(agent_t, modelImpl), 
                           H5::StrType(0,64));
    data_desc.insertMember("parent id", HOFFSET(agent_t, parentID), 
                           H5::PredType::NATIVE_INT);
    data_desc.insertMember("birth date", HOFFSET(agent_t, bornOn), 
                           H5::PredType::NATIVE_INT);
    data_desc.insertMember("death date", HOFFSET(agent_t, diedOn), 
                           H5::PredType::NATIVE_INT);
    
    // fill the agent data array from cyclus' agent list
    int numStructs = std::max(1, (int)agent_data_.size());
    agent_t agent_data[numStructs];
    // take care of the special case where there are no agents
    if (agent_data_.size() == 0) {
      agent_data[0].ID = -1;
      agent_data[0].parentID = -1;
      agent_data[0].bornOn = -1;
      agent_data[0].diedOn = -1;
      strcpy(agent_data[0].modelImpl, "");
      strcpy(agent_data[0].name, ""); 
    // take care of the normal case where there are agents
    } else {
      std::map<int, ParamMap>::iterator iter;
      int count = 0;
      for (iter = agent_data_.begin(); iter != agent_data_.end(); ++iter) {
        int id = iter->first;

        agent_data[count].ID = id;
        strcpy(agent_data[count].name, modelDatum<std::string>(id, "name").c_str()); 
        strcpy(agent_data[count].modelImpl, modelDatum<std::string>(id, "modelImpl").c_str());
        agent_data[count].parentID = modelDatum<int>(id, "parentID");
        agent_data[count].bornOn = modelDatum<int>(id, "bornOn");
        agent_data[count].diedOn = modelDatum<int>(id, "diedOn");

        count++;
      }
    }
    
    // describe dataspace and write the dataset
    hsize_t data_dims[1] = {numStructs};
    int data_rank = 1;
    writeDataSet(agent_data, data_desc, data_rank, data_dims, dataset_name, output_dir);

  } catch (Exception error) {
    error.printError();
  }
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeTransList(){
  try{
    // Turn off the auto-printing when failure occurs so that we can
    // handle the errors appropriately
    Exception::dontPrint();

    // set output and dataset names
    std::string output_dir = Message::outputDir();
    std::string dataset_name = "transactionList";
    
    // describe the data type to fill this table
    size_t trans_struct_size = sizeof(trans_t);
    H5::CompType data_desc(trans_struct_size);

    data_desc.insertMember("id", HOFFSET(trans_t, transID), 
                           PredType::NATIVE_INT); 
    data_desc.insertMember("supplier id", HOFFSET(trans_t, supplierID), 
                           PredType::NATIVE_INT); 
    data_desc.insertMember("requester id", HOFFSET(trans_t, requesterID), 
                           PredType::NATIVE_INT); 
    data_desc.insertMember("timestamp", HOFFSET(trans_t, timestamp), 
                           PredType::NATIVE_INT); 
    data_desc.insertMember("price", HOFFSET(trans_t, price), 
                           PredType::IEEE_F64LE); 
    data_desc.insertMember("commodity", HOFFSET(trans_t, commodName), 
                           H5::StrType(0,64));

    // create an array of the transaction structs
    int numStructs = std::max(1, (int)transactions_.size());
    trans_t* trans_data = new trans_t[numStructs];
    // take care of the special case where there are no transactions
    if(transactions_.size()==0) {
      std::string str1="";
      trans_data[0].transID=-1;
      trans_data[0].supplierID=-1;
      trans_data[0].requesterID=-1;
      trans_data[0].timestamp=-1;
      trans_data[0].price=-1;
      strcpy(trans_data[0].commodName, "");
    // take care of the normal case where there are transactions
    } else{
      for (int i=0; i<transactions_.size(); i++){
        trans_data[i].transID = transactions_[i].transID; 
        trans_data[i].supplierID = transactions_[i].supplierID;
        trans_data[i].requesterID = transactions_[i].requesterID;
        trans_data[i].timestamp = transactions_[i].timestamp;
        trans_data[i].price = transactions_[i].price;
        strcpy(trans_data[i].commodName,transactions_[i].commodName);
      }
    }

    // describe dataspace and write the dataset
    hsize_t data_dims[1] = {numStructs};
    int data_rank = 1;
    writeDataSet(trans_data, data_desc, data_rank, data_dims, dataset_name, output_dir);
    delete[] trans_data;

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeMatComps(Group* subgroup){

  // define some useful variables.
  const H5std_string entryID_memb = "entryID";
  const H5std_string stateID_memb = "stateID";
  const H5std_string iso_memb = "iso";
  const H5std_string comp_memb = "comp";

  const H5std_string output_name = "/output";
  const H5std_string subgroup_name = "materials";
  const H5std_string dataset_name = "compositions";

  // create an array of the model structs
  int n_entries = comp_entries_.size();
  comp_entry_t* comp_entries = new comp_entry_t[n_entries];
  for (int i = 0; i < n_entries; i++) {
    comp_entries[i].entryID = comp_entries_.at(i).entryID;
    comp_entries[i].stateID = comp_entries_.at(i).stateID;
    comp_entries[i].iso = comp_entries_.at(i).iso;
    comp_entries[i].comp = comp_entries_.at(i).comp;
  }

  // Create a datatype for models based on the struct
  CompType mtype( sizeof(comp_entry_t) );
  mtype.insertMember( entryID_memb, HOFFSET(comp_entry_t, entryID), PredType::NATIVE_INT); 
  mtype.insertMember( stateID_memb, HOFFSET(comp_entry_t, stateID), PredType::NATIVE_INT); 
  mtype.insertMember( iso_memb, HOFFSET(comp_entry_t, iso), PredType::NATIVE_INT); 
  mtype.insertMember( comp_memb, HOFFSET(comp_entry_t, comp), PredType::NATIVE_DOUBLE); 

  // Open the file and the dataset.
  this->openDB();

  // describe the data in an hdf5-y way
  hsize_t dim[] = {max((int)comp_entries_.size(), 1)};

  // if there's only one model, the dataspace is a vector, which  
  // hdf5 doesn't like to think of as a matrix 
  int rank = 1;

  DataSpace* dataspace;
  dataspace = new DataSpace( rank, dim );

  DataSet* dataset;
  dataset = new DataSet(subgroup->createDataSet( dataset_name , mtype , *dataspace ));

  // write it, finally 
  dataset->write( comp_entries , mtype );

  delete[] comp_entries;
  delete dataspace;
  delete dataset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeMatHist(){

  // define some useful variables.
  const H5std_string stateID_memb = "stateID";
  const H5std_string materialID_memb = "materialID";
  const H5std_string transID_memb = "transID";
  const H5std_string timestamp_memb = "timestamp";
  const H5std_string quantity_memb = "quantity";
  const H5std_string units_memb = "units";
  const H5std_string name_memb = "name";

  const H5std_string output_name = "/output";
  const H5std_string subgroup_name = "materials";
  const H5std_string dataset_name = "material_states";

  int numHists = materials_.size();

  int numStructs;
  if(numHists==0)
    numStructs=1;
  else
    numStructs=numHists;


  // create an array of the model structs
  mat_hist_t* matHist = new mat_hist_t[numStructs];

  // describe the data in an hdf5-y way
  hsize_t dim[] = {numStructs};
  // if there's only one model, the dataspace is a vector, which  
  // hdf5 doesn't like to think of as a matrix 
  int rank = 1;

  // Open the file and the dataset.
  this->openDB();

  Group* outputgroup;
  outputgroup = new Group(this->getDB()->openGroup(output_name));
  Group* subgroup;
  subgroup = new Group(outputgroup->createGroup(subgroup_name));
  DataSpace* dataspace;
  dataspace = new DataSpace( rank, dim );

  int numComps = NUMISOS;
  for (int i=0; i<numHists; i++){
    matHist[i].stateID = i;
    matHist[i].materialID = materials_[i].materialID;
    matHist[i].transID = materials_[i].transID;
    matHist[i].timestamp = materials_[i].timestamp;

    matHist[i].quantity = materials_[i].quantity;
    strcpy(matHist[i].units, materials_[i].units);
    strcpy(matHist[i].name, materials_[i].name);

    comp_entry_t comp_entry;
    for (int j = 0; j < numComps; j++) {
      comp_entry.entryID = next_comp_entry_id_++;
      comp_entry.stateID = i;
      comp_entry.iso = materials_[i].iso[j];
      comp_entry.comp = materials_[i].comp[j];

      comp_entries_.push_back(comp_entry);
    }
  }
  writeMatComps(subgroup);

  // If there are no materials, make a null entry
  if(numHists==0){
    matHist[0].stateID=0;
    matHist[0].materialID=0;
    matHist[0].transID=0;
    matHist[0].timestamp=0;

  };

  // create an array type
  // describe the data in an hdf5-y way
  hsize_t arraydim[] = {NUMISOS};

  size_t charlen = sizeof(char[64]);
  StrType strtype(PredType::C_S1,charlen);

  // Create a datatype for models based on the struct
  CompType mtype( sizeof(mat_hist_t) );
  mtype.insertMember( stateID_memb, HOFFSET(mat_hist_t, stateID), PredType::NATIVE_INT); 
  mtype.insertMember( materialID_memb, HOFFSET(mat_hist_t, materialID), PredType::NATIVE_INT); 
  mtype.insertMember( transID_memb, HOFFSET(mat_hist_t, transID), PredType::NATIVE_INT); 
  mtype.insertMember( timestamp_memb, HOFFSET(mat_hist_t, timestamp), PredType::NATIVE_INT); 
  mtype.insertMember( quantity_memb, HOFFSET(mat_hist_t, quantity), PredType::NATIVE_DOUBLE);
  mtype.insertMember( units_memb, HOFFSET(mat_hist_t, units), strtype); 
  mtype.insertMember( name_memb, HOFFSET(mat_hist_t, name), strtype); 

  DataSet* dataset;
  dataset = new DataSet(subgroup->createDataSet( dataset_name , mtype , *dataspace ));

  // write it, finally 
  dataset->write( matHist , mtype );

  delete[] matHist;
  delete outputgroup;
  delete subgroup;
  delete dataspace;
  delete dataset;

  //} catch ( Exception error) {
  //  error.printError();
  //}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::prepareSpaces(std::string dsname, DataType type, DataSpace &memspace, 
    DataSpace &filespace, DataSet &dataset) {
    
    // define the dataset to match the dataspace
    dataset = this->getDB()->createDataSet(dsname, type, memspace) ; 

    // define the file dataspace to match the dataset
    filespace = dataset.getSpace();
  
    // select the whole dataset as the memory dataspace
    memspace.selectAll();
    
    // select the whole dataset as the memory dataspace
    filespace.selectAll();
};

