// ResourceBuff.cpp
#include "CycLimits.h"
#include "ResourceBuff.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ResourceBuff::ResourceBuff()
  : capacity_(0),
    qty_(0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ResourceBuff::~ResourceBuff() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double ResourceBuff::capacity() {
  return capacity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::setCapacity(double cap) {
  if (quantity() - cap > cyclus::eps_rsrc()) {
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
  return qty_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double ResourceBuff::space() {
  return capacity_ - qty_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Manifest ResourceBuff::popQty(double qty) {
  if (qty - quantity() > cyclus::eps_rsrc()) {
    throw CycNegQtyException("Removal quantity larger than store tot quantity.");
  }
  if (qty < cyclus::eps_rsrc()) {
    throw CycNegQtyException("Removal quantity cannot be negative.");
  }

  Manifest manifest;
  rsrc_ptr mat, leftover;
  double left = qty;
  double quan;
  while (left > cyclus::eps_rsrc()) {
    mat = mats_.front();
    mats_.pop_front();
    quan = mat->quantity();
    if ((quan - left) > cyclus::eps_rsrc()) {
      // too big - split the mat before pushing
      leftover = mat->clone();
      leftover->setQuantity(quan - left);
      mat->setQuantity(left);
      mats_.push_front(leftover);
      qty_ -= left;
    } else {
      qty_ -= quan;
    }

    manifest.push_back(mat);
    left -= quan;
  }

  return manifest;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Manifest ResourceBuff::popNum(int num) {
  if (mats_.size() < num) {
    throw CycNegQtyException("Remove count larger than store count.");
  }

  Manifest manifest;
  for (int i = 0; i < num; i++) {
    rsrc_ptr mat = mats_.front();
    mats_.pop_front();
    manifest.push_back(mat);
    qty_ -= mat->quantity();
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
  qty_ -= mat->quantity();
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::pushOne(rsrc_ptr mat) {
  if (mat->quantity() - space() > cyclus::eps_rsrc()) {
    throw CycOverCapException("Material pushing of breaks capacity limit.");
  }
  std::list<rsrc_ptr>::iterator iter;
  for (iter = mats_.begin(); iter != mats_.end(); iter++) {
    if ((*iter) == mat) {
      throw CycDupResException("Duplicate material pushing attempted.");
    }
  }
  qty_ += mat->quantity();
  mats_.push_back(mat);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::pushAll(Manifest mats) {
  double tot_qty = 0;
  for (int i = 0; i < mats.size(); i++) {
    tot_qty += mats.at(i)->quantity();
  }
  if (tot_qty - space() > cyclus::eps_rsrc()) {
    throw CycOverCapException("Material pushing breaks capacity limit.");
  }
  std::list<rsrc_ptr>::iterator iter;
  for (iter = mats_.begin(); iter != mats_.end(); iter++) {
    for (int i = 0; i < mats.size(); i++) {
      if ((*iter) == mats.at(i)) {
        throw CycDupResException("Duplicate material pushing attempted.");
      }
    }
  }

  for (int i = 0; i < mats.size(); i++) {
    mats_.push_back(mats.at(i));
  }
  qty_ += tot_qty;
}

