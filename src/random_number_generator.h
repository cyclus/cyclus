#ifndef CYCLUS_SRC_RNG_H
#define CYCLUS_SRC_RNG_H

#include <boost/random.hpp>
#include <cstdint>

#include "error.h"

class SimInitTest;
class RandomTest;

namespace cyclus {

class SimInfo;

typedef boost::random::mt19937 Generator;
typedef boost::random::variate_generator<Generator&,
                                         boost::random::normal_distribution<>>
    NormalDist;

/// A random number generator.
class RandomNumberGenerator {
  friend class ::SimInitTest;
  friend class ::RandomTest;

  // all distributions are fiends
  friend class FixedDoubleDist;
  friend class UniformDoubleDist;
  friend class NormalDoubleDist;
  friend class PoissonDoubleDist;
  friend class ExponentialDoubleDist;
  friend class BinaryDoubleDist;
  friend class FixedIntDist;
  friend class UniformIntDist;
  friend class NormalIntDist;
  friend class BinomialIntDist;
  friend class NegativeBinomialIntDist;
  friend class PoissonIntDist;
  friend class ExponentialIntDist;
  friend class BinaryIntDist;

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
  //    myNormalDist = RandomNumberGenerator::makeNormalDist(mean, std_dev, min,
  //    max);
  // }
  //
  // void Foo::Tick() {
  //     bar = myNormalDist();
  // }
  //

  static NormalDist makeNormalDist(double mean, double std_dev, double min,
                                   double max) {
    boost::random::normal_distribution<> dist(mean, std_dev);
    NormalDist rn(gen_, dist);

    return rn;
  }

  /// wrappers for boost::random distributions

  /// generate a random number between [0, 1)
  double random_01();

  /// generate a random integer between [low, high)
  int random_uniform_int(int low, int high);

  /// generate a random real number between [low, high)
  double random_uniform_real(double low, double high);

  /// generate a double from a normal distribution, with truncation
  /// at low and high
  double random_normal_real(double mean, double std_dev, double low = 0,
                            double high = std::numeric_limits<double>::max());

  /// generates an integer from a normal distribution, with truncation
  /// uses rounding to convert double to int
  int random_normal_int(double mean, double std_dev, int low = 0,
                        int high = std::numeric_limits<int>::max());
};

class DoubleDistribution {
 public:
  typedef boost::shared_ptr<DoubleDistribution> Ptr;

  virtual double sample() = 0;
  virtual double max() = 0;
};

class FixedDoubleDist : public DoubleDistribution {
 private:
  double value;

 public:
  typedef boost::shared_ptr<FixedDoubleDist> Ptr;

  FixedDoubleDist(double value_) : value(value_){};
  virtual double sample() { return value; };
  virtual double max() { return value; };
};

class UniformDoubleDist : public DoubleDistribution {
 private:
  boost::random::uniform_real_distribution<> dist;

 public:
  typedef boost::shared_ptr<UniformDoubleDist> Ptr;

  UniformDoubleDist(double min = 0, double max = 1) : dist(min, max){};
  virtual double sample() { return dist(RandomNumberGenerator::gen_); }
  virtual double max() { return dist.max(); }
};

class NormalDoubleDist : public DoubleDistribution {
 private:
  boost::random::normal_distribution<> dist;
  double min_;
  double max_;

 public:
  typedef boost::shared_ptr<NormalDoubleDist> Ptr;

  NormalDoubleDist(double mean, double std_dev, double min = 0, double max = 1)
      : dist(mean, std_dev), min_(min), max_(max) {
    if (min_ == max_) {
      throw ValueError(
          "Min and max cannot be equal for a normal distribution. Either use "
          "FixedDoubleDist or change the min/max.");
    }
    if (max_ < (mean - 3 * std_dev) || min_ > (mean + 3 * std_dev)) {
      Warn<VALUE_WARNING>(
          "Dist is sampling from a tail of a truncated normal more than 3 "
          "standard deviations from the mean. Drawing sampling may be "
          "inefficient");
    }
  };
  virtual double sample();
  virtual double max() { return max_; }
};

/// Poisson distribution requires a mean
class PoissonDoubleDist : public DoubleDistribution {
 private:
  boost::random::poisson_distribution<> dist;
  double mean_;

 public:
  typedef boost::shared_ptr<PoissonDoubleDist> Ptr;

  PoissonDoubleDist(double mean) : dist(mean_) {
    if (mean_ < 0) {
      throw ValueError("Mean must be positive");
    }
  };
  virtual double sample() { return dist(RandomNumberGenerator::gen_); }
  virtual double mean() { return dist.mean(); }
};

/// Exponential distribution requires lambda
class ExponentialDoubleDist : public DoubleDistribution {
 private:
  boost::random::exponential_distribution<> dist;
  double lambda_;

 public:
  typedef boost::shared_ptr<ExponentialDoubleDist> Ptr;

  ExponentialDoubleDist(double lambda) : dist(lambda), lambda_(lambda) {
    if (lambda_ < 0) {
      throw ValueError("Lambda must be positive");
    }
  };
  virtual double sample() { return dist(RandomNumberGenerator::gen_); }
  virtual double lambda() { return lambda_; }
};

/// Binary distribution requires twp options and a probability
class BinaryDoubleDist : public DoubleDistribution {
 private:
  boost::random::binomial_distribution<int> dist;
  double p_success_;
  double choice_a_, choice_b_;

