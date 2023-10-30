#include "random_number_generator.h"

#include <iostream>
#include <string>

#include "logger.h"
#include "sim_init.h"
#include "context.h"

namespace cyclus{

    void RandomNumberGenerator::Initialize(SimInfo si){

        Generator gen_(si.seed);

        CLOG(LEV_INFO1) << "Pseudo random number generator initialized with seed: " << si.seed;
    }

    std::uint32_t RandomNumberGenerator::random(){
        return gen_();
    }

    double RandomNumberGenerator::random_01(){
        boost::uniform_01<> dist;
        return dist(gen_);
    }
    
    int RandomNumberGenerator::random_uniform_int(int low, int high){
        boost::uniform_int<> dist(low, high);
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rn(gen_, dist);
        return rn();
    }

    double RandomNumberGenerator::random_uniform_real(double low, double high){
        boost::uniform_real<> dist(low, high);
        boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rn(gen_, dist);
        return rn();
    }

    double RandomNumberGenerator::random_normal_real(double mean, double std_dev, double low, double high){
        boost::normal_distribution<> dist(mean, std_dev);
        boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > rn(gen_, dist);
        double val = rn();
        while (val < low || val > high){
            val = rn();
        }
        return val;
    }

    int RandomNumberGenerator::random_normal_int(double mean, double std_dev, int low, int high){
        boost::normal_distribution<> dist(mean, std_dev);
        boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > rn(gen_, dist);
        double val = rn();
        while (val < low || val > high){
            val = rn();
        }
        int rounded_val = std::lrint(val);
        return val;
    }

}