#ifndef CYCLUS_SRC_DISCOVERY_H_
#define CYCLUS_SRC_DISCOVERY_H_

#include <map>
#include <set>
#include <string>

#include "pyne.h"

namespace cyclus {

/// This function returns a vector of archetype names in a given
/// string that is the binary represnetation of a module/shared-object/library.
std::set<std::string> DiscoverArchetypes(const std::string s);

/// Discover archetype specifications for a path and library.
std::set<std::string> DiscoverSpecs(std::string p, std::string lib);

/// Discover archetype specifications that live recursively in modules in a dir.
std::set<std::string> DiscoverSpecsInDir(std::string d);

/// Discover archetype specifications that live recursively in CYCLUS_PATH directories.
std::set<std::string> DiscoverSpecsInCyclusPath();

/// Discover archetype metadata in cyclus path
/// @return a Json::Value having the structure
///  {
///   "specs": ["<spec1>", "<spec2>", ...],
///   "annotations": {"<spec1>": <annotation dict1>, ...},
///   "schema": {"<spec1>": "<schema1>"}
///  }
Json::Value DiscoverMetadataInCyclusPath();

}  // namespace cyclus

#endif  // CYCLUS_SRC_DISCOVERY_H_
