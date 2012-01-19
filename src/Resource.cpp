
// Resource.cpp
#include "Resource.h"

int Resource::nextID_ = 0;

Resource::Resource() {
  ID_ = nextID_++;
}

bool Resource::checkEquality(Resource* other) {
  bool toRet;
  (this->checkQuality(other) && this->checkQuantityEqual(other)) ? toRet = true : toRet = false;
  return toRet; 
}

