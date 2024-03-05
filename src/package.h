#ifndef CYCLUS_SRC_PACKAGE_H_
#define CYCLUS_SRC_PACKAGE_H_

#include <limits>
#include <string>
#include <vector>
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

    /// Given a single resource and a package type, returns optimal fill mass
    /// for the resource to be packaged. Can be used to determine how to   
    /// respond to requests for material, and to actually package and send off
    /// trades
    double GetFillMass(Resource::Ptr r, Package::Ptr pkg);

    /// Repackages a single resource into a package. If some quantity of the 
    /// resource cannot be packaged, the remainder is left in the resource
    /// object. 
    std::vector<typename T::Ptr> Repackage(typename T::Ptr r, Package::Ptr pkg);

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

}  // namespace cyclus

#endif  // CYCLUS_SRC_PACKAGE_H_