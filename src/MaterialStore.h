// MaterialStore.h
#if !defined(_MATERIALSTORE_H)
#define _MATERIALSTORE_H

#include "CycException.h"
#include "Material.h"

#define STORE_EPS 1e-6

typedef std::vector<mat_rsrc_ptr> MatManifest;

class CycOverCapException: public CycException {
  public: CycOverCapException(std::string msg) : CycException(msg) {};
};
class CycNegQtyException: public CycException {
  public: CycNegQtyException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*!
MaterialStore is a helper function that provides semi-automated management of
material resource buffers (e.g. model stocks and inventories).

Methods that begin with a "set", "make", "add", or "remove" prefix change the
state/behavior of the store; other methods do not.  Default constructed
material store has zero (finite) capacity. Resource removal occurs in the order
the resources were added (i.e. oldest materials are removed first).
*/
class MaterialStore {

public:

  MaterialStore();

  virtual ~MaterialStore();

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
  quantity of material resources that already exist in the store.
  */
  void setCapacity(double cap);

  /*!
  count returns the total number of constituent material objects
  in the store. Never throws.
  */
  int count();

  /*!
  quantity returns the total resource quantity of constituent material objects
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
  quantity of material resources that already exist in the store.
  */
  void makeLimited(double cap);

  /*!
  removeQty removes the specified quantity of material resources from the
  store.

  Materials are split if necessary in order to remove the exact quantity
  specified (within STORE_EPS).  Materials are retrieved in the order they were
  added (i.e. oldest first).

  @throws CycNegQtyException the specified removal quantity is larger (by
  STORE_EPS) than the store's current quantity.
  */
  std::vector<mat_rsrc_ptr> removeQty(double qty);

  /*!
  removeNum removes the specified number or count of material objects from the
  store.

  Materials are not split.  Materials are retrieved in the order they were
  added (i.e. oldest first).

  @throws CycNegQtyException the specified removal number is larger than the
  store's current inventoryNum or the specified number is negative.
  */
  std::vector<mat_rsrc_ptr> removeNum(int num);

  /*!
  removeOne removes the one material objects from the store.

  Materials are not split.  Materials are retrieved in the order they were
  added (i.e. oldest first).

  @throws CycNegQtyException the store is empty.
  */
  mat_rsrc_ptr removeOne();

  /*!
  addOne adds a single material object to the store.

  Material resource objects are never combined in the store; they are stored as
  unique objects. The material object is only added to the store if it does not
  cause the store to exceed its capacity

  @throws CycOverCapException the addition of the given material object would
  cause the store to exceed its capacity.
  */
  void addOne(mat_rsrc_ptr mat);

  /*!
  addAll adds one or more material objects (as a std::vector) to the store.

  Material resource objects are never combined in the store; they are stored as
  unique objects. The material objects are only added to the store if they do
  not cause the store to exceed its capacity; otherwise none of the given
  material objects are added to the store.

  @throws CycOverCapException the addition of the given material objects would
  cause the store to exceed its capacity.
  */
  void addAll(std::vector<mat_rsrc_ptr> mats);

private:

  /// true if this store has an infinite capacity
  bool unlimited_;

  /// true if mat retrieval splits objects to match request qty exactly
  bool makeSplitable_;

  /// true if unsplitable retrievals should return more than request qty
  bool over_;

  /// maximum quantity of material resource this store can hold
  double capacity_;

  /// list of constituent material objects forming the store's inventory
  std::vector<mat_rsrc_ptr> mats_;

};

#endif
