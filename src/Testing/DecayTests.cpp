
#include <gtest/gtest.h>
#include "Material.h"
#include "Timer.h"

class SubMaterial : public Material {
  public:
    std::vector<int> decay_times;

    virtual void decay() {
      decay_times.push_back(TI->time());
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(DecayTest, DISABLED_GlobalFreq) {
  // all materials are automatically added to a global static list via their
  // constructor. The submaterial object tracks decay invocations and the
  // submaterial object is then checked for appropriate # decay invocations and
  // intervals between them.
  SubMaterial mat;

  int decay_interval = 3;
  int duration = 20;
  TI->initialize(duration, 1, 2010, 0, decay_interval);
  TI->runSim();

  EXPECT_EQ(mat.decay_times.size(), duration / decay_interval);

  int prev_time = mat.decay_times.at(0);
  for (int i = 1; i < mat.decay_times.size(); i++) {
    int time = mat.decay_times.at(i);
    EXPECT_EQ(time - prev_time, decay_interval) 
        << "interval " << i - 1 << " to " << i << " is broken.";
    prev_time = time;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(DecayTest, DISABLED_MaterialDeltaTracking) {
  // Simulation pauses are simulated by doing a runSim repeatedly with
  // different sim durations. At each pause, we decay a material and check to
  // see that the appropriate decay interval is propogated down to the CompMap
  // object.
  int times[5] = {0, 19, 30, 37, 49};

  TI->initialize(times[0], 1, 2010, 0, 0);
  Material mat;

  for (int i = 1; i < 5; i++) {
    int curr_time = times[i];
    TI->initialize(times[i], 1, 2010, 0, 0);
    TI->runSim();
    mat.decay();
    int exp_delta = times[i] - times[i-1];
    EXPECT_EQ(mat.isoVector().comp()->decay_time(), exp_delta);
  }
}

