// MatStore.h
#if !defined(_MATSTORE_H)
#define _MATSTORE_H

#include "CycException.h"
#include "Material.h"

#define STORE_EPS 1e-6

class CycOverCapException: public CycException {
  public: CycOverCapException(std::string msg) : CycException(msg) {};
};
class CycNegQtyException: public CycException {
  public: CycNegQtyException(std::string msg) : CycException(msg) {};
};
class CycDupMatException: public CycException {
  public: CycDupMatException(std::string msg) : CycException(msg) {};
};

/*!
MatStore is an abstract class that defines an interface for helper classes that
provide semi-automated management of material resource buffers (e.g. model
stocks and inventories).

Methods that begin with a "set", "make", "push", or "pop" prefix change the
state/behavior of the store; other methods do not.
*/
class MatStore {

public:

  /*!
  capacity returns the maximum resource quantity this store can hold (units
  based on constituent resource objects' units). 
  */
  virtual double capacity() = 0;

  /*!
  setCapacity sets the maximum quantity this store can hold (units based
  on constituent resource objects' units).
  */
  virtual void setCapacity(double cap) = 0;

  /*!
  count returns the total number of constituent material objects
  in the store.
  */
  virtual int count() = 0;

  /*!
  quantity returns the total resource quantity of constituent material objects
  in the store.
  */
  virtual double quantity() = 0;

  /*!
  space returns the quantity of space remaining in this store.
  */
  virtual double space() = 0;

  /// unlimited returns whether this store has unlimited capacity. Never throws.
  virtual bool unlimited() = 0;

  /// makeUnlimited sets the store's capacity to be infinite. Never throws.
  virtual void makeUnlimited() = 0;

  /*!
  makeLimited sets the store's capacity finite and sets it to the specified
  value.
  */
  virtual void makeLimited(double cap) = 0;

  /*!
  popQty pops the specified quantity of material resources from the
  store.
  */
  virtual std::vector<mat_rsrc_ptr> popQty(double qty) = 0;

  /*!
  popNum pops the specified number or count of material objects from the
  store.
  */
  virtual std::vector<mat_rsrc_ptr> popNum(int num) = 0;

  /// popOne pops one material object from the store.
  virtual mat_rsrc_ptr popOne() = 0;

  /*!
  pushOne pushs a single material object to the store.
  */
  virtual void pushOne(mat_rsrc_ptr mat) = 0;

  /// pushAll pushs one or more material objects (as a std::vector) to the store.
  virtual void pushAll(std::vector<mat_rsrc_ptr> mats) = 0;

};

#endif
