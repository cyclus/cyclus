// XMLQueryEngineTests.h
#include <gtest/gtest.h>

#include <string>
#include <sstream>

#include "XMLQueryEngine.h"
#include "XMLParser.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class XMLQueryEngineTest : public ::testing::Test {

 protected:

  XMLQueryEngine *xqe;
  std::string root_node_, content_node_, inner_node_, unknown_node_;
  std::string content_;
  void getContent(std::stringstream &ss);
  std::string subcontent();
  std::string unknowncontent();
  int ncontent_;
  int ninner_nodes_;
  XMLParser* parser_;
  void loadParser();
 public:
  virtual void SetUp();
  virtual void TearDown();

}; 
