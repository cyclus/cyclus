// MaterialStore.h
#if !defined(_MATERIALSTORE_H)
#define _MATERIALSTORE_H

#include "CycException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CycOverCapException: public CycException {
  public: CycOverCapException(std::string msg) : CycException(msg) {};
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

  @exception CycOverCapException thrown if the new capacity is lower than
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
  makeUnlimited sets the store's capacity to be infinite.

  Never throws.
  */
  void makeUnlimited();

  /*!
  notSplitableOver sets removal behavior to never split resource
  objects and to return just larger than the specified remove quantity.

  Setting this behavior is "sticky" - all resource removal following the
  invocation of this method from this store will follow this behavior
  (unless another behavior changing method is called such as
  noSplitableOver).  If an attempt to remove quantity 10.5 from the store
  is made and the store contains several resurce objects all of quantity
  2.5, the remove method would return a set of 5 resource objects totaling
  quantity 12.5.  Never throws.
  */
  void notSplitableOver();

  /*!
  notSplitableUnder sets removal behavior to never split resource
  objects and to return just smaller than the specified remove quantity.

  Setting this behavior is "sticky" - all resource removal following the
  invocation of this method from this store will follow this behavior
  (unless another behavior changing method is called such as
  noSplitableUnder).  If an attempt to remove quantity 10.5 from the store
  store contains several resurce objects all of quantity 2.5, the remove
  method would return a set of 4 resource objects totaling quantity 10.0.
  Never throws.
  */
  void notSplitableUnder();

  /*!
  splitable sets removal behavior to split resource objects in order to
  return the exact specified remove quantity.

  Setting this behavior is "sticky" - all resource removal following the
  invocation of this method from this store will follow this behavior
  (unless another behavior changing method is called such as
  noSplitableUnder).  If an attempt to remove quantity 10.5 from the store
  store contains several resurce objects all of quantity 2.5, the remove
  method would return a set of 5 resource objects totaling quantity 10.5.
  The fifth resource object would have to be split into ones of quantity
  0.5 and 2.0.  The resource with quantity 2.0 would be retained in the
  store while the one of quantity 0.5 would be the fifth of the returned
  resources.  Never throws.
  */
  void splitable();

  std::vector<mat_rsrc_ptr> removeQty(double qty);

  std::vector<mat_rsrc_ptr> removeNum(int num);

  mat_rsrc_ptr removeOne();

  void addOne(mat_rsrc_ptr mat);

  void addAll(std::vector<mat_rsrc_ptr> mats);

private:

  bool unlimited_;

  bool splitable_;

  bool over_;

  double capacity_;

  std::vector<mat_rsrc_ptr> mats_;

};

#endif
