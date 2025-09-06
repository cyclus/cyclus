#ifndef CYCLUS_SRC_PACKAGE_H_
#define CYCLUS_SRC_PACKAGE_H_

#include <limits>
#include <string>
#include <vector>
#include <cmath>
#include <boost/shared_ptr.hpp>

#include "error.h"
#include "logger.h"
#include "random_number_generator.h"

namespace cyclus {

/// Package is a class that packages materials into discrete items in ways
// that mimic realistic nuclear material handling. Package is a parameter
// of materials and products, with default unpackaged
class Package {
 public:
  typedef boost::shared_ptr<Package> Ptr;

  // create a new package type. Should be called by the context only
  // (see Context::AddPackage), unless you want an untracked package
  //  type (which you probably don't)
  static Ptr Create(std::string name, double fill_min = 0,
                    double fill_max = std::numeric_limits<double>::max(),
                    std::string strategy = "first");

  /// Returns optimal fill mass for a resource to be packaged, and number of
  /// packages that can be crated at that fill mass (note that up to one
  /// additional package may be possible to create with a lower fill mass,
  /// which much be checked separately).
  /// Can be used to determine how to respond to requests for material, and
  /// to actually package and send off trades.
  /// Packaging strategy "first" simply fills the packages one by one to the
  /// maximum fill. Therefore, it should always try to max fill.
  /// Packaging strategy "equal" tries to fill all packages to the same mass.
  /// This tries to find the optimal number and fill mass of packages given
  /// the packaging limitations. It does this by calculating bounding fills,
  /// floor(quantity/fill_min) and ceiling(quantity/fill_max).
  /// Packaging strategy "uniform" fills packages with a random mass between
  /// fill_min and fill_max, if at least fill_max is available. If less than
  /// fill_max is available, a partial package is filled with the total mass.
  /// Packaging strategy "normal" fills packages with a random mass between
  /// fill_min and fill_max, with a normal distribution. Mean is the middle
  /// of fill_min and fill_max, standard deviation is 1/6 of the range such
  /// that 3 sigma is the range. If less than fill_max is available, a
  /// partial package is filled with the total mass (no dist sampling).
  /// There might be a scenario where there is no solution, i.e. an integer
  /// number of packages cannot be filled with no remainder. In this case,
  /// the most effective fill strategy is to fill to the max. Numeric example:
  /// quantity = 5, fill_min = 3, fill_max = 4. num_min_fill = floor(5/3) = 1,
  /// num_max_fill = ceil(5/4) = 2. num_min_fill < num_max_fill, so fill to
  /// the max.
  std::vector<double> GetFillMass(double qty);

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

  static void ExceedsSplitLimits(int pkgs, std::string ss_extra = "") {
    if (pkgs > SplitLimit() || pkgs < (0.9 * std::numeric_limits<int>::min())) {
      throw ValueError("Resource::Package() cannot package into more than " +
                       std::to_string(SplitLimit()) + " items at once." +
                       ss_extra);
    } else if (pkgs > SplitWarn()) {
      // if pkgs is int min, overflow has occurred
      CLOG(cyclus::LEV_INFO1)
          << "Resource::Package() is attempting to " << "package into " << pkgs
          << " items at once, is this intended? " << ss_extra;
    }
    return;
  }

  // When a resource is split into individual items, warn when more than
  // one million items are trying to be created at once
  static int SplitWarn() { return 100000; }

  // Numeric limits for splitting resources is based on vector limits and
  // memory constraints. Use unsigned int max / 100 to be safe
  static int SplitLimit() { return (std::numeric_limits<int>::max() / 10); }

  bool IsValidStrategy(std::string strategy) {
    return strategy == "first" || strategy == "equal" ||
           strategy == "uniform" || strategy == "normal";
  }

  void SetDistribution();

 private:
  Package(std::string name, double fill_min = 0,
          double fill_max = std::numeric_limits<double>::max(),
          std::string strategy = "first");

  static constexpr char unpackaged_name_[11] = "unpackaged";
  static Ptr unpackaged_;

  std::string name_;
  double fill_min_;
  double fill_max_;
  std::string strategy_;
  DoubleDistribution::Ptr dist_ = NULL;
};

/// TransportUnit is a class that can be used in conjunction with packages to
/// restrict the amount of material that can be traded between facilities.
/// Unlike Packages, TransportUnits are not a property of resources. They are
/// simply applied at the response to request for bids phase and then the trade
/// execution to determine whether the available number of packages is
/// allowable given the TransportUnit parameters. Default is unrestricted
/// Strategy "first" simply fill transport units one by one to max fill
/// Strategy "equal" tries to fill all transport units with the
/// Strategy "hybrid" is iterative, recursively filling transport units
/// with the max fill until the remaining quantity can be filled with the
/// at least the min fill. This is the most efficient strategy
class TransportUnit {
 public:
  typedef boost::shared_ptr<TransportUnit> Ptr;

  /// create a new transport unit type. Should be called by the context only
  /// (see Context::AddTransportUnit), unless you want an untracked transport
  /// unit type (which you probably don't)
  static Ptr Create(std::string name, int fill_min = 0,
                    int fill_max = std::numeric_limits<int>::max(),
                    std::string strategy = "first");

  /// Returns number of packages for each transport unit.
  /// same number of packages
  int GetTransportUnitFill(int qty);

  /// Returns the max number of transport units that can be shipped from the
  /// available quantity
  int MaxShippablePackages(int pkgs);

  // returns transport unit name
  std::string name() const { return name_; }
  // returns transport unit fill min
  int fill_min() const { return fill_min_; }
  // returns transport unit fill max
  int fill_max() const { return fill_max_; }
  // returns transport unit strategy
  std::string strategy() const { return strategy_; }

  // returns the unrestricted id (1)
  static int unrestricted_id() { return unrestricted_id_; }

  // returns the unrestricted transport unit name
  static std::string unrestricted_name() { return unrestricted_name_; }

  // returns the unrestricted singleton object
  static Ptr& unrestricted();

 private:
  TransportUnit(std::string name, int fill_min = 0,
                int fill_max = std::numeric_limits<int>::max(),
                std::string strategy = "hybrid");

  static const int unrestricted_id_ = 1;
  static constexpr char unrestricted_name_[13] = "unrestricted";
  static Ptr unrestricted_;
  static int next_tranport_unit_id_;

  std::string name_;
  int id_;
  int fill_min_;
  int fill_max_;
  std::string strategy_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_PACKAGE_H_