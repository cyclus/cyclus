#ifndef CYCLUS_SRC_TOOLKIT_INFILE_CONVERTERS_H_
#define CYCLUS_SRC_TOOLKIT_INFILE_CONVERTERS_H_

#include <string>

namespace cyclus {
namespace toolkit {

/// Converts a JSON string into an equivalent XML string
std::string JsonToXml(std::string s);

/// Converts an XML string into an equivalent JSON string
std::string XmlToJson(std::string s);

// PyToJson declared in pyhooks.h
// JsonToPy declared in pyhooks.h

/// Converts a Python string into an equivalent XML string
std::string PyToXml(std::string s);

/// Converts an XML string into an equivalent JSON string
std::string XmlToPy(std::string s);

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_INFILE_CONVERTERS_H_
