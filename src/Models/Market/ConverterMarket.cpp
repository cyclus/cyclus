// ConverterMarket.cpp
// Implements the ConverterMarket class
#include <iostream>

#include "ConverterMarket.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::init(xmlNodePtr cur)
{ 
  MarketModel::init(cur);
  // The commodity initialized as the mktcommodity is the request commodity.
  
  offer_commod = NULL; 

  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/ConverterMarket");

  commod_name = XMLinput->get_xpath_content(cur,"offercommodity");
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

  converter_name = XMLinput->get_xpath_content(cur, "converter");
  converter = LI->getConverter(converter_name);
  if (NULL == converter_name)
    throw GenException("Converter sub-model'" + converter_name 
                       + "' does not exist for converter market '" + getName() 
                       + "'."); 
  
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void ConverterMarket::copy(ConverterMarket* src)
{ 
  MarketModel::copy(src);
  offer_commod = src->offer_commod;
  offer_commod->setMarket(this);
  req_commod = src->req_commod;
  req_commod->setMarket(this);
  converter = src->getConverter();
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
    offerMsg = converter->convert((*offer).second, (*request).second, commodity);
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

      cout << "ConverterMarket has resolved a match from "
          << maybe_offer->getSupplierID()
          << " to "
          << maybe_offer->getRequesterID() 
          << " for the amount:  " 
          << maybe_offer->getAmount() << endl;

      // make a new offer with reduced amount
      offerAmt -= requestAmt;
      Message *new_offer = new Message(*offerMsg);
      new_offer->setAmount(offerAmt);

      // call this method for consistency
      receiveMessage(new_offer);

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
          << " rejected. "<<endl;
      reject_request(request);
    }
    // remove this request
    messages.erase((*request).second);
    requests.erase(request);
  }

  executeOrderQueue();
}

