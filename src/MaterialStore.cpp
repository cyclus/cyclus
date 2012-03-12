// MaterialStore.cpp

#include "MaterialStore.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MaterialStore::MaterialStore() {
  unlimited_ = false;
  capacity_ = 0.0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MaterialStore::~MaterialStore() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialStore::capacity() {
  if (unlimited_) {
    return -1;
  }
  return capacity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MaterialStore::setCapacity(double cap) {
  if (quantity() - cap > STORE_EPS) {
    throw CycOverCapException("New capacity lower than existing quantity");
  }
  capacity_ = cap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int MaterialStore::count() {
  return mats_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialStore::quantity() {
  double tot = 0;
  std::list<mat_rsrc_ptr>::iterator iter = mats_.begin();
  while (iter != mats_.end()) {
    tot += (*iter)->quantity();
    iter++;
  }
  return tot;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialStore::space() {
  if (unlimited_) {
    return -1;
  }
  return capacity_ - quantity();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool MaterialStore::unlimited() {
  return unlimited_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MaterialStore::makeUnlimited() {
  unlimited_ = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MaterialStore::makeLimited(double cap) {
  setCapacity(cap);
  unlimited_ = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest MaterialStore::removeQty(double qty) {
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
  double added = 0;

  while (true) {
    mat = mats_.front();
    if (fabs(mat->quantity() - left) < STORE_EPS) {
      // mat small enough to not be split
      mats_.pop_front();
      manifest.push_back(mat);
      added += mat->quantity();
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
MatManifest MaterialStore::removeNum(int num) {
  MatManifest manifest;
  return manifest;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr MaterialStore::removeOne() {
  mat_rsrc_ptr mat(new Material());
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MaterialStore::addOne(mat_rsrc_ptr mat) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MaterialStore::addAll(MatManifest mats) {
}

