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
  messages.insert(msg);

  if (msg->getAmount() > 0){
    offers.insert(indexedMsg(msg->getAmount(),msg));
  }
  else{
    requests.insert(indexedMsg(-msg->getAmount(),msg));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GreedyMarket::reject_request(sortedMsgList::iterator request)
{
  // send a failure message to the facility
  //  Transaction trans;
  //  trans.amount = 0;
  //  orders.push_back(new Message(down, trans, this, 
  //                               (*request).second->getRequester()));

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
void GreedyMarket::process_request()
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
bool GreedyMarket::match_request(sortedMsgList::iterator request)
{
  sortedMsgList::iterator offer;
  double requestAmt,offerAmt;
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

      // tenatively queue a new order (don't execute yet)
      matchedOffers.insert(offerMsg);

      //Message* maybe_offer = new Message(*offerMsg);
      //orders.push_back(maybe_offer);
      orders.push_back(offerMsg);

      requestAmt -= offerAmt;
    } 
    else {
      // split offer

      // queue a new order
      offerMsg->setAmount(requestAmt);

      matchedOffers.insert(offerMsg);
      Message* maybe_offer = new Message(*offerMsg);
      maybe_offer->setDir(down);
      maybe_offer->setRequesterID(requestMsg->getRequesterID());

      orders.push_back(maybe_offer);

      // zero out request
      requestAmt = 0;

      // make a new offer with reduced amount
      offerAmt -= requestAmt;
      Message *new_offer = new Message(*offerMsg);
      new_offer->setAmount(offerAmt);

      // call this method for consistency
      receiveMessage(new_offer);
    }
  }

  return (0 == requestAmt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GreedyMarket::resolve()
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
      cout << "to: "<< (*request).second->getRequesterID() << " for  " 
          << (*request).first << " matched."<<endl;
    } 
    else {
      cout << "to: "<< (*request).second->getRequesterID() 
          << " from: " << (*request).second->getSupplierID()
          << " for  " << (*request).first 
          << " rejected. "<<endl;
      reject_request(request);
    }
    // remove this request
    messages.erase((*request).second);
    requests.erase(request);
  }

  executeOrderQueue();
}

