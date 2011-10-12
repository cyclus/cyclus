// Communicator.cpp
// Implements the Communicator class.
//
#include "Communicator.h"

#include "GenException.h"
#include <string>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator::~Communicator() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::receiveMessage(Message* msg) {
  string warn_msg = "Recipient did not override receiveMessage(msg)";
  throw GenException(warn_msg);
}
