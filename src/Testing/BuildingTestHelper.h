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
  cyclus::ActionBuilding::Builder* builder1;

  /// second builder
  cyclus::ActionBuilding::Builder* builder2;

};
