#include <sstream>

#include "infile_converters.h"
#include "pyne.h"

namespace cyclus {
namespace toolkit {

void AddJsonToXml(Json::Value& node, std::stringstream& ss,
                  std::string parent_name, std::string indent) {
  using std::string;
  using Json::Value;
  if (node.isObject()) {
    string name;
    string newindent = indent + "  ";
    Value::Members members = node.getMemberNames();
    for (int n = 0; n < members.size(); ++n) {
      name = members[n];
      ss << indent << "<" << name << ">";
      AddJsonToXml(node[name], ss, name, newindent);
      ss << "</" << name << ">\n";
    }
  } else if (node.isArray()){
    std::string newindent = indent;
    indent = indent.substr(0, indent.size() - 2);
    int nchildren = node.size();
    for (int n = 0; n < nchildren; ++n) {
      name = members[n];
      if (n > 0)
        ss << indent << "<" << parent_name << ">";
      AddJsonToXml(node[n], ss, parent_name, newindent);
      if (n < nchildren - 1)
        ss << "</" << parent_name << ">\n";
    }
  } else {
    // plain old data
    ss << node.asString();
  }
}

// Converts a JSON string into an equivalent XML string
std::string JsonToXml(std::string s) {
  using std::string;
  using std::stringstream;
  using Json::Value;

  stringstring x;

  // parse the JSON string
  Value root;
  Json::Reader reader;
  bool parsed = reader.parse(s, root, false);
  if (!parsed) {
    string msg = "Failed to parse JSON file into XML:\n" + \
                 reader.getFormattedErrorMessages();
    throw ValidationError(msg);
  }

  // convert to XML
  AddJsonToXml(root, x, "", "");
  return x.str();
}

// Converts an XML string into an equivalent JSON string
std::string XmlToJson(std::string s) {
  using std::string;
  using std::stringstream;
  stringstream js;
  return js.str();
}

}  // namespace toolkit
}  // namespace cyclus
