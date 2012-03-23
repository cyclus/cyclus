// ResourceBuff.h
#if !defined(_RESOURCEBUFF_H)
#define _RESOURCEBUFF_H

#include "Resource.h"
#include "Material.h"
#include "CycException.h"
#include <list>
#include <vector>

#define STORE_EPS 1e-6

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

  /// toRes is a helper function for casting std::vector<Material> to
  /// std::vector<Resource>
  static std::vector<rsrc_ptr> toRes(std::vector<mat_rsrc_ptr> mats) {
    std::vector<rsrc_ptr> resources;
    for (int i = 0; i < mats.size(); i++) {
      resources.push_back(boost::dynamic_pointer_cast<Resource>(mats.at(i)));
    }
    return resources;
  }

  /// toMat is a helper function for casting std::vector<Resource> to
  /// std::vector<Material>
  static std::vector<mat_rsrc_ptr> toMat(std::vector<rsrc_ptr> resources) {
    std::vector<mat_rsrc_ptr> mats;
    for (int i = 0; i < resources.size(); i++) {
      mats.push_back(boost::dynamic_pointer_cast<Material>(resources.at(i)));
    }
    return mats;
  }

  ResourceBuff();

  virtual ~ResourceBuff();

  /*!
  capacity returns the maximum resource quantity this store can hold (units
  based on constituent resource objects' units). 
Never throws.  Returns -1 if the store is unlimited.
  */
  double capacity();

  /*!
  setCapacity sets the maximum quantity this store can hold (units based
  on constituent resource objects' units).

  @throws CycOverCapException the new capacity is lower (by STORE_EPS) than the
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
  Never throws.  Returns -1 if the store is unlimited.
  */
  double space();

  /// unlimited returns whether this store has unlimited capacity. Never throws.
  bool unlimited();

  /// makeUnlimited sets the store's capacity to be infinite. Never throws.
  void makeUnlimited();

  /*!
  makeLimited sets the store's capacity finite and sets it to the specified value.

  @throws CycOverCapException the new capacity is lower (by STORE_EPS) than the
  quantity of resources that already exist in the store.
  */
  void makeLimited(double cap);

  /*!
  popQty pops the specified quantity of resources from the
  store.

  Resources are split if necessary in order to pop the exact quantity
  specified (within STORE_EPS).  Resources are retrieved in the order they were
  pushed (i.e. oldest first).

  @throws CycNegQtyException the specified pop quantity is larger (by
  STORE_EPS) than the store's current quantity.
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

  @throws CycDupMatException the resource object to be pushed is already present
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

  @throws CycDupMatException one or more of the resource objects to be pushed
  are already present in the store.
  */
  void pushAll(std::vector<rsrc_ptr> mats);

private:

  /// true if this store has an infinite capacity
  bool unlimited_;

  /// maximum quantity of resources this store can hold
  double capacity_;

  /// list of constituent resource objects forming the store's inventory
  std::list<rsrc_ptr> mats_;

};

#endif
