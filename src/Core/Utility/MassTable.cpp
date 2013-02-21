// MassTable class

#include <iostream>
#include <stdlib.h>

#include "MassTable.h"
#include "SqliteDb.h"

#include "Env.h"
#include "CycException.h"

using namespace std;

MassTable* MassTable::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable* MassTable::Instance() {
  // If we haven't created a MassTable yet, create it, and then and return it
  // either way.
  if (0 == instance_) {
    instance_ = new MassTable();
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable::MassTable() {
  initializeSQL();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable::~MassTable() {
  //Should close the 'mass.h5' file
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MassTable::gramsPerMol(int tope) {
  double toRet = nuclide_vec_[isoIndex_[tope]].mass;
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MassTable::initializeSQL() {
  // get the file location
  string file_path = Env::getBuildPath() + "/share/mass.sqlite";
  SqliteDb *db = new SqliteDb(file_path);

  std::vector<StrList> znums = db->query("SELECT Z FROM isotopemasses");
  std::vector<StrList> anums = db->query("SELECT A FROM isotopemasses");
  std::vector<StrList> mnums = db->query("SELECT Mass FROM isotopemasses");
  
  for (int i = 0; i < znums.size(); i++){
    // // obtain the database row and declare the appropriate members
    string aStr = anums.at(i).at(0);
    string zStr = znums.at(i).at(0);
    string mStr = mnums.at(i).at(0);
    int z = atoi( zStr.c_str() );
    int a = atoi( aStr.c_str() );
    double mass = atof( mStr.c_str() );
    // create a nuclide member and add it to the nuclide vector
    nuclide_t n = {z, a, mass};
    nuclide_vec_.push_back(n);
    // create an index and log it accordingly
    int tope = z * 1000 + a;
    isoIndex_.insert(make_pair(tope, i));
  }
  // set the total number of nuclides
  nuclide_len_ = nuclide_vec_.size();

  delete db;
}


