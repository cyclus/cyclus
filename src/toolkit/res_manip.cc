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
  // squash always removes package id (results in default packaging)
  p->ChangePackage();
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
  m->ChangePackage();
  return m;
}

Resource::Ptr Squash(std::vector<Resource::Ptr> rs) {
  if (rs.size() == 0) {
    throw Error("cannot squash zero resources together");
  }

  std::vector<Material::Ptr> mats = ::cyclus::ResCast<Material>(rs);
  if (mats[0] != NULL) {
    return Squash(mats);
  }
  std::vector<Product::Ptr> prods = ::cyclus::ResCast<Product>(rs);
  if (prods[0] != NULL) {
    return Squash(prods);
  }

  throw Error("cannot squash resource type " + rs[0]->type());
}

std::vector<Resource::Ptr> ResCast(std::vector<Material::Ptr> rs) {
  std::vector<Resource::Ptr> casted;
  for (int i = 0; i < rs.size(); ++i) {
    casted.push_back(boost::dynamic_pointer_cast<Resource>(rs[i]));
  }
  return casted;
}

std::vector<Resource::Ptr> ResCast(std::vector<Product::Ptr> rs) {
  std::vector<Resource::Ptr> casted;
  for (int i = 0; i < rs.size(); ++i) {
    casted.push_back(boost::dynamic_pointer_cast<Resource>(rs[i]));
  }
  return casted;
}

std::vector<Resource::Ptr> ResCast(std::vector<Resource::Ptr> rs) {
  return rs;
}

}  // namespace toolkit
}  // namespace cyclus

