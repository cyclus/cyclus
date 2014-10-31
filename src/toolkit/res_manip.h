#ifndef CYCLUS_SRC_TOOLKIT_RES_MANIP_H_
#define CYCLUS_SRC_TOOLKIT_RES_MANIP_H_

#include "material.h"
#include "resource.h"
#include "product.h"

namespace cyclus {
namespace toolkit {

/// Abstract class for mapping nuclides to arbitrary values.  
class NucVals {
  public:
    virtual double Val(Nuc n) = 0;
};

/// A tool for building up collections of nuclides to be used for things like
/// creating compositions and separations.
class NucMap : public NucVals {
  public:
    NucMap();

    /// Create a NucMap with an initial mapping of nuclides to values.
    NucMap(CompMap m);

    /// Adds the named nuclide to the map, overwriting any previous value it
    /// may have had.  Use fractions from 0 to 1 for separations.
    NucMap& Add(std::string nuc, double val);

    /// Adds the named nuclide to the map, overwriting any previous value it
    /// may have had.  Use fractions from 0 to 1 for separations.
    NucMap& Add(Nuc n, double val);

    /// Generates a composition from the accumulated nuclides treating the
    /// values as atom-based relative quantities.
    Composition::Ptr Atom();

    /// Generates a composition from the accumulated nuclides treating the
    /// values as mass-based relative quantities.
    Composition::Ptr Mass();

    /// Returns the underlying nuclide-value map.
    CompMap Map();

    /// Implements NucVals interface for use in things like the Separate
    /// function.
    virtual double Val(Nuc n);

  private:
    CompMap nucs_;
};

/// A tool for building up collections of elements to be used for things like
/// separations.
class EltMap : public NucVals {
  public:
    EltMap();

    /// Create an EltMap with an initial mapping of element Z number to
    /// values.
    EltMap(std::map<int, double> elts);

    /// Adds the named element to the map, overwriting any previous value it
    /// may have had.
    EltMap& Add(std::string elt, double val);

    /// Adds the named element to the map, overwriting any previous value it
    /// may have had.  elt_z must be the Z number for an element.
    EltMap& Add(int elt_z, double val);

    /// Implements NucVals interface for use in things like the Separate
    /// function.  The nuclide's Z number is used to look up the value to
    /// return.  If "U" was added with a value of 0.5, for every nuclide of
    /// uranium, this function would return 0.5.
    virtual double Val(Nuc n);

    /// Returns the underlying element-value map.
    std::map<int, double> Map();

  private:
    std::map<int, double> elts_;
};

/// Squash combines all products in ps and returns the resulting single
/// product.
Product::Ptr Squash(std::vector<Product::Ptr> ps);

/// Squash combines all materials in ms and returns the resulting single
/// material.
Material::Ptr Squash(std::vector<Material::Ptr> ms);

/// Squash combines all resources in rs and returns the resulting single
/// resource.
Resource::Ptr Squash(std::vector<Resource::Ptr> rs);

/// Separate extracts from m according to the fractional nuclide efficiencies
/// in fracs.  fracs must provide values for nuclides as fractional
/// efficiencies between 0 and 1.  This function performs the actual
/// extraction - mutating m.  For Example:
///
/// @code
///
/// Material::Ptr m = ...;
/// EltMap effs;
/// effs.Add("U", 1.0).Add("Pu", 0.5);
/// Material::Ptr stream = Separate(m, effs);
///
/// // stream now contains U235 and Pu239.
/// // m now contains no U235 and only half the Pu239 it started with.
///
/// @endcode
Material::Ptr Separate(Material::Ptr m, NucVals& fracs);
  
}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_RES_MANIP_H_
