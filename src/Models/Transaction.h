// Transaction.h
// conveys offer/request transaction info via inter-model Messages

#if !defined(_TRANSACTION_H)
#define _TRANSACTION_H

#include "Resource.h"

class MarketModel;
class Model;

enum TransType {OFFER, REQUEST};

class Transaction {

  public:

    /**
    Creates a new transaction with 'creator' as either the supplier or
    requester depending on the transaction type.

    @param creator supplier if type=OFFER, requester if type=REQUEST

    @param type indicates the directionality of the transaction w.r.t. the creator.
    */
    Transaction(Model* creator, TransType type);

    virtual ~Transaction();

    Transaction* clone();

    /**
       Returns the market corresponding to the transaction's commodity 
     */
    MarketModel* market() const;
    
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

    /**
      The minimum fraction of the specified commodity that the 
      requester is willing to accept or the offerer is willing to send. 
     */
    double minfrac;

  private:

    /// The commodity that is being requested or offered in this Message. 
    std::string commod_;

    TransType type_;

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

