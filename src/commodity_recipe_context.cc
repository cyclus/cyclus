#include <utility>

#include "commodity_recipe_context.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::AddInCommod(
    std::string in_commod,
    std::string in_recipe,
    std::string out_commod,
    std::string out_recipe) {

  in_commods_.push_back(in_commod);
  out_commods_.push_back(out_commod);
  out_commod_map_.insert(std::make_pair(in_commod, out_commod));
  in_recipes_.insert(std::make_pair(in_commod, in_recipe));
  out_recipes_.insert(std::make_pair(in_recipe, out_recipe));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::AddMat(std::string commod, Material::Ptr mat) {
  mat_commod_map_.insert(std::make_pair(mat, commod));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::RemoveMat(Material::Ptr mat) {
  mat_commod_map_.erase(mat);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::UpdateMat(std::string commod, Material::Ptr mat) {
  mat_commod_map_[mat] = commod;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::UpdateInRec(
    std::string incommod,
    std::string recipe) {
  out_recipes_[recipe] = out_recipes_[in_recipes_[incommod]];
  in_recipes_[incommod] = recipe;
}

} // namespace cyclus
