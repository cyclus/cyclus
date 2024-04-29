#ifndef CYCLUS_SRC_PACKAGE_H_
#define CYCLUS_SRC_PACKAGE_H_

#include <limits>
#include <string>
#include <vector>
#include <cmath>
#include <boost/shared_ptr.hpp>

namespace cyclus {

/// Packager is a class that packages materials into discrete items in ways that mimic realistic nuclear material handling. Packages will eventually be a required parameter of resources.
class Package {

  public:
    typedef boost::shared_ptr<Package> Ptr;

    /// Returns optimal fill mass for a resource to be packaged. Can be used
    /// to determine how to respond to requests for material, and to actually
    /// package and send off trades.
    /// Packaging strategy "first" simply fills the packages one by one to the
    /// maximum fill. Therefore, it should always try to max fill.
    /// Packaging strategy "equal" tries to fill all packages to the same mass.
    /// This tries to find the optimal number and fill mass of packages given
    /// the packaging limitations. It does this by calculating bounding fills, 
    /// floor(quantity/fill_min) and ceiling(quantity/fill_max). 
    /// There might be a scenario where there is no solution, i.e. an integer
    /// number of packages cannot be filled with no remainder. In this case,
    /// the most effective fill strategy is to fill to the max. Numeric example:
    /// quantity = 5, fill_min = 3, fill_max = 4. num_min_fill = floor(5/3) = 1,
    /// num_max_fill = ceil(5/4) = 2. num_min_fill < num_max_fill, so fill to
    /// the max.
    template <class T>
    double GetFillMass(typename T::Ptr r);

    /// Repackages a single resource into a package. If some quantity of the 
    /// resource cannot be packaged using the given packaging strategy and
    /// restrictions, the remainder is left in the resource object. 
    template <class T>
    std::vector<typename T::Ptr> PackageResource(typename T::Ptr r);

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

    // returns the unpackaged id (1)
    static int unpackaged_id() { return unpackaged_id_; }

  protected:
    Package();
    Package(std::string name, double fill_min, double fill_max, std::string strategy);

  private:
    static const int unpackaged_id_ = 1;
    static int next_id_;

    std::string name_;
    int id_;
    double fill_min_;
    double fill_max_;
    std::string strategy_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_PACKAGE_H_