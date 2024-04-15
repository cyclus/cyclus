#include "resource.h"

namespace cyclus {

int Resource::nextstate_id_ = 1;
int Resource::nextobj_id_ = 1;

void Resource::BumpStateId() {
  state_id_ = nextstate_id_;
  nextstate_id_++;
}

std::vector<Resource::Ptr> Resource::Package(Package::Ptr pkg) {
  std::vector<Resource::Ptr> rs_pkgd;
  Resource::Ptr r_pkgd;

  double fill_mass = pkg->GetFillMass(quantity());
  if (fill_mass == 0) {
    return rs_pkgd;
  }

  while (quantity() > pkg->fill_min()) {
    double pkg_fill = std::min(quantity(), fill_mass);
    r_pkgd = ExtractRes(pkg_fill);
    r_pkgd->ChangePackageId(pkg->id());
    rs_pkgd.push_back(r_pkgd);
  }
  return rs_pkgd;
}

}  // namespace cyclus
