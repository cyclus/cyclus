#include <sstream>

#include <boost/shared_ptr.hpp>

#include "error.h"
#include "infile_converters.h"
#include "infile_tree.h"
#include "pyne.h"

namespace cyclus {
namespace toolkit {

void AddJsonToXml(Json::Value& node, std::stringstream& ss,
                  std::string parent_name, std::string indent) {
  using std::string;
  using Json::Value;
  if (node.isObject()) {
    bool indent_child;
    string name;
    string newindent = indent + "  ";
    Value::Members members = node.getMemberNames();
    for (int n = 0; n < members.size(); ++n) {
      name = members[n];
      indent_child = node[name].isObject();
      if (!indent_child && node[name].isArray())
        indent_child = node[name][0].isObject() || node[name][0].isArray();
      ss << indent << "<" << name << ">";
      if (indent_child)
        ss << "\n";
      AddJsonToXml(node[name], ss, name, newindent);
      if (indent_child)
        ss << indent;
      ss << "</" << name << ">\n";
    }
  } else if (node.isArray()){
    bool indent_child;
    std::string newindent = indent;
    indent = indent.substr(0, indent.size() - 2);
    int nchildren = node.size();
    for (int n = 0; n < nchildren; ++n) {
      indent_child = node[n].isObject() || node[n].isArray();
      if (n > 0) {
        ss << indent << "<" << parent_name << ">";
        if (indent_child)
          ss << "\n";
      }
      AddJsonToXml(node[n], ss, parent_name, newindent);
      if (n < nchildren - 1) {
        if (indent_child)
          ss << indent;
        ss << "</" << parent_name << ">\n";
      }
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

  stringstream x;

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

// inserts a value, or appends to an array
void JsonInsertOrAppend(Json::Value& node, std::string key, Json::Value& val) {
  using std::string;
  using Json::Value;
  if (node.isMember(key)) {
    Value keynode = node[key];
    if (keynode.isArray()) {
      keynode.append(val);
    } else {
      keynode = Value(Json::arrayValue);
      keynode.append(node[key]);
      keynode.append(val);
      node[key] = keynode;
    }
  } else {
    node[key] = val;
  }
}

void AddXmlToJson(InfileTree* xnode, Json::Value& jnode,
                  std::string parent_name) {
  using std::string;
  using Json::Value;
  int n = xnode->NElements();
  if (n == 0) {
    Value val(xnode->GetString("."));
    JsonInsertOrAppend(jnode, parent_name, val);
  } else {
    for (int i = 0; i < n; ++i) {
      string name = xnode->GetElementName(i);
      Value val (Json::objectValue);
      AddXmlToJson(xnode->SubTree("*", i), val, name);
      JsonInsertOrAppend(jnode, name, val);
    }
  }
}

// Converts an XML string into an equivalent JSON string
std::string XmlToJson(std::string s) {
  using std::string;
  using std::stringstream;
  using Json::Value;
  stringstream ss (s);
  boost::shared_ptr<XMLParser> parser = boost::shared_ptr<XMLParser>(
                                                            new XMLParser());
  parser->Init(ss);
  InfileTree xroot(*parser);
  Value jroot(Json::objectValue);
  AddXmlToJson(&xroot, jroot, "");
  Json::CustomWriter writer = Json::CustomWriter("{", "}", "[", "]", ": ",
                                                 ", ", " ", 80);
  return writer.write(jroot);
}

}  // namespace toolkit
}  // namespace cyclus
