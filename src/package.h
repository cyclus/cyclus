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

    // create a new package type. Should be called by the context only
    // (see Context::AddPackage), unless you want an untracked package
    //  type (which you probably don't)
    static Ptr Create(std::string name, double fill_min = 0,
                      double fill_max = std::numeric_limits<double>::max(),
                      std::string strategy = "first");

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
    double GetFillMass(double qty);

    // returns package name
    std::string name() const { return name_; }
    // returns package fill min
    double fill_min() const { return fill_min_; }
    // returns package fill max
    double fill_max() const { return fill_max_; }
    // returns package strategy
    std::string strategy() const { return strategy_; }

    // returns the unpackaged package name
    static std::string unpackaged_name() { return unpackaged_name_; }

    // returns the unpackaged singleton object
    static Ptr& unpackaged();

  private:
    Package(std::string name, 
            double fill_min = 0, 
            double fill_max = std::numeric_limits<double>::max(), 
            std::string strategy = "first");

    static constexpr char unpackaged_name_[11] = "unpackaged";
    static Ptr unpackaged_;

    std::string name_;
    double fill_min_;
    double fill_max_;
    std::string strategy_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_PACKAGE_H_