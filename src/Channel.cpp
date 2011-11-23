// Channel.cpp

#include "Channel.h"

Channel::Channel() {

};

void Channel::approve() {
  approval_count_++;

  if (approval_count_ > 2) {
    transactResources();
  }
};

void Channel::transactResources() {
  Resource resource = msg_->resource();
  Model* supplier = msg_->getSupplier();
  Model* requester = msg_->getRequester();
  
  resource = supplier->removeResource(resource);

  try {
    requester->receiveResource(resource)
  } catch (CycException err) {
    supplier->replaceResource(resource);
  }
};

