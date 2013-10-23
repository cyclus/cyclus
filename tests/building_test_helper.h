#include <string>

#include "builder.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BuildingTestHelper {
 public:
  /// constructor
  BuildingTestHelper();

  /// destructor
  ~BuildingTestHelper();
  
  /// first builder
  cyclus::Builder* builder1;

  /// second builder
  cyclus::Builder* builder2;

};
