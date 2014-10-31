#include "res_buf.h"
#include "cyc_arithmetic.h"

#include <iomanip>

namespace cyclus {
namespace toolkit {

template <class T>
void ResBuf<T>::cap(double cap) {
  if (qty() - cap > eps_rsrc()) {
    std::stringstream ss;
    ss << std::setprecision(17) <<"new capacity " << cap
       << " lower than existing quantity " << qty();
    throw ValueError(ss.str());
  }
  cap_ = cap;
}

template <class T>
typename T::Ptr ResBuf<T>::Peek() {
  if (mats_.size() < 1) {
    throw ValueError("cannot peek at resource from an empty buff");
  }
  return mats_.front();
}

template <class T>
std::vector<Resource::Ptr> ResBuf<T>::PopQtyRes(double qty) {
  return ResCast<Resource>(PopQty(qty));
}

template <class T>
std::vector<Resource::Ptr> ResBuf<T>::PopQtyRes(double qty, double eps) {
  return ResCast<Resource>(PopQty(qty, eps));
}

template <class T>
std::vector<Resource::Ptr> ResBuf<T>::PopNRes(int num) {
  return ResCast<Resource>(PopN(num));
}

template <class T>
std::vector<typename T::Ptr> ResBuf<T>::PopQty(double qty) {
  if (qty > this->qty()) {
    std::stringstream ss;
    ss << std::setprecision(17) <<"removal quantity " << qty
       << " larger than buff quantity " << this->qty();
    throw ValueError(ss.str());
  }

  std::vector<typename T::Ptr> rs;
  typename T::Ptr r, tmp;
  double left = qty;
  double quan;
  while (left > 0 && n() > 0) {
    r = mats_.front();
    mats_.pop_front();
    quan = r->qty();
    if (quan > left) {
      // too big - split the res before popping
      tmp = r->ExtractRes(left);
      mats_.push_front(r);
      r = tmp;
    } else {
      mats_present_.erase(r);
    }

    rs.push_back(r);
    left -= quan;
  }

  UpdateQty();

  return rs;
}

template <class T>
std::vector<typename T::Ptr> ResBuf<T>::PopQty(double qty, double eps) {
  if (qty > this->qty() + eps) {
    std::stringstream ss;
    ss << std::setprecision(17) <<"removal quantity " << qty
       << " larger than buff quantity " << this->qty();
    throw ValueError(ss.str());
  }

  if (qty >= this->qty()) {
    return PopN(n());
  }
  return PopQty(qty);
}

template <class T>
void ResBuf<T>::UpdateQty() {
  std::list<Resource::Ptr>::iterator it;
  std::vector<double> qtys;
  for (it = mats_.begin(); it != mats_.end(); ++it) {
    qtys.push_back((*it)->quantity());
  }
  qty_ = CycArithmetic::KahanSum(qtys);
}

template <class T>
std::vector<typename T::Ptr> ResBuf<T>::PopN(int num) {
  if (n() < num || num < 0) {
    std::stringstream ss;
    ss << "remove count " << num << " larger than buff count " << n();
    throw ValueError(ss.str());
  }

  std::vector<typename T::Ptr> rs;
  for (int i = 0; i < num; i++) {
    typename T::Ptr r = mats_.front();
    mats_.pop_front();
    rs.push_back(r);
    mats_present_.erase(r);
  }

  UpdateQty();
  return rs;
}

template <class T>
typename T::Ptr ResBuf<T>::Pop() {
  if (mats_.size() < 1) {
    throw ValueError("cannot pop resource from an empty buff");
  }

  typename T::Ptr r = mats_.front();
  mats_.pop_front();
  mats_present_.erase(r);
  UpdateQty();
  return r;
}

template <class T>
typename T::Ptr ResBuf<T>::PopBack() {
  if (mats_.size() < 1) {
    throw ValueError("cannot pop resource from an empty buff");
  }

  typename T::Ptr r = mats_.back();
  mats_.pop_back();
  mats_present_.erase(r);
  UpdateQty();
  return r;
}

template <class T>
void ResBuf<T>::Push(Resource::Ptr r) {
  if (r->quantity() - space() > eps_rsrc()) {
    std::stringstream ss;
    ss << "resource pushing breaks capacity limit: space=" << space()
       << ", rsrc->qty()=" << r->quantity();
    throw ValueError(ss.str());
  } else if (mats_present_.count(r) == 1) {
    throw KeyError("duplicate resource push attempted");
  }

  Material::Ptr m = boost::dynamic_pointer_cast<T>(r);
  mats_.push_back(m);
  mats_present_.insert(m);
  UpdateQty();
}

}  // namespace toolkit
}  // namespace cyclus
