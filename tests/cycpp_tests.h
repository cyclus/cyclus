#include <string>

// Spies are secret agents, get it?!

class Spy {
public:
  #pragma cyclus var dict(\
    default=7,\
    units="unitless",\
    userlevel=10,\
    tooltip="eg, 007",\
    help="The spy's agent number - TOP SECRET!",\
    )
  int num;
private:
  std::string name;
  double not_var;
};

