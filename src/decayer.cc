// decayer.cc
#include "decayer.h"

#include <fstream>
#include <string>

#include "env.h"
#include "error.h"
#include "logger.h"
#include "uniform_taylor.h"

namespace cyclus {

ParentMap Decayer::parent_ = ParentMap();
DaughtersMap Decayer::daughters_ = DaughtersMap();
Matrix Decayer::decay_matrix_ = Matrix();
NucList Decayer::nuclides_tracked_ = NucList();

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Decayer::Decayer(const CompMap& comp) {
  int nuc;
  int col;
  long double atom_count;
  bool needs_build = false;

  std::map<int, double>::const_iterator comp_iter = comp.begin();
  for (comp_iter = comp.begin(); comp_iter != comp.end(); ++comp_iter) {
    nuc = comp_iter->first;
    atom_count = comp_iter->second;
    if (!IsNucTracked(nuc)) {
      needs_build = true;
      AddNucToMaps(nuc);
    }
  }

  if (needs_build)
    BuildDecayMatrix();

  pre_vect_ = Vector(parent_.size(), 1);
  for (comp_iter = comp.begin(); comp_iter != comp.end(); ++comp_iter) {
    nuc = comp_iter->first;
    atom_count = comp_iter->second;
    col = parent_[nuc].first;
    pre_vect_.Print();
    pre_vect_(col, 1) = atom_count;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Decayer::AddNucToMaps(int nuc) {
  int i;
  int col;
  int daughter;
  std::set<int> daughters;
  std::set<int>::iterator d; 

  if (IsNucTracked(nuc))
    return;

  col = parent_.size() + 1;
  parent_[nuc] = std::make_pair(col, pyne::decay_const(nuc));  
  AddNucToList(nuc);

  i = 0;
  daughters = pyne::decay_children(nuc);
  std::vector< std::pair<int, double> > dvec(daughters.size());
  for (d = daughters.begin(); d != daughters.end(); ++d) {
    daughter = *d;
    AddNucToMaps(daughter);
    dvec[i] = std::make_pair<int, double>(daughter, pyne::branch_ratio(nuc, daughter));
    i++;
  }
  daughters_[col] = dvec;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Decayer::IsNucTracked(int nuc) {
  return (find(nuclides_tracked_.begin(), nuclides_tracked_.end(), nuc)
          != nuclides_tracked_.end());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Decayer::AddNucToList(int nuc) {
  if (!IsNucTracked(nuc)) {
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
  double decayConst = 0;  // decay constant, in inverse secs
  int jcol = 1;
  int n = parent_.size();
  decay_matrix_ = Matrix(n, n);

  ParentMap::const_iterator parent_iter = parent_.begin();  // get first parent

  // populates the decay matrix column by column
  while (parent_iter != parent_.end()) {
    jcol = parent_iter->second.first;  // determines column index
    decayConst = parent_iter->second.second;
    // Gross heuristic for mostly stable nuclides 2903040000 sec / 100 years
    if ((long double) exp(-2903040000 * decayConst) == 0.0)
      decayConst = 0.0;
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
