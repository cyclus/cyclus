// recipe.h
#ifndef RECIPE_H_
#define RECIPE_H_

#include "query_engine.h"
#include "context.h"

namespace cyclus {
namespace recipe {
/// loads a specific recipe
void Load(Context* ctx, QueryEngine* qe);

} // namespace recipe
} // namespace cyclus

#endif

