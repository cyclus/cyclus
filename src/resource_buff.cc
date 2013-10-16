// resource_buff.cc
#include "cyc_limits.h"
#include "resource_buff.h"
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
void ResourceBuff::SetCapacity(double cap) {
  if (quantity() - cap > eps_rsrc()) {
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
Manifest ResourceBuff::PopQty(double qty) {
  if (qty - quantity() > eps_rsrc()) {
    throw ValueError("Removal quantity larger than store tot quantity.");
  } else if (qty < eps_rsrc()) {
    throw ValueError("Removal quantity cannot be negative.");
  }

  Manifest manifest;
  Resource::Ptr mat, leftover;
  double left = qty;
  double quan;
  while (left > eps_rsrc()) {
    mat = mats_.front();
    mats_.pop_front();
    quan = mat->quantity();
    if ((quan - left) > eps_rsrc()) {
      // too big - split the mat before pushing
      leftover = mat->ExtractRes(quan - left);
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
Manifest ResourceBuff::PopNum(int num) {
  if (mats_.size() < num) {
    throw ValueError("Remove count larger than store count.");
  }

  Manifest manifest;
  for (int i = 0; i < num; i++) {
    Resource::Ptr mat = mats_.front();
    mats_.pop_front();
    manifest.push_back(mat);
    mats_present_.erase(mat);
    qty_ -= mat->quantity();
  }

  return manifest;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr ResourceBuff::PopOne() {
  if (mats_.size() < 1) {
    throw ValueError("Cannot pop material from an empty store.");
  }
  Resource::Ptr mat = mats_.front();
  qty_ -= mat->quantity();

  mats_.pop_front();
  mats_present_.erase(mat);
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::PushOne(Resource::Ptr mat) {
  if (mat->quantity() - space() > eps_rsrc()) {
    throw ValueError("Resource pushing breaks capacity limit.");
  } else if (mats_present_.count(mat) == 1) {
    throw KeyError("Duplicate resource pushing attempted");
  }

  qty_ += mat->quantity();
  mats_.push_back(mat);
  mats_present_.insert(mat);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::PushAll(Manifest mats) {
  double tot_qty = 0;
  for (int i = 0; i < mats.size(); i++) {
    tot_qty += mats.at(i)->quantity();
  }
  if (tot_qty - space() > eps_rsrc()) {
    throw ValueError("Resource pushing breaks capacity limit.");
  }

  for (int i = 0; i < mats.size(); i++) {
    if (mats_present_.count(mats.at(i)) == 1) {
      throw KeyError("Duplicate resource pushing attempted");
    }
  }

  for (int i = 0; i < mats.size(); i++) {
    Resource::Ptr mat = mats.at(i);
    mats_.push_back(mat);
    mats_present_.insert(mat);
  }
  qty_ += tot_qty;
}

} // namespace cyclus
