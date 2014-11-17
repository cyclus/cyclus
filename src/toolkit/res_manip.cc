#include "res_manip.h"
#include "comp_math.h"

namespace cyclus {
namespace toolkit {

Product::Ptr Squash(std::vector<Product::Ptr> ps) {
  if (ps.size() == 0) {
    throw Error("cannot squash zero resources together");
  }

  Product::Ptr p = ps[0];
  for (int i = 1; i < ps.size(); ++i) {
    p->Absorb(ps[i]);
  }
  return p;
}

Material::Ptr Squash(std::vector<Material::Ptr> ms) {
  if (ms.size() == 0) {
    throw Error("cannot squash zero resources together");
  }

  Material::Ptr m = ms[0];
  for (int i = 1; i < ms.size(); ++i) {
    m->Absorb(ms[i]);
  }
  return m;
}

Resource::Ptr Squash(std::vector<Resource::Ptr> rs) {
  if (rs.size() == 0) {
    throw Error("cannot squash zero resources together");
  }

  std::vector<Material::Ptr> mats = ResCast<Material>(rs);
  if (mats[0] != NULL) {
    return Squash(mats);
  }
  std::vector<Product::Ptr> prods = ResCast<Product>(rs);
  if (prods[0] != NULL) {
    return Squash(prods);
  }

  throw Error("cannot squash resource type " + rs[0]->type());
}
  
}  // namespace toolkit
}  // namespace cyclus

