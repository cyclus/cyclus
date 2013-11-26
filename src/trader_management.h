#ifndef CYCLUS_TRADER_MANAGEMENT_H_
#define CYCLUS_TRADER_MANAGEMENT_H_

#include "material.h"
#include "generic_resource.h"
#include "trader.h"
#include "error.h"

namespace cyclus {
  
// template specializations to support inheritance and virtual functions
template<class T>
    inline static std::set<typename RequestPortfolio<T>::Ptr>
    QueryRequests(Trader* t) {
  throw StateError("Templated version of QueryRequests not supported");
}

template<>
    inline std::set<RequestPortfolio<Material>::Ptr>
    QueryRequests<Material>(Trader* t) {
  return t->AddMatlRequests();
}

template<>
inline std::set<RequestPortfolio<GenericResource>::Ptr>
    QueryRequests<GenericResource>(Trader* t) {
  return t->AddGenRsrcRequests();
}

template<class T> class ExchangeContext;
  
template<class T>
inline static std::set<typename BidPortfolio<T>::Ptr>
    QueryBids(Trader* t, ExchangeContext<T>* ec) {
  throw StateError("Templated version of QueryBids not supported");
}
  
template<>
inline std::set<BidPortfolio<Material>::Ptr>
    QueryBids<Material>(Trader* t, ExchangeContext<Material>* ec) {
  return t->AddMatlBids(ec);
}

template<>
inline std::set<BidPortfolio<GenericResource>::Ptr>
    QueryBids<GenericResource>(Trader* t, ExchangeContext<GenericResource>* ec) {
  return t->AddGenRsrcBids(ec);
}

template<class T>
inline static typename T::Ptr ExecTradeOffer(const Trade<T>& trade) {
  return typename T::Ptr(new T());
}

template<>
inline Material::Ptr
    ExecTradeOffer<Material>(const Trade<Material>& trade) {
  return trade.bid->bidder()->OfferMatlTrade(trade);
}

template<>
inline GenericResource::Ptr
    ExecTradeOffer<GenericResource>(const Trade<GenericResource>& trade) {
  return trade.bid->bidder()->OfferGenRsrcTrade(trade);
}

template<class T>
inline static void ExecTradeAccept(const Trade<T>& trade,
                                   typename T::Ptr rsrc) { }

template<>
inline void ExecTradeAccept<Material>(const Trade<Material>& trade,
                                      Material::Ptr rsrc) {
  return trade.request->requester()->AcceptMatlTrade(trade, rsrc);
}

template<>
inline void ExecTradeAccept<GenericResource>(
    const Trade<GenericResource>& trade,
    GenericResource::Ptr rsrc) {
  return trade.request->requester()->AcceptGenRsrcTrade(trade, rsrc);
}

template<class T>
inline static void ExecuteTrade(const Trade<T>& trade) {
  typename T::Ptr rsrc = ExecTradeOffer(trade);
  if (rsrc->quantity() != trade.amt) {
    throw ValueError("Trade amt and resource qty must match");
  }
  ExecTradeAccept(trade, rsrc);
}

template<class T>
inline static void PopulateTradeResponses(
    Trader* trader,
    const std::vector< Trade<T> >& trades,
    std::vector< std::pair<Trade<T>, typename T::Ptr> >& responses) {
  throw StateError("Templated version of PopulateTradeResponses not supported");
}

template<>
inline void PopulateTradeResponses<Material>(
    Trader* trader,
    const std::vector< Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>, Material::Ptr> >&
    responses) {
  trader->PopulateMatlTradeResponses(trades, responses);
}

template<>
inline void PopulateTradeResponses<GenericResource>(
    Trader* trader,
    const std::vector< Trade<GenericResource> >& trades,
    std::vector<std::pair<Trade<GenericResource>,
    GenericResource::Ptr> >& responses) {
  trader->PopulateGenRsrcTradeResponses(trades, responses);
}

template<class T>
inline static void AcceptTrades(
    Trader* trader,
    const std::vector< std::pair<Trade<T>, typename T::Ptr> >& responses) {
  throw StateError("Templated version of AcceptTrades not supported");
}

template<>
inline void AcceptTrades(
    Trader* trader,
    const std::vector< std::pair<Trade<Material>,
    typename Material::Ptr> >& responses) {
  trader->AcceptMatlTrades(responses);
}

template<>
inline void AcceptTrades(
    Trader* trader,
    const std::vector< std::pair<Trade<GenericResource>,
    typename GenericResource::Ptr> >& responses) {
  trader->AcceptGenRsrcTrades(responses);
}
} // namespace cyclus

#endif // ifndef CYCLUS_TRADER_MANAGEMENT_H_
