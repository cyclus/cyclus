// ResourceBuff.h
#if !defined(_RESOURCEBUFF_H)
#define _RESOURCEBUFF_H

#include "Resource.h"

#include "CycException.h"
#include <list>
#include <vector>
#include <set>
#include <limits>

static double const kBuffInfinity = std::numeric_limits<double>::max();

class CycOverCapException: public CycException {
    public: CycOverCapException(std::string msg) : CycException(msg) {};
};
class CycNegQtyException: public CycException {
    public: CycNegQtyException(std::string msg) : CycException(msg) {};
};
class CycDupResException: public CycException {
    public: CycDupResException(std::string msg) : CycException(msg) {};
};

typedef std::vector<rsrc_ptr> Manifest;

/*!
ResourceBuff is a helper function that provides semi-automated management of
resource buffers (e.g. model stocks and inventories).

Methods that begin with a "set", "make", "push", or "pop" prefix change the
state/behavior of the store; other methods do not.  Default constructed
resource store has zero (finite) capacity. Resource popping occurs in the order
the resources were pushed (i.e. oldest resources are popd first).
*/
class ResourceBuff {

public:

  ResourceBuff();

  virtual ~ResourceBuff();

  /*!
  capacity returns the maximum resource quantity this store can hold (units
  based on constituent resource objects' units). 
  Never throws.
  */
  double capacity();

  /*!
  setCapacity sets the maximum quantity this store can hold (units based
  on constituent resource objects' units).

  @throws CycOverCapException the new capacity is lower (by cyclus::eps_rsrc()) than the
  quantity of resources that already exist in the store.
  */
  void setCapacity(double cap);

  /*!
  count returns the total number of constituent resource objects
  in the store. Never throws.
  */
  int count();

  /*!
  quantity returns the total resource quantity of constituent resource objects
  in the store. Never throws.
  */
  double quantity();

  /*!
  space returns the quantity of space remaining in this store.

  It is effectively the difference between the capacity and the quantity.
  Never throws.
  */
  double space();

  /*!
  popQty pops the specified quantity of resources from the
  store.

  Resources are split if necessary in order to pop the exact quantity
  specified (within cyclus::eps_rsrc()).  Resources are retrieved in the order they were
  pushed (i.e. oldest first).

  @throws CycNegQtyException the specified pop quantity is larger (by
  cyclus::eps_rsrc()) than the store's current quantity.
  */
  std::vector<rsrc_ptr> popQty(double qty);

  /*!
  popNum pops the specified number or count of resource objects from the
  store.

  Resources are not split.  Resources are retrieved in the order they were
  pushed (i.e. oldest first).

  @throws CycNegQtyException the specified pop number is larger than the
  store's current inventoryNum or the specified number is negative.
  */
  std::vector<rsrc_ptr> popNum(int num);

  /*!
  popOne pops one resource object from the store.

  Resources are not split.  Resources are retrieved in the order they were
  pushed (i.e. oldest first).

  @throws CycNegQtyException the store is empty.
  */
  rsrc_ptr popOne();

  /*!
  pushOne pushs a single resource object to the store.

  Resource objects are never combined in the store; they are stored as
  unique objects. The resource object is only pushed to the store if it does not
  cause the store to exceed its capacity

  @throws CycOverCapException the pushing of the given resource object would
  cause the store to exceed its capacity.

  @throws CycDupResException the resource object to be pushed is already present
  in the store.
  */
  void pushOne(rsrc_ptr mat);

  /*!
  pushAll pushs one or more resource objects (as a std::vector) to the store.

  Resource objects are never combined in the store; they are stored as
  unique objects. The resource objects are only pushed to the store if they do
  not cause the store to exceed its capacity; otherwise none of the given
  resource objects are pushed to the store.

  @throws CycOverCapException the pushing of the given resource objects would
  cause the store to exceed its capacity.

  @throws CycDupResException one or more of the resource objects to be pushed
  are already present in the store.
  */
  void pushAll(Manifest mats);

  /**
     returns true if there are no mats in mats_
  */
  bool empty() {return mats_.empty();}

private:

  double qty_;

  /// maximum quantity of resources this store can hold
  double capacity_;

  /// list of constituent resource objects forming the store's inventory
  std::list<rsrc_ptr> mats_;
  std::set<rsrc_ptr> mats_present_;
};

#endif
