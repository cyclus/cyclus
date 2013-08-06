#include <iostream>
#include <string>
#include <fstream>

#include "env.h"
#include "error.h"
#include "logger.h"
#include "uniform_taylor.h"

#include "decay_handler.h"

namespace cyclus {

bool DecayHandler::decay_info_loaded_ = false;
ParentMap DecayHandler::parent_ = ParentMap();
DaughtersMap DecayHandler::daughters_ = DaughtersMap();
Matrix DecayHandler::decayMatrix_ = Matrix();
IsoList DecayHandler::IsotopesTracked_ = IsoList();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DecayHandler::DecayHandler() {
  if (!decay_info_loaded_) {
    DecayHandler::LoadDecayInfo();
    decay_info_loaded_ = true;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DecayHandler::LoadDecayInfo() {
  std::string path = Env::GetBuildPath() + "/share/decayInfo.dat";
  std::ifstream decayInfo (path.c_str());

  if ( decayInfo.is_open() ) {
    int jcol = 1;
    int iso = 0;
    int nDaughters = 0;
    double decayConst = 0; // decay constant, in inverse years
    double branchRatio = 0;
      
    decayInfo >> iso;  // get first parent
    
    // checks to see if there are isotopes in 'decayInfo.dat'
    if ( decayInfo.eof() ) {
      std::string err_msg = "There are no isotopes in the 'decayInfo.dat' file";
      throw ValidationError(err_msg);
    }
    
    // processes 'decayInfo.dat'
    while ( !decayInfo.eof() ) {
      // make parent
      decayInfo >> decayConst;
      decayInfo >> nDaughters;
      AddIsoToList(iso);

      // checks for duplicate parent isotopes
      if ( parent_.find(iso) == parent_.end() ) {
        parent_[iso] = std::make_pair(jcol, decayConst);
           
        // make daughters
        std::vector< std::pair<int,double> > temp(nDaughters);
        for ( int i = 0; i < nDaughters; ++i ) {
          decayInfo >> iso;
          decayInfo >> branchRatio;
          AddIsoToList(iso);
          
          // checks for duplicate daughter isotopes
          for ( int j = 0; j < nDaughters; ++j ) {
            if ( temp[j].first == iso ) {
              throw ValidationError(std::string("A duplicate daughter isotope, %i , was found in decayInfo.dat", iso));
            } 
          }
          temp[i] = std::make_pair(iso, branchRatio);
        }
           
        daughters_[jcol] = temp;
        ++jcol; // set next column
      } else {
        std::string err_msg;
        err_msg = "A duplicate parent isotope was found in 'decayInfo.dat'";
        throw ValidationError(err_msg);
      }
      decayInfo >> iso; // get next parent
    }
    // builds the decay matrix from the parent and daughter maps
    BuildDecayMatrix();
  } else {
    throw IOError("Could not find file 'decayInfo.dat'.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DecayHandler::AddIsoToList(int iso) {
  bool exists = (find(IsotopesTracked_.begin(), IsotopesTracked_.end(),iso)!=IsotopesTracked_.end());
  if (!exists){
    IsotopesTracked_.push_back(iso);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DecayHandler::SetComp(CompMapPtr comp) {
  atom_comp_ = comp;
  BuildDecayMatrix();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DecayHandler::SetComp(Vector comp) {
  atom_comp_.reset(new CompMap(ATOM));

  // loops through the ParentMap and populates the new composition map with
  // the number density from the comp parameter for each isotope
  ParentMap::const_iterator parent_iter = parent_.begin(); // get first parent
  while( parent_iter != parent_.end() ) {
    int iso = parent_iter->first;
    int col = parent_.find(iso)->second.first; // get Vector position
    
    // checks to see if the Vector position is valid
    if ( col <= comp.NumRows() ) {
      double atom_count = comp(col,1);
      // adds isotope to the map if its number density is non-zero
      if ( atom_count != 0 )
        (*atom_comp_)[iso] = atom_count;
    } else {
      LOG(LEV_ERROR, "none!") << "Decay Error - invalid Vector position";
    }
    ++parent_iter; // get next parent
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Vector DecayHandler::CompAsVector() {

  // solves the decay equation for the final composition
        
  Vector comp_vector = Vector(parent_.size(),1);

  std::map<int, double>::const_iterator comp_iter = atom_comp_->begin();
  while( comp_iter != atom_comp_->end() ) {
    int iso = comp_iter->first;
    long double atom_count = comp_iter->second;

    // if the isotope is tracked in the decay matrix  
    if ( parent_.count(iso) > 0 ) {
      int col = parent_.find(iso)->second.first; // get Vector position
      comp_vector(col,1) = atom_count;
    // if it is not in the decay matrix, then it is added as a stable isotope
    } else {
      double decayConst = 0;
      int col = parent_.size() + 1;
      parent_[iso] = std::make_pair(col, decayConst);  // add isotope to parent map

      int nDaughters = 0;
      std::vector< std::pair<int,double> > temp(nDaughters);
      daughters_[col] = temp;  // add isotope to daughters map
      
      std::vector<long double> row(1, atom_count);
      comp_vector.AddRow(row);  // add isotope to the end of the Vector
    }

    ++comp_iter; // get next isotope
  }

  return comp_vector;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CompMapPtr DecayHandler::comp() {
  return atom_comp_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DecayHandler::BuildDecayMatrix() {
  double decayConst = 0; // decay constant, in inverse years
  int jcol = 1;
  int n = parent_.size();
  decayMatrix_ = Matrix(n,n);

  ParentMap::const_iterator parent_iter = parent_.begin(); // get first parent

  // populates the decay matrix column by column
  while( parent_iter != parent_.end() ) {
    jcol = parent_iter->second.first; // determines column index
    decayConst = parent_iter->second.second;
    decayMatrix_(jcol,jcol) = -1 * decayConst; // sets A(i,i) value
 
    // processes the vector in the daughters map if it is not empty
    if ( !daughters_.find(jcol)->second.empty() ) {
      // an iterator that points to 1st daughter in the vector
      // pair<isotope,branchratio>
      std::vector< std::pair<int, double> >::const_iterator iso_iter = \
                                              daughters_.find(jcol)->second.begin();

      // processes all daughters of the parent
      while ( iso_iter != daughters_.find(jcol)->second.end() ) {
        int iso = iso_iter->first;
        int irow = parent_.find(iso)->second.first; // determines row index
        double branchRatio = iso_iter->second;
        decayMatrix_(irow,jcol) = branchRatio * decayConst; // sets A(i,j) value
    
        ++iso_iter; // get next daughter
      }
    }
    ++parent_iter; // get next parent
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DecayHandler::Decay(double years) {
  // solves the decay equation for the final composition
  Vector vect = UniformTaylor::MatrixExpSolver(decayMatrix_, CompAsVector(), years);
  SetComp(vect);
}
} // namespace cyclus
