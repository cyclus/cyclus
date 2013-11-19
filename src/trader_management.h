#ifndef CYCLUS_TRADER_MANAGEMENT_H_
#define CYCLUS_TRADER_MANAGEMENT_H_

#include "material.h"
#include "generic_resource.h"
#include "trader.h"

namespace cyclus {

// template specializations to support inheritance and virtual functions
template<class T> std::set< RequestPortfolio<T> > QueryRequests(Trader* t) {
  return std::set< RequestPortfolio<T> >();
}

template<> std::set< RequestPortfolio<Material> >
    QueryRequests<Material>(Trader* t) {
  return t->AddMatlRequests();
}

template<> std::set< RequestPortfolio<GenericResource> >
    QueryRequests<GenericResource>(Trader* t) {
  return t->AddGenRsrcRequests();
}

template<class T> class ExchangeContext;
  
template<class T> std::set< BidPortfolio<T> >
    QueryBids(Trader* t, ExchangeContext<T>* ec) {
  return std::set< BidPortfolio<T> >();
}
  
template<> std::set< BidPortfolio<Material> >
    QueryBids<Material>(Trader* t, ExchangeContext<Material>* ec) {
  return t->AddMatlBids(ec);
}

template<> std::set< BidPortfolio<GenericResource> >
    QueryBids<GenericResource>(Trader* t, ExchangeContext<GenericResource>* ec) {
  return t->AddGenRsrcBids(ec);
}

template<class T>
    typename T::Ptr ExecTradeOffer(const Trade<T>& trade) {
  return typename T::Ptr(new T());
}

template<> Material::Ptr ExecTradeOffer<Material>(const Trade<Material>& trade) {
  return trade.bid->bidder->OfferMatlTrade(trade);
}

template<> GenericResource::Ptr
    ExecTradeOffer<GenericResource>(const Trade<GenericResource>& trade) {
  return trade.bid->bidder->OfferGenRsrcTrade(trade);
}

template<class T>
    void ExecTradeAccept(const Trade<T>& trade, typename T::Ptr rsrc) { }

template<> void ExecTradeAccept<Material>(
    const Trade<Material>& trade,
    Material::Ptr rsrc) {
  return trade.request->requester->AcceptMatlTrade(trade, rsrc);
}

template<> void ExecTradeAccept<GenericResource>(
    const Trade<GenericResource>& trade,
    GenericResource::Ptr rsrc) {
  return trade.request->requester->AcceptGenRsrcTrade(trade, rsrc);
}

template<class T> void Execute(const Trade<T>& trade) {
  typename T::Ptr rsrc = ExecTradeOffer(trade);
  ExecTradeAccept(trade, rsrc);
}

} // namespace cyclus

#endif // ifndef CYCLUS_TRADER_MANAGEMENT_H_
