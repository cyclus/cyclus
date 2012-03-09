// MaterialStore.h
#if !defined(_MATERIALSTORE_H)
#define _MATERIALSTORE_H

#include "CycException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycOverCapException: public CycException {
  public: CycOverCapException(std::string msg) : CycException(msg) {};
};
class CycNegQtyException: public CycException {
  public: CycNegQtyException(std::string msg) : CycException(msg) {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MaterialStore {

public:

  MaterialStore();

  virtual ~MaterialStore();

  /*!
  capacity returns the maximum quantity this store can hold (units based
  on constituent resource objects' units). 

  Never throws.  Returns -1 if the store is unlimited.
  */
  double capacity();

  /*!
  setCapacity sets the maximum quantity this store can hold (units based
  on constituent resource objects' units).

  @throws CycOverCapException thrown if the new capacity is lower than
  the quantity of material resources that already exist in the store. This
  includes attempting to set the capacity to a negative value.
  */
  void setCapacity(double cap);

  /*!
  space returns the quantity of space remaining in this store.

  It is effectively the difference between the capacity and the sum total
  of constituent material resource quantities. Never throws.  Returns -1 if
  the store is unlimited
  */
  double space();

  /*!
  inventory returns the total quantity of constituent material objects in the
  store. Never throws.
  */
  double inventory();

  /// makeUnlimited sets the store's capacity to be infinite. Never throws.
  void makeUnlimited();

  /*!
  notSplitableOver sets removal behavior to never split resource objects and to
  return just larger than the specified remove quantity. Never throws.

  Example: If an attempt to remove quantity 10.5 from the store is made and the
  store contains several resurce objects all of quantity 2.5, the remove method
  would return a set of 5 resource objects totaling quantity 12.5.
  */
  void notSplitableOver();

  /*!
  notSplitableUnder sets removal behavior to never split resource objects and
  to return just smaller than the specified remove quantity. Never Throws.

  Example: If an attempt to remove quantity 10.5 from the store
  store contains several resurce objects all of quantity 2.5, the remove
  method would return a set of 4 resource objects totaling quantity 10.0.
  */
  void notSplitableUnder();

  /*!
  splitable sets removal behavior to split resource objects in order to
  return the exact specified remove quantity. Never throws.

  Example: If an attempt to remove quantity 10.5 from the store store contains
  several resurce objects all of quantity 2.5, the remove method would return a
  set of 5 resource objects totaling quantity 10.5.  The fifth resource object
  would have to be split into ones of quantity 0.5 and 2.0.  The resource with
  quantity 2.0 would be retained in the store while the one of quantity 0.5
  would be the fifth of the returned resources.
  */
  void splitable();

  /*!
  removeQty removes the specified quantity of material resources from the
  store.

  Materials are split or not split according to the behavior set by the
  splitable, notSplitableUnder, and notSplitableOver methods.  Materials are
  retrieved in the order they were added (i.e. oldest first).

  @throws CycNegQtyException the specified removal quantity is larger than the
  store's current inventory.
  */
  std::vector<mat_rsrc_ptr> removeQty(double qty);

  /*!
  removeNum removes the specified number or count of material objects from the
  store.

  Materials are not split.  Materials are retrieved in the order they were
  added (i.e. oldest first).

  @throws CycNegQtyException the specified removal number is larger than the
  store's current inventory.
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
  addOne 

  Material resource objects are never combined in the store; they are stored as
  unique objects. The material object is only added to the store if it does not
  cause the store to exceed its capacity

  @throws CycOverCapException the addition of the given material object would
  cause the store to exceed its capacity.
  */
  void addOne(mat_rsrc_ptr mat);

  /*!
  addAll 

  Material resource objects are never combined in the store; they are stored as
  unique objects. The material objects are only added to the store if they do
  not cause the store to exceed its capacity; otherwise none of the given
  material objects are added to the store.

  @throws CycOverCapException the addition of the given material objects would
  cause the store to exceed its capacity.
  */
  void addAll(std::vector<mat_rsrc_ptr> mats);

private:

  bool unlimited_;

  bool splitable_;

  bool over_;

  double capacity_;

  std::vector<mat_rsrc_ptr> mats_;

};

#endif
