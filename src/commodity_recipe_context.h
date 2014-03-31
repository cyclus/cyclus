#ifndef CYCLUS_SRC_COMMODITY_RECIPE_CONTEXT_H_
#define CYCLUS_SRC_COMMODITY_RECIPE_CONTEXT_H_

#include <map>
#include <string>
#include <vector>

#include "cyc_std.h"
#include "resource.h"
#include "state_wrangler.h"

namespace cyclus {

/// @class CommodityRecipeContext
///
/// @brief a CommodityRecipeContext contains relationships between commodities,
/// recipes and resources
class CommodityRecipeContext : public StateWrangler {
 public:
  /// @brief add an input commodity and its relations
  void AddInCommod(std::string in_commod,
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
    return rsrc_commod_map_[rsrc->id()];
  }

  inline bool operator==(const CommodityRecipeContext& other) const {
    return (in_commods_.size() == other.in_commods_.size()
            && out_commods_.size() == other.out_commods_.size()
            && map_compare(out_commod_map_, other.out_commod_map_)
            && map_compare(in_recipes_, other.in_recipes_)
            && map_compare(out_recipes_, other.out_recipes_)
            && map_compare(rsrc_commod_map_, other.rsrc_commod_map_));
  }

  inline bool operator!=(const CommodityRecipeContext& other) const {
    return !operator==(other);
  }

  virtual CommodityRecipeContext* Clone();
  virtual void InfileToDb(QueryEngine* qe, DbInit di);
  virtual void InitFrom(QueryBackend* b);
  virtual void Snapshot(DbInit di);
  virtual std::string schema();

 private:
  std::vector<std::string> in_commods_;
  std::vector<std::string> out_commods_;
  std::map<std::string, std::string> out_commod_map_;
  std::map<std::string, std::string> in_recipes_;
  std::map<std::string, std::string> out_recipes_;
  std::map<int, std::string> rsrc_commod_map_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_COMMODITY_RECIPE_CONTEXT_H_
