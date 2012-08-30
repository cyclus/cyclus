//RelaxNGValidator.h

/* relaxngvalidator.h
 * this class is modeled off of the schemavalidator in libxml++
 * here is their license statement:
 * 
 * libxml++ and this file are copyright (C) 2000 by Ari Johnson,
 * (C) 2002-2004 by the libxml dev team and
 * are covered by the GNU Lesser General Public License, which should be
 * included with libxml++ as the file COPYING.
 */

#ifndef _RELAXNGVALIDATOR_H
#define _RELAXNGVALIDATOR_H

#include <libxml/relaxng.h>
#include <libxml++/document.h>
#include <glibmm/ustring.h>

/** 
    Schema Validator
 */
class RelaxNGValidator {
 public:
  RelaxNGValidator();
  ~RelaxNGValidator();
  
  void parse_memory(const Glib::ustring& contents);

  bool validate(const xmlpp::Document* doc);

 protected:
  void release_underlying();
  void parse_context(xmlRelaxNGParserCtxtPtr context);
  
  xmlRelaxNGPtr schema_;
  xmlRelaxNGValidCtxtPtr valid_context_;
};

#include "CycException.h"
/**
   Exception helper classes
 */
class CycValidityException : public CycException {
 public: 
 CycValidityException(std::string msg) : CycException(msg) {};
};
#endif
