#ifndef CYCLUS_SRC_PACKAGE_H_
#define CYCLUS_SRC_PACKAGE_H_

#include <limits>
#include <string>
#include <vector>
#include <cmath>
#include <boost/shared_ptr.hpp>
#include "resource.h"

namespace cyclus {

/// Packager is a class that packages materials into discrete items in ways that mimic realistic nuclear material handling. Packages will eventually be a required parameter of resources.
class Package {

  public:
    typedef boost::shared_ptr<Package> Ptr;

    // create a new package type with default values 
    static Ptr Create();

    // create a new package type
    static Ptr Create(std::string name, double fill_min, double fill_max, std::string strategy);

    // returns package id
    int id() const { return id_; }
    // returns package name
    std::string name() const { return name_; }
    // returns package fill min
    double fill_min() const { return fill_min_; }
    // returns package fill max
    double fill_max() const { return fill_max_; }
    // returns package strategy
    std::string strategy() const { return strategy_; }

  protected:
    Package();
    Package(std::string name, double fill_min, double fill_max, std::string strategy);

  private:
    std::string name_;
    static int next_id_;
    int id_;
    double fill_min_;
    double fill_max_;
    std::string strategy_;
};

/// Returns optimal fill mass for a resource to be packaged. Can be used
/// to determine how to respond to requests for material, and to actually
/// package and send off trades.
/// Packaging strategy "first" simply fills the packages one by one to the
/// maximum fill. Therefore, it should always try to max fill.
/// Packaging strategy "equal" tries to fill all packages to the same mass.
/// This tries to find the optimal number and fill mass of packages given the
/// packaging limitations. It does this by calculating bounding fills, 
/// floor(quantity/fill_min) and ceiling(quantity/fill_max). 
/// There might be a scenario w
double GetFillMass(Resource::Ptr r, Package::Ptr pkg);

/// Repackages a single resource into a package. If some quantity of the 
/// resource cannot be packaged using the given packaging strategy and
/// restrictions, the remainder is left in the resource object. 
template <class T>
std::vector<typename T::Ptr> Repackage(typename T::Ptr r, Package::Ptr pkg) {
  std::vector<typename T::Ptr> rs_pkgd;
  typename T::Ptr r_pkgd;
  
  double fill_mass = GetFillMass(r, pkg);
  if (fill_mass ==0) {
    return rs_pkgd;
  }

  while (r->quantity() > pkg->fill_min()) {
    double pkg_fill = std::min(r->quantity(), fill_mass);
    r_pkgd = boost::dynamic_pointer_cast<T>(r->ExtractRes(pkg_fill));
    r_pkgd->ChangePackageId(pkg->id());
    rs_pkgd.push_back(r_pkgd);
  }
  return rs_pkgd;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_PACKAGE_H_