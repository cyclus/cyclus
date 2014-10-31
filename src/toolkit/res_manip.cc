#include "res_manip.h"
#include "comp_math.h"

namespace cyclus {
namespace toolkit {

NucMap::NucMap() { }

NucMap::NucMap(CompMap m) {
  nucs_ = m;
}

NucMap& NucMap::Add(std::string nuc, double val) {
  nucs_[pyne::nucname::id(nuc)] = val;
  return *this;
}
NucMap& NucMap::Add(Nuc n, double val) {
  nucs_[n] = val;
  return *this;
}
Composition::Ptr NucMap::Atom() {
  return Composition::CreateFromAtom(nucs_);
}
double NucMap::Val(Nuc n) {
  if (nucs_.count(n) == 0) {
    return 0;
  }
  return nucs_[n];
}
Composition::Ptr NucMap::Mass() {
  return Composition::CreateFromMass(nucs_);
}
CompMap NucMap::Map() {
  return nucs_;
}

EltMap::EltMap() { }

EltMap::EltMap(std::map<int, double> elts) {
  elts_ = elts;
}

EltMap& EltMap::Add(std::string elt, double val) {
  elts_[pyne::nucname::znum(elt)] = val;
  return *this;
}
EltMap& EltMap::Add(int elt_z, double val) {
  elts_[elt_z] = val;
  return *this;
}
double EltMap::Val(Nuc n) {
  int z = pyne::nucname::znum(n);
  if (elts_.count(z) == 0) {
    return 0;
  }
  return elts_[z];
}

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
  
Material::Ptr Separate(Material::Ptr m, NucVals& fracs) {
  CompMap cm = m->comp()->mass();
  compmath::Normalize(&cm, m->quantity());
  CompMap::iterator it;
  double qty = 0;
  for (it = cm.begin(); it != cm.end(); ++it) {
    Nuc n = it->first;
    if (fracs.Val(n) == 0) {
      cm[n] = 0;
    } else {
      cm[n] *= fracs.Val(n);
      qty += cm[n];
    }
  }

  Composition::Ptr c = Composition::CreateFromMass(cm);
  return m->ExtractComp(std::min(qty, m->quantity()), c);
}

}  // namespace toolkit
}  // namespace cyclus

