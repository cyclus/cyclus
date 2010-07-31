// NullRegion.cpp
// Implements the NullRegion class
#include <iostream>

#include "NullRegion.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void NullRegion::receiveMessage(Message* msg){
 // The NullRegion isn't insterested in fooling with messages.
 // Just pass them along. 
 // If it's going up, send it to the market.
 // If it's going down, send it to the inst.
 MessageDir dir = msg->getDir();
 if (dir == up){
   Communicator* nextRecipient = msg->getMkt();
   nextRecipient->receiveMessage(msg);
 }
 else if (dir == down){
   Communicator* nextRecipient = msg->getInst();
   nextRecipient->receiveMessage(msg);
 }

}

