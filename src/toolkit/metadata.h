#ifndef CYCLUS_SRC_TOOLKIT_METADATAS_H_
#define CYCLUS_SRC_TOOLKIT_METADATAS_H_

#include <string>
#include <unordered_map>
#include "pyne.h"
//#include "cyclus.h"


namespace cyclus {
class Agent;
namespace toolkit {


/// @class Metadata
///
/// Brief Description of :
///

class Metadata {
 public:
  /// The default constructor for Metadata. 
  Metadata();

  /// The default destructor for Metadata
  ~Metadata();
  
  virtual void LoadData(std::map<std::string, std::string> data);

  // write the metadata in the output table
  virtual void RecordMetadata(Agent* agent); 

  /// container for arbitrary metadata, following the JSON rules.
  Json::Value metadata;

  void LoadUsageData(std::map<std::string, std::map<std::string, double >> data);
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_METADATAS_H_
