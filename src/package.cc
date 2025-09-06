#include "package.h"
#include "error.h"
#include "cyc_limits.h"

namespace cyclus {

Package::Ptr Package::unpackaged_ = NULL;

Package::Ptr Package::Create(std::string name, double fill_min, double fill_max,
                             std::string strategy) {
  if (fill_min < 0 || fill_max < 0) {
    throw ValueError("fill_min and fill_max must be non-negative");
  } else if (fill_min > fill_max) {
    throw ValueError("fill_min must be less than or equal to fill_max");
  }
  Ptr p(new Package(name, fill_min, fill_max, strategy));
  return p;
}

// singleton pattern:
// if the static member is not yet set, create a new object
// otherwise return the object that already exists
Package::Ptr& Package::unpackaged() {
  if (!unpackaged_) {
    unpackaged_ = Ptr(new Package(unpackaged_name_));
  }

  return unpackaged_;
}

void Package::SetDistribution() {
  if (strategy_ == "uniform") {
    dist_ = UniformDoubleDist::Ptr(new UniformDoubleDist(fill_min_, fill_max_));
  } else if (strategy_ == "normal") {
    dist_ = NormalDoubleDist::Ptr(new NormalDoubleDist(
        (fill_min_ + fill_max_) / 2, (fill_max_ - fill_min_) / 6, fill_min_,
        fill_max_));
  }
}

std::vector<double> Package::GetFillMass(double qty) {
  std::vector<double> packages;
  if ((qty - fill_min_) < -eps_rsrc()) {
    // less than one pkg of material available
    return packages;
  }

  // simple check for whether vector limits *might* be exceeded
  ExceedsSplitLimits(qty / fill_max_);

  double fill_mass;
  int num_at_fill_mass;

  if (strategy_ == "first") {
    fill_mass = fill_max_;
  } else if (strategy_ == "equal") {
    int num_min_fill = std::floor(qty / fill_min_);
    int num_max_fill = std::ceil(qty / fill_max_);
    if (num_min_fill >= num_max_fill) {
      // all material can fit in package(s)
      fill_mass = qty / num_max_fill;
    } else {
      // some material will remain unpackaged, fill up as many max packages as
      // possible
      fill_mass = fill_max_;
    }
  }

  if (strategy_ == "first" || strategy_ == "equal") {
    fill_mass = std::min(qty, fill_mass);
    num_at_fill_mass = static_cast<int>(std::floor(qty / fill_mass));
    ExceedsSplitLimits(num_at_fill_mass);
    packages.assign(num_at_fill_mass, fill_mass);

    qty -= num_at_fill_mass * fill_mass;
  }

  if (strategy_ == "uniform" || strategy_ == "normal") {
    // only use random if a full package amount is available. if less than one
    // full amount is available, below will fill a partial package (no random).
    while (qty >= std::max(eps_rsrc(), fill_max_)) {
      fill_mass = dist_->sample();
      packages.push_back(fill_mass);
      qty -= fill_mass;
    }
  }

  if (qty >= std::max(eps_rsrc(), fill_min_)) {
    // leftover material is enough to fill one more partial package.
    packages.push_back(qty);
  }

  Package::ExceedsSplitLimits(packages.size());

  return packages;
}

Package::Package(std::string name, double fill_min, double fill_max,
                 std::string strategy)
    : name_(name),
      fill_min_(fill_min),
      fill_max_(fill_max),
      strategy_(strategy) {
  if (name == unpackaged_name_) {
    if (unpackaged_) {
      throw ValueError("can't create a new package with name 'unpackaged'");
    }
  }
  if (!IsValidStrategy(strategy_)) {
    throw ValueError("Invalid strategy for package: " + strategy_);
  }
  SetDistribution();
}

TransportUnit::Ptr TransportUnit::unrestricted_ = NULL;

TransportUnit::Ptr TransportUnit::Create(std::string name, int fill_min,
                                         int fill_max, std::string strategy) {
  if (fill_min < 0 || fill_max < 0) {
    throw ValueError("fill_min and fill_max must be non-negative");
  } else if (fill_min > fill_max) {
    throw ValueError("fill_min must be less than or equal to fill_max");
  }
  Ptr t(new TransportUnit(name, fill_min, fill_max, strategy));
  return t;
}

// singleton pattern:
// if the static member is not yet set, create a new object
// otherwise return the object that already exists
TransportUnit::Ptr& TransportUnit::unrestricted() {
  if (!unrestricted_) {
    unrestricted_ = Ptr(new TransportUnit(unrestricted_name_));
  }
  return unrestricted_;
}

int TransportUnit::GetTransportUnitFill(int qty) {
  if (qty < fill_min_) {
    // less than one TransportUnit of material available
    return 0;
  }

  if (strategy_ == "first") {
    return fill_max_;
  } else if (strategy_ == "equal" || strategy_ == "hybrid") {
    // int division automatically rounds down. don't need floor in min, and
    // get ceil by hand instead
    int num_at_min_fill = qty / fill_min_;
    int num_at_max_fill = (qty + fill_max_ - 1) / fill_max_;

    if (num_at_min_fill >= num_at_max_fill) {
      // all material *might* fit transport units. However, this is more
      // challenging than packages because transport units are discrete.
      double dbl_fill_mass = (double)qty / (double)num_at_max_fill;
      return std::floor(dbl_fill_mass);
    }
    // some material will remain unrestricted, fill up as many transport
    // units as possible. Or, perfect fill is possible but not with integer
    // fill (see above).
  }
  return fill_max_;
}

int TransportUnit::MaxShippablePackages(int pkgs) {
  int TU_fill;
  int shippable = 0;

  if (pkgs == 0 || pkgs < fill_min_) {
    return 0;

  } else if (name_ == unrestricted_name_) {
    return pkgs;

  } else if (strategy_ == "first" || strategy_ == "equal") {
    TU_fill = GetTransportUnitFill(pkgs);
    if (TU_fill == 0) {
      return 0;
    }
    shippable = std::min(pkgs, (pkgs / TU_fill) * TU_fill);
    return shippable;

  } else if (strategy_ == "hybrid") {
    while ((pkgs > 0) && (pkgs >= fill_min_)) {
      TU_fill = GetTransportUnitFill(pkgs);
      shippable += TU_fill;
      pkgs -= TU_fill;
    }
    return shippable;
  }
  return 0;
}

TransportUnit::TransportUnit(std::string name, int fill_min, int fill_max,
                             std::string strategy)
    : name_(name),
      fill_min_(fill_min),
      fill_max_(fill_max),
      strategy_(strategy) {
  if (name == unrestricted_name_) {
    if (unrestricted_) {
      throw ValueError(
          "can't create a new transport unit with name 'unrestricted'");
    }
  }
  if (strategy != "first" && strategy != "equal" && strategy != "hybrid") {
    throw ValueError("Invalid strategy for transport unit: " + strategy_);
  }
}

}  // namespace cyclus
