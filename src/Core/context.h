// context.h
#ifndef CONTEXT_H_
#define CONTEXT_H_

namespace cyclus {

class Context {
 public:
  Context(Timer* ti, RecipeLibrary* rl, EventManager* em);

  void RegisterModel(std::string name, Model* m);

  template <class T>
  T* GetModel(std::string name);

  void RegisterProto(std::string name, Model* m);

  template <class T>
  T* CreateModel(std::string proto_name);

  int time();

  Event* NewEvent();

 private:
  std::map<std::string, Model*> models_;
  std::map<std::string, Model*> templates_;

  Timer* ti_;
  RecipeLibrary* rl_;
  EventManager* em_;

};

} // namespace cyclus

#endif
