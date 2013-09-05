// context.h
#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <map>
#include <string>

#include "composition.h"
#include "event_manager.h"
#include "prototype.h"

namespace cyclus {

class EventManager;
class Event;
class MarketModel;
class Model;
class Timer;
class TimeAgent;

class Context {
 public:
  Context(Timer* ti, EventManager* em);

  boost::uuids::uuid sim_id();

  void RegisterModel(std::string name, Model* m);

  template <class T>
  T* GetModel(std::string name) {
    if (models_.count(name) == 0) {
      throw KeyError("Invalid model name " + name);
    }

    Model* m = models_[name];
    T* casted(NULL);
    casted = dynamic_cast<T*>(m);
    if (casted == NULL) {
      throw CastError("Invalid model cast for model name " + name);
    }
    return casted;
  };

  void RegisterProto(std::string name, Prototype* p);

  template <class T>
  T* CreateModel(std::string proto_name) {
    if (protos_.count(proto_name) == 0) {
      throw KeyError("Invalid prototype name " + proto_name);
    }

    Prototype* p = protos_[proto_name];
    T* casted(NULL);
    casted = dynamic_cast<T*>(p->clone());
    if (casted == NULL) {
      throw CastError("Invalid prototype cast for prototype " + proto_name);
    }
    return casted;
  };

  void RegisterRecipe(std::string name, Composition::Ptr c);
  Composition::Ptr GetRecipe(std::string name);

  void RegisterTicker(TimeAgent* ta);
  void RegisterResolver(MarketModel* mkt);

  void InitTime(int start, int duration, int decay, int m0 = 1, int y0 = 2010,
                std::string handle = "");

  int time();
  int start_time();
  int sim_dur();

  Event* NewEvent(std::string title);

 private:
  std::map<std::string, Model*> models_;
  std::map<std::string, Prototype*> protos_;
  std::map<std::string, Composition::Ptr> recipes_;

  Timer* ti_;
  EventManager* em_;
};

} // namespace cyclus

#endif
