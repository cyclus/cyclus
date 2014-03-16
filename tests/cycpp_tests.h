#include <string>

// Spies are secret agents, get it?!

#pragma cyclus exec x = 5; import time
#pragma cyclus exec import math

namespace jolly_old_england {};

namespace mi6 {

using namespace std;

namespace joe = jolly_old_england;

typedef double caliber_t;
typedef caliber_t cbt;

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

  //using std::string;
  using namespace std;

  #pragma cyclus var dict(\
    default="James Bond, 00" + str(num['default']),\
    units="unitless",\
    userlevel=x,\
    tooltip="eg, James Bond",\
    help="time {0}".format(time.time()),\
    )
  string name;
  //std::string name;
  //char * name;

  #pragma cyclus var {\
    "default": 0.22,\
    "units": "unitless",\
    "userlevel": 99,\
    "tooltip": "weapon caliber",\
    "help": "favored weapon size",\
    }
  cbt caliber;
private:
  double not_var;
};

}; // namespace mi6

using std::string;

typedef unsigned long ulong;
typedef int int_t, *intp_t, (&fp)(int, ulong), arr_t[10];
typedef struct {int a; int b;} S, *pS;
