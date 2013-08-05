// ResourceBuff.cpp
#include "CycLimits.h"
#include "ResourceBuff.h"
#include "error.h"

namespace cyclus {

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
    throw ValueError("New capacity lower than existing quantity");
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
    throw ValueError("Removal quantity larger than store tot quantity.");
  } else if (qty < cyclus::eps_rsrc()) {
    throw ValueError("Removal quantity cannot be negative.");
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
      mats_present_.erase(mat);
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
    throw ValueError("Remove count larger than store count.");
  }

  Manifest manifest;
  for (int i = 0; i < num; i++) {
    rsrc_ptr mat = mats_.front();
    mats_.pop_front();
    manifest.push_back(mat);
    mats_present_.erase(mat);
    qty_ -= mat->quantity();
  }

  return manifest;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rsrc_ptr ResourceBuff::popOne() {
  if (mats_.size() < 1) {
    throw ValueError("Cannot pop material from an empty store.");
  }
  rsrc_ptr mat = mats_.front();
  qty_ -= mat->quantity();

  mats_.pop_front();
  mats_present_.erase(mat);
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::pushOne(rsrc_ptr mat) {
  if (mat->quantity() - space() > cyclus::eps_rsrc()) {
    throw ValueError("Resource pushing breaks capacity limit.");
  } else if (mats_present_.count(mat) == 1) {
    throw KeyError("Duplicate resource pushing attempted");
  }

  qty_ += mat->quantity();
  mats_.push_back(mat);
  mats_present_.insert(mat);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::pushAll(Manifest mats) {
  double tot_qty = 0;
  for (int i = 0; i < mats.size(); i++) {
    tot_qty += mats.at(i)->quantity();
  }
  if (tot_qty - space() > cyclus::eps_rsrc()) {
    throw ValueError("Resource pushing breaks capacity limit.");
  }

  for (int i = 0; i < mats.size(); i++) {
    if (mats_present_.count(mats.at(i)) == 1) {
      throw KeyError("Duplicate resource pushing attempted");
    }
  }

  for (int i = 0; i < mats.size(); i++) {
    rsrc_ptr mat = mats.at(i);
    mats_.push_back(mat);
    mats_present_.insert(mat);
  }
  qty_ += tot_qty;
}

} // namespace cyclus
