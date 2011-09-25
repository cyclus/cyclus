// GreedyMarket.cpp
// Implements the GreedyMarket class
#include <iostream>

#include "GreedyMarket.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GreedyMarket::receiveMessage(Message *msg)
{
  messages_.insert(msg);

  if (msg->getAmount() > 0){
    offers_.insert(indexedMsg(msg->getAmount(),msg));
  }
  else{
    requests_.insert(indexedMsg(-msg->getAmount(),msg));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GreedyMarket::reject_request(sortedMsgList::iterator request)
{
  // send a failure message to the facility
  //  Transaction trans;
  //  trans.amount = 0;
  //  orders_.push_back(new Message(down, trans, this, 
  //                               (*request).second->getRequester()));

  // delete the tentative orders
  while ( orders_.size() > firmOrders_)
  {
    delete orders_.back();
    orders_.pop_back();
  }

  // put all matched offers_ back in the sorted list
  while (matchedOffers_.size() > 0)
  {
    Message *msg = *(matchedOffers_.begin());
    offers_.insert(indexedMsg(msg->getAmount(),msg));
    matchedOffers_.erase(msg);
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GreedyMarket::process_request()
{
  // update pointer to firm orders
  firmOrders_ = orders_.size();

  while (matchedOffers_.size() > 0)
  {
    Message *msg = *(matchedOffers_.begin());
    messages_.erase(msg);
    matchedOffers_.erase(msg);
  }
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool GreedyMarket::match_request(sortedMsgList::iterator request)
{
  sortedMsgList::iterator offer;
  double requestAmt,offerAmt;
  Message *offerMsg, *requestMsg;

  requestAmt = (*request).first;
  requestMsg = (*request).second;
  
  // if this request is not yet satisfied &&
  // there are more offers_ left
  while ( requestAmt > 0 && offers_.size() > 0)
  {
    // get a new offer
    offer = offers_.end();
    offer--;
    offerAmt = (*offer).first;
    offerMsg = (*offer).second;

    // pop off this offer
    offers_.erase(offer);
  
    if (requestAmt > offerAmt) { 
      // put a new message in the order stack
      // it goes down to supplier
      offerMsg->setRequesterID(requestMsg->getRequesterID());
      offerMsg->setDir(DOWN_MSG);

      // tenatively queue a new order (don't execute yet)
      matchedOffers_.insert(offerMsg);

      orders_.push_back(offerMsg);

      cout << "GreedyMarket has resolved a match from "
          << offerMsg->getSupplierID()
          << " to "
          << offerMsg->getRequesterID() 
          << " for the amount:  " 
          << offerMsg->getAmount() << endl;

      requestAmt -= offerAmt;
    } 
    else {
      // split offer
      if (NULL == offerMsg)
        throw GenException("offer message does not exist in market '" 
            + getName() + "'.");
      
      // queue a new order
      Message* maybe_offer = new Message(*offerMsg);
      maybe_offer->setAmount(requestAmt);
      maybe_offer->setDir(DOWN_MSG);
      maybe_offer->setRequesterID(requestMsg->getRequesterID());

      matchedOffers_.insert(offerMsg);

      orders_.push_back(maybe_offer);

      cout << "GreedyMarket has resolved a match from "
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
    }
  }

  return (0 == requestAmt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GreedyMarket::resolve()
{
  sortedMsgList::iterator request;

  firmOrders_ = 0;

  /// while requests_ remain and there is at least one offer left
  while (requests_.size() > 0)
  {
    request = requests_.end();
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
    messages_.erase((*request).second);
    requests_.erase(request);
  }

  executeOrderQueue();
}

