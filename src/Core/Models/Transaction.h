// Transaction.h
// conveys offer/request transaction info via inter-model Messages

#if !defined(_TRANSACTION_H)
#define _TRANSACTION_H

#include "Resource.h"

namespace cyclus {

class MarketModel;
class Model;

enum TransType {OFFER, REQUEST};

class Transaction {

  public:

    /**
    Creates a new transaction with 'creator' as either the supplier or
    requester depending on the transaction type.

    @param creator supplier if type=OFFER, requester if type=REQUEST
    @param type indicates the directionality of the transaction w.r.t. the creator
    @param res a resource to be set as the (cloned) resource in this transaction
    @param price The price per unit of the commodity being requested or offered. 
    @param minfrac minimum commodity fraction acceptible for sending/receiving
    */
    Transaction(Model* creator, TransType type, rsrc_ptr res=NULL, const double price=0.0, 
        const double minfrac=0.0);

    virtual ~Transaction();

    /**
    Deep clone of this transaction (the new clone gets a clone of this
    transaction's resource).

    @return a copy of this transaction.
    */
    Transaction* clone();

    /**
       Initiate the market-matched transaction - Resource(s) are taken from the
       supplier and sent to the requester.
        
       This should be the sole way of transferring resources between simulation
       agents/models. Book keeping of transactions (and corresponding resource
       states) are taken care of automatically.
     */
    void ApproveTransfer();

    /**
    Used by markets to pair matched offers and requests.

    This method automatically sets the supplier/requester of both this and the
    "other" transaction. Note that "offerTrans.MatchWith(requestTrans)" is
    equivelent to "requestTrans.MatchWith(offerTrans)".

    @param other the offer or request transaction to pair with

    @warning using a transaction more than once with this method will result in
             previous paring info being erased

    @exception ValueError this transaction and "other" are of
               the same TransType.
    */
    void MatchWith(Transaction& other);
  
    /**
       @return the market that deals in this this transaction's commodity

       @exception ValueError the transactions commodity has
                  no corresponding market.
     */
    MarketModel* market() const;
    
    /**
       @return a pointer to the supplier in this transaction
     */
    Model* supplier() const;

    /**
       @return a pointer to the requester in this transaction. 
     */
    Model* requester() const;

    /**
       @return the commodity requested or offered in this transaction. 
     */
    std::string commod() const;

    /**
       @param new_commod the commodity to be requested or offered in this
       transaction
     */
    void SetCommod(std::string new_commod);

    /**
       @return true if the transaction is an offer, false if it is a request.
     */
    bool IsOffer() const;

    /**
      Price for this transaction's resource.

      @return the price (in dollars) being requested or offered in this
      transaction. 
    */
    double price() const;

    /**
       Set the price being requested or offered in this transaction. 

       @param new_price price in dollars
     */
    void SetPrice(double new_price);

    /**
       @return a pointer to the resource being requested or offered in this
       transaction. 
     */
    rsrc_ptr Resource() const;

    /**
       Sets the transaction's resource to a copy of the passed resource.

       @param new_resource its clone will be stored in this transaction
     */
    void SetResource(rsrc_ptr new_resource);

    /**
       @return the minimum fraction (0-1) acceptible by the supplier or 
       receiver of this transaction
     */
    double Minfrac() const;

    /**
       Sets the transaction's minimum fraction to new_minfrac (0-1).

       @param new_minfrac the minimum commodity fraction acceptible
     */
    void SetMinFrac(double new_minfrac);

  private:
    /**
      The minimum fraction of the specified commodity that the 
      requester is willing to accept or the offerer is willing to send. 
     */
    double minfrac_;

    /// The commodity that is being requested or offered in this Message. 
    std::string commod_;

    TransType type_;

    /// The price per unit of the commodity being requested or offered. 
    double price_;

    /// A specific resource with which this transaction is concerned.
    rsrc_ptr resource_;

    Model* supplier_;

    Model* requester_;

    /// unique to a matched offer/request pair, set by matchWith
    int trans_id_;

    /// stores the next available transaction ID 
    static int next_trans_id_;

 private:
  /**
     add a transaction to the transaction table 
   */
  void AddTransToTable();

  /**
     add a transaction to the transaction table 
     @param position the position in the manifest 
     @param resource the resource being transacted 
   */
  void AddResourceToTable(int position, rsrc_ptr resource);
};
} // namespace cyclus
#endif

