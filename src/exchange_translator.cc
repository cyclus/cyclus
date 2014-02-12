#include "exchange_translator.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeTranslator::ToProg(ExchangeGraph* g, OsiSolverInterface* iface) {
  TranslateObj(g, iface);
  
  const std::vector<SupplyGroup::Ptr>& sgs = g->supply_groups();
  std::vector<SupplyGroup::Ptr>::const_iterator sg_it;
  for (sg_it = sgs->begin(); sg_it != sgs->end(); ++sg_it) {
    TranslateSGRow(*sg_it);
  }

  const std::vector<RequestGroup::Ptr>& rgs = g->request_groups();
  std::vector<RequestGroup::Ptr>::const_iterator rg_it;
  for (rg_it = rgs->begin(); rg_it != rgs->end(); ++rg_it) {
    TranslateRGRow(*rg_it);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeTranslator::TranslateObj(ExchangeGraph* g,
                                      OsiSolverInterface* iface) {
  iface->setObjSense(-1.0); // maximize
  const std::vector<Arc>& arcs = g->arcs();
  std::vector<Arcs>::const_iterator a_it;
  for (a_it = arcs->begin(); a_it != arcs->end(); ++a_it) {
    double id = g->arc_ids()[*a_it];
    double pref = a_it->unode->prefs[*a_it];
    if (a_it->exclusive) {
      iface->setObjCoeff(id, pref * a_it->excl_arc);
      ExclArcToProg(g, iface, *a_it);
    } else {
      iface->setObjCoeff(id, pref);
      ArcToProg(g, iface, *a_it);
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeTranslator::ArcToProg(ExchangeGraph* g, OsiSolverInterface* iface,
                                   const Arc& a) {
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeTranslator::ExclArcToProg(ExchangeGraph* g, OsiSolverInterface* iface,
                                       const Arc& a) {
}
