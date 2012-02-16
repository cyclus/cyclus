
// Resource.cpp
#include "Resource.h"

int Resource::nextID_ = 0;

Resource::Resource() {
  ID_ = nextID_++;
  MLOG(LEV_DEBUG4) << "Resource ID=" << ID_ << ", ptr=" << this << " created.";
}

Resource::~Resource() {
  MLOG(LEV_DEBUG4) << "Resource ID=" << ID_ << ", ptr=" << this << " deleted.";
}

bool Resource::checkEquality(rsrc_ptr other) {
  bool toRet;
  (this->checkQuality(other) && this->checkQuantityEqual(other)) ? toRet = true : toRet = false;
  return toRet; 
}

