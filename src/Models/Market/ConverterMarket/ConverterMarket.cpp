// ConverterMarket.cpp
// Implements the ConverterMarket class

#include <iostream>
#include <cmath>

#include "ConverterMarket.h"

#include "ConverterModel.h"
#include "Resource.h"
#include "Logger.h"
#include "CycException.h"
#include "InputXML.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::init(xmlNodePtr cur) { 
  MarketModel::init(cur);  /// get facility list
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/ConverterMarket/converter");
  
  // Find out what convertermodel matches the name given
  xmlNodePtr conv_node = nodes->nodeTab[0];
  conv_name_ = XMLinput->get_xpath_content(conv_node,"type");
  Model* converter = Model::getModelByName(conv_name_);

  // make one
  Model* new_converter = Model::create(converter);

  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/ConverterMarket");

  // The commodity initialized as the mktcommodity is the request commodity.
  offer_commod_ = XMLinput->get_xpath_content(cur,"offercommodity");
  req_commod_ = XMLinput->get_xpath_content(cur,"reqcommodity");

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::copy(ConverterMarket* src)
{ 
  MarketModel::copy(src);
  offer_commod_ = src->offer_commod_;
  req_commod_ = src->req_commod_;
  conv_name_ = src->conv_name_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::copyFreshModel(Model* src)
{ 
  copy(dynamic_cast<ConverterMarket*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string ConverterMarket::str() { 
  std::string s = MarketModel::str()
                + "with offer commodity '"
                + offer_commod_
                + "' and request commodity '"
                + req_commod_
                + "'. ";
  return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
ConverterModel* ConverterMarket::getConverter() {
  Model* converter;
  converter = Model::getModelByName(conv_name_);

  return dynamic_cast<ConverterModel*>(converter);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void ConverterMarket::receiveMessage(msg_ptr msg)
{
  messages_.insert(msg);

  if (msg->trans().isOffer()){
    offers_.insert(indexedMsg(msg->trans().resource()->quantity(),msg));
  }
  else if (!msg->trans().isOffer()) {
    requests_.insert(indexedMsg(msg->trans().resource()->quantity(),msg));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void ConverterMarket::reject_request(sortedMsgList::iterator request)
{

  // delete the tentative orders
  while ( orders_.size() > firmOrders_) {
    orders_.pop_back();
  }

  // put all matched offers back in the sorted list
  while (matchedOffers_.size() > 0) {
    msg_ptr msg = *(matchedOffers_.begin());
    offers_.insert(indexedMsg(msg->trans().resource()->quantity(),msg));
    matchedOffers_.erase(msg);
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void ConverterMarket::process_request()
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
bool ConverterMarket::match_request(sortedMsgList::iterator request)
{
  sortedMsgList::iterator offer;
  double requestAmt,offerAmt;
  msg_ptr offerMsg;
  msg_ptr requestMsg;

  requestAmt = (*request).first;
  requestMsg = (*request).second;
  
  // if this request is not yet satisfied &&
  // there are more offers_ left
  while ( requestAmt > 0 && offers_.size() > 0)
  {
    // get a new offer
    offer = offers_.end();
    offer--;
    // convert it
    offerMsg = (this->getConverter())->convert((*offer).second, requestMsg);
    offerAmt = offerMsg->trans().resource()->quantity();

    // pop off this offer
    offers_.erase(offer);
    if (requestMsg->trans().resource()->checkQuality(offerMsg->trans().resource())){
      if (requestAmt > offerAmt) { 
        // put a new message in the order stack
        // it goes down to supplier
        offerMsg->trans().matchWith(requestMsg->trans());

        // tenatively queue a new order (don't execute yet)
        matchedOffers_.insert(offerMsg);

        orders_.push_back(offerMsg);

        LOG(LEV_DEBUG2, "none!") << "ConverterMarket has resolved a match from "
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

        LOG(LEV_DEBUG2, "none!") << "ConverterMarket has resolved a partial match from "
          << maybe_offer->trans().supplier()->ID()
          << " to "
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
          // call this method for consistency
          receiveMessage(new_offer);
        }

        // zero out request
        requestAmt = 0;
      }
    }
  }

  return (0 == requestAmt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void ConverterMarket::resolve()
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
      LOG(LEV_DEBUG2, "none!") << "The request from Requester "<< 
          (*request).second->trans().requester()->ID()
          << " for the amount " << (*request).first 
          << " rejected by the ConverterMarket. ";
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
extern "C" Model* constructConverterMarket() {
  return new ConverterMarket();
}

/* -------------------- */

