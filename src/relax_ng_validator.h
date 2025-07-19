/* relaxngvalidator.h
 * this class is agented off of the schemavalidator in libxml++
 * here is their license statement:
 *
 * libxml++ and this file are copyright (C) 2000 by Ari Johnson,
 * (C) 2002-2004 by the libxml dev team and
 * are covered by the GNU Lesser General Public License, which should be
 * included with libxml++ as the file COPYING.
 */

#ifndef CYCLUS_SRC_RELAX_NG_VALIDATOR_H_
#define CYCLUS_SRC_RELAX_NG_VALIDATOR_H_

#include <glibmm/ustring.h>

class _xmlRelaxNG;
class _xmlRelaxNGParserCtxt;
class _xmlRelaxNGValidCtxt;
typedef _xmlRelaxNG xmlRelaxNG;
typedef _xmlRelaxNGParserCtxt xmlRelaxNGParserCtxt;
typedef _xmlRelaxNGValidCtxt xmlRelaxNGValidCtxt;

namespace xmlpp {
class Document;
}

namespace cyclus {

/// RelaxNGValidator
///
/// This class provides a simple interface to validate xml documents
/// agaisnt a given RelaxNG schema.
class RelaxNGValidator {
 public:
  /// constructor
  RelaxNGValidator();

  /// destructor
  ~RelaxNGValidator();

  /// parse a relaxng schema xml file
  /// @param contents the contents of the xml file
  void parse_memory(const Glib::ustring& contents);

  /// validate an xml file agaisnt the given schema
  /// @param doc the xml file document
  bool Validate(const xmlpp::Document* doc);

 protected:
  /// free xml-related memory
  void release_underlying();

  /// parse a relaxng schema context
  /// @param context the context
  void parse_context(xmlRelaxNGParserCtxt* context);

  /// the schema
  xmlRelaxNG* schema_;

  /// the validated context
  xmlRelaxNGValidCtxt* valid_context_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_RELAX_NG_VALIDATOR_H_
