
// mat_buff.h
#if !defined(_MATBUFF_H)
#define _MATBUFF_H

#include "resource_buff.h"

#include "material.h"
#include <vector>

namespace cyclus {

typedef std::vector<Material::Ptr> MatManifest;

/*!
MatBuff is a helper function that provides semi-automated management of
Material resource buffers (e.g. model stocks and inventories).

Note that documentation for inherited methods is with the ResourceBuff class.
For documentation, see corresponding method doc in ResourceBuff class.  All
methods here simply wrap corresponding ResourceBuff methods and automatically
convert between Resource::Ptr and Material::Ptr.
*/
class MatBuff: public ResourceBuff {

 public:

  /// toRes is a helper function for casting std::vector<Material> to
  /// std::vector<Resource>
  static std::vector<Resource::Ptr> ToRes(std::vector<Material::Ptr> mats) {
    std::vector<Resource::Ptr> resources;
    for (int i = 0; i < mats.size(); i++) {
      resources.push_back(boost::dynamic_pointer_cast<Resource>(mats.at(i)));
    }
    return resources;
  }

  /// toMat is a helper function for casting std::vector<Resource> to
  /// std::vector<Material>
  static std::vector<Material::Ptr> ToMat(std::vector<Resource::Ptr> resources) {
    std::vector<Material::Ptr> mats;
    for (int i = 0; i < resources.size(); i++) {
      mats.push_back(boost::dynamic_pointer_cast<Material>(resources.at(i)));
    }
    return mats;
  }

  MatManifest PopQty(double qty);

  MatManifest PopNum(int num);

  Material::Ptr PopOne();

  void PushOne(Material::Ptr mat);

  void PushAll(MatManifest mats);
};
} // namespace cyclus
#endif
