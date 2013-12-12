#ifndef CYCLUS_COMMODITY_RECIPE_CONTEXT_H_
#define CYCLUS_COMMODITY_RECIPE_CONTEXT_H_

#include <map>
#include <string>
#include <vector>

#include "resource.h"

namespace cyclus {

/// @class CommodityRecipeContext
///
/// @brief a CommodityRecipeContext contains relationships between commodities,
/// recipes and resources
class CommodityRecipeContext {
 public:
  /// @brief add an input commodity and its relations
  void AddInCommod(
      std::string in_commod,
      std::string in_recipe,
      std::string out_commod,
      std::string out_recipe);

  /// @brief add a resource and its commodity affiliation
  void AddRsrc(std::string commod, Resource::Ptr rsrc);

  /// @brief update a resource and its commodity affiliation  
  void UpdateRsrc(std::string commod, Resource::Ptr rsrc);
  
  /// @brief removes a resource from the context
  void RemoveRsrc(Resource::Ptr rsrc);
  
  /// @brief update an input recipe and its commodity affiliation  
  void UpdateInRec(std::string in_commod, std::string recipe);

  /// @return input commodities
  inline const std::vector<std::string>& in_commods() const {
    return in_commods_;
  }

  /// @return output commodities  
  inline const std::vector<std::string>& out_commods() const {
    return out_commods_;
  }

  /// @return output commodity of an input commodity
  inline std::string out_commod(std::string in_commod) {
    return out_commod_map_[in_commod];
  }

  /// @return input recipe of an input commodity    
  inline std::string in_recipe(std::string in_commod) {
    return in_recipes_[in_commod];
  }

  /// @return output recipe of an input recipe
  inline std::string out_recipe(std::string in_recipe) {
    return out_recipes_[in_recipe];
  }

  /// @return commodity of a material
  /// @warning returns a blank string if material isn't found
  inline std::string commod(Resource::Ptr rsrc) {
    return rsrc_commod_map_[rsrc];
  }

  inline bool operator==(const CommodityRecipeContext& other) {
    return (in_commods_ == other.in_commods_
            && out_commods_ == other.out_commods_
            && out_commod_map_ == other.out_commod_map_
            && in_recipes_ == other.in_recipes_
            && out_recipes_ == other.out_recipes_
            && rsrc_commod_map_ == other.rsrc_commod_map_);
  }
      
 private:
  std::vector<std::string> in_commods_;
  std::vector<std::string> out_commods_;
  std::map<std::string, std::string> out_commod_map_;
  std::map<std::string, std::string> in_recipes_;
  std::map<std::string, std::string> out_recipes_;
  std::map<Resource::Ptr, std::string> rsrc_commod_map_;
};
  
} // namespace cyclus

#endif // CYCLUS_COMMODITY_RECIPE_CONTEXT_H_
