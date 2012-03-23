// MassTable class

#include <iostream>
#include <stdlib.h>

#include "MassTable.h"

#include "Env.h"
#include "Database.h"
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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable::~MassTable() {
  //Should close the 'mass.h5' file
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MassTable::getMassInGrams(int tope) {
  double toRet = nuclide_vec_[isoIndex_[tope]].mass;
  return toRet;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MassTable::initializeSQL() {
  // get the file location
  string file_path = Env::getCyclusPath() + "/Data";
  string file_name = "mass.sqlite";
  Database *db = new Database( file_name, file_path );

  // get mass info
  db->open();

  query_result result = db->query("SELECT * FROM IsotopeMasses");
  
  int nResults = result.size();
  for (int i = 0; i < nResults; i++){
    // // obtain the database row and declare the appropriate members
    query_row row = result.at(i);
    string Zstr = row.at(0), Astr = row.at(1), Mstr = row.at(2);
    int Znum = atoi( Zstr.c_str() );
    int Anum = atoi( Astr.c_str() );
    double mass = atof( Mstr.c_str() );
    // create a nuclide member and add it to the nuclide vector
    nuclide_t n = {Znum,Anum,mass};
    nuclide_vec_.push_back(n);
    // create an index and log it accordingly
    int tope = Znum*1000 + Anum;
    isoIndex_.insert(make_pair(tope,i));
  }
  // set the total number of nuclides
  nuclide_len_ = nuclide_vec_.size();
};
