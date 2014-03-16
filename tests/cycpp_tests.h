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
  #pragma cyclus var dict(\
    default="James Bond, 00" + str(num['default']),\
    units="unitless",\
    userlevel=10,\
    tooltip="eg, James Bond",\
    help="The spy's name",\
    )
  std::string name;
private:
  double not_var;
};

