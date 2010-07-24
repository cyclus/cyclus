// Message.h
// Message classes for inter-entity requests, instructions, etc.

#if !defined(_MESSAGE_H)
# define _MESSAGE_H

class Commodity;
class Communicator;

/**
 * An enumerative type to specify which direction (up or down the class 
 * hierarchy) this message is moving.
 */
enum MessageDir {up, down};

struct Transaction
{
  /**
   * The Commodity that is being requested or offered in this Message.
   */
  Commodity* myCommod;
  
  /**
   * The amount of the specified commodity being requested or offered. 
   * Units vary. 
   *
   * cake -- mass of uranium (metric tons)
   * uUF6 -- mass of uranium (metric tons)
   * eUF6 -- number of batches
   * fuel -- number of batches (reactor), tons of uranium (fabricator)
   * usedFuel -- number of assemblies (for now, at least)
   * 
   * Note: positive amounts mean you want something, negative amounts 
   * mean you want to get rid of something.
   */
  double amount;
  double minAmt;
  
  /**
   * The price of the commodity being requested or offered.
   */
  double price;
};

struct RoutingSlip
{
  /**
   * The direction this message is traveling (up or down the class 
   * hierarchy).
   */
  MessageDir dir;
  
  /**
   * The path that the message takes from one end to the other
   */
  Communicator *sndr, *rcvr, 
               *mkt, *reg, *inst, *fac;
};    

struct ShippingRoute
{
  Communicator *shipper, *receiver;
};

/**
 * A message class for offers & requests
 */
class OfferRequest {
  
private:
  
  Transaction trans;

  RoutingSlip msgPath;

public:
  
  /// sample constructor sets some basic data
  OfferRequest(Communicator* sender);
  
  double getAmount() { return trans.amount; };
  double setAmount(double newAmt) { trans.amount = newAmt; };
  double getPrice()   { return trans.price; };
  Transaction getTrans() { return trans; };

  MessageDir getDir() {return msgPath.dir; };
  Communicator* getReceiver() { return msgPath.rcvr; };
  Communicator* getMkt()  { return msgPath.mkt;  };
  Communicator* getReg()  { return msgPath.reg;  };
  Communicator* getInst() { return msgPath.inst; };
  Communicator* getFac()  { return msgPath.fac;  };

};

/**
 * A message class for shipments
 */

class Shipment {

private:
  Transaction trans;
  
  ShippingRoute shipPath;

public:
  Shipment(Transaction trans, Communicator* sender, Communicator* receiver);

  double getAmount() { return trans.amount; };
  double getPrice()   { return trans.price; };

  Communicator* getShipper()  { return shipPath.shipper;  };
  Communicator* getReceiver() { return shipPath.receiver; };

  void execute();

};
#endif
