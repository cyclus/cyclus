#include "resource.h"

namespace cyclus {

int Resource::nextstate_id_ = 1;
int Resource::nextobj_id_ = 1;

void Resource::BumpStateId() {
  state_id_ = nextstate_id_;
  nextstate_id_++;
}

double Resource::GetFillMass(Package::Ptr pkg) {
  if (quantity() < pkg->fill_min()) {
    // less than one pkg of material available
    return 0;
  }

  std::vector<Resource::Ptr> rs;
  Resource::Ptr r_pkgd;
  double fill_mass;
  if (pkg->strategy() == "first") {
    fill_mass = pkg->fill_max();
  } else if (pkg->strategy() == "equal") {
    int num_min_fill = std::floor(quantity() / pkg->fill_min());
    int num_max_fill = std::ceil(quantity() / pkg->fill_max());
    if (num_min_fill >= num_max_fill) {
      // all material can fit in a package
      double fill_mass = quantity() / num_max_fill;
    } else {
      // some material will remain unpackaged, fill up as many max packages as possible
      fill_mass = pkg->fill_max();
    }
  }
  return fill_mass;
}

std::vector<Resource::Ptr> Resource::PackageResource(Package::Ptr pkg) {
  std::vector<Resource::Ptr> rs_pkgd;
  Resource::Ptr r_pkgd;
  
  double fill_mass = GetFillMass(pkg);
  if (fill_mass == 0) {
    return rs_pkgd;
  }

  while (quantity() > pkg->fill_min()) {
    double pkg_fill = std::min(quantity(), fill_mass);
    r_pkgd = boost::dynamic_pointer_cast<Resource>(ExtractRes(pkg_fill));
    r_pkgd->ChangePackageId(pkg->id());
    rs_pkgd.push_back(r_pkgd);
  }
  return rs_pkgd;
}

}  // namespace cyclus
