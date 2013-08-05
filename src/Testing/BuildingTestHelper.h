#include <string>

#include "Builder.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BuildingTestHelper {
 public:
  /// constructor
  BuildingTestHelper();

  /// destructor
  ~BuildingTestHelper();
  
  /// first builder
  cyclus::action_building::Builder* builder1;

  /// second builder
  cyclus::action_building::Builder* builder2;

};
