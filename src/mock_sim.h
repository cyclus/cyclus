#ifndef CYCLUS_SRC_MOCK_SIM_H_
#define CYCLUS_SRC_MOCK_SIM_H_

#include "cyclus.h"
#include "sqlite_back.h"
#include "timer.h"

namespace cyclus {

class Source;
class Sink;

class MockAgent {
  public:
    MockAgent(Context* ctx, Recorder* rec, SqliteBack* b, bool is_source);

    MockAgent commod(std::string commod);
    MockAgent recipe(std::string recipe);
    MockAgent capacity(double cap);
    MockAgent start(int timestep);
    MockAgent lifetime(int duration);
    // returns the name of the created prototype
    std::string Finalize();
   
  private:
    static int nextid_;
    bool source_;
    std::string commod_;
    std::string recipe_;
    double cap_;
    int start_;
    int lifetime_;
    std::string proto_;
    Context* ctx_;
    Recorder* rec_;
    SqliteBack* back_;
};

class MockSim {
 public:
  MockSim(AgentSpec spec, std::string config, int duration);

  MockAgent AddSource(std::string commod);
  MockAgent AddSink(std::string commod);
  void AddRecipe(std::string name, Composition::Ptr c);
  void Run();
  SqliteBack& db();
  
 private:
  Context ctx_;
  Timer ti_;
  Recorder rec_;
  SqliteBack* back_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_MOCK_SIM_H_
