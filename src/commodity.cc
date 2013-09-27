#include "commodity.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::Commodity() : name_("") {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::Commodity(std::string name) : name_(name) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Commodity::name() const {
  return name_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Commodity::operator==(const Commodity& other) const {
  return (name_ == other.name());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Commodity::operator!=(const Commodity& other) const {
  return !(*this == other);
}
} // namespace cyclus
