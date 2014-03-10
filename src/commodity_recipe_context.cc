#include <utility>

#include "commodity_recipe_context.h"

namespace cyclus {

#define SHOW(X) \
  std::cout << __FILE__ << ":" << __LINE__ << ": "#X" = " << X << "\n"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::AddInCommod(std::string in_commod,
                                         std::string in_recipe,
                                         std::string out_commod,
                                         std::string out_recipe) {
  SHOW("AddedCommod: " + in_commod);
  in_commods_.push_back(in_commod);
  out_commods_.push_back(out_commod);
  out_commod_map_.insert(std::make_pair(in_commod, out_commod));
  in_recipes_.insert(std::make_pair(in_commod, in_recipe));
  out_recipes_.insert(std::make_pair(in_recipe, out_recipe));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::AddRsrc(std::string commod, Resource::Ptr rsrc) {
  SHOW("AddedRsrc: " + commod);
  rsrc_commod_map_[rsrc->id()] = commod;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::RemoveRsrc(Resource::Ptr rsrc) {
  SHOW("RemovedRsrc");
  rsrc_commod_map_.erase(rsrc->id());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::UpdateRsrc(std::string commod, Resource::Ptr rsrc) {
  SHOW("UpdatedRsrc");
  rsrc_commod_map_[rsrc->id()] = commod;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityRecipeContext::UpdateInRec(std::string incommod,
                                         std::string recipe) {
  SHOW("UpdatedRecipe: " + recipe);
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
      ->AddVal("in_commod", qe->GetString("incommodity"))
      ->AddVal("in_recipe", qe->GetString("inrecipe"))
      ->AddVal("out_commod", qe->GetString("outcommodity"))
      ->AddVal("out_recipe", qe->GetString("outrecipe"))
      ->Record();
  }
}

void CommodityRecipeContext::InitFrom(QueryBackend* b) {
  QueryResult qr = b->Query("CommodityRecipeContext_inoutmap", NULL);
  for (int i = 0; i < qr.rows.size(); ++i) {
    AddInCommod(qr.GetVal<std::string>(i, "in_commod"), 
                qr.GetVal<std::string>(i, "in_recipe"), 
                qr.GetVal<std::string>(i, "out_commod"), 
                qr.GetVal<std::string>(i, "out_recipe"));
  }

  try {
    qr = b->Query("CommodityRecipeContext_resmap", NULL);
  } catch(std::exception err) {} // table doesn't exist (okay)
  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string commod = qr.GetVal<std::string>(i, "commod");
    int id = qr.GetVal<int>(i, "id");
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
