// Transaction.h
// conveys offer/request transaction info via inter-model Messages

#if !defined(_TRANSACTION_H)
#define _TRANSACTION_H

#include "Resource.h"
#include "Table.h"

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
    */
    Transaction(Model* creator, TransType type);

    virtual ~Transaction();

    /**
    Deep clone of this transaction (the new clone gets a clone of this
    transaction's resource).

    @return a copy of this transaction.
    */
    Transaction* clone();

    /**
       Initiate the market-matched transaction - resource(s) are taken from the
       supplier and sent to the requester.
        
       This should be the sole way of transferring resources between simulation
       agents/models. Book keeping of transactions (and corresponding resource
       states) are taken care of automatically.
     */
    void approveTransfer();
  
    /**
       @return the market that deals in this this transaction's commodity

       @exception CycMarketlessCommodException the transactions commodity has
       no corresponding market.
     */
    MarketModel* market() const;
    
    /**
       @return a pointer to the supplier in this transaction

       @exception CycNullMsgParamException supplier is uninitialized (NULL)
     */
    Model* supplier() const;

    /**
       @param supplier the assigned supplier of the material for this
       transaction
     */
    void setSupplier(Model* supplier);

    /**
       @return a pointer to the requester in this transaction. 

       @exception CycNullMsgParamException requester is uninitialized (NULL)
     */
    Model* requester() const;

    /**
       @param requester model that will receive the material for this
       transaction
     */
    void setRequester(Model* requester);

    /**
       @return the commodity requested or offered in this transaction. 
     */
    std::string commod() const;

    /**
       @param new_commod the commodity to be requested or offered in this
       transaction
     */
    void setCommod(std::string new_commod);

    /**
       @return true if the transaction is an offer, false if it is a request.
     */
    bool isOffer() const;

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
    void setPrice(double new_price);

    /**
       @return a pointer to the resource being requested or offered in this
       transaction. 
     */
    rsrc_ptr resource() const;

    /**
       Sets the transaction's resource to a copy of the passed resource.

       @param new_resource its clone will be stored in this transaction
     */
    void setResource(rsrc_ptr new_resource);

    /**
      The minimum fraction of the specified commodity that the 
      requester is willing to accept or the offerer is willing to send. 
     */
    double minfrac;

  private:

    /// The commodity that is being requested or offered in this Message. 
    std::string commod_;

    TransType type_;

    /// The price per unit of the commodity being requested or offered. 
    double price_;

    /// A specific resource with which this transaction is concerned.
    rsrc_ptr resource_;

    Model* supplier_;

    Model* requester_;

    /// stores the next available transaction ID 
    static int next_trans_id_;

///////////////////////////////////////////////////////////////////////////////
////////////// Output db recording code ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

 public:
  /**
     the transaction output database Table 
   */
  static table_ptr trans_table;

  /**
     the transacted resource output database Table 
   */
  static table_ptr trans_resource_table;

 private:
  /**
     Define the transaction database table 
   */
  static void define_trans_table();

  /**
     Define the transacted resource database table 
   */
  static void define_trans_resource_table();

  /**
     add a transaction to the transaction table 
     @param id the message id 
   */
  void addTransToTable(int id);

  /**
     add a transaction to the transaction table 
     @param id the message id 
     @param position the position in the manifest 
     @param resource the resource being transacted 
   */
  void addResourceToTable(int id, int position, rsrc_ptr resource);

  /**
     the transaction primary key 
   */
  primary_key_ref pkref_trans_;

  /**
     the resource primary key 
   */
  primary_key_ref pkref_rsrc_;
};

#endif

