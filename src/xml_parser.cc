#include "xml_parser.h"

#include <stdlib.h>
#include <string>
#include <libxml++/libxml++.h>

#include "error.h"
#include "logger.h"
#include "relax_ng_validator.h"

namespace cyclus {

#if LIBXMLXX_MAJOR_VERSION == 2 
  typedef xmlpp::NodeSet NodeSet;
  typedef xmlpp::Node::NodeList const_NodeList;
#else
  typedef xmlpp::Node::NodeSet NodeSet;
  typedef xmlpp::Node::const_NodeList const_NodeList;
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLParser::XMLParser() : parser_(NULL) {
  parser_ = new xmlpp::DomParser();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLParser::~XMLParser() {
  delete parser_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLParser::Init(const std::string& xml_input_snippet) {
  if (parser_ != NULL) {
    delete parser_;
  }
  parser_ = new xmlpp::DomParser();
  try {
    CLOG(LEV_DEBUG5) << "Parsing the snippet: " << xml_input_snippet;

    parser_->parse_memory(xml_input_snippet);
    if (!parser_) {
      throw ValidationError("Could not parse xml file.");
    }
  } catch (const std::exception& ex) {
    throw ValidationError("Error loading xml file: " + std::string(ex.what()));
  }
}

void XMLParser::Init(const std::stringstream& xml_input_snippet) {
  Init(xml_input_snippet.str());
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
  bool generate_xinclude_nodes = true;
  bool fixup_base_uris = false;
  #if LIBXMLXX_MAJOR_VERSION == 2 
  doc->process_xinclude(generate_xinclude_nodes);
  #else
  doc->process_xinclude(generate_xinclude_nodes, fixup_base_uris);
  #endif
  // This removes the stupid xml:base attribute that including adds,
  // but which is unvalidatable. The web is truly cobbled together
  // by a race of evil gnomes.
  xmlpp::Element* root = doc->get_root_node();
  NodeSet have_base = root->find("//*[@xml:base]");
  NodeSet::iterator it = have_base.begin();
  for (; it != have_base.end(); ++it) {
    reinterpret_cast<xmlpp::Element*>(*it)->remove_attribute("base", "xml");
  }
  return doc;
}

}  // namespace cyclus
