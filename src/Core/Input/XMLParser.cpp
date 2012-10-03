// XMLParser.cpp

#include "XMLParser.h"

#include <stdlib.h>
#include <string>
#include "RelaxNGValidator.h"

#include "Logger.h"

using namespace std;
using namespace xmlpp;
using namespace boost;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLParser::XMLParser(std::stringstream& xml_input_snippet, 
                     std::stringstream& xml_schema_snippet) {
  initParser(xml_input_snippet);
  if (xml_schema_snippet.rdbuf()->in_avail() > 0) {
    validateFileAgaisntSchema(xml_schema_snippet);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLParser::XMLParser(std::stringstream& xml_input_snippet) {
  initParser(xml_input_snippet);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLParser::~XMLParser() {
  parser_.reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLParser::validateFileAgaisntSchema(std::stringstream&
                                          xml_schema_snippet) {
  RelaxNGValidator validator;
  validator.parse_memory(xml_schema_snippet.str());
  try {
    validator.validate(this->document());
  } catch(CycValidityException& e) {
    throw CycLoadXMLException(e.what());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLParser::initParser(std::stringstream& xml_input_snippet) {
  parser_ = shared_ptr<DomParser>(new DomParser());
  try {
    
    CLOG(LEV_DEBUG5) << "Parsing the snippet: " << xml_input_snippet.str();

    parser_->parse_memory(xml_input_snippet.str());
    if(!parser_) {
      throw CycLoadXMLException("Could not parse xml file.");
    }
  } catch(const std::exception& ex) {
    throw CycLoadXMLException("Error loading xml file: " + 
                              string(ex.what()));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
xmlpp::Document* XMLParser::document() {
  return parser_->get_document();
}
