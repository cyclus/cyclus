// resource_buff.cc
#include "resource_buff.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::set_capacity(double cap) {
  if (quantity() - cap > eps_rsrc()) {
    throw ValueError("New capacity lower than existing quantity");
  }
  capacity_ = cap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Manifest ResourceBuff::PopQty(double qty) {
  if (qty > quantity()) {
    throw ValueError("Removal quantity larger than store tot quantity.");
  }

  Manifest manifest;
  Resource::Ptr r, tmp;
  double left = qty;
  double quan;
  while (left > 0 && count() > 0) {
    r = mats_.front();
    mats_.pop_front();
    quan = r->quantity();
    if (quan > left) {
      // too big - split the res before popping
      tmp = r->ExtractRes(left);
      mats_.push_front(r);
      r = tmp;
      qty_ -= left;
    } else {
      mats_present_.erase(r);
      qty_ -= quan;
    }

    manifest.push_back(r);
    left -= quan;
  }

  return manifest;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Manifest ResourceBuff::PopN(int num) {
  if (mats_.size() < num) {
    throw ValueError("Remove count larger than store count.");
  }

  Manifest manifest;
  for (int i = 0; i < num; i++) {
    Resource::Ptr r = mats_.front();
    mats_.pop_front();
    manifest.push_back(r);
    mats_present_.erase(r);
    qty_ -= r->quantity();
  }

  return manifest;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr ResourceBuff::Pop(AccessDir dir) {
  if (mats_.size() < 1) {
    throw ValueError("Cannot pop material from an empty store.");
  }
  
  Resource::Ptr r;
  if (dir == FRONT) {
    r = mats_.front();
    mats_.pop_front();
  } else {
    r = mats_.back();
    mats_.pop_back();
  }
  
  qty_ -= r->quantity();
  mats_present_.erase(r);
  return r;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::Push(Resource::Ptr r) {
  if (r->quantity() - space() > eps_rsrc()) {
    throw ValueError("Resource pushing breaks capacity limit.");
  } else if (mats_present_.count(r) == 1) {
    throw KeyError("Duplicate resource pushing attempted");
  }

  qty_ += r->quantity();
  mats_.push_back(r);
  mats_present_.insert(r);
};

} // namespace cyclus
