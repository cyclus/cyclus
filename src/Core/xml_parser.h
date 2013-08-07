//xml_parser.h
#ifndef _XMLPARSER_H
#define _XMLPARSER_H

#include <sstream>
#include <libxml++/libxml++.h>
#include <boost/shared_ptr.hpp>

namespace cyclus {

/**
   A helper class to hold xml file data and provide automatic
   validation
 */
class XMLParser {
 public:
  /// constructor
  XMLParser();

  /// destructor
  ~XMLParser();

  /**
     initializes a parser with an xml snippet
     @param input an xml snippet to be used as input
   */
  void Init(const std::stringstream& input);

  /**
     validates the file agaisnt a schema
     @param schema the schema to validate agaisnt
   */
  void Validate(const std::stringstream& schema);

  /**
     @return the parser's document
   */
  xmlpp::Document* Document();

 private:
  /// file parser
  boost::shared_ptr<xmlpp::DomParser> parser_;
};
} // namespace cyclus

#endif
