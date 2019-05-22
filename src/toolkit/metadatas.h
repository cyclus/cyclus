#ifndef CYCLUS_SRC_TOOLKIT_METADATAS_H_
#define CYCLUS_SRC_TOOLKIT_METADATAS_H_

#include <string>
#include <unordered_map>
#include "pyne.h"
//#include "cyclus.h"


namespace cyclus {
class Agent;
namespace toolkit {


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
  
  virtual void LoadData(std::map<std::string, std::string> data);

  // write the metadata in the output table
  virtual void RecordMetadatas(Agent* agent); 

  /// container for arbitrary metadata, following the JSON rules.
  Json::Value metadatas;


};


class UsageMetadatas: public Metadatas {
 public:
  /// The default constructor for Metadatas. 
  UsageMetadatas();

  UsageMetadatas(std::map<std::string, std::map<std::string, double >> datas);
  /// The default destructor for Metadatas
  ~UsageMetadatas() {};

  void LoadData(std::map<std::string, std::map<std::string, double >> data);

  // write the metadata in the output table
  void RecordMetadatas(Agent* agent); 
  // check consistency in the keyword names
  void CheckConstistency();


};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_METADATAS_H_
