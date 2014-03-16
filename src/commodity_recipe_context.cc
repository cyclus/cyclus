#include "commodity_recipe_context.h"

#include <utility>

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::AddInCommod(std::string in_commod,
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
void CommodityRecipeContext::AddRsrc(std::string commod, Resource::Ptr rsrc) {
  rsrc_commod_map_[rsrc->id()] = commod;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::RemoveRsrc(Resource::Ptr rsrc) {
  rsrc_commod_map_.erase(rsrc->id());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::UpdateRsrc(std::string commod, Resource::Ptr rsrc) {
  rsrc_commod_map_[rsrc->id()] = commod;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::UpdateInRec(std::string incommod,
                                         std::string recipe) {
  out_recipes_[recipe] = out_recipes_[in_recipes_[incommod]];
  in_recipes_[incommod] = recipe;
}

CommodityRecipeContext* CommodityRecipeContext::Clone() {
  return new CommodityRecipeContext(*this);
}

void CommodityRecipeContext::InfileToDb(QueryEngine* qe, DbInit di) {
  int nfuel = qe->NElementsMatchingQuery("fuel");
  for (int i = 0; i < nfuel; i++) {
    QueryEngine* fuel = qe->QueryElement("fuel", i);
    di.NewDatum("CommodityRecipeContext_inoutmap")
      ->AddVal("in_commod", fuel->GetString("incommodity"))
      ->AddVal("in_recipe", fuel->GetString("inrecipe"))
      ->AddVal("out_commod", fuel->GetString("outcommodity"))
      ->AddVal("out_recipe", fuel->GetString("outrecipe"))
      ->Record();
  }
}

void CommodityRecipeContext::InitFrom(QueryBackend* b) {
  QueryResult qr = b->Query("CommodityRecipeContext_inoutmap", NULL);
  for (int i = 0; i < qr.rows.size(); ++i) {
    AddInCommod(qr.GetVal<std::string>("in_commod", i),
                qr.GetVal<std::string>("in_recipe", i),
                qr.GetVal<std::string>("out_commod", i),
                qr.GetVal<std::string>("out_recipe", i));
  }

  try {
    qr = b->Query("CommodityRecipeContext_resmap", NULL);
  } catch (std::exception err) {
    return;  // table doesn't exist (okay)
  }
  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string commod = qr.GetVal<std::string>("commod", i);
    int id = qr.GetVal<int>("res_id", i);
    rsrc_commod_map_[id] = commod;
  }
}

void CommodityRecipeContext::Snapshot(DbInit di) {
  for (int i = 0; i < in_commods_.size(); ++i) {
    std::string c = in_commods_[i];
    di.NewDatum("CommodityRecipeContext_inoutmap")
      ->AddVal("in_commod", c)
      ->AddVal("in_recipe", in_recipes_[c])
      ->AddVal("out_commod", out_commod_map_[c])
      ->AddVal("out_recipe", out_recipes_[c])
      ->Record();
  }

  std::map<int, std::string>::iterator it = rsrc_commod_map_.begin();
  for (; it != rsrc_commod_map_.end(); ++it) {
    di.NewDatum("CommodityRecipeContext_resmap")
      ->AddVal("commod", it->second)
      ->AddVal("res_id", it->first)
      ->Record();
  }
}

std::string CommodityRecipeContext::schema() {
  return
      "  <oneOrMore>                                 \n"
      "  <element name=\"fuel\">                     \n"
      "   <ref name=\"incommodity\"/>                \n"
      "   <ref name=\"inrecipe\"/>                   \n"
      "   <ref name=\"outcommodity\"/>               \n"
      "   <ref name=\"outrecipe\"/>                  \n"
      "  </element>                                  \n"
      "  </oneOrMore>                                \n";
}

}  // namespace cyclus
