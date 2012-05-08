// IsoVector.h
#if !defined(_ISOVECTOR_H)
#define _ISOVECTOR_H

/* -- Includes -- */
#include "CompMap.h"
#include "Logger.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
/* -- */

/* -- Typedefs -- */
/**
   shared pointer to another isovector
 */
class IsoVector;
typedef boost::shared_ptr<IsoVector> IsoVectorPtr;
/* -- */

/* -- Sensitive Includes -- */
#include "RecipeLogger.h"
/* -- */

/** 
    @class IsoVector
    
    @section Introduction
    The IsoVector class is designed to intelligently manage a given 
    isotopic composition in the form of a CompMap. Specifically, the IsoVector 
    class defines all relevant operators to manage the decay, mixing, and 
    separation of compositions.

    While a CompMap stores a normalized composition, an IsoVector provides
    methods to combine different compositions.

    @section Addition
    Addition of compositions is a specific case of the mix() method. All
    values of a given composition are added to another at a ratio of 1:1. The
    finalized composition is then normalized.

    @section Subtraction
    Subtraction is not the reverse of addition, because it models the process
    of separation (and calls the separate() method). Isotopic separation removes
    all isotopes at a given efficiency (if the efficiency is less than 1, some
    of those isotopes remain). Subtraction, accordingly, is separation with
    efficiency = 1. All isotopes are removed, and the remaining are normalized.

    @section Decay
    The IsoVector class manages the decay of CompMaps via the decay(time) 
    method. New children will have their parent and decay time set accordingly.
*/
class IsoVector : public boost::enable_shared_from_this<IsoVector> {  
 public:
  /* --- Constructors and Destructors --- */
  /**
     default constructor
   */
  IsoVector();

  /**
     specialized constructor, assuming comp is already mass based.
     @param comp the composition to be copied into composition_
   */
  IsoVector(CompMapPtr comp);

  /**
     copy constructor
   */
  IsoVector(const IsoVector& other);

  /**
     default destructor, releases composition_
   */
  ~IsoVector();
  /* --- */

  /* --- Operators --- */
  /**
     assignment operator
   */
  IsoVector& operator=(IsoVector rhs);

  /**
     adds two compositions
     uses the mix() function with a ratio of 1
   */
  IsoVector& operator+=(const IsoVector& rhs);

  /**
     subtracts two compositions
   */
  IsoVector& operator-=(const IsoVector& rhs);

  /**
     adds two compositions
     uses the mix() function with a ratio of 1
   */
  const IsoVector operator+(const IsoVector& rhs) const;

  /**
     subtracts two compositions
   */
  const IsoVector operator-(const IsoVector& rhs) const;

  /**
     compares composition_ to rhs.comp()
   */
  bool operator==(const IsoVector& rhs) const;

  /**
     calls the == operator preceded by not (!)
   */
  bool operator!=(const IsoVector& rhs) const;
  /* --- */

  /* --- Instance Access --- */
  /**
     return the composition map
   */
  CompMapPtr comp() const;

  /**
     Return the mass fraction of an isotope in the composition
   */
  double massFraction(Iso tope);

  /**
     returns the atom fraction of an isotope in the composition
   */
  double atomFraction(Iso tope);

  /**
     calls composition_'s normalize
  */
  void normalize();

  /**
     validates the composition_, insuring all Isotopes numbers 
     and fraction values comply
  */
  void validate();

  /**
     resets all members to their initialized values
   */
  void reset();

  /**
     calls CompMap's printing functionality
   */
  void print();

  /**
     calls compEqual on other.comp()
   */
  bool compEquals(const IsoVector& other);

  /**
     calls equality operator on the values of composition_ and
     comp
   */
  bool compEquals(const CompMap& comp);
  /* --- */

  /* --- Transformations --- */
  /**
     mixes two IsoVectors with a given ratio of c1:c2
     @param c1 the first IsoVector
     @param c2 the second IsoVector
     @param ratio the amount of c1 compared to c2
     @return a shared pointer to the resulting composition
   */
  void mix(const IsoVector& other, double ratio);

  /**
     calls mix() on values of p_c1 and p_c2
   */
  void mix(const IsoVectorPtr& p_other, double ratio);

  /**
     separates one IsoVector from another at a given efficiency
     @param c1 the base IsoVector
     @param c2 the IsoVector to extract from c1
     @param efficiency the effiency of the separation
     @return a shared pointer to the resulting composition
   */
  void separate(const IsoVector& other, double efficiency);

  /**
     calls separate() on values of p_c1 and p_c2
   */
  void separate(const IsoVectorPtr& p_other, double efficiency);

  /**
     decays a composition for a given time, assumed to be in months
     
     this public function checks with the RecipeLogger to see if comp is
     logged as a decayable parent. if so, it will intelligently decay comp
     if a daughter has not already been decayed. if one has, a copy will be
     returned.
     @param comp the composition to be decayed
     @param time the decay time, in months
     @return a pointer to the result of this decay
   */
  void decay(double time);
  /* --- */
  
 private:
  /* --- Instance Management --- */
  /**
     the log level for all IsoVector instances
   */
  static LogLevel log_level_;

  /**
     a shared pointer to the CompMap managed by this IsoVector
   */
  CompMapPtr composition_;

  /**
     sets composition_ to comp
   */
  void setComp(CompMapPtr comp);

  /**
     this private function uses the DecayHandler to decay a composition
     by a given time
     @param comp the composition to be decayed
     @param time the decay time, in months
     @return a pointer to the result of this decay
   */
  static CompMapPtr executeDecay(CompMapPtr parent, double time);
  /* --- */
};

#endif
