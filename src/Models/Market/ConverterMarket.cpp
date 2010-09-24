// ConverterMarket.cpp
// Implements the ConverterMarket class
#include <iostream>

#include "ConverterMarket.h"
#include "ConverterModel.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::init(xmlNodePtr cur)
{ 
  MarketModel::init(cur);  /// get facility list
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/ConverterMarket/converter");
  
  xmlNodePtr conv_node = nodes->nodeTab[0];
  conv_name = XMLinput->get_xpath_content(conv_node,"type");
  
  Model* converter = NULL; 
  converter = LI->getConverterByName(conv_name);

  if (NULL == converter){
    throw GenException("Converter '" 
        + conv_name 
        + "' is not defined in this problem.");
    }
  Model* new_converter = Model::create(converter);

  // The commodity initialized as the mktcommodity is the request commodity.

  offer_commod = NULL; 

  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/ConverterMarket");

  string commod_name = XMLinput->get_xpath_content(cur,"offercommodity");
  offer_commod = LI->getCommodity(commod_name);
  if (NULL == offer_commod)
    throw GenException("Offer commodity '" + commod_name 
                       + "' does not exist for converter market '" + getName() 
                       + "'.");
  
  offer_commod->setMarket(this);
  
  commod_name = XMLinput->get_xpath_content(cur,"reqcommodity");
  req_commod = LI->getCommodity(commod_name);
  if (NULL == req_commod)
    throw GenException("Request commodity '" + commod_name 
                       + "' does not exist for converter market '" + getName() 
                       + "'.");
  
  req_commod->setMarket(this);

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::copy(ConverterMarket* src)
{ 
  MarketModel::copy(src);
  offer_commod = src->offer_commod;
  offer_commod->setMarket(this);
  req_commod = src->req_commod;
  req_commod->setMarket(this);
  conv_name = src->conv_name;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::print()
{ 
  MarketModel::print();
  cout << "where the offer commodity is {"
      << offer_commod->getName()
      << "}, the request commodity is {"
      << req_commod->getName()
      << "}. "
      << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
ConverterModel* ConverterMarket::getConverter()
{
  Model* converter = NULL;
  converter = LI->getConverterByName(conv_name);

  if (NULL == converter){
    throw GenException("Converter '" 
        + conv_name 
        + "' is not defined in this problem.");
    }

  return ((ConverterModel*)converter);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void ConverterMarket::receiveMessage(Message *msg)
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
void ConverterMarket::reject_request(sortedMsgList::iterator request)
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
void ConverterMarket::process_request()
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
bool ConverterMarket::match_request(sortedMsgList::iterator request)
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
    offerMsg = (this->getConverter())->convert((*offer).second, (*request).second);
    offerAmt = offerMsg->getAmount();

    // pop off this offer
    offers.erase(offer);
  
    if (requestAmt > offerAmt) { 
      // put a new message in the order stack
      // it goes down to supplier
      offerMsg->setRequesterID(requestMsg->getRequesterID());
      offerMsg->setDir(down);

      // tenatively queue a new order (don't execute yet)
      matchedOffers.insert(offerMsg);

      orders.push_back(offerMsg);

      cout << "ConverterMarket has resolved a match from "
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

      cout << "ConverterMarket has resolved a partial match from "
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
void ConverterMarket::resolve()
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
          << " rejected by the ConverterMarket. "<<endl;
      reject_request(request);
    }
    // remove this request
    messages.erase((*request).second);
    requests.erase(request);
  }

  executeOrderQueue();
}

