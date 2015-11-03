#include "xml_parser.h"

#include <stdlib.h>
#include <string>
#include <libxml++/libxml++.h>

#include "error.h"
#include "logger.h"
#include "relax_ng_validator.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLParser::XMLParser() : parser_(NULL) {
  parser_ = new xmlpp::DomParser();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLParser::~XMLParser() {
  delete parser_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLParser::Init(const std::stringstream& xml_input_snippet) {
  if (parser_ != NULL) {
    delete parser_;
  }
  parser_ = new xmlpp::DomParser();
  try {
    CLOG(LEV_DEBUG5) << "Parsing the snippet: " << xml_input_snippet.str();

    parser_->parse_memory(xml_input_snippet.str());
    if (!parser_) {
      throw ValidationError("Could not parse xml file.");
    }
  } catch (const std::exception& ex) {
    throw ValidationError("Error loading xml file: " + std::string(ex.what()));
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
  // This adds the capability to have nice include semantics
  doc->process_xinclude();
  // This removes the stupid xml:base attribute that including adds,
  // but which is unvalidatable. The web is truly cobbled together
  // by a race of evil gnomes.
  xmlpp::Element* root = doc->get_root_node();
  xmlpp::NodeSet have_base = root->find("//*[@xml:base]");
  xmlpp::NodeSet::iterator it = have_base.begin();
  for (; it != have_base.end(); ++it) {
    reinterpret_cast<xmlpp::Element*>(*it)->remove_attribute("base", "xml");
  }
  return doc;
}

}  // namespace cyclus
