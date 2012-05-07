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
    The IsoVector class is designed to intelligently manage a given 
    isotopic composition in the form of a CompMap. Specifically, the IsoVector 
    class defines all relevant operators to manage the decay, mixing, and 
    separation of compositions.
*/
class IsoVector : public boost::enable_shared_from_this<IsoVector> {  
 public:
  /* --- Constructors and Destructors --- */
  /**
     specialized constructor, assuming comp is already mass based.
     @param comp the composition to be copied into composition_
   */
  IsoVector(CompMapPtr comp);

  /**
     default destructor, releases composition_
   */
  ~IsoVector();

  /**
     swaps members, used by copy constructor and assignment copy operator
   */
  friend void swap(IsoVector& v1, IsoVector& v2);

  /**
     copy constructor
   */
  IsoVector(const IsoVector& other);
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
  CompMapPtr executeDecay(CompMapPtr parent, double time);
  /* --- */
};

#endif
