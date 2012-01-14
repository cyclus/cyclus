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

#include "Teuchos_ParameterList.hpp"
#include "Teuchos_Version.hpp"

#include "Timer.h"
#include "CycException.h"
#include "Material.h"
#include "Message.h"
#include "Model.h"
#include "Logger.h"

BookKeeper* BookKeeper::instance_ = 0;

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
H5File* BookKeeper::getDB()
{
  return myDB_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::openDB()
{
  if(dbIsOpen_==false){
    //If the database is already open, throw an exception; the caller probably 
    // doesn't realize this.
    try{ 
      myDB_ = new H5File(dbName_, H5F_ACC_RDWR);
      dbIsOpen_ = true;
    }
    catch( FileIException error )
    {
      error.printError();
    }
  };
};

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

//Function only used in tests (MJG)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BookKeeper::isGroup(std::string grp) {
  return true;
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::registerTrans(Message* msg, std::vector<Resource*> manifest){
  // grab each material object off of the manifest
  // and add its transaction to the list
  for (vector<Resource*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++) {
    trans_t toRegister;
    msg->setID();
    toRegister.transID=msg->getID();
    toRegister.requesterID=msg->getRequester()->ID();
    toRegister.supplierID=msg->getSupplier()->ID();
    toRegister.materialID=dynamic_cast<Material*>(*thisMat)->ID(); 
    toRegister.timestamp=TI->getTime();
    toRegister.price = msg->getPrice();
     
    strcpy(toRegister.commodName, msg->commod().c_str());
    transactions_.push_back(toRegister);
    // checking msg and transaction equality
    // msg->printTrans();
    // printTrans(toRegister);
  };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::printTrans(trans_t trans){
  std::cout << "Transaction info (via BookKeeper):" << std::endl <<
    "    Transaction ID: " << trans.transID << std::endl <<
    "    Requester ID: " << trans.requesterID << std::endl <<
    "    Supplier ID: " << trans.supplierID << std::endl <<
    "    Material ID: " << trans.materialID << std::endl <<
    "    Timestamp: " << trans.timestamp << std::endl <<
    "    Price: "  << trans.price << std::endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::registerMatState(int trans_id, Material* mat){
  mat_hist_t toRegister;
  fill_n(toRegister.iso, NUMISOS, 0);
  fill_n(toRegister.comp, NUMISOS, 0.0);

  double total = mat->getQuantity();
  toRegister.materialID = mat->ID(); 
  toRegister.transID = trans_id; 
  /// @todo allow registerMaterialState for arbitrary timestamp (katyhuff).
  toRegister.timestamp = TI->getTime();
  CompMap comp = (mat->isoVector()).comp();
  CompMap::const_iterator it = comp.begin();
  int i=0;
  for(it=comp.begin(); it != comp.end(); it++){
    toRegister.iso[i] = it->first;
    toRegister.comp[i] = (it->second)*(total);
    i++;
  }
  // if it's registered in some other timestamp, register the material 
  materials_.push_back(toRegister);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeModelList() {
  H5std_string ID_memb = "ID";
  H5std_string name_memb = "name";
  H5std_string modelImpl_memb = "modelImpl";
  H5std_string parentID_memb = "parentID";
  H5std_string bornOn_memb = "bornOn";
  H5std_string diedOn_memb = "diedOn";
  H5std_string output_name = "/output";

  // define some useful variables.
  std::string subgroup_name = "agents";
  std::string dataset_name = "agentList";

  std::vector<Model*> model_list = Model::getModelList();
  std::vector<Model*> short_list;
  
  // store in short_list non-template Model* only
  for (int i = 0; i < model_list.size(); i++) {
    Model* theModel = model_list.at(i);
    if (!theModel->isTemplate()) {
      short_list.push_back(theModel);
    }
  }

  // create an array of the model structs
  int numStructs = std::max(1, (int)short_list.size());
  model_t modelList[numStructs];
  if (short_list.size() == 0) {
    modelList[0].ID = 0;
    strcpy(modelList[0].modelImpl, "");
    strcpy(modelList[0].name, ""); 
  } else {
    for (int i = 0; i < short_list.size(); i++) {
      Model* theModel = short_list.at(i);

      modelList[i].ID = theModel->ID();
      modelList[i].parentID = theModel->parentID();
      modelList[i].bornOn = theModel->bornOn();
      modelList[i].diedOn = theModel->diedOn();
      strcpy(modelList[i].modelImpl, theModel->getModelImpl().c_str());
      strcpy(modelList[i].name, theModel->name().c_str()); 
    }
  }

  model_t* pModelList = modelList;
  doModelWrite(ID_memb, name_memb, modelImpl_memb, parentID_memb, 
	       bornOn_memb, diedOn_memb, output_name, subgroup_name, 
	       dataset_name, numStructs, short_list.size(), pModelList);
}

void BookKeeper::doModelWrite(H5std_string ID_memb,
			      H5std_string name_memb,
                              H5std_string modelImpl_memb,
			      H5std_string parentID_memb,
			      H5std_string bornOn_memb,
			      H5std_string diedOn_memb,
                              H5std_string output_name, 
                              std::string subgroup_name, 
                              std::string dataset_name,
                              int numStructs, int numModels, 
                              model_t* modelList){
  try{
    // Turn off the auto-printing when failure occurs so that we can
    // handle the errors appropriately
    Exception::dontPrint();
    
    // Open the file and the dataset.
    this->openDB();

    // describe the data in an hdf5-y way
    hsize_t dim[] = {numStructs};
    // if there's only one model, the dataspace is a vector, which  
    // hdf5 doesn't like to think of as a matrix 
    int rank;
    if(numModels <= 1) {
      rank = 1;
    } else {
      rank = 1;
    }

    Group* outputgroup;
    outputgroup = new Group(this->getDB()->openGroup(output_name));
    Group* subgroup;

    subgroup = new Group(outputgroup->createGroup(subgroup_name));
    DataSpace* dataspace;
    dataspace = new DataSpace( rank, dim );

    //create a variable length string types
    size_t charlen = sizeof(char[64]);
    StrType strtype(PredType::C_S1,charlen); 
   
    // Create a datatype for models based on the struct
    CompType mtype( sizeof(model_t) );
    mtype.insertMember( ID_memb, HOFFSET(model_t, ID), 
			PredType::NATIVE_INT); 
    mtype.insertMember( parentID_memb, HOFFSET(model_t, parentID), 
			PredType::NATIVE_INT); 
    mtype.insertMember( bornOn_memb, HOFFSET(model_t, bornOn), 
			PredType::NATIVE_INT); 
    mtype.insertMember( diedOn_memb, HOFFSET(model_t, diedOn), 
			PredType::NATIVE_INT); 
    mtype.insertMember( name_memb, HOFFSET(model_t, name), strtype);
    mtype.insertMember( modelImpl_memb, HOFFSET(model_t, modelImpl), 
			strtype);

    DataSet* dataset;
    dataset = new DataSet(subgroup->createDataSet( dataset_name , 
						   mtype , *dataspace ));

    // write it, finally 
    dataset->write( modelList , mtype );
    delete outputgroup;
    delete subgroup;
    delete dataspace;
    delete dataset;

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeTransList(){

  // define some useful variables.
  const H5std_string transID_memb = "transID";
  const H5std_string supplierID_memb = "supplierID";
  const H5std_string requesterID_memb = "requesterID";
  const H5std_string materialID_memb = "materialID";
  const H5std_string timestamp_memb = "timestamp";
  const H5std_string price_memb = "price";
  const H5std_string commodName_memb = "commodName";
  const H5std_string output_name = "/output";
  const H5std_string subgroup_name = "transactions";
  const H5std_string dataset_name = "transList";

  int numTrans = transactions_.size();

  int numStructs;
  if(numTrans==0)
    numStructs=1;
  else
    numStructs=numTrans;

  // create an array of the model structs
  trans_t transList[numStructs];
  for (int i=0; i<numTrans; i++){
    transList[i].transID = transactions_[i].transID; 
    transList[i].supplierID = transactions_[i].supplierID;
    transList[i].requesterID = transactions_[i].requesterID;
    transList[i].materialID = transactions_[i].materialID;
    transList[i].timestamp = transactions_[i].timestamp;
    transList[i].price = transactions_[i].price;
    strcpy( transList[i].commodName,transactions_[i].commodName);
  };
  // If there are no transactions, make a null transaction entry
  if(numTrans==0){
    std::string str1="";
    transList[0].transID=0;
    transList[0].supplierID=0;
    transList[0].requesterID=0;
    transList[0].materialID=0;
    transList[0].timestamp=0;
    transList[0].price=0;
    strcpy(transList[0].commodName, str1.c_str());
  };

  try{
    // Turn off the auto-printing when failure occurs so that we can
    // handle the errors appropriately
    Exception::dontPrint();
    
    // Open the file and the dataset.
    this->openDB();

    // describe the data in an hdf5-y way
    hsize_t dim[] = {numStructs};
    // if there's only one model, the dataspace is a vector, which  
    // hdf5 doesn't like to think of as a matrix 
    // (MJG) - what does this do? rank = 1 or rank = 1 ??
    int rank;
    if(numTrans <= 1)
      rank = 1;
    else
      rank = 1;

    Group* outputgroup;
    outputgroup = new Group(this->getDB()->openGroup(output_name));
    Group* subgroup;
    subgroup = new Group(outputgroup->createGroup(subgroup_name));
    DataSpace* dataspace;
    dataspace = new DataSpace( rank, dim );

    //create a variable length std::string types
    size_t charlen = sizeof(char[64]);
    StrType strtype(PredType::C_S1,charlen); 
   
    // Create a datatype for models based on the struct
    CompType mtype( sizeof(trans_t) );
    mtype.insertMember( transID_memb, HOFFSET(trans_t, transID), PredType::NATIVE_INT); 
    mtype.insertMember( supplierID_memb, HOFFSET(trans_t, supplierID), PredType::NATIVE_INT); 
    mtype.insertMember( requesterID_memb, HOFFSET(trans_t, requesterID), PredType::NATIVE_INT); 
    mtype.insertMember( materialID_memb, HOFFSET(trans_t, materialID), PredType::NATIVE_INT); 
    mtype.insertMember( timestamp_memb, HOFFSET(trans_t, timestamp), PredType::NATIVE_INT); 
    mtype.insertMember( price_memb, HOFFSET(trans_t, price), PredType::IEEE_F64LE); 
    mtype.insertMember( commodName_memb, HOFFSET(trans_t, commodName), strtype);

    DataSet* dataset;
    dataset = new DataSet(subgroup->createDataSet( dataset_name , mtype , *dataspace ));

    // write it, finally 
    dataset->write( transList , mtype );

    delete outputgroup;
    delete subgroup;
    delete dataspace;
    delete dataset;

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeMatHist(){

  // define some useful variables.
  const H5std_string materialID_memb = "materialID";
  const H5std_string transID_memb = "transID";
  const H5std_string timestamp_memb = "timestamp";
  const H5std_string iso_memb = "iso";
  const H5std_string comp_memb = "comp";
  const H5std_string output_name = "/output";
  const H5std_string subgroup_name = "materials";
  const H5std_string dataset_name = "matHist";

  int numHists = materials_.size();

  int numStructs;
  if(numHists==0)
    numStructs=1;
  else
    numStructs=numHists;

  // create an array of the model structs
  mat_hist_t matHist[numStructs];
  size_t arrintlen = sizeof(int[NUMISOS]);
  size_t arrdoublelen = sizeof(double[NUMISOS]);
  for (int i=0; i<numHists; i++){
    matHist[i].materialID = materials_[i].materialID;
    matHist[i].transID = materials_[i].transID;
    matHist[i].timestamp = materials_[i].timestamp;
    memcpy(matHist[i].iso, materials_[i].iso, arrintlen);
    memcpy(matHist[i].comp, materials_[i].comp, arrdoublelen);
  };
  // If there are no materials, make a null entry
  if(numHists==0){
    matHist[0].materialID=0;
    matHist[0].transID=0;
    matHist[0].timestamp=0;
    fill_n(matHist[0].iso, NUMISOS, 0);
    fill_n(matHist[0].comp, NUMISOS, 0.0);
  };

  //try{
  // Turn off the auto-printing when failure occurs so that we can
  // handle the errors appropriately
  //Exception::dontPrint();

  // Open the file and the dataset.
  this->openDB();

  // describe the data in an hdf5-y way
  hsize_t dim[] = {numStructs};
  // if there's only one model, the dataspace is a vector, which  
  // hdf5 doesn't like to think of as a matrix 
  int rank;
  if(numHists <= 1)
    rank = 1;
  else
    rank = 1;

  Group* outputgroup;
  outputgroup = new Group(this->getDB()->openGroup(output_name));
  Group* subgroup;
  subgroup = new Group(outputgroup->createGroup(subgroup_name));
  DataSpace* dataspace;
  dataspace = new DataSpace( rank, dim );

  // create an array type
  // describe the data in an hdf5-y way
  hsize_t arraydim[] = {NUMISOS};
  ArrayType arrinttype(PredType::NATIVE_INT , 1, arraydim ); 
  ArrayType arrdoubletype(PredType::IEEE_F64LE, 1, arraydim ); 

  // Create a datatype for models based on the struct
  CompType mtype( sizeof(mat_hist_t) );
  mtype.insertMember( materialID_memb, HOFFSET(mat_hist_t, materialID), PredType::NATIVE_INT); 
  mtype.insertMember( transID_memb, HOFFSET(mat_hist_t, transID), PredType::NATIVE_INT); 
  mtype.insertMember( timestamp_memb, HOFFSET(mat_hist_t, timestamp), PredType::NATIVE_INT); 
  mtype.insertMember( iso_memb, HOFFSET(mat_hist_t, iso), arrinttype); 
  mtype.insertMember( comp_memb, HOFFSET(mat_hist_t, comp), arrdoubletype); 

  DataSet* dataset;
  dataset = new DataSet(subgroup->createDataSet( dataset_name , mtype , *dataspace ));

  // write it, finally 
  dataset->write( matHist , mtype );

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
    DataSpace &filespace, DataSet &dataset){
    
    // define the dataset to match the dataspace
    dataset = this->getDB()->createDataSet(dsname, type, memspace) ; 

    // define the file dataspace to match the dataset
    filespace = dataset.getSpace();
  
    // select the whole dataset as the memory dataspace
    memspace.selectAll();
    
    // select the whole dataset as the memory dataspace
    filespace.selectAll();
};
