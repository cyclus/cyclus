#ifndef CYCLUS_SRC_CAPACITY_TYPES_H_
#define CYCLUS_SRC_CAPACITY_TYPES_H_

namespace cyclus {

/// the type of capacity, currently less-than-or-equal-to (LTEQ) and
/// greater-than-or-equal-to (GTEQ) are supported
enum CapType {
  LTEQ = 0,
  GTEQ,
  NONE, // default
} typedef cap_t;

}  // namespace cyclus

#endif  // CYCLUS_SRC_CAPACITY_TYPES_H_
