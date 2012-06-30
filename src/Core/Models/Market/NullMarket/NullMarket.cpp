// NullMarket.cpp
// Implements the NullMarket class
#include <iostream>

#include "NullMarket.h"

#include "Logger.h"
#include "CycException.h"
#include "InputXML.h"
#include "Resource.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
std::string NullMarket::str() {
  return MarketModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void NullMarket::receiveMessage(msg_ptr msg) {
  messages_.insert(msg);

  if (msg->trans().isOffer()){
    offers_.insert(indexedMsg(msg->trans().resource()->quantity(),msg));
  }
  else if (!msg->trans().isOffer()){
    requests_.insert(indexedMsg(msg->trans().resource()->quantity(),msg));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void NullMarket::reject_request(sortedMsgList::iterator request)
{
  // delete the tentative orders
  while ( orders_.size() > firmOrders_) {
    orders_.pop_back();
  }

  // put all matched offers_ back in the sorted list
  while (matchedOffers_.size() > 0) {
    msg_ptr msg = *(matchedOffers_.begin());
    offers_.insert(indexedMsg(msg->trans().resource()->quantity(),msg));
    matchedOffers_.erase(msg);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void NullMarket::process_request()
{
  // update pointer to firm orders
  firmOrders_ = orders_.size();

  while (matchedOffers_.size() > 0)
  {
    msg_ptr msg = *(matchedOffers_.begin());
    messages_.erase(msg);
    matchedOffers_.erase(msg);
  }
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool NullMarket::match_request(sortedMsgList::iterator request)
{
  sortedMsgList::iterator offer;
  double requestAmt, offerAmt, toRet;
  msg_ptr offerMsg;
  msg_ptr requestMsg;

  requestAmt = request->first;
  requestMsg = request->second;
  
  // if this request is not yet satisfied &&
  // there are more offers_ left
  while ( requestAmt > 0 && offers_.size() > 0)
  {
    // get a new offer
    offer = offers_.end();
    offer--;
    offerAmt = offer->first;
    offerMsg = offer->second;

    // pop off this offer
    offers_.erase(offer);
  
    if (requestMsg->trans().resource()->checkQuality(offerMsg->trans().resource())){
      if (requestAmt > offerAmt) { 
        // put a new message in the order stack
        // it goes down to supplier
        offerMsg->trans().matchWith(requestMsg->trans());

        // Queue an order
        matchedOffers_.insert(offerMsg);
        // Zero out the boolean. 
        // At least some of the request will be met.
        toRet = 0;

        orders_.push_back(offerMsg);

        LOG(LEV_DEBUG2, "none!") 
	  << "NullMarket has resolved a transaction "
	  << " which is a match from "
          << offerMsg->trans().supplier()->ID()
          << " to "
          << offerMsg->trans().requester()->ID()
          << " for the amount:  " 
          << offerMsg->trans().resource()->quantity();

        requestAmt -= offerAmt;
      } 
      else if (offerAmt >= requestAmt){
        // split offer

        // queue a new order
        msg_ptr maybe_offer = offerMsg->clone(); 
        maybe_offer->trans().resource()->setQuantity(requestAmt);
        maybe_offer->trans().matchWith(requestMsg->trans());

        matchedOffers_.insert(offerMsg);

        orders_.push_back(maybe_offer);

        LOG(LEV_DEBUG2, "none!") << "NullMarket has resolved a match from "
          << maybe_offer->trans().supplier()->ID()
          << " to "
          << maybe_offer->trans().requester()->ID()
          << " for the amount:  " 
          << maybe_offer->trans().resource()->quantity();

        // reduce the offer amount
        offerAmt -= requestAmt;

        // if the residual is above threshold,
        // make a new offer with reduced amount

        if(offerAmt > EPS_RSRC){
          msg_ptr new_offer = offerMsg->clone();
          new_offer->trans().resource()->setQuantity(offerAmt);
          receiveMessage(new_offer);
        }

        // zero out request
        requestAmt = 0;

        // Zero out the boolean. 
        // All of the request will be met.
        toRet = 0;
      }
    }
  }

  // if any of the request has been met, return true.
  return (0 == toRet);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void NullMarket::resolve()
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
      LOG(LEV_DEBUG2, "none!") << "The request from Requester "<< (*request).second->trans().requester()->ID()
          << " for the amount " << (*request).first 
          << " rejected. ";
      reject_request(request);
    }
    // remove this request
    messages_.erase((*request).second);
    requests_.erase(request);
  }

  for (int i = 0; i < orders_.size(); i++) {
    msg_ptr msg = orders_.at(i);
    msg->setDir(DOWN_MSG);
    msg->sendOn();
  }
  orders_.clear();
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
extern "C" Model* constructNullMarket() {
  return new NullMarket();
}


/* -------------------- */
