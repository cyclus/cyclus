// XMLParser.cpp


#include <stdlib.h>
#include <string>
#include "XMLParser.h"
#include "RelaxNGValidator.h"

#include "Logger.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLParser::XMLParser() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLParser::~XMLParser() {
  parser_.reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLParser::init(const std::stringstream& xml_input_snippet) {
  parser_ = boost::shared_ptr<xmlpp::DomParser>(new xmlpp::DomParser());
  try {    
    CLOG(LEV_DEBUG5) << "Parsing the snippet: " << xml_input_snippet.str();

    parser_->parse_memory(xml_input_snippet.str());
    if(!parser_) {
      throw CycLoadXMLException("Could not parse xml file.");
    }
  } catch(const std::exception& ex) {
    throw CycLoadXMLException("Error loading xml file: " + 
                              std::string(ex.what()));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLParser::validate(const std::stringstream& xml_schema_snippet) {
  RelaxNGValidator validator;
  validator.parse_memory(xml_schema_snippet.str());
  try {
    validator.validate(this->document());
  } catch(CycValidityException& e) {
    throw CycLoadXMLException(e.what());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
xmlpp::Document* XMLParser::document() {
  return parser_->get_document();
}
} // namespace cyclus

