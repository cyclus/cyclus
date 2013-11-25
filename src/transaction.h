// transaction.h
// conveys offer/request transaction info via inter-model Messages
#ifndef CYCLUS_TRANSACTION_H_
#define CYCLUS_TRANSACTION_H_

#include <string>

namespace cyclus {

class Context;

/// @class Transaction
///
/// @brief A Transaction is a class used for recording resource transactions
/// within a simulation. 
class Transaction {
 public:
  /// @brief constructor
  ///
  /// @param ctx the simulation Context
  /// @param sender_id the ID of the sender Model
  /// @param receiver_id the ID of the receiver Model
  /// @param rsrc_id the ID of the Resource
  /// @param commodity the commodity associated with the transaction
  /// @param price a price value of the transaction
  Transaction(Context* ctx, int sender_id, int receiver_id, int rsrc_id,
              std::string commodity, double price = 0.0);

  /// @return the id of the sender
  inline int sender_id() const {return sender_id_;}
  
  /// @return the id of the receiver
  inline int receiver_id() const {return receiver_id_;}

  /// @return the id of the resource
  inline int resource_id() const {return resource_id_;}
  
  /// @return the commodity requested or offered in this transaction.
  inline std::string commodity() const {return commod_;}

  /// @return the price (in dollars) being requested or offered in this
  /// transaction.
  inline double price() const {return price_;}

  /// @brief records the transaction with any backends the context knows about
  void Record();
  
  /* -------------------- private methods and members -------------------------- */
  Context* ctx_;
  std::string commod_;
  double price_;
  int id_, sender_id_, receiver_id_, resource_id_;
  static int next_id_;
};

inline bool operator ==(const Transaction& rhs, const Transaction& lhs) {
  return (rhs.commodity() == rhs.commodity(),
          rhs.sender_id() == rhs.sender_id(),
          rhs.receiver_id() == rhs.receiver_id(),
          rhs.resource_id() == rhs.resource_id(),
          rhs.price() == rhs.price());
}

} // namespace cyclus

#endif // CYCLUS_TRANSACTION_H_

