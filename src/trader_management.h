#ifndef CYCLUS_TRADER_MANAGEMENT_H_
#define CYCLUS_TRADER_MANAGEMENT_H_

#include "exchange_context.h"
#include "material.h"
#include "generic_resource.h"
#include "trader.h"
#include "transaction.h"
#include "error.h"

namespace cyclus {
  
// template specializations to support inheritance and virtual functions
template<class T>
    inline static std::set<typename RequestPortfolio<T>::Ptr>
    QueryRequests(Trader* t) {
  throw StateError("Non-specialized version of QueryRequests not supported");
}

template<>
    inline std::set<RequestPortfolio<Material>::Ptr>
    QueryRequests<Material>(Trader* t) {
  return t->GetMatlRequests();
}

template<>
inline std::set<RequestPortfolio<GenericResource>::Ptr>
    QueryRequests<GenericResource>(Trader* t) {
  return t->GetGenRsrcRequests();
}

template<class T>
inline static std::set<typename BidPortfolio<T>::Ptr>
    QueryBids(Trader* t, const typename CommodMap<T>::type& map) {
  throw StateError("Non-specialized version of QueryBids not supported");
}
  
template<>
inline std::set<BidPortfolio<Material>::Ptr>
    QueryBids<Material>(Trader* t, const CommodMap<Material>::type& map) {
  return t->GetMatlBids(map);
}

template<>
inline std::set<BidPortfolio<GenericResource>::Ptr>
    QueryBids<GenericResource>(Trader* t, const CommodMap<GenericResource>::type& map) {
  return t->GetGenRsrcBids(map);
}

template<class T>
inline static void PopulateTradeResponses(
    Trader* trader,
    const std::vector< Trade<T> >& trades,
    std::vector< std::pair<Trade<T>, typename T::Ptr> >& responses) {
  throw StateError("Non-specialized version of PopulateTradeResponses not supported");
}

template<>
inline void PopulateTradeResponses<Material>(
    Trader* trader,
    const std::vector< Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>, Material::Ptr> >&
    responses) {
  trader->GetMatlTrades(trades, responses);
}

template<>
inline void PopulateTradeResponses<GenericResource>(
    Trader* trader,
    const std::vector< Trade<GenericResource> >& trades,
    std::vector<std::pair<Trade<GenericResource>,
    GenericResource::Ptr> >& responses) {
  trader->GetGenRsrcTrades(trades, responses);
}

template<class T>
inline static void AcceptTrades(
    Trader* trader,
    const std::vector< std::pair<Trade<T>, typename T::Ptr> >& responses) {
  throw StateError("Non-specialized version of AcceptTrades not supported");
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
