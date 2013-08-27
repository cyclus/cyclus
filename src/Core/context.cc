
#include "context.h"


Context::Context(Timer* ti, RecipeLibrary* rl, EventManager* em)
    : ti_(t), rl_(rl), em_(em) { };

void Context::RegisterModel(std::string name, Model* m);

template <class T>
T* Context::GetModel(std::string name);

void Context::RegisterProto(std::string name, Model* m);

template <class T>
T* Context::CreateModel(std::string proto_name);

int Context::time();

Event* Context::NewEvent();
