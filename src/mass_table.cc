// MassTable class

#include <iostream>
#include <stdlib.h>

#include "mass_table.h"
#include "sqlite_db.h"

#include "env.h"
#include "error.h"

namespace cyclus {

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
  InitializeSQL();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable::~MassTable() {
  //Should close the 'mass.h5' file
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MassTable::GramsPerMol(int nuc) {
  double toRet = nuclide_vec_[nucIndex_[nuc]].mass;
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MassTable::InitializeSQL() {
  // get the file location
  std::string file_path = Env::GetBuildPath() + "/share/mass.sqlite";
  bool readonly = true;
  SqliteDb* db = new SqliteDb(file_path, readonly);

  std::vector<StrList> znums = db->Query("SELECT Z FROM isotopemasses");
  std::vector<StrList> anums = db->Query("SELECT A FROM isotopemasses");
  std::vector<StrList> mnums = db->Query("SELECT Mass FROM isotopemasses");

  for (int i = 0; i < znums.size(); i++) {
    // // obtain the database row and declare the appropriate members
    std::string aStr = anums.at(i).at(0);
    std::string zStr = znums.at(i).at(0);
    std::string mStr = mnums.at(i).at(0);
    int z = atoi(zStr.c_str());
    int a = atoi(aStr.c_str());
    double mass = atof(mStr.c_str());
    // create a nuclide member and add it to the nuclide vector
    Nuclide n = {z, a, 0, mass};
    nuclide_vec_.push_back(n);
    // create an index and log it accordingly
    int nuc = (z * 10000000) + (a * 10000);
    nucIndex_.insert(std::make_pair(nuc, i));
  }
  // set the total number of nuclides
  nuclide_len_ = nuclide_vec_.size();

  delete db;
}

} // namespace cyclus
