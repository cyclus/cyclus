// Transaction.h
// conveys offer/request transaction info via inter-model Messages

#if !defined(_TRANSACTION_H)
#define _TRANSACTION_H

#include "Resource.h"

class MarketModel;
class Model;

class Transaction {

  private:

    /// use Request() and Offer() factories to create Transaction objects
    Transaction(Model* creator, bool isoffer);

  public:

    /// create an transaction with 'requester' being the Model requesting 
    static Transaction Request(Model* requester);

    /// create an transaction with 'supplier' being the Model offering 
    static Transaction Offer(Model* supplier);

    virtual ~Transaction();

    /**
       Returns the market corresponding to the transaction's commodity 
     */
    MarketModel* market();
    
    /**
       Returns a pointer to the supplier in this Message. 

       @exception CycNullMsgParamException supplier is uninitialized (NULL)
     */
    Model* supplier() const;

    /**
       Sets the assigned supplier of the material for the 
       transaction in this message. 
     */
    void setSupplier(Model* supplier);

    /**
       Returns a pointer to the requester in this Message. 

       @exception CycNullMsgParamException requester is uninitialized (NULL)
     */
    Model* requester() const;

    /**
       Sets the assigned requester to receive the material 
       for the transaction in this message. 
     */
    void setRequester(Model* requester);

    /**
       Returns the commodity requested or offered in this Message. 
     */
    std::string commod() const;

    /**
       Sets the commodity being requested or offered in this Message. 
     */
    void setCommod(std::string newCommod);

    /**
       True if the transaction is an offer, false if it is a request.
     */
    bool isOffer() const;

    /**
       Returns the price (in dollars) being requested or offered in this message. 
     */
    double price() const;

    /**
       Set the price (in dollars) being requested or offered in this message. 
     */
    void setPrice(double newPrice);

    /**
       Returns a pointer to the Resource being requested or offered in this message. 
     */
    rsrc_ptr resource() const;

    /**
       Sets the transaction's resource to a copy of the passed resource.
     */
    void setResource(rsrc_ptr newResource);

  private:

    /// The commodity that is being requested or offered in this Message. 
    std::string commod_;

    /// true if this is an offer, false if it's a request 
    bool isOffer_;

    /**
      The minimum fraction of the specified commodity that the 
      requester is willing to accept or the offerer is willing to send. 
     */
    double minfrac_;

    /**
      The price per unit of the commodity being requested or offered. 
     */
    double price_;

    /// A specific resource with which this transaction is concerned.
    rsrc_ptr resource_;

    Model* supplier_;

    Model* requester_;
};

#endif

