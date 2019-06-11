#include <math.h>
#include <stdio.h>
#include <sstream>

#include "agent.h"
#include "context.h"
#include "metadatas.h"
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

Metadatas::Metadatas() {}
Metadatas::~Metadatas() {}

void Metadatas::RecordMetadatas(Agent* agent) {
  std::string tblname = "Metadatas";
  Json::Value::Members keys = metadatas.getMemberNames();
  Json::Value::Members::const_iterator ikey = keys.begin();
  Json::Value::Members::const_iterator ikey_end = keys.end();
  for (; ikey != ikey_end; ++ikey) {
    std::string value = "";
    std::string type = "";
    switch (metadatas[*ikey].type()) {
      case Json::nullValue:
        break;
      case Json::stringValue:
        type = "string";
        value = metadatas[*ikey].asString();
        break;
      case Json::booleanValue:
        type = "bool";
        value = metadatas[*ikey].asString();
        break;
      case Json::intValue:
        type = "int";
        value = std::to_string(metadatas[*ikey].asInt());
        break;
      case Json::uintValue:
        type = "uint";
        value = std::to_string(metadatas[*ikey].asInt());
        break;
      case Json::realValue:
        type = "double";
        value = std::to_string(metadatas[*ikey].asDouble());
        break;
      case Json::objectValue:
        ValueError("Type is not convertible to string");
      case Json::arrayValue:
        Json::ObjectValues::iterator it;
        for (it = metadatas[*ikey].begin(); it != metadatas[*ikey].end(); it ++) {
          type = it->first;
          agent->context()
              ->NewDatum(tblname)
              ->AddVal("AgentId", agent->id())
              ->AddVal("keyword", *ikey)
              ->AddVal("Type", it->first)
              ->AddVal("Value", std::to_string(it->second).asDouble())
              ->Record();
        
        }
      default:
        ValueError("Type is not known.");
    }
    if (value != "") {
      agent->context()
          ->NewDatum(tblname)
          ->AddVal("AgentId", agent->id())
          ->AddVal("keyword", *ikey)
          ->AddVal("Type", type)
          ->AddVal("Value", value)
          ->Record();
    }
  }
}

void Metadatas::LoadData(std::map<std::string, std::string> data) {
  for (auto data : datas) {
    std::string keyword = data.first;
    std::string type = data.second.substr(data.second.length() - 2);
    std::string value = data.second.substr(0, data.second.length() - 2);
    if (type.substr(0, 1) == "%") {
    switch (type_map[type.substr(1)]) {
        case s:
          metadatas[keyword] = value;
          break;

        case b:
          if (pyne::to_lower(value) == "true") {
            metadatas[keyword] = true;
          } else if (pyne::to_lower(value) == "false") {
            metadatas[keyword] = false;
          } else {
            metadatas[keyword] = std::stoi(value);
          }
          break;

        case i:
          metadatas[keyword] = std::stoi(value);
          break;

        case u:
          metadatas[keyword] = uint(std::stoul(value));
          break;

        case d:
          metadatas[keyword] = std::stod(value);
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



void Metadatas::LoadUsageData(
    std::map<std::string, std::map<std::string, double>> datas) {
  for (auto keyword_datas : datas) {
    std::string keyword = keyword_datas.first;
    for (auto usage : keyword_datas.second) {
      metadatas[usage.first][keyword] = usage.second;
    }
  }
}

void RecordMetadatas(Agent* agent) {
  std::string tblname = "UsageMetadatas";
  Json::Value::Members keys = metadatas.getMemberNames();
  Json::Value::Members::const_iterator ikey = keys.begin();
  Json::Value::Members::const_iterator ikey_end = keys.end();
  for (; ikey != ikey_end; ++ikey) {
    std::string keyword = (*ikey).substr(3);
    std::string usage_key = "";
    switch (key_map[(*ikey).substr(0, 2)]) {
      case DP:
        usage_key = "deployment";
        break;
      case DC:
        usage_key = "decomision";
        break;
      case TI:
        usage_key = "timestep";
        break;
      case TH:
        usage_key = "throughput";
        break;

      default:
        std::stringstream msg;
        msg << "Bad usage key. \"" << (*ikey).substr(0, 2)
            << "\" is not a know key!";
        throw ValueError(msg.str());
    }
    agent->context()
        ->NewDatum(tblname)
        ->AddVal("AgentId", agent->id())
        ->AddVal("keyword", keyword)
        ->AddVal("Value", metadatas[*ikey].asDouble())
        ->AddVal("Usage", usage_key)
        ->Record();
  }
}

}  // namespace toolkit
}  // namespace cyclus
