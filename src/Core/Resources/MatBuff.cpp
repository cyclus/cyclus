
#include "MatBuff.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest MatBuff::PopQty(double qty) {
  return ToMat(ResourceBuff::PopQty(qty));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MatManifest MatBuff::PopNum(int num) {
  return ToMat(ResourceBuff::PopNum(num));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr MatBuff::PopOne() {
  return boost::dynamic_pointer_cast<Material>(ResourceBuff::PopOne());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MatBuff::PushOne(mat_rsrc_ptr mat) {
  ResourceBuff::PushOne(boost::dynamic_pointer_cast<Material>(mat));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MatBuff::PushAll(MatManifest mats) {
  ResourceBuff::PushAll(ToRes(mats));
}

} // namespace cyclus
