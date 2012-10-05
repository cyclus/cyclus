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
  ActionBuilding::Builder* builder1;

  /// second builder
  ActionBuilding::Builder* builder2;

};
