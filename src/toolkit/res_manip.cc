#include "res_manip.h"

namespace cyclus {
namespace toolkit {

Product::Ptr SquashProd(std::vector<Product::Ptr> rs) {
  if (rs.size() == 0) {
    throw Error("cannot squash zero resources together");
  }

  Product::Ptr r = rs[0];
  for (int i = 1; i < rs.size(); ++i) {
    r->Absorb(rs[i]);
  }
  return r;
}

Material::Ptr SquashMat(std::vector<Material::Ptr> rs) {
  if (rs.size() == 0) {
    throw Error("cannot squash zero resources together");
  }

  Material::Ptr r = rs[0];
  for (int i = 1; i < rs.size(); ++i) {
    r->Absorb(rs[i]);
  }
  return r;
}

Resource::Ptr Squash(std::vector<Resource::Ptr> rs) {
  if (rs.size() == 0) {
    throw Error("cannot squash zero resources together");
  }

  std::vector<Material::Ptr> mats = ResCast<Material>(rs);
  if (mats[0] != NULL) {
    return SquashMat(mats);
  }
  std::vector<Product::Ptr> prods = ResCast<Product>(rs);
  if (prods[0] != NULL) {
    return SquashProd(prods);
  }

  throw Error("cannot squash resource type " + rs[0]->type());
}
  
}  // namespace toolkit
}  // namespace cyclus

