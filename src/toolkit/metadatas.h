#ifndef CYCLUS_SRC_TOOLKIT_METADATAS_H_
#define CYCLUS_SRC_TOOLKIT_METADATAS_H_

#include <string>
#include <unordered_map>
#include "pyne.h"
#include "cyclus.h"

namespace cyclus {
namespace toolkit {

enum key_hash {
  DP, deployment = DP,
  DC, decomision = DC,
  TI, timestep = TI,
  TH, throughput = TH
};

std::unordered_map<std::string, int> key_map = {
  std::make_pair("DP", DP), std::make_pair("deployment", DP),
  std::make_pair("DC", DC), std::make_pair("decomission", DC),
  std::make_pair("TI", TI), std::make_pair("timestep", TI),
  std::make_pair("TH", TH), std::make_pair("throughput", TH)
};


/// @class Metadatas
///
/// Brief Description of :
///

class Metadatas {
 public:
  /// The default constructor for Metadatas. 
  Metadatas();

  /// The default destructor for Metadatas
  ~Metadatas();

  // write the metadata in the output table
  virtual void RecordMetadatas(cyclus::Agent* agent); 

  /// container for arbitrary metadata, following the JSON rules.
  Json::Value metadatas;


};


class UsageMetadatas: public Metadatas {
 public:
  /// The default constructor for Metadatas. 
  UsageMetadatas();

  /// The default destructor for Metadatas
  ~UsageMetadatas();

  void LoadUsageMetadatas(std::map<std::string, std::map<std::string, float >>);

  // write the metadata in the output table
  void RecordMetadatas(cyclus::Agent* agent); 
  // check consistency in the keyword names
  void CheckConstistency();


};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_METADATAS_H_
