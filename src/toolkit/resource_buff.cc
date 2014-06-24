#include "resource_buff.h"

#include <iomanip>

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::set_capacity(double cap) {
  if (quantity() - cap > eps_rsrc()) {
    std::stringstream ss;
    ss << std::setprecision(17) <<"new capacity " << cap
       << " lower than existing quantity " << quantity();
    throw ValueError(ss.str());
  }
  capacity_ = cap;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Manifest ResourceBuff::PopQty(double qty) {
  if (qty > quantity()) {
    std::stringstream ss;
    ss << std::setprecision(17) <<"removal quantity " << qty
       << " larger than buff quantity " << quantity();
    throw ValueError(ss.str());
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

  if (count() == 0) {
    qty_ = 0;
  }

  return manifest;
}

Manifest ResourceBuff::PopQty(double qty, double eps) {
  if (qty > quantity() + eps) {
    std::stringstream ss;
    ss << std::setprecision(17) <<"removal quantity " << qty
       << " larger than buff quantity " << quantity();
    throw ValueError(ss.str());
  }

  if (qty >= quantity()) {
    return PopN(count());
  }
  return PopQty(qty);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Manifest ResourceBuff::PopN(int num) {
  if (count() < num || num < 0) {
    std::stringstream ss;
    ss << "remove count " << num << " larger than buff count " << count();
    throw ValueError(ss.str());
  }

  Manifest manifest;
  for (int i = 0; i < num; i++) {
    Resource::Ptr r = mats_.front();
    mats_.pop_front();
    manifest.push_back(r);
    mats_present_.erase(r);
    qty_ -= r->quantity();
  }

  if (count() == 0) {
    qty_ = 0;
  }

  return manifest;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr ResourceBuff::Pop(AccessDir dir) {
  if (mats_.size() < 1) {
    throw ValueError("cannot pop material from an empty buff");
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

  if (count() == 0) {
    qty_ = 0;
  }

  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ResourceBuff::Push(Resource::Ptr r) {
  if (r->quantity() - space() > eps_rsrc()) {
    throw ValueError("resource pushing breaks capacity limit");
  } else if (mats_present_.count(r) == 1) {
    throw KeyError("duplicate resource push attempted");
  }

  qty_ += r->quantity();
  mats_.push_back(r);
  mats_present_.insert(r);
}

}  // namespace toolkit
}  // namespace cyclus
