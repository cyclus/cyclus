//XMLParser.h
#ifndef _XMLPARSER_H
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
  /// constructor
  XMLParser();

  /// destructor
  ~XMLParser();

  /**
     initializes a parser with an xml snippet
     @param input an xml snippet to be used as input
   */
  void init(std::stringstream& input);

  /**
     @return the parser's document
   */
  xmlpp::Document* document();

  /**
     validates the file agaisnt a schema
     @param schema the schema to validate agaisnt
   */
  void validate(std::stringstream& schema);
  
 private:
  /// file parser
  boost::shared_ptr<xmlpp::DomParser> parser_;
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
