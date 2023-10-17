#ifndef CYCLUS_SRC_RNG_H
#define CYCLUS_SRC_RNG_H

#include <boost/random.hpp>

class SimInitTest;

namespace cyclus {

class Context;
class SimInfo;

/// A random number generator.
class RandomNumberGenerator {
    friend class ::SimInitTest;
  public:
    RandomNumberGenerator();

    ~RandomNumberGenerator();

    /// Initialize from seed
    void Initialize(Context* ctx, SimInfo si);

    boost::mt19937 gen_;
    
    /// Returns a random number for use in a distribution
    int randomnumber();

};

}

#endif // CYCLUS_SRC_RNG_H
