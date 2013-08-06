
// MatBuff.h
#if !defined(_MATBUFF_H)
#define _MATBUFF_H

#include "ResourceBuff.h"

#include "Material.h"
#include <vector>

namespace cyclus {

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

  /// toRes is a helper function for casting std::vector<Material> to
  /// std::vector<Resource>
  static std::vector<rsrc_ptr> ToRes(std::vector<mat_rsrc_ptr> mats) {
    std::vector<rsrc_ptr> resources;
    for (int i = 0; i < mats.size(); i++) {
      resources.push_back(boost::dynamic_pointer_cast<Resource>(mats.at(i)));
    }
    return resources;
  }

  /// toMat is a helper function for casting std::vector<Resource> to
  /// std::vector<Material>
  static std::vector<mat_rsrc_ptr> ToMat(std::vector<rsrc_ptr> resources) {
    std::vector<mat_rsrc_ptr> mats;
    for (int i = 0; i < resources.size(); i++) {
      mats.push_back(boost::dynamic_pointer_cast<Material>(resources.at(i)));
    }
    return mats;
  }

  MatManifest PopQty(double qty);

  MatManifest PopNum(int num);

  mat_rsrc_ptr PopOne();

  void PushOne(mat_rsrc_ptr mat);

  void PushAll(MatManifest mats);
};
} // namespace cyclus
#endif
