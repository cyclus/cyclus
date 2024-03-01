#include "package.h"
#include "error.h"

namespace cyclus {

int Package::next_id_ = 1;

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

std::vector<typename T::Ptr> Package::Repackage(typename T::Ptr r,
                                                Package::Ptr pkg) {
    std::vector<typename T::Ptr> rs_pkgd;
    typename T::Ptr r_pkgd;
    
    double fill_mass = GetFillMass(r, pkg);
    if (fill_mass ==0) {
      return rs_pkgd;
    }

    while (r->quantity() > pkg->fill_min()) {
      double pkg_fill = std::min(r->quantity(), fill_mass);
      r_pkgd = boost::dynamic_pointer_cast<T>(r->ExtractRes(pkg_fill));
      r_pkgd->ChangePackageId(pkg->id());
      rs_pkgd.push_back(r_pkgd);
    }
    return rs_pkgd;
  }
  
Package::Package() : id_(next_id_++), fill_min_(0), fill_max_(std::numeric_limits<double>::max()) {}

Package::Package(std::string name, double fill_min, double fill_max, std::string strategy) : name_(name), id_(next_id_++), fill_min_(fill_min), fill_max_(fill_max), strategy_(strategy) {}

} // namespace cyclus
