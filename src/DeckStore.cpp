// DeckStore.cpp

#include "DeckStore.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DeckStore::DeckStore() {
  unlimited_ = false;
  capacity_ = 0.0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DeckStore::~DeckStore() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double DeckStore::capacity() {
  if (unlimited_) {
    return -1;
  }
  return capacity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DeckStore::setCapacity(double cap) {
  if (quantity() - cap > STORE_EPS) {
    throw CycOverCapException("New capacity lower than existing quantity");
  }
  capacity_ = cap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int DeckStore::count() {
  return mats_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double DeckStore::quantity() {
  double tot = 0;
  std::list<mat_rsrc_ptr>::iterator iter = mats_.begin();
  while (iter != mats_.end()) {
    tot += (*iter)->quantity();
    iter++;
  }
  return tot;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double DeckStore::space() {
  if (unlimited_) {
    return -1;
  }
  return capacity_ - quantity();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool DeckStore::unlimited() {
  return unlimited_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DeckStore::makeUnlimited() {
  unlimited_ = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DeckStore::makeLimited(double cap) {
  setCapacity(cap);
  unlimited_ = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest DeckStore::removeQty(double qty) {
  if (qty - quantity() > STORE_EPS) {
    throw CycNegQtyException("Removal quantity larger than store tot quantity.");
  }
  if (qty < 0.0) {
    throw CycNegQtyException("Removal quantity cannot be negative.");
  }

  MatManifest manifest;
  MatManifest::iterator iter;
  mat_rsrc_ptr mat, leftover;
  double left = qty;

  while (true) {
    mat = mats_.front();
    if (fabs(mat->quantity() - left) < STORE_EPS) {
      // mat small enough to not be split
      mats_.pop_front();
      manifest.push_back(mat);
      left -= mat->quantity();
    } else {
      // split the mat before adding
      leftover = mat->extract(mat->quantity() - left);
      mats_.pop_front();
      mats_.push_front(leftover);
      manifest.push_back(mat);
      break;
    }
  }
  return manifest;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest DeckStore::removeNum(int num) {
  MatManifest manifest;
  return manifest;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr DeckStore::removeOne() {
  mat_rsrc_ptr mat(new Material());
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DeckStore::addOne(mat_rsrc_ptr mat) {
  if (mat->quantity() > space() + STORE_EPS) {
    throw CycOverCapException("Material addition breaks capacity limit.");
  }
  mats_.push_back(mat);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DeckStore::addAll(MatManifest mats) {
}