 public:
  BinaryDoubleDist(double p_success, double choice_a, double choice_b)
      : dist(1, p_success), choice_a_(choice_a), choice_b_(choice_b) {
    if (p_success < 0) {
      throw ValueError("Probability of choice A must be positive");
    }
  };
  virtual double sample() {
    return dist(RandomNumberGenerator::gen_) ? choice_a_ : choice_b_;
  }
  virtual double p() { return p_success_; }
};

class IntDistribution {
 public:
  typedef boost::shared_ptr<IntDistribution> Ptr;
  virtual int sample() = 0;
};

class FixedIntDist : public IntDistribution {
 private:
  int value;

 public:
  typedef boost::shared_ptr<FixedIntDist> Ptr;

  FixedIntDist(int value_) : value(value_){};
  virtual int sample() { return value; };
};

class UniformIntDist : public IntDistribution {
 private:
  boost::random::uniform_int_distribution<> dist;

 public:
  typedef boost::shared_ptr<UniformIntDist> Ptr;

  UniformIntDist(int min = 0, int max = 1) : dist(min, max){};
  virtual int sample() { return dist(RandomNumberGenerator::gen_); }
  virtual int max() { return dist.max(); }
};

class NormalIntDist : public IntDistribution {
 private:
  boost::random::normal_distribution<> dist;
  int min_;
  int max_;

 public:
  typedef boost::shared_ptr<NormalIntDist> Ptr;

  NormalIntDist(double mean, double std_dev, int min = 0, int max = 1)
      : dist(mean, std_dev), min_(min), max_(max) {
    if (min_ == max_) {
      throw ValueError(
          "Min and max cannot be equal for a normal distribution. Either use "
          "FixedIntDist or change the min/max.");
    }
    if (max_ < (mean - 3 * std_dev) || min_ > (mean + 3 * std_dev)) {
      Warn<VALUE_WARNING>(
          "Dist is sampling from a tail of a truncated normal more than 3 "
          "standard deviations from the mean. Drawing sampling may be "
          "inefficient");
    }
  };
  virtual int sample();
  virtual int max() { return max_; }
};

/// Binomial distribution requries an integer number of trials and a
/// probability of success for each trial. Sampling returns the number of
/// successes. When trials is one, this is equivalent to a Bernoulli dist
class BinomialIntDist : public IntDistribution {
 private:
  boost::random::binomial_distribution<int> dist;
  int trials_;
  double p_success_;

 public:
  typedef boost::shared_ptr<BinomialIntDist> Ptr;

  BinomialIntDist(int trials, double p_success)
      : dist(trials, p_success), trials_(trials), p_success_(p_success) {
    if (trials_ < 1) {
      throw ValueError("Max must be positive and greater than zero");
    }
    if (p_success_ > 1 || p_success_ < 0) {
      throw ValueError("Probability must be between zero and one");
    }
  };
  virtual int sample() { return dist(RandomNumberGenerator::gen_); }
  virtual int trials() { return trials_; }
  virtual int p() { return p_success_; }
};

/// NegativeBinomialIntDist takes the number of successes desired and a
/// probability of success on a single trial and returns the number of trials
/// needed to reach the desired successes. When successes is one, this is
/// equivalent to a Geometric distribution.
class NegativeBinomialIntDist : public IntDistribution {
 private:
  boost::random::negative_binomial_distribution<> dist;
  int successes_;
  double p_success_;

 public:
  typedef boost::shared_ptr<NegativeBinomialIntDist> Ptr;

  NegativeBinomialIntDist(int successes, double p_success)
      : dist(successes, p_success),
        successes_(successes),
        p_success_(p_success) {
    if (successes < 1) {
      throw ValueError("Successes must be positive and greater than zero");
    }
    if (p_success > 1 || p_success < 0) {
      throw ValueError("Probability must be between zero and one");
    }
  };
  virtual int sample() { return dist(RandomNumberGenerator::gen_); }
  virtual int successes() { return successes_; }
  virtual int p() { return p_success_; }
};

/// Poisson distribution requires a mean
class PoissonIntDist : public IntDistribution {
 private:
  boost::random::poisson_distribution<> dist;
  double mean_;

 public:
  typedef boost::shared_ptr<PoissonIntDist> Ptr;

  PoissonIntDist(double mean) : dist(mean_) {
    if (mean_ < 0) {
      throw ValueError("Mean must be positive");
    }
  };
  virtual int sample() { return dist(RandomNumberGenerator::gen_); }
  virtual double mean() { return dist.mean(); }
};

/// Exponential distribution requires lambda
class ExponentialIntDist : public IntDistribution {
 private:
  boost::random::exponential_distribution<> dist;
  double lambda_;

 public:
  typedef boost::shared_ptr<ExponentialIntDist> Ptr;

  ExponentialIntDist(double lambda) : dist(lambda), lambda_(lambda) {
    if (lambda_ < 0) {
      throw ValueError("Lambda must be positive");
    }
  };
  virtual int sample() { return dist(RandomNumberGenerator::gen_); }
  virtual double lambda() { return lambda_; }
};

/// Binary distribution requires twp options and a probability
class BinaryIntDist : public IntDistribution {
 private:
  boost::random::binomial_distribution<int> dist;
  double p_success_;
  int choice_a_, choice_b_;

 public:
  BinaryIntDist(double p_success, int choice_a, int choice_b)
      : dist(1, p_success), choice_a_(choice_a), choice_b_(choice_b) {
    if (p_success < 0) {
      throw ValueError("Probability of choice A must be positive");
    }
  };
  virtual int sample() {
    return dist(RandomNumberGenerator::gen_) ? choice_a_ : choice_b_;
  }
  virtual double p() { return p_success_; }
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_RNG_H
