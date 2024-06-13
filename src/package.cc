#include "package.h"
#include "error.h"

namespace cyclus {

Package::Ptr Package::unpackaged_ = NULL;

Package::Ptr Package::Create(std::string name, double fill_min,
                             double fill_max,std::string strategy) {
  if (fill_min < 0 || fill_max < 0) {
    throw ValueError("fill_min and fill_max must be non-negative");
  }
  else if (fill_min > fill_max) {
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

double Package::GetFillMass(double qty) {
  if (qty < fill_min_) {
    // less than one pkg of material available
    return 0;
  }

  double fill_mass;
  if (strategy_ == "first") {
    fill_mass = fill_max_;
  } else if (strategy_ == "equal") {
    int num_min_fill = std::floor(qty / fill_min_);
    int num_max_fill = std::ceil(qty / fill_max_);
    if (num_min_fill >= num_max_fill) {
      // all material can fit in package(s)
      fill_mass = qty / num_max_fill;
    } else {
      // some material will remain unpackaged, fill up as many max packages as possible
      fill_mass = fill_max_;
    }
  }
  return std::min(qty, fill_mass);
}
  
Package::Package(std::string name, double fill_min, double fill_max,
                 std::string strategy) : 
  name_(name), fill_min_(fill_min), fill_max_(fill_max), strategy_(strategy) {
    if (name == unpackaged_name_) {
      if (unpackaged_) {
        throw ValueError("can't create a new package with name 'unpackaged'");
      }
  }
}

TransportUnit::Ptr TransportUnit::unrestricted_ = NULL;

TransportUnit::Ptr TransportUnit::Create(std::string name, int fill_min, int fill_max, std::string strategy) {
  if (fill_min < 0 || fill_max < 0) {
    throw ValueError("fill_min and fill_max must be non-negative");
  }
  else if (fill_min > fill_max) {
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
  } else if (strategy_ == "equal") {
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

  if (pkgs == 0 && pkgs < fill_min_) {
    return 0;
  }

  while ((pkgs > 0) && (pkgs >= fill_min_)) {
    TU_fill = GetTransportUnitFill(pkgs);
    shippable += TU_fill;
    pkgs -= TU_fill;
  }
  return shippable;
}
  
TransportUnit::TransportUnit(std::string name, int fill_min, int fill_max, std::string strategy) : 
  name_(name), fill_min_(fill_min), fill_max_(fill_max), strategy_(strategy) {
    if (name == unrestricted_name_) {
      if (unrestricted_) {
        throw ValueError("can't create a new transport unit with name 'unrestricted'");
      }
  }
}

} // namespace cyclus
