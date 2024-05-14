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

// unrestricted id is 1, so start the user-declared transport id at 2
int TransportUnit::next_package_id_ = 2;
TransportUnit::Ptr TransportUnit::unrestricted_ = NULL;

TransportUnit::Ptr TransportUnit::Create(std::string name, int fill_min, int fill_max, std::string strategy) {
  if (fill_min < 0 || fill_max < 0) {
    throw ValueError("fill_min and fill_max must be non-negative");
  }
  else if (fill_min > fill_max) {
    throw ValueError("fill_min must be less than or equal to fill_max");
  }
  Ptr p(new TransportUnit(name, fill_min, fill_max, strategy));
  return p;
}

// singleton pattern: 
// if the static member is not yet set, create a new object
// otherwise return the object that already exists
TransportUnit::Ptr& TransportUnit::unrestricted() {

  if (!unrestricted) {
    unrestricted_ = Ptr(new TransportUnit(unrestricted_name_));
  }
  return unrestricted_;
}

int TransportUnit::GetFillMass(double qty) {
  if (qty < fill_min_) {
    // less than one pkg of material available
    return 0;
  }

  int fill_mass;
  if (strategy_ == "first") {
    fill_mass = fill_max_;
  } else if (strategy_ == "equal") {
    int num_min_fill = std::floor(qty / fill_min_);
    int num_max_fill = std::ceil(qty / fill_max_);
    if (num_min_fill >= num_max_fill) {
      // all material can fit in a package
      int fill_mass = qty / num_max_fill;
    } else {
      // some material will remain unrestricted, fill up as many max packages as possible
      fill_mass = fill_max_;
    }
  }
  return fill_mass;
}
  
TransportUnit::TransportUnit(std::string name, int fill_min, int fill_max, std::string strategy) : 
  name_(name), fill_min_(fill_min), fill_max_(fill_max), strategy_(strategy) {
    if (name == unrestricted_name_) {
      if (unrestricted_) {
        throw ValueError("can't create a new transport unit with name 'unrestricted'");
      }
      id_ = unrestricted_id_;
    } else {
      id_ = next_transport_unit_id_++;
    }
  }

} // namespace cyclus
