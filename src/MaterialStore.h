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

  double capacity();

  void setCapacity(double cap);

  double space();

  void makeUnlimited();

  void notSplitableOver();

  void notSplitableUnder();

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
