// MaterialStore.cpp

#include "MaterialStore.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MaterialStore::MaterialStore() {
  unlimited_ = false;
  capacity_ = 0.0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
virtual MaterialStore::~MaterialStore() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialStore::capacity() {
  if (unlimited_) {
    return -1;
  }
  return capacity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MaterialStore::setCapacity(double cap) {
  if (cap < quantity()) {
    throw CycOverCapException("New capacity lower than existing quantity");
  }

  capacity_ = cap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int MaterialStore::count() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialStore::quantity() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double MaterialStore::space() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool MaterialStore::unlimited() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MaterialStore::makeUnlimited() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MaterialStore::makeLimited(double cap) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest MaterialStore::removeQty(double qty) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest MaterialStore::removeNum(int num) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr MaterialStore::removeOne() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MaterialStore::addOne(mat_rsrc_ptr mat) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MaterialStore::addAll(MatManifest mats) {
}

