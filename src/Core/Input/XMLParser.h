//XMLParser.h
#if !defined(_XMLPARSER_H)
#define _XMLPARSER_H

#include <sstream>
#include <libxml++/libxml++.h>
#include <boost/shared_ptr.hpp>

/**
   A helper class to hold xml file data and provide automatic 
   validation
 */
class XMLParser {
 public:
  /**
     constructor given a schema
     @param xml_input_snippet snippet of xml
     @param xml_schema_snippet snippet of xml schema
   */
  XMLParser(std::stringstream& xml_input_snippet, 
            std::stringstream& xml_schema_snippet);

  /**
     constructor without schema
     @param xml_input_snippet snippet of xml
   */
  XMLParser(std::stringstream& xml_input_snippet);

  /// destructor
  ~XMLParser();

  /**
     @return the parser's document
   */
  xmlpp::Document* document();
  
 private:
  /// file parser
  boost::shared_ptr<xmlpp::DomParser> parser_;

  /**
     initializes parse with an xml snippet
     @param xml_input_snippet the xml snippet
   */
  void initParser(std::stringstream& xml_input_snippet);

  /**
     validates the file agaisnt a schema, if requested
     @param xml_schema_snippet snippet of xml schema
   */
  void validateFileAgaisntSchema(std::stringstream& xml_schema_snippet);
};

#include "CycException.h"
/**
   An exception class for an xpath that can not be evaluated
*/
class CycLoadXMLException : public CycException {
 public: 
 CycLoadXMLException(std::string msg) : CycException(msg) {};
};
#endif
