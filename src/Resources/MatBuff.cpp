
#include "MatBuff.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest MatBuff::popQty(double qty) {
  return ResourceBuff::toMat(ResourceBuff::popQty(qty));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest MatBuff::popNum(int num) {
  return ResourceBuff::toMat(ResourceBuff::popNum(num));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr MatBuff::popOne() {
  return boost::dynamic_pointer_cast<Material>(ResourceBuff::popOne());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MatBuff::pushOne(mat_rsrc_ptr mat) {
  ResourceBuff::pushOne(boost::dynamic_pointer_cast<Material>(mat));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MatBuff::pushAll(MatManifest mats) {
  ResourceBuff::pushAll(ResourceBuff::toRes(mats));
}

