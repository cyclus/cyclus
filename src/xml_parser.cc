// xml_parser.cc
#include "xml_parser.h"

#include <stdlib.h>
#include <string>

#include "error.h"
#include "logger.h"
#include "relax_ng_validator.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLParser::XMLParser() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLParser::~XMLParser() {
  parser_.reset();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLParser::Init(const std::stringstream& xml_input_snippet) {
  parser_ = boost::shared_ptr<xmlpp::DomParser>(new xmlpp::DomParser());
  try {
    CLOG(LEV_DEBUG5) << "Parsing the snippet: " << xml_input_snippet.str();

    parser_->parse_memory(xml_input_snippet.str());
    if (!parser_) {
      throw ValidationError("Could not parse xml file.");
    }
  } catch (const std::exception& ex) {
    throw ValidationError("Error loading xml file: " +
                          std::string(ex.what()));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLParser::Validate(const std::stringstream& xml_schema_snippet) {
  RelaxNGValidator validator;
  validator.parse_memory(xml_schema_snippet.str());
  validator.Validate(this->Document());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
xmlpp::Document* XMLParser::Document() {
  xmlpp::Document* doc = parser_->get_document();
  doc->process_xinclude();
  return doc;
}
}  // namespace cyclus

