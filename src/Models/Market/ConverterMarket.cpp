// ConverterMarket.cpp
// Implements the ConverterMarket class
#include <iostream>
#include "Logger.h"

#include "ConverterMarket.h"
#include "ConverterModel.h"

#include "Logician.h"
#include "CycException.h"
#include "InputXML.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::init(xmlNodePtr cur)
{ 
  MarketModel::init(cur);  /// get facility list
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/ConverterMarket/converter");
  
  xmlNodePtr conv_node = nodes->nodeTab[0];
  conv_name_ = XMLinput->get_xpath_content(conv_node,"type");
  
  Model* converter; 
  converter = LI->getModelByName(conv_name_, CONVERTER);

  Model* new_converter = Model::create(converter);

  // The commodity initialized as the mktcommodity is the request commodity.

  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/ConverterMarket");

  string commod_name = XMLinput->get_xpath_content(cur,"offercommodity");
  offer_commod_ = Commodity::getCommodity(commod_name);
  
  offer_commod_->setMarket(this);
  
  commod_name = XMLinput->get_xpath_content(cur,"reqcommodity");
  req_commod_ = Commodity::getCommodity(commod_name);
  
  req_commod_->setMarket(this);

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::copy(ConverterMarket* src)
{ 
  MarketModel::copy(src);
  offer_commod_ = src->offer_commod_;
  offer_commod_->setMarket(this);
  req_commod_ = src->req_commod_;
  req_commod_->setMarket(this);
  conv_name_ = src->conv_name_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::copyFreshModel(Model* src)
{ 
  copy(dynamic_cast<ConverterMarket*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::print()
{ 
  MarketModel::print();
  LOG(LEV_DEBUG2) << "where the offer commodity is {"
      << offer_commod_->name()
      << "}, the request commodity is {"
      << req_commod_->name()
      << "}. ";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
ConverterModel* ConverterMarket::getConverter() {
  Model* converter;
  converter = LI->getModelByName(conv_name_, CONVERTER);

  return dynamic_cast<ConverterModel*>(converter);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void ConverterMarket::receiveMessage(Message *msg)
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
void ConverterMarket::reject_request(sortedMsgList::iterator request)
{
  // send a failure message to the facility
  //  Transaction trans;
  //  trans.amount = 0;
  //  orders.push_back(new Message(down, trans, this, 
  //                               (*request).second->getRequester()));

  // delete the tentative orders
  while ( orders_.size() > firmOrders_)
  {
    delete orders_.back();
    orders_.pop_back();
  }

  // put all matched offers back in the sorted list
  while (matchedOffers_.size() > 0)
  {
    Message *msg = *(matchedOffers_.begin());
    offers_.insert(indexedMsg(msg->getAmount(),msg));
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
    Message *msg = *(matchedOffers_.begin());
    messages_.erase(msg);
    matchedOffers_.erase(msg);
  }
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool ConverterMarket::match_request(sortedMsgList::iterator request)
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
    offerMsg = (this->getConverter())->convert((*offer).second, (*request).second);
    offerAmt = offerMsg->getAmount();

    // pop off this offer
    offers_.erase(offer);
  
    if (requestAmt > offerAmt) { 
      // put a new message in the order stack
      // it goes down to supplier
      offerMsg->setRequester(requestMsg->getRequester());
      offerMsg->setDir(DOWN_MSG);

      // tenatively queue a new order (don't execute yet)
      matchedOffers_.insert(offerMsg);

      orders_.push_back(offerMsg);

      LOG(LEV_DEBUG2) << "ConverterMarket has resolved a match from "
          << offerMsg->getSupplier()->ID()
          << " to "
          << offerMsg->getRequester()->ID()
          << " for the amount:  " 
          << offerMsg->getAmount();

      requestAmt -= offerAmt;
    } 
    else {
      // split offer

      // queue a new order
      Message* maybe_offer = new Message(*offerMsg);
      maybe_offer->setAmount(requestAmt);
      maybe_offer->setDir(DOWN_MSG);
      maybe_offer->setRequester(requestMsg->getRequester());

      matchedOffers_.insert(offerMsg);

      orders_.push_back(maybe_offer);

      LOG(LEV_DEBUG2) << "ConverterMarket has resolved a partial match from "
          << maybe_offer->getSupplier()->ID()
          << " to "
          << maybe_offer->getRequester()->ID()
          << " for the amount:  " 
          << maybe_offer->getAmount();

      // reduce the offer amount
      offerAmt -= requestAmt;

      // if the residual is above threshold,
      // make a new offer with reduced amount

      if(offerAmt > EPS_KG){
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
      LOG(LEV_DEBUG2) << "The request from Requester "<< 
          (*request).second->getRequester()->ID()
          << " for the amount " << (*request).first 
          << " rejected by the ConverterMarket. ";
      reject_request(request);
    }
    // remove this request
    messages_.erase((*request).second);
    requests_.erase(request);
  }

  executeOrderQueue();
}

