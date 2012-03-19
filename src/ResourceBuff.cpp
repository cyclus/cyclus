// ResourceBuff.cpp

#include "ResourceBuff.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ResourceBuff::ResourceBuff() {
  unlimited_ = false;
  capacity_ = 0.0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ResourceBuff::~ResourceBuff() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double ResourceBuff::capacity() {
  if (unlimited_) {
    return -1;
  }
  return capacity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::setCapacity(double cap) {
  if (quantity() - cap > STORE_EPS) {
    throw CycOverCapException("New capacity lower than existing quantity");
  }
  capacity_ = cap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int ResourceBuff::count() {
  return mats_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double ResourceBuff::quantity() {
  double tot = 0;
  std::list<rsrc_ptr>::iterator iter;
  for (iter = mats_.begin(); iter != mats_.end(); iter++) {
    tot += (*iter)->quantity();
  }
  return tot;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double ResourceBuff::space() {
  if (unlimited_) {
    return -1;
  }
  return capacity_ - quantity();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ResourceBuff::unlimited() {
  return unlimited_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::makeUnlimited() {
  unlimited_ = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::makeLimited(double cap) {
  setCapacity(cap);
  unlimited_ = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest ResourceBuff::popQty(double qty) {
  if (qty - quantity() > STORE_EPS) {
    throw CycNegQtyException("Removal quantity larger than store tot quantity.");
  }
  if (qty < 0.0) {
    throw CycNegQtyException("Removal quantity cannot be negative.");
  }

  MatManifest manifest;
  rsrc_ptr mat, leftover;
  double left = qty;
  double quan;
  while (left > STORE_EPS) {
    mat = mats_.front();
    mats_.pop_front();
    quan = mat->quantity();
    if ((quan - left) > STORE_EPS) {
      // too big - split the mat before pushing
      leftover = mat->extract(quan - left);
      mats_.push_front(leftover);
    }
    manifest.push_back(mat);
    left -= quan;
  }
  return manifest;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest ResourceBuff::popNum(int num) {
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
rsrc_ptr ResourceBuff::popOne() {
  if (mats_.size() < 1) {
    throw CycNegQtyException("Cannot pop material from an empty store.");
  }
  rsrc_ptr mat = mats_.front();
  mats_.pop_front();
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::pushOne(rsrc_ptr mat) {
  if (mat->quantity() - space() > STORE_EPS) {
    throw CycOverCapException("Material pushition breaks capacity limit.");
  }
  std::list<rsrc_ptr>::iterator iter;
  for (iter = mats_.begin(); iter != mats_.end(); iter++) {
    if ((*iter) == mat) {
      throw CycDupMatException("Duplicate material pushition attempted.");
    }
  }
  mats_.push_back(mat);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::pushAll(MatManifest mats) {
  double tot_qty = 0;
  for (int i = 0; i < mats.size(); i++) {
    tot_qty += mats.at(i)->quantity();
  }
  if (tot_qty - space() > STORE_EPS) {
    throw CycOverCapException("Material pushition breaks capacity limit.");
  }
  std::list<rsrc_ptr>::iterator iter;
  for (iter = mats_.begin(); iter != mats_.end(); iter++) {
    for (int i = 0; i < mats.size(); i++) {
      if ((*iter) == mats.at(i)) {
        throw CycDupMatException("Duplicate material pushition attempted.");
      }
    }
  }
  for (int i = 0; i < mats.size(); i++) {
    mats_.push_back(mats.at(i));
  }
}

