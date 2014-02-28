// decayer.cc
#include "decayer.h"

#include <fstream>
#include <string>

#include "env.h"
#include "error.h"
#include "logger.h"
#include "uniform_taylor.h"

namespace cyclus {

bool Decayer::decay_info_loaded_ = false;
ParentMap Decayer::parent_ = ParentMap();
DaughtersMap Decayer::daughters_ = DaughtersMap();
Matrix Decayer::decay_matrix_ = Matrix();
NucList Decayer::nuclides_tracked_ = NucList();

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Decayer::Decayer(const CompMap& comp) {
  int nuc;
  int col;
  long double atom_count;
  pre_vect_ = Vector(parent_.size(), 1);
  std::map<int, double>::const_iterator comp_iter = comp.begin();
  for (comp_iter = comp.begin(); comp_iter != comp.end(); ++comp_iter) {
    nuc = comp_iter->first;
    atom_count = comp_iter->second;

    // if the nuclide is tracked in the decay matrix
    if (parent_.count(nuc) > 0) {
      int col = parent_[nuc].first;  // get Vector position
      pre_vect_(col, 1) = atom_count;
      // if it is not in the decay matrix, then it is added as a stable nuclide
    } else {
      col = parent_.size() + 1;
      parent_[nuc] = std::make_pair(col, pyne::decay_const(nuc));  // add nuclide to parent map

      int nDaughters = 0;
      std::vector< std::pair<int, double> > temp(nDaughters);
      daughters_[col] = temp;  // add nuclide to daughters map

      std::vector<long double> row(1, atom_count);
      pre_vect_.AddRow(row);  // add nuclide to the end of the Vector
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Decayer::LoadDecayInfo() {
  std::string path = Env::GetBuildPath() + "/share/decayInfo.dat";
  std::ifstream decayInfo(path.c_str());

  if (!decayInfo.is_open()) {
    throw IOError("Could not find file 'decayInfo.dat'.");
  }

  int jcol = 1;
  int nuc = 0;
  int nDaughters = 0;
  double decayConst = 0;  // decay constant, in inverse years
  double branchRatio = 0;

  decayInfo >> nuc;  // get first parent
  nuc *= 10000;  // put in id form

  // checks to see if there are nuclides in 'decayInfo.dat'
  if (decayInfo.eof()) {
    std::string err_msg = "There are no nuclides in the 'decayInfo.dat' file";
    throw ValidationError(err_msg);
  }

  // processes 'decayInfo.dat'
  while (!decayInfo.eof()) {
    if (parent_.find(nuc) != parent_.end()) {
      std::string err_msg;
      err_msg = "A duplicate parent nuclide was found in 'decayInfo.dat'";
      throw ValidationError(err_msg);
    }

    // make parent
    decayInfo >> decayConst;
    decayInfo >> nDaughters;
    AddNucToList(nuc);

    parent_[nuc] = std::make_pair(jcol, decayConst);

    // make daughters
    std::vector< std::pair<int, double> > temp(nDaughters);
    for (int i = 0; i < nDaughters; ++i) {
      decayInfo >> nuc;
      nuc *= 10000;  // put in id form
      decayInfo >> branchRatio;
      AddNucToList(nuc);

      // checks for duplicate daughter nuclides
      for (int j = 0; j < nDaughters; ++j) {
        if (temp[j].first == nuc) {
          throw ValidationError(
            std::string("A duplicate daughter nuclide, %i , was found in decayInfo.dat",
                        nuc));
        }
      }
      temp[i] = std::make_pair(nuc, branchRatio);
    }

    daughters_[jcol] = temp;
    ++jcol;  // set next column
    decayInfo >> nuc;  // get next parent
    nuc *= 10000;  // put in id form
  }
  BuildDecayMatrix();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Decayer::AddNucToList(int nuc) {
  bool exists = (find(nuclides_tracked_.begin(), nuclides_tracked_.end(), nuc)
                 != nuclides_tracked_.end());
  if (!exists) {
    nuclides_tracked_.push_back(nuc);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Decayer::GetResult(CompMap& comp) {
  // loops through the ParentMap and populates the passed CompMap with
  // the number density from the comp parameter for each nuclide
  ParentMap::const_iterator parent_iter = parent_.begin();  // get first parent
  while (parent_iter != parent_.end()) {
    int nuc = parent_iter->first;
    int col = parent_.find(nuc)->second.first;  // get Vector position

    // checks to see if the Vector position is valid
    if (col <= post_vect_.NumRows()) {
      double atom_count = post_vect_(col, 1);
      // adds nuclide to the map if its number density is non-zero
      if (atom_count > 0) {
        comp[nuc] = atom_count;
      }
    } else {
      LOG(LEV_ERROR, "none!") << "Decay Error - invalid Vector position";
    }
    ++parent_iter;  // get next parent
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Decayer::BuildDecayMatrix() {
  double decayConst = 0;  // decay constant, in inverse years
  int jcol = 1;
  int n = parent_.size();
  decay_matrix_ = Matrix(n, n);

  ParentMap::const_iterator parent_iter = parent_.begin();  // get first parent

  // populates the decay matrix column by column
  while (parent_iter != parent_.end()) {
    jcol = parent_iter->second.first;  // determines column index
    decayConst = parent_iter->second.second;
    decay_matrix_(jcol, jcol) = -1 * decayConst;  // sets A(i,i) value

    // processes the vector in the daughters map if it is not empty
    if (!daughters_.find(jcol)->second.empty()) {
      // an iterator that points to 1st daughter in the vector
      // pair<nuclide,branchratio>
      std::vector< std::pair<int, double> >::const_iterator
      nuc_iter = daughters_.find(jcol)->second.begin();

      // processes all daughters of the parent
      while (nuc_iter != daughters_.find(jcol)->second.end()) {
        int nuc = nuc_iter->first;
        int irow = parent_.find(nuc)->second.first;  // determines row index
        double branchRatio = nuc_iter->second;
        decay_matrix_(irow, jcol) = branchRatio * decayConst;  // sets A(i,j) value

        ++nuc_iter;  // get next daughter
      }
    }
    ++parent_iter;  // get next parent
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Decayer::Decay(double secs) {
  // solves the decay equation for the final composition
  post_vect_ = UniformTaylor::MatrixExpSolver(decay_matrix_, pre_vect_, secs);
}

}  // namespace cyclus
