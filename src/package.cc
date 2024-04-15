#include "package.h"
#include "error.h"

namespace cyclus {

// unpackaged id is 1, so start the user-declared packaging id at 2
int Package::next_package_id_ = 2;

Package::Ptr Package::Create(std::string name, double fill_min, double fill_max, std::string strategy) {
  if (fill_min < 0 || fill_max < 0) {
    throw ValueError("fill_min and fill_max must be non-negative");
  }
  else if (fill_min > fill_max) {
    throw ValueError("fill_min must be less than or equal to fill_max");
  }
  Ptr p(new Package(name, fill_min, fill_max, strategy));
  return p;
}

Package::Ptr Package::CreateUnpackaged() {
  Ptr p(new Package(unpackaged_id_, unpackaged_name_));
  return p;
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
      // all material can fit in a package
      double fill_mass = qty / num_max_fill;
    } else {
      // some material will remain unpackaged, fill up as many max packages as possible
      fill_mass = fill_max_;
    }
  }
  return fill_mass;
}
  
Package::Package() : id_(next_package_id_++), fill_min_(0), fill_max_(std::numeric_limits<double>::max()) {
  name_ = "unnamed";
}

Package::Package(std::string name, double fill_min, double fill_max, std::string strategy) : name_(name), id_(next_package_id_++), fill_min_(fill_min), fill_max_(fill_max), strategy_(strategy) {}

Package::Package(int id, std::string name) : id_(id), name_(name), fill_min_(0), fill_max_(std::numeric_limits<double>::max()) {}

} // namespace cyclus
