#ifndef CYCLUS_SRC_TOOLKIT_RES_MANIP_H_
#define CYCLUS_SRC_TOOLKIT_RES_MANIP_H_

#include "material.h"
#include "resource.h"
#include "product.h"

namespace cyclus {
namespace toolkit {

/// SquashProd combines all products in ps and returns the resulting single
/// product.
Product::Ptr SquashProd(std::vector<Product::Ptr> ps);

/// SquashProd combines all materials in ms and returns the resulting single
/// material.
Material::Ptr SquashMat(std::vector<Material::Ptr> ms);

/// SquashProd combines all resources in rs and returns the resulting single
/// resource.
Resource::Ptr Squash(std::vector<Resource::Ptr> rs);

/// An alias for nuclide separations efficienes.
typedef std::map<Nuc, double> NucEffs;

/// Separate extracts from m according to the fractional nuclide efficiencies
/// in fracs.  fracs maps nuclide id's to efficiencies between 0 and 1.  This
/// function performs the actual extraction mutating m.  For Example:
///
/// @code
///
/// Material::Ptr m = ...;
/// NucEffs effs;
/// effs[922350000] = 1;
/// effs[942390000] = 0.5;
/// Material::Ptr stream = Separate(m, effs);
///
/// // stream now contains U235 and Pu239.
/// // m now contains no U235 and only half the Pu239 it started with
///
/// @endcode
Material::Ptr Separate(Material::Ptr m, NucEffs fracs);
  
}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_RES_MANIP_H_
