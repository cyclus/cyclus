#ifndef CYCLUS_SRC_RNG_H
#define CYCLUS_SRC_RNG_H

#include <boost/random.hpp>
#include <cstdint>

class SimInitTest;
class RandomTest;

namespace cyclus {

class SimInfo;

typedef boost::random::mt19937 Generator;
typedef boost::random::variate_generator<Generator&, boost::random::normal_distribution<> > NormalDist;

/// A random number generator.
class RandomNumberGenerator {
    friend class ::SimInitTest;
    friend class ::RandomTest;

    // all distributions are fiends
    friend class FixedDoubleDist;
    friend class UniformDoubleDist;
    friend class NormalDoubleDist;
    friend class FixedIntDist;
    friend class UniformIntDist;
    friend class NormalIntDist;

  private:
    /// Returns a random number for use in a distribution
    static Generator gen_;

  public:
    /// Initialize from seed
    void Initialize(SimInfo si);

    std::uint32_t random();

    // in archetype code
    //
    // #include "random_number_generator.h"
    //
    // class Foo:
    //    private:
    //       NormalDist myNormalDist;
    //
    // Foo::Foo() {
    //    myNormalDist = RandomNumberGenerator::makeNormalDist(mean, std_dev, min, max);
    // }
    //
    // void Foo::Tick() {
    //     bar = myNormalDist();
    // }
    //
    
    static NormalDist makeNormalDist(double mean, double std_dev, double min, double max) {
        boost::random::normal_distribution<> dist(mean, std_dev);
        NormalDist rn(gen_, dist);

        return rn;
    }

    /// wrappers for boost::random distributions

    /// geenerate a random number between [0, 1)
    double random_01();
    
    /// generate a random integer between [low, high)
    int random_uniform_int(int low, int high);

    /// generate a random real number between [low, high)
    double random_uniform_real(double low, double high);

    /// generate a double from a normal distribution, with truncation
    /// at low and high
    double random_normal_real(double mean, double std_dev, double low=0,
                              double high=std::numeric_limits<double>::max());

    /// generates an integer from a normal distribution, with truncation
    /// uses rounding to convert double to int
    int random_normal_int(double mean, double std_dev, int low=0,
                          int high=std::numeric_limits<int>::max());

};

class DoubleDistribution {
  public:
    virtual double sample() = 0;
    virtual double max() = 0;
};

class FixedDoubleDist : public DoubleDistribution {
  private:
    double value;
  public:
    FixedDoubleDist(double value_) : value(value_) {};
    virtual double sample() { return value; };
    virtual double max() { return value; };
};

class UniformDoubleDist : public DoubleDistribution {
  private:
    boost::random::uniform_real_distribution<> dist;
  public:
    UniformDoubleDist(double min = 0, double max=1) : dist(min, max) {};
    virtual double sample() { return dist(RandomNumberGenerator::gen_); }
    virtual double max() { return dist.max(); }
};

class NormalDoubleDist : public DoubleDistribution {
  private:
    boost::random::normal_distribution<> dist;
    double min_;
    double max_;
  public:
    NormalDoubleDist(double mean, double std_dev, double min=0, double max=std::numeric_limits<double>::max()) : dist(mean, std_dev), min_(min), max_(max) {};
    virtual double sample();
    virtual double max() { return max_; }
};

class IntDistribution {
  public:
    virtual int sample() = 0;
};

class FixedIntDist : public IntDistribution {
  private:
    int value;
  public:
    FixedIntDist(int value_) : value(value_) {};
    virtual int sample() { return value; };
};

class UniformIntDist : public IntDistribution {
  private:
    boost::random::uniform_int_distribution<> dist;
  public:
    UniformIntDist(int min = 0, int max=1) : dist(min, max) {};
    virtual int sample() { return dist(RandomNumberGenerator::gen_); }
    virtual int max() { return dist.max(); }
};

class NormalIntDist : public IntDistribution {
  private:
    boost::random::normal_distribution<> dist;
    int min_;
    int max_;
  public:
    NormalIntDist(double mean, double std_dev, int min=0, int max=std::numeric_limits<int>::max()) : dist(mean, std_dev), min_(min), max_(max) {};
    virtual int sample();
    virtual int max() { return max_; }
};

}

#endif // CYCLUS_SRC_RNG_H
