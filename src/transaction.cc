// transaction.cc

#include "transaction.h"

#include "context.h"

namespace cyclus {

// initialize static variables
int Transaction::next_id_ = 1;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction::Transaction(Context* ctx, int sender_id, int receiver_id,
                         int rsrc_id, std::string commodity,
                         double price)
  : id_(next_id_++),
    ctx_(ctx),
    sender_id_(sender_id),
    receiver_id_(receiver_id),
    resource_id_(rsrc_id),
    commod_(commodity),
    price_(price) { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::Record() {
  ctx_->NewEvent("Transactions")
  ->AddVal("ID", id_)
  ->AddVal("SenderID", sender_id_)
  ->AddVal("ReceiverID", receiver_id_)
  ->AddVal("ResourceID", resource_id_)
  ->AddVal("Commodity", commod_)
  ->AddVal("Price", price_)
  ->AddVal("Time", ctx_->time())
  ->Record();
}

} // namespace cyclus
