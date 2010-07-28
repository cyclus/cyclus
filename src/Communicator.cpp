// Communicator.cpp
// Implements the Communicator class.
//
#include "FacilityModel.h"
#include "Communicator.h"
#include "GenException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator::~Communicator()
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::sendMessage()
{
  throw GenException
    ("Sender did not override sendMessage()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::receiveMessage(Message* msg)
{
  throw GenException
    ("Recipient did not override receiveMessage()");
}
