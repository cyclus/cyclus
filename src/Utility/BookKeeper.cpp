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

#include "Timer.h"
#include "CycException.h"
#include "Material.h"
#include "Message.h"
#include "Logician.h"
#include "Model.h"


BookKeeper* BookKeeper::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BookKeeper* BookKeeper::Instance() {
  // If we haven't created a BookKeeper yet, create and return it.
  if (0 == instance_)
    instance_ = new BookKeeper();  
  return instance_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BookKeeper::BookKeeper() {
  dbIsOpen_ = false;
  dbExists_ = false;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::createDB() {
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

// This function is not currently used (MJG)
// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DataSet BookKeeper::createDataSet(hsize_t rank, hsize_t* dims, DataType type, std::string dsName){
//   DataSet dataset;
//   try{
//     // create the dataspace from rank and dimension information
//     DataSpace* dataspace = new DataSpace(rank, dims);

//     // create a dataset to match the dataspace
//     dataset = this->getDB()->createDataSet(dsName, type, *dataspace) ; 

//   }
//   catch( FileIException error )
//   {
//     error.printError();
//   }
//   catch( DataSetIException error )
//   {
//     error.printError();
//   }
//   catch( GroupIException error )
//   {
//     error.printError();
//   }
//   return dataset;
// };

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
void BookKeeper::registerTrans(Message* msg, std::vector<Material*> manifest){
  // grab each material object off of the manifest
  // and add its transaction to the list
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    trans_t toRegister;
    toRegister.requesterID=msg->getRequester()->ID();
    toRegister.supplierID=msg->getSupplier()->ID();
    toRegister.materialID=(*thisMat)->ID(); 
    toRegister.timestamp=TI->getTime();
    toRegister.price = msg->getPrice();
     
    strcpy(toRegister.commodName, msg->getCommod()->name().c_str());
    transactions_.push_back(toRegister);
  };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::printTrans(trans_t trans){
  std::cout << "Transaction info (via BookKeeper):" << std::endl <<
    "    Requester ID: " << trans.requesterID << std::endl <<
    "    Supplier ID: " << trans.supplierID << std::endl <<
    "    Material ID: " << trans.materialID << std::endl <<
    "    Timestamp: " << trans.timestamp << std::endl <<
    "    Price: "  << trans.price << std::endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::registerMatChange(Material* mat){
  mat_hist_t toRegister;
  fill_n(toRegister.iso, NUMISOS, 0);
  fill_n(toRegister.comp, NUMISOS, 0.0);

  if (!(mat->isTemplate())){
    double total = mat->getTotMass();
    toRegister.materialID = mat->ID(); 
    /// @todo allow registerMaterialChange for arbitrary timestamp (katyhuff).
    toRegister.timestamp = TI->getTime();
    CompMap comp = mat->getMassComp();
    CompMap::const_iterator it = comp.begin();
    int i=0;
    for(it=comp.begin(); it != comp.end(); it++){
      toRegister.iso[i] = it->first;
      toRegister.comp[i] = (it->second)*(total);
      i++;
    }
    // if this material has registered 
    if (last_mat_idx_.find(toRegister.materialID)!=last_mat_idx_.end()){
      // in this timestamp
      if ((materials_[last_mat_idx_[toRegister.materialID]]).timestamp == toRegister.timestamp){
        //replace the entry.
        materials_.at(last_mat_idx_[toRegister.materialID])=toRegister;
        // the index of the last registeration stays the same
      } else {
        // if it's registered in some other timestamp, register the material 
        materials_.push_back(toRegister);
        // set the new last registeration index for this material 
        last_mat_idx_[toRegister.materialID]= (materials_.size()-1);
      }
    }else{
      // if it's never been registed, register the material anew
      materials_.push_back(toRegister);
      // set the index of the newly registered material
      last_mat_idx_[toRegister.materialID] = (materials_.size()-1);
    }
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::registerRepoComponent(int ID, std::string name, 
    std::string thermalModel, std::string nuclideModel,
    int parentID, double innerRadius, double outerRadius, double x, double y, 
    double z){

  repo_component_t toRegister;

  toRegister.ID = ID;
  toRegister.parentID = parentID;
  toRegister.innerRadius = innerRadius;
  toRegister.outerRadius = outerRadius;
  toRegister.x = x;
  toRegister.y = y;
  toRegister.z = z;
  toRegister.timestamp = TI->getTime();

  strcpy(toRegister.name, name.c_str());
  strcpy(toRegister.thermalModel, thermalModel.c_str());
  strcpy(toRegister.nuclideModel, nuclideModel.c_str());

  repo_components_.push_back(toRegister);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeModelList(ModelType type) {

  // define some useful variables.
  const H5std_string ID_memb = "ID";
  const H5std_string name_memb = "name";
  const H5std_string modelImpl_memb = "modelImpl";
  const H5std_string output_name = "/output";

  std::string subgroup_name;
  std::string dataset_name;
  int numStructs, numModels;
  
  numModels = LI->getNumModels(type);
  if (numModels==0) {
    numStructs=1;
  } else {
    numStructs=numModels;
  }
  
  // parse the cases.
  switch( type ) {
  case REGION:
    subgroup_name = "regions";
    dataset_name = "regionList"; 
    break;
  case INST:
    subgroup_name = "insts";
    dataset_name = "instList"; 
    break;
  case FACILITY:
    subgroup_name = "facilities";
    dataset_name = "facList"; 
    break;
  case MARKET:
    subgroup_name = "markets";
    dataset_name = "marketList"; 
    break;
  case CONVERTER: 
    subgroup_name = "converters";
    dataset_name = "converterList"; 
    break;
  };
  
  // create an array of the model structs
  model_t modelList[numStructs];
  int i = 0;
  for (ModelList::iterator model_iter = LI->begin(type);
       model_iter != LI->end(type);
       model_iter++) {
    Model* theModel = model_iter->second;
    modelList[i].ID = theModel->ID();
    strcpy(modelList[i].modelImpl, theModel->getModelImpl().c_str());
    strcpy(modelList[i].name, theModel->name().c_str()); 
    i++;
  };
  if(numModels==0) {
    std::string str1="";
    std::string str2="";
    modelList[0].ID=0;
    strcpy(modelList[0].modelImpl, str1.c_str());
    strcpy(modelList[0].name, str2.c_str()); 
  };

  // Work in Progress (MJG)
  // setUpModelWrite(type,ID_memb, name_memb, modelImpl_memb,  \
  //       output_name, subgroup_name, dataset_name,  \
  //       numStructs, numModels, modelList);

  model_t *pModelList = modelList;
  doModelWrite(ID_memb, name_memb, modelImpl_memb, \
         output_name, subgroup_name, dataset_name, \
         numStructs, numModels, pModelList);
};

// Work in Progress (MJG)
// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// void BookKeeper::setUpModelWrite(ModelType type,       \
//                      H5std_string ID_memb, H5std_string name_memb, \
//                      H5std_string modelImpl_memb, H5std_string output_name, \
//          std::string subgroup_name, std::string dataset_name, \
//                 int numStructs, int numModels, model_t modelList[]){
//   try{
//   } catch (Exception error) {
//     error.printError();
//   }
// };

void BookKeeper::doModelWrite(H5std_string ID_memb, H5std_string name_memb,  \
                              H5std_string modelImpl_memb, H5std_string output_name, \
                              std::string subgroup_name, std::string dataset_name, \
                              int numStructs, int numModels, model_t* modelList){
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
    mtype.insertMember( ID_memb, HOFFSET(model_t, ID), PredType::NATIVE_INT); 
    mtype.insertMember( name_memb, HOFFSET(model_t, name), strtype);
    mtype.insertMember( modelImpl_memb, HOFFSET(model_t, modelImpl), strtype);

    DataSet* dataset;
    dataset = new DataSet(subgroup->createDataSet( dataset_name , mtype , *dataspace ));

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
    matHist[i].timestamp = materials_[i].timestamp;
    memcpy(matHist[i].iso, materials_[i].iso, arrintlen);
    memcpy(matHist[i].comp, materials_[i].comp, arrdoublelen);
  };
  // If there are no materials, make a null entry
  if(numHists==0){
    matHist[0].materialID=0;
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
void BookKeeper::writeRepoComponents(){

  // define some useful variables.
  const H5std_string ID_memb = "ID";
  const H5std_string name_memb = "name";
  const H5std_string thermalModel_memb = "thermalModel";
  const H5std_string nuclideModel_memb = "nuclideModel";
  const H5std_string parentID_memb = "parentID";
  const H5std_string innerRadius_memb = "innerRadius";
  const H5std_string outerRadius_memb = "outerRadius";
  const H5std_string x_memb = "x";
  const H5std_string y_memb = "y";
  const H5std_string z_memb = "z";
  const H5std_string timestamp_memb = "timestamp";
  const H5std_string output_name = "/output";
  const H5std_string subgroup_name = "repoComponents";
  const H5std_string dataset_name = "components";

  int numComponents = repo_components_.size();

  int numStructs;
  if(numComponents==0)
    numStructs=1;
  else
    numStructs=numComponents;

  // create an array of the repo component structs
  repo_component_t repoComponent[numStructs];
  for (int i=0; i<numComponents; i++){
    repoComponent[i].ID = repo_components_[i].ID;
    strcpy(repoComponent[i].name, repo_components_[i].name);
    strcpy(repoComponent[i].thermalModel, (repo_components_[i].thermalModel));
    strcpy(repoComponent[i].nuclideModel, (repo_components_[i].nuclideModel));
    repoComponent[i].parentID = repo_components_[i].parentID;
    repoComponent[i].innerRadius = repo_components_[i].innerRadius;
    repoComponent[i].outerRadius = repo_components_[i].outerRadius;
    repoComponent[i].x = repo_components_[i].x;
    repoComponent[i].y = repo_components_[i].y;
    repoComponent[i].z = repo_components_[i].z;
    repoComponent[i].timestamp = repo_components_[i].timestamp;
  };

  // If there are no repo_components, make a null entry
  if(numComponents==0){
    std::string str1="";
    repoComponent[0].ID=0;
    strcpy(repoComponent[0].name, str1.c_str());
    strcpy(repoComponent[0].thermalModel, str1.c_str());
    strcpy(repoComponent[0].nuclideModel, str1.c_str());
    repoComponent[0].parentID=0;
    repoComponent[0].innerRadius=0;
    repoComponent[0].outerRadius=0;
    repoComponent[0].x=0;
    repoComponent[0].y=0;
    repoComponent[0].z=0;
    repoComponent[0].timestamp=0;
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
    int rank;
    if(numComponents <= 1)
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
    CompType mtype( sizeof(repo_component_t) );
    mtype.insertMember( ID_memb, HOFFSET(repo_component_t, ID), PredType::NATIVE_INT); 
    mtype.insertMember( name_memb, HOFFSET(repo_component_t, name), strtype); 
    mtype.insertMember( thermalModel_memb, HOFFSET(repo_component_t, thermalModel), strtype); 
    mtype.insertMember( nuclideModel_memb, HOFFSET(repo_component_t, nuclideModel), strtype); 
    mtype.insertMember( parentID_memb, HOFFSET(repo_component_t, parentID), PredType::NATIVE_INT); 
    mtype.insertMember( innerRadius_memb, HOFFSET(repo_component_t, innerRadius), PredType::IEEE_F64LE); 
    mtype.insertMember( outerRadius_memb, HOFFSET(repo_component_t, outerRadius), PredType::IEEE_F64LE); 
    mtype.insertMember( x_memb, HOFFSET(repo_component_t, x), PredType::IEEE_F64LE); 
    mtype.insertMember( y_memb, HOFFSET(repo_component_t, y), PredType::IEEE_F64LE); 
    mtype.insertMember( z_memb, HOFFSET(repo_component_t, z), PredType::IEEE_F64LE); 
    mtype.insertMember( timestamp_memb, HOFFSET(repo_component_t, timestamp), PredType::NATIVE_INT); 

    DataSet* dataset;
    dataset = new DataSet(subgroup->createDataSet( dataset_name , mtype , *dataspace ));

    // write it, finally 
    dataset->write( repoComponent , mtype );

    delete outputgroup;
    delete subgroup;
    delete dataspace;
    delete dataset;

  } catch (Exception error) {
    error.printError();
  }
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(intData1d data, std::string dsname){ 
  try {
    hsize_t nrows = data.size(); 
  
    hsize_t dims[1];
    dims[0]=nrows;
    hsize_t rank = 1;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims);
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::NATIVE_INT);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array - is this memcpy ugly?
    double dat_array[nrows];
    memcpy( dat_array, &data[0], sizeof( int ) * data.size() );
  
    // write it
    dataset.write(dat_array, type, memspace, filespace );

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(intData2d data, std::string dsname){
  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    
    hsize_t dims[] = {nrows, ncols};
    hsize_t rank = 2;
    
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::NATIVE_INT);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array - is this memcpy ugly?
    int dat_array[nrows][ncols];
    for(int row=0; row<nrows; row++){
      memcpy( dat_array[row], &data[row][0], sizeof( int ) * ncols );
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(intData3d data, std::string dsname){

  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    hsize_t nlayers = data[0][0].size(); 
    
    hsize_t dims[] = {nrows, ncols,nlayers};
    hsize_t rank = 3;
    
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::NATIVE_INT);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array - is this memcpy ugly?
    int dat_array[nrows][ncols][nlayers];
    for(int row=0; row<nrows; row++){
      for(int col=0; col<ncols; col++){
        memcpy( dat_array[row][col], &data[row][col][0], sizeof( int ) * nlayers);
      };
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(dblData1d data, std::string dsname){
  try {
    hsize_t nrows = data.size(); 
  
    hsize_t dims[1];
    dims[0]=nrows;
    hsize_t rank = 1;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::IEEE_F64LE);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
  
    // the data needs to be an array - is this memcpy ugly?
    double dat_array[nrows];
    memcpy( dat_array, &data[0], sizeof( double ) * data.size() );
  
    // write it
    dataset.write(dat_array, type, memspace, filespace );

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(dblData2d data, std::string dsname){
  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    
    hsize_t dims[] = {nrows, ncols};
    hsize_t rank = 2;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::IEEE_F64LE);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);

    // the data needs to be an array - is this memcpy ugly?
    double dat_array[nrows][ncols];
    for(int row=0; row<nrows; row++){
      memcpy( dat_array[row], &data[row][0], sizeof( double ) * ncols );
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(dblData3d data, std::string dsname){
  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    hsize_t nlayers = data[0][0].size(); 
    
    hsize_t dims[] = {nrows, ncols,nlayers};
    hsize_t rank = 3;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::IEEE_F64LE);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array - is this memcpy ugly?
    double dat_array[nrows][ncols][nlayers];
    for(int row=0; row<nrows; row++){
      for(int col=0; col<ncols; col++){
        memcpy( dat_array[row][col], &data[row][col][0], sizeof( double ) * nlayers);
      };
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(strData1d data, std::string dsname){
  try {
    hsize_t nrows = data.size(); 
  
    hsize_t dims[1];
    dims[0]=nrows;
    hsize_t rank = 1;
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    //create a variable length std::string types
    StrType vls_type(0, H5T_VARIABLE); 
    DataType type = DataType(vls_type);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);

    // the data needs to be an array
    std::string dat_array[nrows];
    dat_array;
    for(int row=0; row < nrows;row++){
      dat_array[row]=data[row];
    };
  
    // write it
    dataset.write(dat_array, type, memspace, filespace );

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(strData2d data, std::string dsname){
  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    
    hsize_t dims[] = {nrows, ncols};
    hsize_t rank = 2;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    //create a variable length std::string types
    StrType vls_type(0, H5T_VARIABLE); 
    DataType type = DataType(vls_type);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array
    std::string dat_array[nrows][ncols];
    for(int row=0; row<nrows; row++){
      for(int col=0; col<ncols; col++){
        dat_array[row][col]=data[row][col];
      };
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(strData3d data, std::string dsname){
  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    hsize_t nlayers = data[0][0].size(); 
    
    hsize_t dims[] = {nrows, ncols,nlayers};
    hsize_t rank = 3;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    //create a variable length std::string types
    StrType vls_type(0, H5T_VARIABLE); 
    DataType type = DataType(vls_type);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array 
    std::string dat_array[nrows][ncols][nlayers];
    for(int row=0; row<nrows; row++){
      for(int col=0; col<ncols; col++){
        for(int layer=0; layer<nlayers;layer++){
          dat_array[row][col][layer]=data[row][col][layer];
        };
      };
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(std::string dsname, intData1d& out_data){

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB_->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 

    // double check that its an integer
    IntType inttype;
    if( type_class == H5T_INTEGER ) 
    {
      // oh good, it's an integer. Now figure out what kind.
      inttype = dataset.getIntType();
    }
    else{
      throw CycTypeException("The dataset " + dsname + " is not of integer type");
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
  
    int out_array[dims[0]];
    for (int i=0; i<dims[0]; i++){
      out_array[i]=1;
    };
  
    dataset.read( out_array, inttype, memspace , filespace );
    
    out_data.assign(out_array, out_array + sizeof(out_array));

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(std::string dsname, intData2d& out_data){

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB_->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its an integer
    IntType inttype;
    if( type_class == H5T_INTEGER ) 
    {
      // oh good, it's an integer. Now figure out what kind.
      inttype = dataset.getIntType();
    }
    else{
      throw CycTypeException("The dataset " + dsname + " is not of integer type");
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
  
    int out_array[dims[0]][dims[1]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        out_array[i][j]=1;
      };
    };
  
    dataset.read( out_array, inttype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        out_data[row][col] = out_array[row][col];
      };
    };

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(std::string dsname, intData3d& out_data){

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB_->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its an integer
    IntType inttype;
    if( type_class == H5T_INTEGER ) 
    {
      // oh good, it's an integer. Now figure out what kind.
      inttype = dataset.getIntType();
    }
    else{
      throw CycTypeException("The dataset " + dsname + " is not of integer type");
    }
  
    // get the file dataspace
    DataSpace filespace = dataset.getSpace();
  
    // find the rank
    int rank = filespace.getSimpleExtentNdims();
  
    // find what the dataspace dimensions are
    hsize_t dims[3];
    int ndims = filespace.getSimpleExtentDims(dims, NULL);
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
  
    // select everything in each dataspace
    memspace.selectAll();
    filespace.selectAll();
  
    int out_array[dims[0]][dims[1]][dims[2]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        for (int k=0; k<dims[2]; k++){
          out_array[i][j][k]=1;
        };
      };
    };
  
    dataset.read( out_array, inttype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        for(int layer=0; layer<dims[2];layer++){
          out_data[row][col][layer] = out_array[row][col][layer];
        };
      };
    };

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(std::string dsname, dblData1d& out_data){
  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB_->openDataSet(dsname);
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
      throw CycTypeException("The dataset " + dsname + " is not of float type");
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
  
    double out_array[dims[0]];
    for (int i=0; i<dims[0]; i++){
      out_array[i]=1;
    };
  
    dataset.read( out_array, dbltype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      out_data[row]= out_array[row];
    };
    
    out_data.assign(out_array, out_array + sizeof(out_array));

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(std::string dsname, dblData2d& out_data){
  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB_->openDataSet(dsname);
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
      throw CycTypeException("The dataset " + dsname + " is not of float type");
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
  
    double out_array[dims[0]][dims[1]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        out_array[i][j]=1.0;
      };
    };
  
    dataset.read( out_array, dbltype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        out_data[row][col] = out_array[row][col];
      };
    };

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(std::string dsname, dblData3d& out_data){

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB_->openDataSet(dsname);
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
      throw CycTypeException("The dataset " + dsname + " is not of double type");
    }
  
    // get the file dataspace
    DataSpace filespace = dataset.getSpace();
  
    // find the rank
    int rank = filespace.getSimpleExtentNdims();
  
    // find what the dataspace dimensions are
    hsize_t dims[3];
    int ndims = filespace.getSimpleExtentDims(dims, NULL);
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
  
    // select everything in each dataspace
    memspace.selectAll();
    filespace.selectAll();
  
    double out_array[dims[0]][dims[1]][dims[2]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        for (int k=0; k<dims[2]; k++){
          out_array[i][j][k]=1.0;
        };
      };
    };
  
    dataset.read( out_array, dbltype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        for(int layer=0; layer<dims[2];layer++){
          out_data[row][col][layer] = out_array[row][col][layer];
        };
      };
    };

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(std::string dsname, strData1d& out_data){
  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB_->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 

    // double check that its a std::string
    StrType strtype;
    if( type_class == H5T_STRING ) 
    {
      // oh good, it's a std::string. Now figure out what kind.
      strtype = dataset.getStrType();
    }
    else{
      throw CycTypeException("The dataset " + dsname + " is not of std::string type");
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
  
    std::string out_array[dims[0]];
    for (int i=0; i<dims[0]; i++){
      out_array[i]="";
    };
  
    dataset.read( out_array, strtype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      out_data[row] = out_array[row];
    };

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(std::string dsname, strData2d& out_data){
  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB_->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its a std::string
    StrType strtype;
    if( type_class == H5T_STRING ) 
    {
      // oh good, it's a std::string. Now figure out what kind.
      strtype = dataset.getStrType();
    }
    else{
      throw CycTypeException("The dataset " + dsname + " is not of std::string type");
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
  
    std::string out_array[dims[0]][dims[1]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        out_array[i][j]="";
      };
    };
  
    dataset.read( out_array, strtype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        out_data[row][col] = out_array[row][col];
      };
    };

  } catch (Exception error) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(std::string dsname, strData3d& out_data){

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB_->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its an std::string
    StrType  strtype;
    if( type_class == H5T_STRING ) 
    {
      // oh good, it's a std::string. Now figure out what kind.
      strtype = dataset.getStrType();
    }
    else{
      throw CycTypeException("The dataset " + dsname + " is not of std::string type");
    }
  
    // get the file dataspace
    DataSpace filespace = dataset.getSpace();
  
    // find the rank
    int rank = filespace.getSimpleExtentNdims();
  
    // find what the dataspace dimensions are
    hsize_t dims[3];
    int ndims = filespace.getSimpleExtentDims(dims, NULL);
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
  
    // select everything in each dataspace
    memspace.selectAll();
    filespace.selectAll();
  
    std::string out_array[dims[0]][dims[1]][dims[2]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        for (int k=0; k<dims[2]; k++){
          out_array[i][j][k]="";
        };
      };
    };
  
    dataset.read( out_array, strtype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        for(int layer=0; layer<dims[2];layer++){
          out_data[row][col][layer] = out_array[row][col][layer];
        };
      };
    };

  } catch (Exception error) {
    error.printError();
  }
};

