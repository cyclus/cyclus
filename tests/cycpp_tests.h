#include <string>

// Spies are secret agents, get it?!

#pragma cyclus exec x = 5; import time
#pragma cyclus exec import math

namespace jolly_old_england {};

namespace mi6 {

using namespace std;

namespace joe = jolly_old_england;

class Spy {
 public:
  #pragma cyclus var dict(\
    default=7,\
    units="unitless",\
    userlevel=10,\
    tooltip="eg, 007",\
    help="The spy's agent number - TOP SECRET!",\
    )
  int 
num;

  #pragma cyclus var dict(\
    default="James Bond, 00" + str(num['default']),\
    units="unitless",\
    userlevel=x,\
    tooltip="eg, James Bond",\
    help="time {0}".format(time.time()),\
    )
  std::string name;
private:
  double not_var;
};

}; // namespace mi6

using std::string;

typedef unsigned long ulong;
typedef int int_t, *intp_t, (&fp)(int, ulong), arr_t[10];
typedef struct {int a; int b;} S, *pS;
