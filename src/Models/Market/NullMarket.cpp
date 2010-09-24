// NullMarket.cpp
// Implements the NullMarket class
#include <iostream>

#include "NullMarket.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void NullMarket::receiveMessage(Message *msg)
{
  messages.insert(msg);

  if (msg->getAmount() > 0){
    offers.insert(indexedMsg(msg->getAmount(),msg));
  }
  else{
    requests.insert(indexedMsg(-msg->getAmount(),msg));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void NullMarket::reject_request(sortedMsgList::iterator request)
{

  // delete the tentative orders
  while ( orders.size() > firmOrders)
  {
    delete orders.back();
    orders.pop_back();
  }

  // put all matched offers back in the sorted list
  while (matchedOffers.size() > 0)
  {
    Message *msg = *(matchedOffers.begin());
    offers.insert(indexedMsg(msg->getAmount(),msg));
    matchedOffers.erase(msg);
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void NullMarket::process_request()
{
  // update pointer to firm orders
  firmOrders = orders.size();

  while (matchedOffers.size() > 0)
  {
    Message *msg = *(matchedOffers.begin());
    messages.erase(msg);
    matchedOffers.erase(msg);
  }
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool NullMarket::match_request(sortedMsgList::iterator request)
{
  sortedMsgList::iterator offer;
  double requestAmt,offerAmt, toRet;
  Message *offerMsg, *requestMsg;

  requestAmt = (*request).first;
  requestMsg = (*request).second;
  
  // if this request is not yet satisfied &&
  // there are more offers left
  while ( requestAmt > 0 && offers.size() > 0)
  {
    // get a new offer
    offer = offers.end();
    offer--;
    offerAmt = (*offer).first;
    offerMsg = (*offer).second;

    // pop off this offer
    offers.erase(offer);
  
    if (requestAmt > offerAmt) { 
      // put a new message in the order stack
      // it goes down to supplier
      offerMsg->setRequesterID(requestMsg->getRequesterID());
      offerMsg->setDir(down);

      // Queue an order
      matchedOffers.insert(offerMsg);
      // Zero out the boolean. 
      // At least some of the request will be met.
      toRet = 0;

      orders.push_back(offerMsg);

      cout << "NullMarket has resolved a match from "
          << offerMsg->getSupplierID()
          << " to "
          << offerMsg->getRequesterID() 
          << " for the amount:  " 
          << offerMsg->getAmount() << endl;

      requestAmt -= offerAmt;
    } 
    else {
      // split offer

      // queue a new order
      Message* maybe_offer = new Message(*offerMsg);
      maybe_offer->setAmount(requestAmt);
      maybe_offer->setDir(down);
      maybe_offer->setRequesterID(requestMsg->getRequesterID());

      matchedOffers.insert(offerMsg);

      orders.push_back(maybe_offer);

      cout << "NullMarket has resolved a match from "
          << maybe_offer->getSupplierID()
          << " to "
          << maybe_offer->getRequesterID() 
          << " for the amount:  " 
          << maybe_offer->getAmount() << endl;

      // reduce the offer amount
      offerAmt -= requestAmt;

      // if the residual is above threshold,
      // make a new offer with reduced amount

      if(offerAmt > eps){
        Message *new_offer = new Message(*offerMsg);
        new_offer->setAmount(offerAmt);
        // call this method for consistency
        receiveMessage(new_offer);
      }
      
      // zero out request
      requestAmt = 0;

      // Zero out the boolean. 
      // All of the request will be met.
      toRet = 0;
    }
  }

  // if any of the request has been met, return true.
  return (0 == toRet);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void NullMarket::resolve()
{
  sortedMsgList::iterator request;

  firmOrders = 0;

  /// while requests remain and there is at least one offer left
  while (requests.size() > 0)
  {
    request = requests.end();
    request--;
    
    if(match_request(request)) {
      process_request();
    } 
    else {
      cout << "The request from Requester "<< (*request).second->getRequesterID() 
          << " for the amount " << (*request).first 
          << " rejected. "<<endl;
      reject_request(request);
    }
    // remove this request
    messages.erase((*request).second);
    requests.erase(request);
  }

  executeOrderQueue();
}

