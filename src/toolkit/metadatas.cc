#include <math.h>
#include <stdio.h>
#include <sstream>

#include "metadatas.h"
#include "agent.h"
#include "context.h"
namespace cyclus {
namespace toolkit {


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
      case Json::booleanValue:
        type = "bool";
        value = metadatas[*ikey].asString();
      case Json::intValue:
        type = "int";
        value = std::to_string(metadatas[*ikey].asInt());
      case Json::uintValue:
        type = "uint";
        value = std::to_string(metadatas[*ikey].asInt());
      case Json::realValue:
        type = "double";
        value = std::to_string(metadatas[*ikey].asDouble());
      case Json::arrayValue:
      case Json::objectValue:
        ValueError("Type is not convertible to string");
      default:
        ValueError("Type is not known.");
    }
    if (value != "") {
      agent->context()
          ->NewDatum(tblname)
          ->AddVal("AgentId", agent->id())
          ->AddVal("keyword", *ikey)
          ->AddVal("Value", value)
          ->AddVal("Type", type)
          ->Record();
    }
  }
}


enum key_hash {
  DP, deployment = DP,
  DC, decomision = DC,
  TI, timestep = TI,
  TH, throughput = TH
};

std::unordered_map<std::string, int> key_map = {
  std::make_pair("DP", DP), std::make_pair("deployment", DP),
  std::make_pair("DC", DC), std::make_pair("decomission", DC),
  std::make_pair("TI", TI), std::make_pair("timestep", TI),
  std::make_pair("TH", TH), std::make_pair("throughput", TH)
};

UsageMetadatas::UsageMetadatas() {}
UsageMetadatas::UsageMetadatas(std::map<std::string, std::map<std::string, double >> datas){
  LoadUsageMetadatas(datas);
}
UsageMetadatas::~UsageMetadatas() {}

void UsageMetadatas::LoadUsageMetadatas(
    std::map<std::string, std::map<std::string, double>> datas) {
  for (auto keyword_datas : datas) {
    std::string keyword = keyword_datas.first;
    std::string meta_key = "";
    for (auto usage : keyword_datas.second) {
      switch (key_map[usage.first]) {
        case DP:
          meta_key = "DP_" + keyword;
          metadatas[meta_key] = usage.second;
          break;

        case DC:
          meta_key = "DC_" + keyword;
          metadatas[meta_key] = usage.second;
          break;

        case TI:
          meta_key = "TI_" + keyword;
          metadatas[meta_key] = usage.second;
          break;

        case TH:
          meta_key = "TH_" + keyword;
          metadatas[meta_key] = usage.second;
          break;

        default:
          std::stringstream msg;
          msg << "Allowed Usage Keyw are:"
              << " deploymemt, decomision, timestep and throughput";
          throw ValueError(msg.str());
      }
    }
  }
}

void UsageMetadatas::RecordMetadatas(Agent* agent) {
  std::string tblname = "UsageMetadatas";
  Json::Value::Members keys = metadatas.getMemberNames();
  Json::Value::Members::const_iterator ikey = keys.begin();
  Json::Value::Members::const_iterator ikey_end = keys.end();
  for (; ikey != ikey_end; ++ikey) {
    std::string keyword = (*ikey).substr(3);
    std::string usage_key = "";
    switch(key_map[(*ikey).substr(0,2)]) {
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
