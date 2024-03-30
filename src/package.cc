#include "package.h"
#include "error.h"

namespace cyclus {

// unpackaged id is 1, so start the user-declared packaging id at 2
int Package::next_id_ = 2;

template <class T>
double Package::GetFillMass(typename T::Ptr r) {
  if (r->quantity() < fill_min_) {
    // less than one pkg of material available
    return 0;
  }

  std::vector<typename T::Ptr> rs;
  typename T::Ptr r_pkgd;
  double fill_mass;
  if (strategy_ == "first") {
    fill_mass = fill_max_;
  } else if (strategy_ == "equal") {
    int num_min_fill = std::floor(r->quantity() / fill_min_);
    int num_max_fill = std::ceil(r->quantity() / fill_max_);
    if (num_min_fill >= num_max_fill) {
      // all material can fit in a package
      double fill_mass = r->quantity() / num_max_fill;
    } else {
      // some material will remain unpackaged, fill up as many max packages as possible
      fill_mass = fill_max_;
    }
  }
  return fill_mass;
}

template <class T>
std::vector<typename T::Ptr> Package::PackageResource(typename T::Ptr r) {
  std::vector<typename T::Ptr> rs_pkgd;
  typename T::Ptr r_pkgd;
  
  double fill_mass = GetFillMass(r);
  if (fill_mass ==0) {
    return rs_pkgd;
  }

  while (r->quantity() > fill_min_) {
    double pkg_fill = std::min(r->quantity(), fill_mass);
    r_pkgd = boost::dynamic_pointer_cast<T>(r->ExtractRes(pkg_fill));
    r_pkgd->ChangePackageId(id_);
    rs_pkgd.push_back(r_pkgd);
  }
  return rs_pkgd;
}

Package::Ptr Package::Create() {
  Ptr p(new Package());
  return p;
}

Package::Ptr Package::Create(std::string name, double fill_min, double fill_max, std::string strategy) {
  if (fill_min < 0 || fill_max < 0) {
    throw ValueError("fill_min and fill_max must be non-negative");
  }
  else if (fill_min > fill_max) {
    throw ValueError("fill_min must be less than or equal to fill_max");
  }
  Ptr p(new Package(name, fill_min, fill_max, strategy));
  return p;
}
  
Package::Package() : id_(next_id_++), fill_min_(0), fill_max_(std::numeric_limits<double>::max()) {}

Package::Package(std::string name, double fill_min, double fill_max, std::string strategy) : name_(name), id_(next_id_++), fill_min_(fill_min), fill_max_(fill_max), strategy_(strategy) {}

} // namespace cyclus
