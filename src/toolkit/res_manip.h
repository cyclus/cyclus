#ifndef CYCLUS_SRC_TOOLKIT_RES_MANIP_H_
#define CYCLUS_SRC_TOOLKIT_RES_MANIP_H_

#include "material.h"
#include "resource.h"
#include "product.h"

namespace cyclus {
namespace toolkit {

Product::Ptr SquashProd(std::vector<Product::Ptr> rs);

Material::Ptr SquashMat(std::vector<Material::Ptr> rs);

Resource::Ptr Squash(std::vector<Resource::Ptr> rs);
  
}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_RES_MANIP_H_
