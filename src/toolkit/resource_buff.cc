#include "resource_buff.h"
#include "cyc_arithmetic.h"

#include <iomanip>

namespace cyclus {
namespace toolkit {

void ResourceBuff::set_capacity(double cap) {
  if (quantity() - cap > eps_rsrc()) {
    std::stringstream ss;
    ss << std::setprecision(17) <<"new capacity " << cap
       << " lower than existing quantity " << quantity();
    throw ValueError(ss.str());
  }
  capacity_ = cap;
}

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
    } else {
      mats_present_.erase(r);
    }

    manifest.push_back(r);
    left -= quan;
  }

  UpdateQty();

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

void ResourceBuff::UpdateQty() {
  std::list<Resource::Ptr>::iterator it;
  std::vector<double> qtys;
  for (it = mats_.begin(); it != mats_.end(); ++it) {
    qtys.push_back((*it)->quantity());
  }
  qty_ = CycArithmetic::KahanSum(qtys);
}

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
  }

  UpdateQty();
  return manifest;
}

Resource::Ptr ResourceBuff::Pop(AccessDir dir) {
  if (mats_.size() < 1) {
    throw ValueError("cannot pop resource from an empty buff");
  }

  Resource::Ptr r;
  if (dir == FRONT) {
    r = mats_.front();
    mats_.pop_front();
  } else {
    r = mats_.back();
    mats_.pop_back();
  }

  mats_present_.erase(r);
  UpdateQty();
  return r;
}

void ResourceBuff::Push(Resource::Ptr r) {
  if (r->quantity() - space() > eps_rsrc()) {
    std::stringstream ss;
    ss << "resource pushing breaks capacity limit: space=" << space()
       << ", rsrc->quantity()=" << r->quantity();
    throw ValueError(ss.str());
  } else if (mats_present_.count(r) == 1) {
    throw KeyError("duplicate resource push attempted");
  }

  mats_.push_back(r);
  mats_present_.insert(r);
  UpdateQty();
}

}  // namespace toolkit
}  // namespace cyclus
