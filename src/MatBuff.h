
// MatBuff.h
#if !defined(_MATBUFF_H)
#define _MATBUFF_H

#include "ResourceBuff.h"

typedef std::vector<mat_rsrc_ptr> MatManifest;

/*!
MatBuff is a helper function that provides semi-automated management of
Material resource buffers (e.g. model stocks and inventories).

Note that documentation for inherited methods is with the ResourceBuff class.
For documentation, see corresponding method doc in ResourceBuff class.  All
methods here simply wrap corresponding ResourceBuff methods and automatically
convert between rsrc_ptr and mat_rsrc_ptr.
*/
class MatBuff: public ResourceBuff {

public:

  std::vector<mat_rsrc_ptr> popQty(double qty);

  std::vector<mat_rsrc_ptr> popNum(int num);

  mat_rsrc_ptr popOne();

  void pushOne(mat_rsrc_ptr mat);

  void pushAll(MatManifest mats);
};

#endif
