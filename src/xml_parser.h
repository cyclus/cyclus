#ifndef CYCLUS_SRC_XML_PARSER_H_
#define CYCLUS_SRC_XML_PARSER_H_

#include <sstream>
#include <boost/shared_ptr.hpp>

namespace xmlpp {
  class DomParser;
  class Document;
}

namespace cyclus {

/// A helper class to hold xml file data and provide automatic
/// validation
class XMLParser {
 public:
  /// constructor
  XMLParser();

  /// destructor
  virtual ~XMLParser();

  /// initializes a parser with an xml snippet
  /// @param input an xml snippet to be used as input
  void Init(const std::stringstream& input);
  void Init(const std::string& input);

  /// validates the file agaisnt a schema
  /// @param schema the schema to validate agaisnt
  void Validate(const std::stringstream& schema);

  /// @return the parser's document
  xmlpp::Document* Document();

 private:
  /// file parser
  xmlpp::DomParser* parser_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_XML_PARSER_H_
