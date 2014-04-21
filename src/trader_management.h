#ifndef CYCLUS_SRC_TRADER_MANAGEMENT_H_
#define CYCLUS_SRC_TRADER_MANAGEMENT_H_

#include "error.h"
#include "exchange_context.h"
#include "product.h"
#include "material.h"
#include "trader.h"

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
inline std::set<RequestPortfolio<Product>::Ptr>
    QueryRequests<Product>(Trader* t) {
  return t->GetProductRequests();
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
inline std::set<BidPortfolio<Product>::Ptr>
    QueryBids<Product>(Trader* t,
                               const CommodMap<Product>::type& map) {
  return t->GetProductBids(map);
}

template<class T>
inline static void PopulateTradeResponses(
    Trader* trader,
    const std::vector< Trade<T> >& trades,
    std::vector< std::pair<Trade<T>, typename T::Ptr> >& responses) {
  throw StateError("Non-specialized version of "
                   "PopulateTradeResponses not supported");
}

template<>
inline void PopulateTradeResponses<Material>(
    Trader* trader,
    const std::vector< Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>, Material::Ptr> >& responses) {
  trader->GetMatlTrades(trades, responses);
}

template<>
inline void PopulateTradeResponses<Product>(
    Trader* trader,
    const std::vector< Trade<Product> >& trades,
    std::vector<std::pair<Trade<Product>,
        Product::Ptr> >& responses) {
  trader->GetProductTrades(trades, responses);
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
        Material::Ptr> >& responses) {
  trader->AcceptMatlTrades(responses);
}

template<>
inline void AcceptTrades(
    Trader* trader,
    const std::vector< std::pair<Trade<Product>,
        Product::Ptr> >& responses) {
  trader->AcceptProductTrades(responses);
}

}  // namespace cyclus

#endif  // CYCLUS_SRC_TRADER_MANAGEMENT_H_
