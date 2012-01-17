// Resource.cpp
// Implements the Resource Class
#include "GenericResource.h"
#include "Logger.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
GenericResource::GenericResource(std::string units,
            std::string quality, double quantity) : Resource() {
    units_ = units;
    quality_ = quality;
    quantity_ = quantity;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
GenericResource* GenericResource::clone() {
  LOG(LEV_DEBUG2) << "resource was cloned";
  return new GenericResource(*this);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GenericResource::print() {

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool GenericResource::checkQuality(Resource* other){
  bool toRet = false;

  toRet = (units_ == other->resourceUnits());
  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool GenericResource::checkQuantityEqual(Resource* other) {
  // KDHFLAG : Should allow epsilon as a parameter to this function?
  bool toRet;
  toRet =( checkQuality(other) && quantity_ == other->quantity());
  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool GenericResource::checkQuantityGT(Resource* other) {
  // KDHFLAG : Should allow epsilon as a parameter to this function?
  bool toRet;
  toRet = ( checkQuality(other) && quantity_ < other->quantity());
  return toRet;
}
