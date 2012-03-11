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
  for (int i = 0; i < mats_.size(); i++) {
    tot += mats_.at(i).quantity();
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
  MatManifest manifest;
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

