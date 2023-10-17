#include "random_number_generator.h"

#include <iostream>
#include <string>
#include <boost/random/mersenne_twister.hpp>

#include "logger.h"
#include "sim_init.h"
#include "context.h"

namespace cyclus{

    void RandomNumberGenerator::Initialize(Context* ctx, SimInfo si){

        boost::mt19937 gen_(si.seed);

        CLOG(LEV_INFO1) << "Pseudo random number generator initialized with seed: " << si.seed;
    }

    int RandomNumberGenerator::randomnumber(){
        return gen_();

}

}