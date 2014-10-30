#include "res_manip.h"
#include "comp_math.h"

namespace cyclus {
namespace toolkit {

Product::Ptr SquashProd(std::vector<Product::Ptr> ps) {
  if (ps.size() == 0) {
    throw Error("cannot squash zero resources together");
  }

  Product::Ptr p = ps[0];
  for (int i = 1; i < ps.size(); ++i) {
    p->Absorb(ps[i]);
  }
  return p;
}

Material::Ptr SquashMat(std::vector<Material::Ptr> ms) {
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
    return SquashMat(mats);
  }
  std::vector<Product::Ptr> prods = ResCast<Product>(rs);
  if (prods[0] != NULL) {
    return SquashProd(prods);
  }

  throw Error("cannot squash resource type " + rs[0]->type());
}
  
Material::Ptr Separate(Material::Ptr m, std::map<Nuc, double> fracs) {
  CompMap cm = m->comp()->mass();
  compmath::Normalize(&cm, m->quantity());
  CompMap::iterator it;
  double qty = 0;
  for (it = cm.begin(); it != cm.end(); ++it) {
    Nuc n = it->first;
    if (fracs.count(n) == 0) {
      cm[n] = 0;
    } else {
      cm[n] *= fracs[n];
      qty += cm[n];
    }
  }

  Composition::Ptr c = Composition::CreateFromMass(cm);
  return m->ExtractComp(std::min(qty, m->quantity()), c);
}

}  // namespace toolkit
}  // namespace cyclus

