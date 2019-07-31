#include <math.h>
#include <stdio.h>
#include <sstream>

#include "agent.h"
#include "context.h"
#include "metadata.h"
namespace cyclus {
namespace toolkit {

enum type_hash {
  s,  // string
  b,  // boolean
  i,  // int
  u,  // uint
  d   // double
};

std::unordered_map<std::string, int> type_map = {
    std::make_pair("s", s),  // string
    std::make_pair("b", b),  // boolean
    std::make_pair("i", i),  // int
    std::make_pair("u", u),  // uint
    std::make_pair("d", d)   // double
};

const std::list<std::string> usages = std::list<std::string>(
    {"decommission", "deployment", "timestep", "throughput"});

Metadata::Metadata() {}
Metadata::~Metadata() {}

void Metadata::RecordMetadata(Agent* agent) {
  Json::Value::Members keys = metadata.getMemberNames();
  Json::Value::Members::const_iterator ikey = keys.begin();
  Json::Value::Members::const_iterator ikey_end = keys.end();
  for (; ikey != ikey_end; ++ikey) {
    std::string value = "";
    std::string type = "";
    switch (metadata[*ikey].type()) {
      case Json::nullValue:
        break;
      case Json::stringValue:
        type = "string";
        value = metadata[*ikey].asString();
        break;
      case Json::booleanValue:
        type = "bool";
        value = metadata[*ikey].asString();
        break;
      case Json::intValue:
        type = "int";
        value = std::to_string(metadata[*ikey].asInt());
        break;
      case Json::uintValue:
        type = "uint";
        value = std::to_string(metadata[*ikey].asInt());
        break;
      case Json::realValue:
        type = "double";
        value = std::to_string(metadata[*ikey].asDouble());
        break;
      case Json::objectValue:
        ValueError("Type is not convertible to string");
      case Json::arrayValue:
        for (auto usage : usages) {
          if (metadata[*ikey].isMember(usage)) {
            agent->context()
                ->NewDatum("Metadata")
                ->AddVal("AgentId", agent->id())
                ->AddVal("keyword", *ikey)
                ->AddVal("Type", usage)
                ->AddVal("Value",
                         std::to_string(metadata[*ikey][usage].asDouble()))
                ->Record();
          }
        }
      default:
        ValueError("Type is not known.");
    }
    if (value != "") {
      agent->context()
          ->NewDatum("Metadata")
          ->AddVal("AgentId", agent->id())
          ->AddVal("keyword", *ikey)
          ->AddVal("Type", type)
          ->AddVal("Value", value)
          ->Record();
    }
  }
}

void Metadata::SetWorkLabel(std::string work_label) {
  std::map<std::string, std::string> work_map = {{"WORKLABEL", work_label}};
  LoadData(work_map);
}

std::string Metadata::GetWorkLabel() {
  return metadata["WORKLABEL"].asString();
}

void Metadata::LoadData(std::map<std::string, std::string> data) {
  for (auto data_elt : data) {
    std::string keyword = data_elt.first;
    std::string type = data_elt.second.substr(data_elt.second.length() - 2);
    std::string value = data_elt.second.substr(0, data_elt.second.length() - 2);
    if (type.substr(0, 1) == "%") {
      switch (type_map[type.substr(1)]) {
        case s:
          metadata[keyword] = value;
          break;

        case b:
          if (pyne::to_lower(value) == "true") {
            metadata[keyword] = true;
          } else if (pyne::to_lower(value) == "false") {
            metadata[keyword] = false;
          } else {
            metadata[keyword] = std::stoi(value);
          }
          break;

        case i:
          metadata[keyword] = std::stoi(value);
          break;

        case u:
          metadata[keyword] = uint(std::stoul(value));
          break;

        case d:
          metadata[keyword] = std::stod(value);
          break;

        default:
          std::stringstream msg;
          msg << "Allowed usage keywords are:"
              << " deployment, decommission, timestep, and throughput";
          throw ValueError(msg.str());
      }
    } else {
      ValueError("type encoding not recognized");
    }
  }
}

void Metadata::LoadData(
    std::map<std::string, std::map<std::string, double>> datas) {
  for (auto keyword_datas : datas) {
    std::string keyword = keyword_datas.first;
    for (auto usage : keyword_datas.second) {
      metadata[keyword][usage.first] = usage.second;
      if (std::find(usages.begin(), usages.end(), usage.first) ==
          usages.end()) {
        std::stringstream ss;
        ss << "Usage " << usage.first << "is unknown... "
           << "The value corresponding to the usage keyword, "
           << "will not be written in the output file. "
           << "Known usage keywords are: "
           << "deployment, decommission, timestep, and throughput.";
        cyclus::Warn<cyclus::VALUE_WARNING>(ss.str());
      }
    }
  }
}
}  // namespace toolkit
}  // namespace cyclus
