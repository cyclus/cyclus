// GreedyMarket.cpp
// Implements the GreedyMarket class

#include <iostream>
#include <cmath>

#include "GreedyMarket.h"

#include "Logger.h"
#include "Resource.h"
#include "CycException.h"
#include "InputXML.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
std::string GreedyMarket::str() {
  return MarketModel::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GreedyMarket::receiveMessage(msg_ptr msg) {
  messages_.insert(msg);

  if (msg->trans().isOffer()) {
    offers_.insert(indexedMsg(msg->trans().resource()->quantity(), msg));
  } else {
    requests_.insert(indexedMsg(msg->trans().resource()->quantity(), msg));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GreedyMarket::resolve() {

  sortedMsgList::iterator request;

  firmOrders_ = 0;

  /// while requests_ remain and there is at least one offer left
  while (requests_.size() > 0) {

    request = requests_.end();
    request--;
    
    if(match_request(request)) {
      process_request();
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GreedyMarket::reject_request(sortedMsgList::iterator request) {
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
void GreedyMarket::process_request() {
  // update pointer to firm orders
  firmOrders_ = orders_.size();

  while (matchedOffers_.size() > 0) {
    msg_ptr msg = *(matchedOffers_.begin());
    messages_.erase(msg);
    matchedOffers_.erase(msg);
  }
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool GreedyMarket::match_request(sortedMsgList::iterator request) {
  sortedMsgList::iterator offer;
  double requestAmt,offerAmt;
  msg_ptr offerMsg;
  msg_ptr requestMsg;

  requestAmt = request->first;
  requestMsg = request->second;
  
  // if this request is not yet satisfied &&
  // there are more offers_ left
  while ( fabs(requestAmt) > EPS_RSRC && offers_.size() > 0) {
    // get a new offer
    offer = offers_.end();
    offer--;
    offerAmt = offer->first;
    offerMsg = offer->second;

    LOG(LEV_DEBUG2, "GreedM") << "offeramt=" << offerAmt
                    << ", requestamt=" << requestAmt;

    // pop off this offer
    offers_.erase(offer);
    if (requestAmt - offerAmt > EPS_RSRC) { 
      // put a new message in the order stack
      // it goes down to supplier
      offerMsg->trans().matchWith(requestMsg->trans());

      // tenatively queue a new order (don't execute yet)
      matchedOffers_.insert(offerMsg);

      orders_.push_back(offerMsg);

      LOG(LEV_DEBUG1, "GreedM") 
        << "GreedyMarket has resolved a transaction "
        << " which is a match from "
        << offerMsg->trans().supplier()->ID()
        << " to "
        << offerMsg->trans().requester()->ID()
        << " for the amount:  " 
        << offerMsg->trans().resource()->quantity();

      requestAmt -= offerAmt;
    } 
    else {
      // split offer

      // queue a new order
      msg_ptr maybe_offer = offerMsg->clone();
      maybe_offer->trans().resource()->setQuantity(requestAmt);
      maybe_offer->trans().matchWith(requestMsg->trans());

      matchedOffers_.insert(offerMsg);

      orders_.push_back(maybe_offer);

      LOG(LEV_DEBUG1, "GreedM")  
        << "GreedyMarket has resolved a transaction "
        << " which is a match from "
        << maybe_offer->trans().supplier()->ID()
        << " (offer split) to "
        << maybe_offer->trans().requester()->ID()
        << " for the amount:  " 
        << maybe_offer->trans().resource()->quantity();

      // reduce the offer amount
      offerAmt -= requestAmt;

      // if the residual is above threshold,
      // make a new offer with reduced amount

      if(offerAmt > EPS_RSRC) {
        msg_ptr new_offer = offerMsg->clone();
        new_offer->trans().resource()->setQuantity(offerAmt);
        receiveMessage(new_offer);
      }

      // request fulfilled via an offer larger than it.
      requestAmt = 0;
    }
  }

  if (fabs(requestAmt) > EPS_RSRC) {
    LOG(LEV_DEBUG2, "GreedM") << "The request from Requester "
      << requestMsg->trans().requester()->ID()
      << " for the amount " << requestAmt << " rejected. ";
      reject_request(request);
  }

  return (fabs(requestAmt) < EPS_RSRC);
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
extern "C" Model* constructGreedyMarket() {
  return new GreedyMarket();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  

/* -------------------- */

