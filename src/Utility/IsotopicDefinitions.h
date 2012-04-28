// IsotopicDefinitions.h
#if !defined(_ISOTOPICDEFINITIONS_H)
#define _ISOTOPICDEFINITIONS_H

#include <map>
#include <iostream>
#include "boost/enable_shared_from_this.hpp"
#include "boost/shared_ptr.hpp"

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
   and a pointer to said map
 */
typedef std::map<Iso, double> CompMap;
typedef boost::shared_ptr<CompMap> CompMap_p;

/**
   a pointer to a composition
 */
//struct comp_t;
class comp_t;
typedef boost::shared_ptr<comp_t> comp_p;
/* -- */

/* -- Useful Structs -- */
/**
   a container fully describing an isotopic composition

   this container is responsible for managing the memory of its
   mass_fractions member. this responsibility is implied through the
   logging process: a composition is given an id when it is logged.
 */
//struct 
class comp_t : public boost::enable_shared_from_this<comp_t> {
public:
  // members
  int ID;
  CompMap_p mass_fractions;
  int decay_time;

  // constructors & destructor
  comp_t() {
    mass_fractions.reset(new CompMap());
    init();
  }
  comp_t(CompMap_p fracs) {
    mass_fractions = fracs;
    init();
  }
  comp_t(const comp_t& other) {
    copy(other);
  }
  ~comp_t() {
    mass_fractions.reset();
    parent_.reset();
  }
  
  // operators
  comp_t& operator=(const comp_t& other) {
    if (this != &other) {
      copy(other);
    }
    return *this;
  };
  bool operator==(const comp_t& other) const {
    // compares memory space pointed to
    bool cond1 = (mass_fractions == other.mass_fractions);
    bool cond2 = (ID == other.ID);
    return (cond1 && cond2); 
  }
  bool operator!=(const comp_t& other) const {
    return !(*this == other);
  }
  bool operator<(const comp_t& other) const {
    if (ID != other.ID) {
      return (ID < other.ID);
    }
    else {
      return (nSpecies() < other.nSpecies());
    }
  }
  friend bool operator<(const comp_p& one, const comp_p& other) {
    return (*one < *other);
  }
  friend std::ostream& operator<<(std::ostream& o, const comp_t& t) {
    return o << t.ID;
  }

  // utility
  void init() {
    ID = 0;
    decay_time = 0;
    parent_.reset();
  }
  void copy(const comp_t& other) {
    ID = other.ID;
    mass_fractions = other.mass_fractions; // ptr copy
    decay_time = other.decay_time;
    parent_ = other.parent(); // ptr copy
  }
  bool logged() {
    return (ID > 0 && nSpecies() > 0);
  }
  void normalize() {
    double total = 0.0;
    for (CompMap::iterator it = mass_fractions->begin(); it != mass_fractions->end(); it++) {
      total += it->second;
    }
    for (CompMap::iterator it = mass_fractions->begin(); it != mass_fractions->end(); it++) {
      it->second /= total;
    }
  }
  void setParent(comp_p parent) {
    parent_ = parent;
  }
  const int nSpecies() const {
    return mass_fractions->size();
  }
  const comp_p parent() const {
    return parent_;
  }
  comp_p me() {
    return shared_from_this();
  }

private:
  comp_p parent_;
  
};
/* -- */

#endif
