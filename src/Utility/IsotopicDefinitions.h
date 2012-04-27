// IsotopicDefinitions.h
#if !defined(_ISOTOPICDEFINITIONS_H)
#define _ISOTOPICDEFINITIONS_H

#include <map>

/* -- Useful Definitions -- */
/**
   avagadro's number
*/
#define AVOGADRO 6.02e23

/**
   smallest kilogram value
*/
#define EPS_KG 1e-6

/**
   smallest percent
*/
#define EPS_PERCENT 1e-14
/* -- */

/* -- Useful Typedefs -- */
/**
   Isotope integer, ZZZAAA
 */
typedef int Iso;

/**
   map of isotope integer to value (mass or atom)
 */
typedef std::map<Iso, double> CompMap;
/* -- */

/* -- Useful Structs -- */
/**
   a container fully describing an isotopic composition

   this container is responsible for managing the memory of its
   mass_fractions member. this responsibility is implied through the
   logging process: a composition is given an id when it is logged.
 */
typedef struct composition {
  // members
  int ID;
  CompMap* mass_fractions;
  composition* parent;
  int decay_time;
  // constructors & destructor
  composition() {
    init(0);
  }
  composition(CompMap* fracs) {
    init(fracs);
  }
  composition(const composition& other) {
    copy(other);
  }
  composition& operator=(const composition& other) {
    copy(other);
    return *this;
  }
  ~composition() {
    if ( !logged() ) {
      delete mass_fractions;
    }
  }
  // utility
  void init(CompMap* fracs) {
    ID = 0;
    mass_fractions = fracs;
    parent = 0;
    decay_time = 0;
  }
  void copy(const composition& other) {
    ID = other.ID;
    mass_fractions = other.mass_fractions;
    parent = other.parent;
    decay_time = other.decay_time;
  }
  bool logged() {
    return (ID > 0 && mass_fractions != 0);
  }
} comp_t;
/* -- */

#endif

