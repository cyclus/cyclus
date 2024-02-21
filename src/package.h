#ifndef CYCLUS_SRC_PACKAGE_H_
#define CYCLUS_SRC_PACKAGE_H_

#include <limits>
#include <string>
#include <boost/shared_ptr.hpp>

namespace cyclus {

/// Packager is a class that packages materials into discrete items in ways that mimic realistic nuclear material handling. Packages will eventually be a required parameter of resources.
class Package {

  public:
    typedef boost::shared_ptr<Package> Ptr;

    // create a new package type with default values 
    static Ptr Create();

    // create a new package type
    static Ptr Create(std::string name, double fill_min, double fill_max, std::string strategy);

    int id();

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