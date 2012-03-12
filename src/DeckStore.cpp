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
  std::list<mat_rsrc_ptr>::iterator iter;
  for (iter = mats_.begin(); iter != mats_.end(); iter++) {
    tot += (*iter)->quantity();
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
  mat_rsrc_ptr mat, leftover;
  double left = qty;
  double quan;
  while (left > STORE_EPS) {
    mat = mats_.front();
    mats_.pop_front();
    quan = mat->quantity();
    if ((quan - left) <= STORE_EPS) {
      // exact match - add entire mat
      manifest.push_back(mat);
    } else if (quan < left) {
      // less than remaining diff - add entire mat
      manifest.push_back(mat);
    } else {
      // too big - split the mat before adding
      leftover = mat->extract(quan - left);
      manifest.push_back(mat);
      mats_.push_front(leftover);
    }
    left -= quan;
  }
  return manifest;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest DeckStore::removeNum(int num) {
  if (mats_.size() < num) {
    throw CycNegQtyException("Remove count larger than store count.");
  }

  MatManifest manifest;
  for (int i = 0; i < num; i++) {
    manifest.push_back(mats_.front());
    mats_.pop_front();
  }
  return manifest;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr DeckStore::removeOne() {
  if (mats_.size() < 1) {
    throw CycNegQtyException("Cannot remove material from an empty store.");
  }
  mat_rsrc_ptr mat = mats_.front();
  mats_.pop_front();
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DeckStore::addOne(mat_rsrc_ptr mat) {
  if (mat->quantity() - space() > STORE_EPS) {
    throw CycOverCapException("Material addition breaks capacity limit.");
  }
  std::list<mat_rsrc_ptr>::iterator iter;
  for (iter = mats_.begin(); iter != mats_.end(); iter++) {
    if ((*iter) == mat) {
      throw CycDupMatException("Duplicate material addition attempted.");
    }
  }
  mats_.push_back(mat);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DeckStore::addAll(MatManifest mats) {
  double tot_qty = 0;
  for (int i = 0; i < mats.size(); i++) {
    tot_qty += mats.at(i)->quantity();
  }
  if (tot_qty - space() > STORE_EPS) {
    throw CycOverCapException("Material addition breaks capacity limit.");
  }
  std::list<mat_rsrc_ptr>::iterator iter;
  for (iter = mats_.begin(); iter != mats_.end(); iter++) {
    for (int i = 0; i < mats.size(); i++) {
      if ((*iter) == mats.at(i)) {
        throw CycDupMatException("Duplicate material addition attempted.");
      }
    }
  }
  for (int i = 0; i < mats.size(); i++) {
    mats_.push_back(mats.at(i));
  }
}

