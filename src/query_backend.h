#ifndef CYCLUS_SRC_QUERY_BACKEND_H_
#define CYCLUS_SRC_QUERY_BACKEND_H_

#include <climits>
#include <list>
#include <map>
#include <set>

#include <boost/uuid/sha1.hpp>

#include "blob.h"
#include "rec_backend.h"
#include "any.hpp"

#define CYCLUS_UUID_SIZE 16
#define CYCLUS_SHA1_SIZE 20
#define CYCLUS_SHA1_NINT 5

namespace cyclus {

/// This is the master list of all supported database types.  All types must
/// have a constant length unless they begin with the prefix VL_, which stands
/// for "variable length" or are implicitly variable length, such as blob.
/// Changing the order here may invalidate previously created databases.
/// Thus only append to this enum if it is post-1.0.
enum DbTypes {
  // primitive types
  BOOL = 0,  // ["bool", 0, ["HDF5", "SQLite"], "BOOL", false]
  INT,  // ["int",  0,  ["HDF5", "SQLite"],  "INT",  false]
  FLOAT,  // ["float", 0, ["HDF5", "SQLite"], "FLOAT", false]
  DOUBLE,  // ["double", 0, ["HDF5", "SQLite"], "DOUBLE", false]
  STRING,  // ["std::string", 1, ["HDF5", "SQLite"], "STRING", false]
  VL_STRING,  // ["std::string", 1, ["HDF5", "SQLite"], "VL_STRING", true]
  BLOB,  // ["cyclus::Blob", 0, ["HDF5", "SQLite"], "BLOB", true]
  UUID,  // ["boost::uuids::uuid", 0, ["HDF5", "SQLite"], "UUID", false]
  // vector types
  VECTOR_BOOL,  // ["std::vector<bool>", 1, [], ["VECTOR", "BOOL"], false]
  VL_VECTOR_BOOL,  // ["std::vector<bool>", 1, [], ["VL_VECTOR", "BOOL"], true]
  VECTOR_INT,  // ["std::vector<int>", 1, ["HDF5", "SQLite"], ["VECTOR", "INT"], false]
  VL_VECTOR_INT,  // ["std::vector<int>", 1, ["HDF5", "SQLite"], ["VL_VECTOR", "INT"], true]
  VECTOR_FLOAT,  // ["std::vector<float>", 1, ["HDF5"], ["VECTOR", "FLOAT"], false]
  VL_VECTOR_FLOAT,  // ["std::vector<float>", 1, ["HDF5"], ["VL_VECTOR", "FLOAT"], true]
  VECTOR_DOUBLE,  // ["std::vector<double>", 1, ["HDF5", "SQLite"], ["VECTOR", "DOUBLE"], false]
  VL_VECTOR_DOUBLE,  // ["std::vector<double>", 1, ["HDF5", "SQLite"], ["VL_VECTOR", "DOUBLE"], true]
  VECTOR_STRING,  // ["std::vector<std::string>", 2, ["HDF5", "SQLite"], ["VECTOR", "STRING"], false]
  VL_VECTOR_STRING,  // ["std::vector<std::string>", 2, ["HDF5", "SQLite"], ["VL_VECTOR", "STRING"], true]
  VECTOR_VL_STRING,  // ["std::vector<std::string>", 2, ["HDF5", "SQLite"], ["VECTOR", "VL_STRING"], false]
  VL_VECTOR_VL_STRING,  // ["std::vector<std::string>", 2, ["HDF5", "SQLite"], ["VL_VECTOR", "VL_STRING"], true]
  VECTOR_BLOB,  // ["std::vector<cyclus::Blob>", 1, ["HDF5"], ["VECTOR", "BLOB"], false]
  VL_VECTOR_BLOB,  // ["std::vector<cyclus::Blob>", 1, ["HDF5"], ["VL_VECTOR", "BLOB"], true]
  VECTOR_UUID,  // ["std::vector<boost::uuids::uuid>", 1, ["HDF5"], ["VECTOR", "UUID"], false]
  VL_VECTOR_UUID,  // ["std::vector<boost::uuids::uuid>", 1, ["HDF5"], ["VL_VECTOR", "UUID"], true]
  // set types
  SET_BOOL,  // ["std::set<bool>", 1, [], ["SET", "BOOL"], false]
  VL_SET_BOOL,  // ["std::set<bool>", 1, [], ["VL_SET", "BOOL"], true]
  SET_INT,  // ["std::set<int>", 1, ["HDF5", "SQLite"], ["SET", "INT"], false]
  VL_SET_INT,  // ["std::set<int>", 1, ["HDF5", "SQLite"], ["VL_SET", "INT"], true]
  SET_FLOAT,  // ["std::set<float>", 1, ["HDF5"], ["SET", "FLOAT"], false]
  VL_SET_FLOAT,  // ["std::set<float>", 1, ["HDF5"], ["VL_SET", "FLOAT"], true]
  SET_DOUBLE,  // ["std::set<double>", 1, ["HDF5"], ["SET", "DOUBLE"], false]
  VL_SET_DOUBLE,  // ["std::set<double>", 1, ["HDF5"], ["VL_SET", "DOUBLE"], true]
  SET_STRING,  // ["std::set<std::string>", 2, ["HDF5", "SQLite"], ["SET", "STRING"], false]
  VL_SET_STRING,  // ["std::set<std::string>", 2, ["HDF5", "SQLite"], ["VL_SET", "STRING"], true]
  SET_VL_STRING,  // ["std::set<std::string>", 2, ["HDF5", "SQLite"], ["SET", "VL_STRING"], false]
  VL_SET_VL_STRING,  // ["std::set<std::string>", 2, ["HDF5", "SQLite"], ["VL_SET", "VL_STRING"], true]
  SET_BLOB,  // ["std::set<cyclus::Blob>", 1, ["HDF5"], ["SET", "BLOB"], false]
  VL_SET_BLOB,  // ["std::set<cyclus::Blob>", 1, ["HDF5"], ["VL_SET", "BLOB"], true]
  SET_UUID,  // ["std::set<boost::uuids::uuid>", 1, ["HDF5"], ["SET", "UUID"], false]
  VL_SET_UUID,  // ["std::set<boost::uuids::uuid>", 1, ["HDF5"], ["VL_SET", "UUID"], true]
  // list types
  LIST_BOOL,  // ["std::list<bool>", 1, ["HDF5"], ["LIST", "BOOL"], false]
  VL_LIST_BOOL,  // ["std::list<bool>", 1, ["HDF5"], ["VL_LIST", "BOOL"], true]
  LIST_INT,  // ["std::list<int>", 1, ["HDF5", "SQLite"], ["LIST", "INT"], false]
  VL_LIST_INT,  // ["std::list<int>", 1, ["HDF5", "SQLite"], ["VL_LIST", "INT"], true]
  LIST_FLOAT,  // ["std::list<float>", 1, ["HDF5"], ["LIST", "FLOAT"], false]
  VL_LIST_FLOAT,  // ["std::list<float>", 1, ["HDF5"], ["VL_LIST", "FLOAT"], true]
  LIST_DOUBLE,  // ["std::list<double>", 1, ["HDF5"], ["LIST", "DOUBLE"], false]
  VL_LIST_DOUBLE,  // ["std::list<double>", 1, ["HDF5"], ["VL_LIST", "DOUBLE"], true]
  LIST_STRING,  // ["std::list<std::string>", 2, ["HDF5", "SQLite"], ["LIST", "STRING"], false]
  VL_LIST_STRING,  // ["std::list<std::string>", 2, ["HDF5", "SQLite"], ["VL_LIST", "STRING"], true]
  LIST_VL_STRING,  // ["std::list<std::string>", 2, ["HDF5", "SQLite"], ["LIST", "VL_STRING"], false]
  VL_LIST_VL_STRING,  // ["std::list<std::string>", 2, ["HDF5", "SQLite"], ["VL_LIST", "VL_STRING"], true]
  LIST_BLOB,  // ["std::list<cyclus::Blob>", 1, ["HDF5"], ["LIST", "BLOB"], false]
  VL_LIST_BLOB,  // ["std::list<cyclus::Blob>", 1, ["HDF5"], ["VL_LIST", "BLOB"], true]
  LIST_UUID,  // ["std::list<boost::uuids::uuid>", 1, ["HDF5"], ["LIST", "UUID"], false]
  VL_LIST_UUID,  // ["std::list<boost::uuids::uuid>", 1, ["HDF5"], ["VL_LIST", "UUID"], true]
  // pairs - not variable length
  PAIR_INT_BOOL,  // ["std::pair<int, bool>", 0, ["HDF5"], ["PAIR", "INT", "BOOL"], false]
  PAIR_INT_INT,  // ["std::pair<int, int>", 0, ["HDF5"], ["PAIR", "INT", "INT"], false]
  PAIR_INT_FLOAT,  // ["std::pair<int, float>", 0, ["HDF5"], ["PAIR", "INT", "FLOAT"], false]
  PAIR_INT_DOUBLE,  // ["std::pair<int, double>", 0, ["HDF5"], ["PAIR", "INT", "DOUBLE"], false]
  PAIR_INT_STRING,  // ["std::pair<int, std::string>", 1, ["HDF5"], ["PAIR", "INT", "STRING"], false]
  PAIR_INT_VL_STRING,  // ["std::pair<int, std::string>", 1, ["HDF5"], ["PAIR", "INT", "VL_STRING"], false]
  PAIR_INT_BLOB,  // ["std::pair<int, cyclus::Blob>", 0, ["HDF5"], ["PAIR", "INT", "BLOB"], false]
  PAIR_INT_UUID,  // ["std::pair<int, boost::uuids::uuid>", 0, ["HDF5"], ["PAIR", "INT", "UUID"], false]
  PAIR_STRING_BOOL,  // ["std::pair<std::string, bool>", 1, ["HDF5"], ["PAIR", "STRING", "BOOL"], false]
  PAIR_STRING_INT,  // ["std::pair<std::string, int>", 1, ["HDF5"], ["PAIR", "STRING", "INT"], false]
  PAIR_STRING_FLOAT,  // ["std::pair<std::string, float>", 1, ["HDF5"], ["PAIR", "STRING", "FLOAT"], false]
  PAIR_STRING_DOUBLE,  // ["std::pair<std::string, double>", 1, ["HDF5"], ["PAIR", "STRING", "DOUBLE"], false]
  PAIR_STRING_STRING,  // ["std::pair<std::string, std::string>", 2, ["HDF5"], ["PAIR", "STRING", "STRING"], false]
  PAIR_STRING_VL_STRING,  // ["std::pair<std::string, std::string>", 2, ["HDF5"], ["PAIR", "STRING", "VL_STRING"], false]
  PAIR_STRING_BLOB,  // ["std::pair<std::string, cyclus::Blob>", 1, ["HDF5"], ["PAIR", "STRING", "BLOB"], false]
  PAIR_STRING_UUID,  // ["std::pair<std::string, boost::uuids::uuid>", 1, ["HDF5"], ["PAIR", "STRING", "UUID"], false]
  PAIR_VL_STRING_BOOL,  // ["std::pair<std::string, bool>", 1, ["HDF5"], ["PAIR", "VL_STRING", "BOOL"], false]
  PAIR_VL_STRING_INT,  // ["std::pair<std::string, int>", 1, ["HDF5"], ["PAIR", "VL_STRING", "INT"], false]
  PAIR_VL_STRING_FLOAT,  // ["std::pair<std::string, float>", 1, ["HDF5"], ["PAIR", "VL_STRING", "FLOAT"], false]
  PAIR_VL_STRING_DOUBLE,  // ["std::pair<std::string, double>", 1, ["HDF5"], ["PAIR", "VL_STRING", "DOUBLE"], false]
  PAIR_VL_STRING_STRING,  // ["std::pair<std::string, std::string>", 2, ["HDF5"], ["PAIR", "VL_STRING", "STRING"], false]
  PAIR_VL_STRING_VL_STRING,  // ["std::pair<std::string, std::string>", 2, ["HDF5"], ["PAIR", "VL_STRING", "VL_STRING"], false]
  PAIR_VL_STRING_BLOB,  // ["std::pair<std::string, cyclus::Blob>", 1, ["HDF5"], ["PAIR", "VL_STRING", "BLOB"], false]
  PAIR_VL_STRING_UUID,  // ["std::pair<std::string, boost::uuids::uuid>", 1, ["HDF5"], ["PAIR", "VL_STRING", "UUID"], false]
  // maps with int keys
  MAP_INT_BOOL,  // ["std::map<int, bool>", 1, ["HDF5"], ["MAP", "INT", "BOOL"], false]
  VL_MAP_INT_BOOL,  // ["std::map<int, bool>", 1, ["HDF5"], ["VL_MAP", "INT", "BOOL"], true]
  MAP_INT_INT,  // ["std::map<int, int>", 1, ["HDF5", "SQLite"], ["MAP", "INT", "INT"], false]
  VL_MAP_INT_INT,  // ["std::map<int, int>", 1, ["HDF5", "SQLite"], ["VL_MAP", "INT", "INT"], true]
  MAP_INT_FLOAT,  // ["std::map<int, float>", 1, ["HDF5"], ["MAP", "INT", "FLOAT"], false]
  VL_MAP_INT_FLOAT,  // ["std::map<int, float>", 1, ["HDF5"], ["VL_MAP", "INT", "FLOAT"], true]
  MAP_INT_DOUBLE,  // ["std::map<int, double>", 1, ["HDF5", "SQLite"], ["MAP", "INT", "DOUBLE"], false]
  VL_MAP_INT_DOUBLE,  // ["std::map<int, double>", 1, ["HDF5", "SQLite"], ["VL_MAP", "INT", "DOUBLE"], true]
  MAP_INT_STRING,  // ["std::map<int, std::string>", 2, ["HDF5", "SQLite"], ["MAP", "INT", "STRING"], false]
  VL_MAP_INT_STRING,  // ["std::map<int, std::string>", 2, ["HDF5", "SQLite"], ["VL_MAP", "INT", "STRING"], true]
  MAP_INT_VL_STRING,  // ["std::map<int, std::string>", 2, ["HDF5", "SQLite"], ["MAP", "INT", "VL_STRING"], false]
  VL_MAP_INT_VL_STRING,  // ["std::map<int, std::string>", 2, ["HDF5", "SQLite"], ["VL_MAP", "INT", "VL_STRING"], true]
  MAP_INT_BLOB,  // ["std::map<int, cyclus::Blob>", 1, ["HDF5"], ["MAP", "INT", "BLOB"], false]
  VL_MAP_INT_BLOB,  // ["std::map<int, cyclus::Blob>", 1, ["HDF5"], ["VL_MAP", "INT", "BLOB"], true]
  MAP_INT_UUID,  // ["std::map<int, boost::uuids::uuid>", 1, ["HDF5"], ["MAP", "INT", "UUID"], false]
  VL_MAP_INT_UUID,  // ["std::map<int, boost::uuids::uuid>", 1, ["HDF5"], ["VL_MAP", "INT", "UUID"], true]
  // maps with fixed-length string keys
  MAP_STRING_BOOL,  // ["std::map<std::string, bool>", 2, ["HDF5"], ["MAP", "STRING", "BOOL"], false]
  VL_MAP_STRING_BOOL,  // ["std::map<std::string, bool>", 2, ["HDF5"], ["VL_MAP", "STRING", "BOOL"], true]
  MAP_STRING_INT,  // ["std::map<std::string, int>", 2, ["HDF5", "SQLite"], ["MAP", "STRING", "INT"], false]
  VL_MAP_STRING_INT,  // ["std::map<std::string, int>", 2, ["HDF5", "SQLite"], ["VL_MAP", "STRING", "INT"], true]
  MAP_STRING_FLOAT,  // ["std::map<std::string, float>", 2, ["HDF5"], ["MAP", "STRING", "FLOAT"], false]
  VL_MAP_STRING_FLOAT,  // ["std::map<std::string, float>", 2, ["HDF5"], ["VL_MAP", "STRING", "FLOAT"], true]
  MAP_STRING_DOUBLE,  // ["std::map<std::string, double>", 2, ["HDF5", "SQLite"], ["MAP", "STRING", "DOUBLE"], false]
  VL_MAP_STRING_DOUBLE,  // ["std::map<std::string, double>", 2, ["HDF5", "SQLite"], ["VL_MAP", "STRING", "DOUBLE"], true]
  MAP_STRING_STRING,  // ["std::map<std::string, std::string>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", "STRING"], false]
  VL_MAP_STRING_STRING,  // ["std::map<std::string, std::string>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", "STRING"], true]
  MAP_STRING_VL_STRING,  // ["std::map<std::string, std::string>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", "VL_STRING"], false]
  VL_MAP_STRING_VL_STRING,  // ["std::map<std::string, std::string>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", "VL_STRING"], true]
  MAP_STRING_BLOB,  // ["std::map<std::string, cyclus::Blob>", 2, ["HDF5"], ["MAP", "STRING", "BLOB"], false]
  VL_MAP_STRING_BLOB,  // ["std::map<std::string, cyclus::Blob>", 2, ["HDF5"], ["VL_MAP", "STRING", "BLOB"], true]
  MAP_STRING_UUID,  // ["std::map<std::string, boost::uuids::uuid>", 2, ["HDF5"], ["MAP", "STRING", "UUID"], false]
  VL_MAP_STRING_UUID,  // ["std::map<std::string, boost::uuids::uuid>", 2, ["HDF5"], ["VL_MAP", "STRING", "UUID"], true]
  // maps with variable length string keys
  MAP_VL_STRING_BOOL,  // ["std::map<std::string, bool>", 2, ["HDF5"], ["MAP", "VL_STRING", "BOOL"], false]
  VL_MAP_VL_STRING_BOOL,  // ["std::map<std::string, bool>", 2, ["HDF5"], ["VL_MAP", "VL_STRING", "BOOL"], true]
  MAP_VL_STRING_INT,  // ["std::map<std::string, int>", 2, ["HDF5", "SQLite"], ["MAP", "VL_STRING", "INT"], false]
  VL_MAP_VL_STRING_INT,  // ["std::map<std::string, int>", 2, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", "INT"], true]
  MAP_VL_STRING_FLOAT,  // ["std::map<std::string, float>", 2, ["HDF5"], ["MAP", "VL_STRING", "FLOAT"], false]
  VL_MAP_VL_STRING_FLOAT,  // ["std::map<std::string, float>", 2, ["HDF5"], ["VL_MAP", "VL_STRING", "FLOAT"], true]
  MAP_VL_STRING_DOUBLE,  // ["std::map<std::string, double>", 2, ["HDF5", "SQLite"], ["MAP", "VL_STRING", "DOUBLE"], false]
  VL_MAP_VL_STRING_DOUBLE,  // ["std::map<std::string, double>", 2, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", "DOUBLE"], true]
  MAP_VL_STRING_STRING,  // ["std::map<std::string, std::string>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", "STRING"], false]
  VL_MAP_VL_STRING_STRING,  // ["std::map<std::string, std::string>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", "STRING"], true]
  MAP_VL_STRING_VL_STRING,  // ["std::map<std::string, std::string>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", "VL_STRING"], false]
  VL_MAP_VL_STRING_VL_STRING,  // ["std::map<std::string, std::string>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", "VL_STRING"], true]
  MAP_VL_STRING_BLOB,  // ["std::map<std::string, cyclus::Blob>", 2, ["HDF5"], ["MAP", "VL_STRING", "BLOB"], false]
  VL_MAP_VL_STRING_BLOB,  // ["std::map<std::string, cyclus::Blob>", 2, ["HDF5"], ["VL_MAP", "VL_STRING", "BLOB"], true]
  MAP_VL_STRING_UUID,  // ["std::map<std::string, boost::uuids::uuid>", 2, ["HDF5"], ["MAP", "VL_STRING", "UUID"], false]
  VL_MAP_VL_STRING_UUID,  // ["std::map<std::string, boost::uuids::uuid>", 2, ["HDF5"], ["VL_MAP", "VL_STRING", "UUID"], true]
  // maps with pair<int,  string> keys and double values
  MAP_PAIR_INT_STRING_DOUBLE,  // ["std::map<std::pair<int, std::string>, double>", 3, ["HDF5"], ["MAP", ["PAIR", "INT", "STRING"], "DOUBLE"], false]
  VL_MAP_PAIR_INT_STRING_DOUBLE,  // ["std::map<std::pair<int, std::string>, double>", 3, ["HDF5"], ["VL_MAP", ["PAIR", "INT", "STRING"], "DOUBLE"], true]
  MAP_PAIR_INT_VL_STRING_DOUBLE,  // ["std::map<std::pair<int, std::string>, double>", 3, ["HDF5"], ["MAP", ["PAIR", "INT", "VL_STRING"], "DOUBLE"], false]
  VL_MAP_PAIR_INT_VL_STRING_DOUBLE,  // ["std::map<std::pair<int, std::string>, double>", 3, ["HDF5"], ["VL_MAP", ["PAIR", "INT", "VL_STRING"], "DOUBLE"], true]

  // map<string,  vector<double> >
  MAP_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::vector<double>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["VECTOR", "DOUBLE"]], false]
  MAP_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::vector<double>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["VL_VECTOR", "DOUBLE"]], false]
  VL_MAP_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::vector<double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VECTOR", "DOUBLE"]], true]
  MAP_VL_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::vector<double>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VECTOR", "DOUBLE"]], false]
  MAP_VL_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::vector<double>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VL_VECTOR", "DOUBLE"]], false]
  VL_MAP_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::vector<double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VL_VECTOR", "DOUBLE"]], true]
  VL_MAP_VL_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::vector<double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VECTOR", "DOUBLE"]], true]
  VL_MAP_VL_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::vector<double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VL_VECTOR", "DOUBLE"]], true]

  // map<string,  map<int,  double> >
  MAP_STRING_MAP_INT_DOUBLE,  // ["std::map<std::string, std::map<int, double>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["MAP", "INT", "DOUBLE"]], false]
  MAP_STRING_VL_MAP_INT_DOUBLE,  // ["std::map<std::string, std::map<int, double>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["VL_MAP", "INT", "DOUBLE"]], false]
  VL_MAP_STRING_MAP_INT_DOUBLE,  // ["std::map<std::string, std::map<int, double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["MAP", "INT", "DOUBLE"]], true]
  MAP_VL_STRING_MAP_INT_DOUBLE,  // ["std::map<std::string, std::map<int, double>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["MAP", "INT", "DOUBLE"]], false]
  MAP_VL_STRING_VL_MAP_INT_DOUBLE,  // ["std::map<std::string, std::map<int, double>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VL_MAP", "INT", "DOUBLE"]], false]
  VL_MAP_STRING_VL_MAP_INT_DOUBLE,  // ["std::map<std::string, std::map<int, double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VL_MAP", "INT", "DOUBLE"]], true]
  VL_MAP_VL_STRING_MAP_INT_DOUBLE,  // ["std::map<std::string, std::map<int, double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["MAP", "INT", "DOUBLE"]], true]
  VL_MAP_VL_STRING_VL_MAP_INT_DOUBLE,  // ["std::map<std::string, std::map<int, double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VL_MAP", "INT", "DOUBLE"]], true]

  // map<string,  pair<double,  map<int,  double> > >
  MAP_STRING_PAIR_DOUBLE_MAP_INT_DOUBLE,  // ["std::map<std::string, std::pair<double, std::map<int, double>>>", 4, ["HDF5", "SQLite"], ["MAP", "STRING", ["PAIR", "DOUBLE", ["MAP", "INT", "DOUBLE"]]], false]
  VL_MAP_STRING_PAIR_DOUBLE_MAP_INT_DOUBLE,  // ["std::map<std::string, std::pair<double, std::map<int, double>>>", 4, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["PAIR", "DOUBLE", ["MAP", "INT", "DOUBLE"]]], true]
  MAP_VL_STRING_PAIR_DOUBLE_MAP_INT_DOUBLE,  // ["std::map<std::string, std::pair<double, std::map<int, double>>>", 4, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["PAIR", "DOUBLE", ["MAP", "INT", "DOUBLE"]]], false]
  MAP_STRING_PAIR_DOUBLE_VL_MAP_INT_DOUBLE,  // ["std::map<std::string, std::pair<double, std::map<int, double>>>", 4, ["HDF5", "SQLite"], ["MAP", "STRING", ["PAIR", "DOUBLE", ["VL_MAP", "INT", "DOUBLE"]]], false]
  VL_MAP_VL_STRING_PAIR_DOUBLE_MAP_INT_DOUBLE,  // ["std::map<std::string, std::pair<double, std::map<int, double>>>", 4, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["PAIR", "DOUBLE", ["MAP", "INT", "DOUBLE"]]], true]
  VL_MAP_STRING_PAIR_DOUBLE_VL_MAP_INT_DOUBLE,  // ["std::map<std::string, std::pair<double, std::map<int, double>>>", 4, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["PAIR", "DOUBLE", ["VL_MAP", "INT", "DOUBLE"]]], true]
  MAP_VL_STRING_PAIR_DOUBLE_VL_MAP_INT_DOUBLE,  // ["std::map<std::string, std::pair<double, std::map<int, double>>>", 4, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["PAIR", "DOUBLE", ["VL_MAP", "INT", "DOUBLE"]]], false]
  VL_MAP_VL_STRING_PAIR_DOUBLE_VL_MAP_INT_DOUBLE,  // ["std::map<std::string, std::pair<double, std::map<int, double>>>", 4, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["PAIR", "DOUBLE", ["VL_MAP", "INT", "DOUBLE"]]], true]

  // map<map< string,  double > >
  MAP_INT_MAP_STRING_DOUBLE,  // ["std::map<int, std::map<std::string, double>>", 3, ["HDF5", "SQLite"], ["MAP", "INT", ["MAP", "STRING", "DOUBLE"]], false]
  MAP_INT_MAP_VL_STRING_DOUBLE,  // ["std::map<int, std::map<std::string, double>>", 3, ["HDF5", "SQLite"], ["MAP", "INT", ["MAP", "VL_STRING", "DOUBLE"]], false]
  VL_MAP_INT_MAP_STRING_DOUBLE,  // ["std::map<int, std::map<std::string, double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "INT", ["MAP", "STRING", "DOUBLE"]], true]
  VL_MAP_INT_MAP_VL_STRING_DOUBLE,  // ["std::map<int, std::map<std::string, double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "INT", ["MAP", "VL_STRING", "DOUBLE"]], true]
  MAP_INT_VL_MAP_STRING_DOUBLE,  // ["std::map<int, std::map<std::string, double>>", 3, ["HDF5", "SQLite"], ["MAP", "INT", ["VL_MAP", "STRING", "DOUBLE"]], false]
  MAP_INT_VL_MAP_VL_STRING_DOUBLE,  // ["std::map<int, std::map<std::string, double>>", 3, ["HDF5", "SQLite"], ["MAP", "INT", ["VL_MAP", "VL_STRING", "DOUBLE"]], false]
  VL_MAP_INT_VL_MAP_STRING_DOUBLE,  // ["std::map<int, std::map<std::string, double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "INT", ["VL_MAP", "STRING", "DOUBLE"]], true]
  VL_MAP_INT_VL_MAP_VL_STRING_DOUBLE,  // ["std::map<int, std::map<std::string, double>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "INT", ["VL_MAP", "VL_STRING", "DOUBLE"]], true]

  // map< string,  vector< pair<int,  pair<string string> > > >
  MAP_STRING_VECTOR_PAIR_INT_PAIR_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "STRING"]]]], false]
  MAP_STRING_VECTOR_PAIR_INT_PAIR_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]]]], false]
  MAP_STRING_VECTOR_PAIR_INT_PAIR_VL_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]]]], false]
  MAP_STRING_VECTOR_PAIR_INT_PAIR_VL_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]]]], false]
  MAP_STRING_VL_VECTOR_PAIR_INT_PAIR_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "STRING"]]]], false]
  MAP_STRING_VL_VECTOR_PAIR_INT_PAIR_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]]]], false]
  MAP_STRING_VL_VECTOR_PAIR_INT_PAIR_VL_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]]]], false]
  MAP_STRING_VL_VECTOR_PAIR_INT_PAIR_VL_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]]]], false]
  MAP_VL_STRING_VECTOR_PAIR_INT_PAIR_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "STRING"]]]], false]
  MAP_VL_STRING_VECTOR_PAIR_INT_PAIR_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]]]], false]
  MAP_VL_STRING_VECTOR_PAIR_INT_PAIR_VL_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]]]], false]
  MAP_VL_STRING_VECTOR_PAIR_INT_PAIR_VL_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]]]], false]
  MAP_VL_STRING_VL_VECTOR_PAIR_INT_PAIR_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "STRING"]]]], false]
  MAP_VL_STRING_VL_VECTOR_PAIR_INT_PAIR_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]]]], false]
  MAP_VL_STRING_VL_VECTOR_PAIR_INT_PAIR_VL_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]]]], false]
  MAP_VL_STRING_VL_VECTOR_PAIR_INT_PAIR_VL_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]]]], false]
  VL_MAP_STRING_VECTOR_PAIR_INT_PAIR_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "STRING"]]]], true]
  VL_MAP_STRING_VECTOR_PAIR_INT_PAIR_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]]]], true]
  VL_MAP_STRING_VECTOR_PAIR_INT_PAIR_VL_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]]]], true]
  VL_MAP_STRING_VECTOR_PAIR_INT_PAIR_VL_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]]]], true]
  VL_MAP_STRING_VL_VECTOR_PAIR_INT_PAIR_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "STRING"]]]], true]
  VL_MAP_STRING_VL_VECTOR_PAIR_INT_PAIR_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]]]], true]
  VL_MAP_STRING_VL_VECTOR_PAIR_INT_PAIR_VL_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]]]], true]
  VL_MAP_STRING_VL_VECTOR_PAIR_INT_PAIR_VL_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]]]], true]
  VL_MAP_VL_STRING_VECTOR_PAIR_INT_PAIR_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "STRING"]]]], true]
  VL_MAP_VL_STRING_VECTOR_PAIR_INT_PAIR_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]]]], true]
  VL_MAP_VL_STRING_VECTOR_PAIR_INT_PAIR_VL_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]]]], true]
  VL_MAP_VL_STRING_VECTOR_PAIR_INT_PAIR_VL_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]]]], true]
  VL_MAP_VL_STRING_VL_VECTOR_PAIR_INT_PAIR_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "STRING"]]]], true]
  VL_MAP_VL_STRING_VL_VECTOR_PAIR_INT_PAIR_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]]]], true]
  VL_MAP_VL_STRING_VL_VECTOR_PAIR_INT_PAIR_VL_STRING_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]]]], true]
  VL_MAP_VL_STRING_VL_VECTOR_PAIR_INT_PAIR_VL_STRING_VL_STRING,  // ["std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>", 7, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]]]], true]

  // list< pair<int,  int>
  LIST_PAIR_INT_INT,  // ["std::list<std::pair<int, int>>", 2, ["HDF5", "SQLite"], ["LIST", ["PAIR", "INT", "INT"]], false]
  VL_LIST_PAIR_INT_INT,  // ["std::list<std::pair<int, int>>", 2, ["HDF5", "SQLite"], ["VL_LIST", ["PAIR", "INT", "INT"]], true]

  // map< string,  pair< string,  vector<double> > > >
  MAP_STRING_PAIR_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["MAP", "STRING", ["PAIR", "STRING", ["VECTOR", "DOUBLE"]]], false]
  MAP_STRING_PAIR_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["MAP", "STRING", ["PAIR", "STRING", ["VL_VECTOR", "DOUBLE"]]], false]
  MAP_STRING_PAIR_VL_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["MAP", "STRING", ["PAIR", "VL_STRING", ["VECTOR", "DOUBLE"]]], false]
  MAP_STRING_PAIR_VL_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["MAP", "STRING", ["PAIR", "VL_STRING", ["VL_VECTOR", "DOUBLE"]]], false]
  MAP_VL_STRING_PAIR_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["PAIR", "STRING", ["VECTOR", "DOUBLE"]]], false]
  MAP_VL_STRING_PAIR_VL_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["PAIR", "VL_STRING", ["VECTOR", "DOUBLE"]]], false]
  MAP_VL_STRING_PAIR_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["PAIR", "STRING", ["VL_VECTOR", "DOUBLE"]]], false]
  MAP_VL_STRING_PAIR_VL_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["PAIR", "VL_STRING", ["VL_VECTOR", "DOUBLE"]]], false]
  VL_MAP_STRING_PAIR_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["PAIR", "STRING", ["VECTOR", "DOUBLE"]]], true]
  VL_MAP_VL_STRING_PAIR_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["PAIR", "STRING", ["VECTOR", "DOUBLE"]]], true]
  VL_MAP_STRING_PAIR_VL_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["PAIR", "VL_STRING", ["VECTOR", "DOUBLE"]]], true]
  VL_MAP_STRING_PAIR_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["PAIR", "STRING", ["VL_VECTOR", "DOUBLE"]]], true]
  VL_MAP_VL_STRING_PAIR_VL_STRING_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["PAIR", "VL_STRING", ["VECTOR", "DOUBLE"]]], true]
  VL_MAP_VL_STRING_PAIR_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["PAIR", "STRING", ["VL_VECTOR", "DOUBLE"]]], true]
  VL_MAP_STRING_PAIR_VL_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["PAIR", "VL_STRING", ["VL_VECTOR", "DOUBLE"]]], true]
  VL_MAP_VL_STRING_PAIR_VL_STRING_VL_VECTOR_DOUBLE,  // ["std::map<std::string, std::pair<std::string, std::vector<double>>>", 5, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["PAIR", "VL_STRING", ["VL_VECTOR", "DOUBLE"]]], true]

  // map<string,  map<string,  int> >
  MAP_STRING_MAP_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["MAP", "STRING", "INT"]], false]
  MAP_STRING_MAP_VL_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["MAP", "VL_STRING", "INT"]], false]
  MAP_STRING_VL_MAP_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["VL_MAP", "STRING", "INT"]], false]
  MAP_STRING_VL_MAP_VL_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["VL_MAP", "VL_STRING", "INT"]], false]
  MAP_VL_STRING_MAP_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["MAP", "STRING", "INT"]], false]
  MAP_VL_STRING_VL_MAP_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VL_MAP", "STRING", "INT"]], false]
  MAP_VL_STRING_MAP_VL_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["MAP", "VL_STRING", "INT"]], false]
  MAP_VL_STRING_VL_MAP_VL_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VL_MAP", "VL_STRING", "INT"]], false]
  VL_MAP_STRING_MAP_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["MAP", "STRING", "INT"]], true]
  VL_MAP_VL_STRING_MAP_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["MAP", "STRING", "INT"]], true]
  VL_MAP_STRING_VL_MAP_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VL_MAP", "STRING", "INT"]], true]
  VL_MAP_STRING_MAP_VL_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["MAP", "VL_STRING", "INT"]], true]
  VL_MAP_STRING_VL_MAP_VL_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VL_MAP", "VL_STRING", "INT"]], true]
  VL_MAP_VL_STRING_MAP_VL_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["MAP", "VL_STRING", "INT"]], true]
  VL_MAP_VL_STRING_VL_MAP_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VL_MAP", "STRING", "INT"]], true]
  VL_MAP_VL_STRING_VL_MAP_VL_STRING_INT,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VL_MAP", "VL_STRING", "INT"]], true]

  // vector<pair<pair<double, double>, map<string, double>>>
  VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_MAP_STRING_DOUBLE, // ["std::vector<std::pair<std::pair<double, double>, std::map<std::string, double>>>", 5, ["HDF5", "SQLite"], ["VECTOR", ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["MAP", "STRING", "DOUBLE"]]], false]
  VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_MAP_VL_STRING_DOUBLE, // ["std::vector<std::pair<std::pair<double, double>, std::map<std::string, double>>>", 5, ["HDF5", "SQLite"], ["VECTOR", ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["MAP", "VL_STRING", "DOUBLE"]]], false]
  VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_VL_MAP_STRING_DOUBLE, // ["std::vector<std::pair<std::pair<double, double>, std::map<std::string, double>>>", 5, ["HDF5", "SQLite"], ["VECTOR", ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["VL_MAP", "STRING", "DOUBLE"]]], false]
  VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_VL_MAP_VL_STRING_DOUBLE, // ["std::vector<std::pair<std::pair<double, double>, std::map<std::string, double>>>", 5, ["HDF5", "SQLite"], ["VECTOR", ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["VL_MAP", "VL_STRING", "DOUBLE"]]], false]
  VL_VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_MAP_STRING_DOUBLE, // ["std::vector<std::pair<std::pair<double, double>, std::map<std::string, double>>>", 5, ["HDF5", "SQLite"], ["VL_VECTOR", ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["MAP", "STRING", "DOUBLE"]]], true]
  VL_VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_MAP_VL_STRING_DOUBLE, // ["std::vector<std::pair<std::pair<double, double>, std::map<std::string, double>>>", 5, ["HDF5", "SQLite"], ["VL_VECTOR", ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["MAP", "VL_STRING", "DOUBLE"]]], true]
  VL_VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_VL_MAP_STRING_DOUBLE, // ["std::vector<std::pair<std::pair<double, double>, std::map<std::string, double>>>", 5, ["HDF5", "SQLite"], ["VL_VECTOR", ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["VL_MAP", "STRING", "DOUBLE"]]], true]
  VL_VECTOR_PAIR_PAIR_DOUBLE_DOUBLE_VL_MAP_VL_STRING_DOUBLE, // ["std::vector<std::pair<std::pair<double, double>, std::map<std::string, double>>>", 5, ["HDF5", "SQLite"], ["VL_VECTOR", ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["VL_MAP", "VL_STRING", "DOUBLE"]]], true]

  // pair<int, pair<string, string>>
  PAIR_INT_PAIR_STRING_STRING,  // ["std::pair<int, std::pair<std::string, std::string>>", 2, ["HDF5"], ["PAIR", "INT", ["PAIR", "STRING", "STRING"]], false]
  PAIR_INT_PAIR_VL_STRING_STRING,  // ["std::pair<int, std::pair<std::string, std::string>>", 2, ["HDF5"], ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]], false]
  PAIR_INT_PAIR_STRING_VL_STRING,  // ["std::pair<int, std::pair<std::string, std::string>>", 2, ["HDF5"], ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]], false]
  PAIR_INT_PAIR_VL_STRING_VL_STRING,  // ["std::pair<int, std::pair<std::string, std::string>>", 2, ["HDF5"], ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]], false]

  // pair<double, double>
  PAIR_DOUBLE_DOUBLE,  // ["std::pair<double, double>", 0, ["HDF5"], ["PAIR", "DOUBLE", "DOUBLE"], false]

  // pair<pair<double, double>, map<string, double>>
  PAIR_PAIR_DOUBLE_DOUBLE_MAP_STRING_DOUBLE,        // ["std::pair<std::pair<double, double>, std::map<std::string, double>>", 4, ["HDF5"], ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["MAP", "STRING", "DOUBLE"]], false]
  PAIR_PAIR_DOUBLE_DOUBLE_MAP_VL_STRING_DOUBLE,     // ["std::pair<std::pair<double, double>, std::map<std::string, double>>", 4, ["HDF5"], ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["MAP", "VL_STRING", "DOUBLE"]], false]
  PAIR_PAIR_DOUBLE_DOUBLE_VL_MAP_STRING_DOUBLE,     // ["std::pair<std::pair<double, double>, std::map<std::string, double>>", 4, ["HDF5"], ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["VL_MAP", "STRING", "DOUBLE"]], false]
  PAIR_PAIR_DOUBLE_DOUBLE_VL_MAP_VL_STRING_DOUBLE,  // ["std::pair<std::pair<double, double>, std::map<std::string, double>>", 4, ["HDF5"], ["PAIR", ["PAIR", "DOUBLE", "DOUBLE"], ["VL_MAP", "VL_STRING", "DOUBLE"]], false]

  // Resource Tools
  MATERIAL,  // ["cyclus::Material", 0, [], "MATERIAL", false]
  PRODUCT,  // ["cyclus::Product", 0, [], "PRODUCT", false]
  RESOURCE_BUFF,  // ["cyclus::toolkit::ResourceBuff", 0, [], "RESOURCE_BUFF", false]
  RES_BUF_MATERIAL,  // ["cyclus::toolkit::ResBuf<cyclus::Material>", 1, [], ["RES_BUF", "MATERIAL"], false]
  RES_BUF_PACKAGED_MATERIAL,  // ["cyclus::toolkit::ResBuf<cyclus::Material>", 1, [], ["RES_BUF", "MATERIAL"], false]
  RES_BUF_PRODUCT,  // ["cyclus::toolkit::ResBuf<cyclus::Product>", 1, [], ["RES_BUF", "PRODUCT"], false]
  RES_MAP_INT_MATERIAL,  // ["cyclus::toolkit::ResMap<int, cyclus::Material>", 2, [], ["RES_MAP", "INT", "MATERIAL"], false]
  RES_MAP_INT_PRODUCT,  // ["cyclus::toolkit::ResMap<int, cyclus::Product>", 2, [], ["RES_MAP", "INT", "PRODUCT"], false]
  RES_MAP_STRING_MATERIAL,  // ["cyclus::toolkit::ResMap<std::string, cyclus::Material>", 2, [], ["RES_MAP", "STRING", "MATERIAL"], false]
  RES_MAP_STRING_PRODUCT,  // ["cyclus::toolkit::ResMap<std::string, cyclus::Product>", 2, [], ["RES_MAP", "STRING", "PRODUCT"], false]
  RES_MAP_VL_STRING_MATERIAL,  // ["cyclus::toolkit::ResMap<std::string, cyclus::Material>", 2, [], ["RES_MAP", "VL_STRING", "MATERIAL"], false]
  RES_MAP_VL_STRING_PRODUCT,  // ["cyclus::toolkit::ResMap<std::string, cyclus::Product>", 2, [], ["RES_MAP", "VL_STRING", "PRODUCT"], false]

  // pair<double, map<int, double>>
  PAIR_DOUBLE_MAP_INT_DOUBLE, // ["std::pair<double, std::map<int, double>>", 2, ["HDF5"], ["PAIR", "DOUBLE", ["MAP", "INT", "DOUBLE"]], false]
  PAIR_DOUBLE_VL_MAP_INT_DOUBLE, // ["std::pair<double, std::map<int, double>>", 2, ["HDF5"], ["PAIR", "DOUBLE", ["VL_MAP", "INT", "DOUBLE"]], false]

  // vector<pair<int, pair<string, string>>>
  VECTOR_PAIR_INT_PAIR_STRING_STRING, // ["std::vector<std::pair<int, std::pair<std::string, std::string>>>", 1, ["HDF5"], ["VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "STRING"]]], false]
  VECTOR_PAIR_INT_PAIR_VL_STRING_STRING, // ["std::vector<std::pair<int, std::pair<std::string, std::string>>>", 1, ["HDF5"], ["VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]]], false]
  VECTOR_PAIR_INT_PAIR_STRING_VL_STRING, // ["std::vector<std::pair<int, std::pair<std::string, std::string>>>", 1, ["HDF5"], ["VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]]], false]
  VECTOR_PAIR_INT_PAIR_VL_STRING_VL_STRING, // ["std::vector<std::pair<int, std::pair<std::string, std::string>>>", 1, ["HDF5"], ["VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]]], false]
  VL_VECTOR_PAIR_INT_PAIR_STRING_STRING, // ["std::vector<std::pair<int, std::pair<std::string, std::string>>>", 1, ["HDF5"], ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "STRING"]]], true]
  VL_VECTOR_PAIR_INT_PAIR_VL_STRING_STRING, // ["std::vector<std::pair<int, std::pair<std::string, std::string>>>", 1, ["HDF5"], ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "STRING"]]], true]
  VL_VECTOR_PAIR_INT_PAIR_STRING_VL_STRING, // ["std::vector<std::pair<int, std::pair<std::string, std::string>>>", 1, ["HDF5"], ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "STRING", "VL_STRING"]]], true]
  VL_VECTOR_PAIR_INT_PAIR_VL_STRING_VL_STRING, // ["std::vector<std::pair<int, std::pair<std::string, std::string>>>", 1, ["HDF5"], ["VL_VECTOR", ["PAIR", "INT", ["PAIR", "VL_STRING", "VL_STRING"]]], true]

  PAIR_STRING_VECTOR_DOUBLE, // ["std::pair<std::string, std::vector<double>>", 1, ["HDF5"], ["PAIR", "STRING", ["VECTOR", "DOUBLE"]], false]
  PAIR_VL_STRING_VECTOR_DOUBLE, // ["std::pair<std::string, std::vector<double>>", 1, ["HDF5"], ["PAIR", "VL_STRING", ["VECTOR", "DOUBLE"]], false]
  PAIR_STRING_VL_VECTOR_DOUBLE, // ["std::pair<std::string, std::vector<double>>", 1, ["HDF5"], ["PAIR", "STRING", ["VL_VECTOR", "DOUBLE"]], false]
  PAIR_VL_STRING_VL_VECTOR_DOUBLE, // ["std::pair<std::string, std::vector<double>>", 1, ["HDF5"], ["PAIR", "VL_STRING", ["VL_VECTOR", "DOUBLE"]], false]

  //map<pair<string, string>, int>
  MAP_PAIR_STRING_STRING_INT, // ["std::map<std::pair<std::string, std::string>, int>", 4, ["HDF5","SQLite"], ["MAP", ["PAIR", "STRING", "STRING"], "INT"], false]
  MAP_PAIR_STRING_VL_STRING_INT, // ["std::map<std::pair<std::string, std::string>, int>", 4, ["HDF5","SQLite"], ["MAP", ["PAIR", "STRING", "VL_STRING"], "INT"], false]
  MAP_PAIR_VL_STRING_STRING_INT, // ["std::map<std::pair<std::string, std::string>, int>", 4, ["HDF5","SQLite"], ["MAP", ["PAIR", "VL_STRING", "STRING"], "INT"], false]
  MAP_PAIR_VL_STRING_VL_STRING_INT, // ["std::map<std::pair<std::string, std::string>, int>", 4, ["HDF5","SQLite"], ["MAP", ["PAIR", "VL_STRING", "VL_STRING"], "INT"], false]
  VL_MAP_PAIR_STRING_STRING_INT, // ["std::map<std::pair<std::string, std::string>, int>", 4, ["HDF5","SQLite"], ["VL_MAP", ["PAIR", "STRING", "STRING"], "INT"], true]
  VL_MAP_PAIR_STRING_VL_STRING_INT, // ["std::map<std::pair<std::string, std::string>, int>", 4, ["HDF5","SQLite"], ["VL_MAP", ["PAIR", "STRING", "VL_STRING"], "INT"], true]
  VL_MAP_PAIR_VL_STRING_STRING_INT, // ["std::map<std::pair<std::string, std::string>, int>", 4, ["HDF5","SQLite"], ["VL_MAP", ["PAIR", "VL_STRING", "STRING"], "INT"], true]
  VL_MAP_PAIR_VL_STRING_VL_STRING_INT, // ["std::map<std::pair<std::string, std::string>, int>", 4, ["HDF5","SQLite"], ["VL_MAP", ["PAIR", "VL_STRING", "VL_STRING"], "INT"], true]

  // map<string,  map<string,  double> >
  MAP_STRING_MAP_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["MAP", "STRING", "INT"]], false]
  MAP_STRING_MAP_VL_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["MAP", "VL_STRING", "INT"]], false]
  MAP_STRING_VL_MAP_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["VL_MAP", "STRING", "INT"]], false]
  MAP_STRING_VL_MAP_VL_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "STRING", ["VL_MAP", "VL_STRING", "INT"]], false]
  MAP_VL_STRING_MAP_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["MAP", "STRING", "INT"]], false]
  MAP_VL_STRING_VL_MAP_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VL_MAP", "STRING", "INT"]], false]
  MAP_VL_STRING_MAP_VL_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["MAP", "VL_STRING", "INT"]], false]
  MAP_VL_STRING_VL_MAP_VL_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["MAP", "VL_STRING", ["VL_MAP", "VL_STRING", "INT"]], false]
  VL_MAP_STRING_MAP_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["MAP", "STRING", "INT"]], true]
  VL_MAP_VL_STRING_MAP_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["MAP", "STRING", "INT"]], true]
  VL_MAP_STRING_VL_MAP_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VL_MAP", "STRING", "INT"]], true]
  VL_MAP_STRING_MAP_VL_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["MAP", "VL_STRING", "INT"]], true]
  VL_MAP_STRING_VL_MAP_VL_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "STRING", ["VL_MAP", "VL_STRING", "INT"]], true]
  VL_MAP_VL_STRING_MAP_VL_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["MAP", "VL_STRING", "INT"]], true]
  VL_MAP_VL_STRING_VL_MAP_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VL_MAP", "STRING", "INT"]], true]
  VL_MAP_VL_STRING_VL_MAP_VL_STRING_DOUBLE,  // ["std::map<std::string, std::map<std::string, int>>", 3, ["HDF5", "SQLite"], ["VL_MAP", "VL_STRING", ["VL_MAP", "VL_STRING", "INT"]], true]

  // append new types only:
};

/// Represents operation codes for condition checking.
enum CmpOpCode {
  LT = 0,
  GT,
  LE,
  GE,
  EQ,
  NE,
};

/// Represents a condition used to filter rows returned by a query.
class Cond {
 public:
  Cond() {}

  Cond(std::string field, std::string op, boost::spirit::hold_any val)
      : field(field),
        op(op),
        val(val) {
    if (op == "<")
      opcode = LT;
    else if (op == ">")
      opcode = GT;
    else if (op == "<=")
      opcode = LE;
    else if (op == ">=")
      opcode = GE;
    else if (op == "==")
      opcode = EQ;
    else if (op == "!=")
      opcode = NE;
    else
      throw ValueError("operation '" + op + "' not valid for field '" + \
                       field + "'.");
  }

  /// table column name
  std::string field;

  /// One of: "<", ">", "<=", ">=", "==", "!="
  std::string op;

  /// The CmpOpCode cooresponding to op.
  CmpOpCode opcode;

  /// value supported by backend(s) in use
  boost::spirit::hold_any val;
};

typedef std::vector<boost::spirit::hold_any> QueryRow;

/// Meta data and results of a query.
class QueryResult {
 public:
  /// names of each field returned by a query
  std::vector<std::string> fields;

  /// types of each field returned by a query.
  std::vector<DbTypes> types;

  /// ordered results of a query
  std::vector<QueryRow> rows;

  void Reset() {
    fields.clear();
    types.clear();
    rows.clear();
  }

  /// Convenience method for retrieving a value from a specific row and named
  /// field (column). The caller is responsible for specifying a valid templated
  /// type to cast to. Example use:
  ///
  /// @code
  ///
  /// QueryResult qr = ...
  ///
  /// for (int i = 0; i < qr.rows.size(); ++i) {
  ///   std::cout << qr.GetVal<int>("field1", i) << "\n";
  ///   std::cout << qr.GetVal<double>("field2", i) << "\n";
  ///   std::cout << qr.GetVal<std::string>("field3", i) << "\n";
  /// }
  ///
  /// @endcode
  template <class T>
  T GetVal(std::string field, int row = 0) {
    if (rows.empty())
      throw StateError("No rows found during query for field " + field);

    if (row >= rows.size()) {
      throw KeyError("index larger than number of query rows for field "
                     + field);
    }

    int field_idx = -1;
    for (int i = 0; i < fields.size(); ++i) {
      if (fields[i] == field) {
        field_idx = i;
        break;
      }
    }
    if (field_idx == -1) {
      throw KeyError("query result has no such field " + field);
    }

    return rows[row][field_idx].cast<T>();
  }
};

/// Represents column information.
struct ColumnInfo {
  ColumnInfo() {};
  ColumnInfo(std::string default_table, std::string default_col, int default_index,
             DbTypes default_dbtype, std::vector<int> default_shape) :
    table(default_table), col(default_col), index(default_index), dbtype(default_dbtype),
    shape(default_shape) {};
  std::string table;
  std::string col;
  int index;
  DbTypes dbtype;
  std::vector<int> shape;
};

/// Interface implemented by backends that support rudimentary querying.
class QueryableBackend {
 public:
  virtual ~QueryableBackend() {}

  /// Return a set of rows from the specificed table that match all given
  /// conditions.  Conditions are AND'd together.  conds may be NULL.
  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) = 0;

  /// Return a map of column names of the specified table to the associated
  /// database type.
  virtual std::map<std::string, DbTypes> ColumnTypes(std::string table) = 0;

  /// Return information about all columns of a table.
  virtual std::list<ColumnInfo> Schema(std::string table) = 0;

  /// Return a set of all table names currently in the database.
  virtual std::set<std::string> Tables() = 0;
};

/// Interface implemented by backends that support recording and querying.
class FullBackend: public QueryableBackend, public RecBackend {
 public:
  virtual ~FullBackend() {}
};

/// Wrapper class for QueryableBackends that injects a set of Cond's into every
/// query before being executed.
class CondInjector: public QueryableBackend {
 public:
  CondInjector(QueryableBackend* b, std::vector<Cond> to_inject)
      : b_(b),
        to_inject_(to_inject) {}

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) {
    if (conds == NULL) {
      return b_->Query(table, &to_inject_);
    }

    std::vector<Cond> c = *conds;
    for (int i = 0; i < to_inject_.size(); ++i) {
      c.push_back(to_inject_[i]);
    }
    return b_->Query(table, &c);
  }

  virtual std::map<std::string, DbTypes> ColumnTypes(std::string table) {
    return b_->ColumnTypes(table);
  }

  virtual std::list<ColumnInfo> Schema(std::string table) {
    return b_->Schema(table);
  }

  virtual std::set<std::string> Tables() { return b_->Tables(); }

 private:
  QueryableBackend* b_;
  std::vector<Cond> to_inject_;
};

/// Wrapper class for QueryableBackends that injects prefix in front of the
/// title/table for every query before being executed.  A query to the
/// "MyAgentTable" table will actually be passed to the wrapped backend as
/// [prefix] + "MyAgentTable".
class PrefixInjector: public QueryableBackend {
 public:
  PrefixInjector(QueryableBackend* b, std::string prefix)
      : b_(b),
        prefix_(prefix) {}

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) {
    return b_->Query(prefix_ + table, conds);
  }

  virtual std::map<std::string, DbTypes> ColumnTypes(std::string table) {
    return b_->ColumnTypes(table);
  }

  virtual std::list<ColumnInfo> Schema(std::string table) {
    return b_->Schema(table);
  }

  virtual std::set<std::string> Tables() { return b_->Tables(); }

 private:
  QueryableBackend* b_;
  std::string prefix_;
};

/// Compares a condiontion for a single value
template <typename T>
inline bool CmpCond(T* x, Cond* cond) {
  bool rtn;
  switch (cond->opcode) {
    case LT: {
      rtn = (*x) < cond->val.cast<T>();
      break;
    }
    case GT: {
      rtn = (*x) > cond->val.cast<T>();
      break;
    }
    case LE: {
      rtn = (*x) <= cond->val.cast<T>();
      break;
    }
    case GE: {
      rtn = (*x) >= cond->val.cast<T>();
      break;
    }
    case EQ: {
      rtn = (*x) == cond->val.cast<T>();
      break;
    }
    case NE: {
      rtn = (*x) != cond->val.cast<T>();
      break;
    }
  }
  return rtn;
}

/// Compares all condiontions for a value
template <typename T>
inline bool CmpConds(T* x, std::vector<Cond*>* conds) {
  int i;
  for (i = 0; i < conds->size(); ++i)
    if (!CmpCond<T>(&(*x), (*conds)[i]))
      return false;
  return true;
}

/// The digest type for SHA1s.
///
/// This class is a hack around a language deficiency in C++. You cannot pass
/// around an array (unsinged int[5]) between function calls. You can only
/// pass pointers, which would involve lost of new/free and heap shenanigans
/// that are not needed for a dumb container. Therefore Sha1::Digest() cannot
/// return what would be most natural. The second most natural thing would be
/// a std::array<unsigned int, 5>. However, std::array is a C++11 feature and
/// we are not yet ready to go down that road.
///
/// To pass an array into and out of a function it has to be inside of struct
/// or a class. I chose a class here since there are many member functions.
///
/// The reason why this is public is that it needs to be directly writable
/// from buffers coming from HDF5. In the future, this really should just be
/// a std::array.
class Digest {
 public:
  unsigned int val[CYCLUS_SHA1_NINT];

  /// Casts the value of this digest to a vector of the templated type.
  template <typename T>
  inline std::vector<T> cast() const {
    std::vector<T> rtn = std::vector<T>(CYCLUS_SHA1_NINT);
    for (unsigned int i = 0; i < CYCLUS_SHA1_NINT; ++i)
      rtn[i] = static_cast<T>(val[i]);
    return rtn;
  }

  // operators
  inline std::ostream& operator<<(std::ostream& out) const {
    return out << "[" << val[0] << ", " << val[1] << ", " <<  val[2] << \
                  ", " << val[3] << ", " << val[4] << "]";
  }

  inline bool operator< (const cyclus::Digest& rhs) const {
    bool rtn = false;
    for (int i = 0; i < CYCLUS_SHA1_NINT; ++i) {
      if (val[i] < rhs.val[i]) {
        rtn = true;
        break;
      } else if (val[i] > rhs.val[i]) {
        rtn = false;
        break;
      }  // else they are equal and we need to check the next index
    }
    return rtn;
  }

  inline bool operator> (const cyclus::Digest& rhs) const {
    return !operator<(rhs) && !operator==(rhs);
  }

  inline bool operator<=(const cyclus::Digest& rhs) const {
    return !operator>(rhs);
  }

  inline bool operator>=(const cyclus::Digest& rhs) const {
    return !operator<(rhs);
  }

  inline bool operator==(const cyclus::Digest& rhs) const {
    bool rtn = true;
    for (int i = 0; i < CYCLUS_SHA1_NINT; ++i) {
      if (val[i] != rhs.val[i]) {
        rtn = false;
        break;
      }  // else they are equal and we need to check the next index.
    }
    return rtn;
  }

  inline bool operator!=(const cyclus::Digest& rhs) const {
    return !operator==(rhs);
  }
};

class Sha1 {
 public:
  Sha1() { hash_ = boost::uuids::detail::sha1(); }

  /// Clears the current hash value to its default state.
  inline void Clear() { hash_.reset(); }

  /// Updates the hash value in-place.
  /// \{
  inline void Update(const std::string& s) {
    hash_.process_bytes(s.c_str(), s.size());
  }

  inline void Update(const Blob& b) { Update(b.str()); }

  inline void Update(const std::vector<int>& x) {
    hash_.process_bytes(&x[0], x.size() * sizeof(int));
  }

  inline void Update(const std::vector<float>& x) {
    hash_.process_bytes(&x[0], x.size() * sizeof(float));
  }

  inline void Update(const std::vector<double>& x) {
    hash_.process_bytes(&x[0], x.size() * sizeof(double));
  }

  inline void Update(const std::vector<std::string>& x) {
    for (unsigned int i = 0; i < x.size(); ++i)
      hash_.process_bytes(x[i].c_str(), x[i].size());
  }

  inline void Update(const std::vector<cyclus::Blob>& x) {
    for (unsigned int i = 0; i < x.size(); ++i)
      hash_.process_bytes(x[i].str().c_str(), x[i].str().size());
  }

  inline void Update(const std::vector<boost::uuids::uuid>& x) {
    std::vector<boost::uuids::uuid>::const_iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), CYCLUS_UUID_SIZE);
  }

  inline void Update(const std::set<int>& x) {
    std::set<int>::iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), sizeof(int));
  }

  inline void Update(const std::set<bool>& x) {
    std::set<bool>::iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), sizeof(bool));
  }

  inline void Update(const std::set<double>& x) {
    std::set<double>::iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), sizeof(double));
  }

  inline void Update(const std::set<float>& x) {
    std::set<float>::iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), sizeof(float));
  }

  inline void Update(const std::set<cyclus::Blob>& x) {
    std::set<cyclus::Blob>::iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(it->str().c_str(), it->str().size());
  }

  inline void Update(const std::set<boost::uuids::uuid>& x) {
    std::set<boost::uuids::uuid>::iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), CYCLUS_UUID_SIZE);
  }

  inline void Update(const std::set<std::string>& x) {
    std::set<std::string>::iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(it->c_str(), it->size());
  }

  inline void Update(const std::list<int>& x) {
    std::list<int>::const_iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), sizeof(int));
  }

  inline void Update(const std::list<bool>& x) {
    std::list<bool>::const_iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), sizeof(bool));
  }

  inline void Update(const std::list<double>& x) {
    std::list<double>::const_iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), sizeof(double));
  }

  inline void Update(const std::list<float>& x) {
    std::list<float>::const_iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), sizeof(float));
  }

  inline void Update(const std::list<std::string>& x) {
    std::list<std::string>::const_iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(it->c_str(), it->size());
  }

  inline void Update(const std::list<cyclus::Blob>& x) {
    std::list<cyclus::Blob>::const_iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(it->str().c_str(), it->str().size());
  }

  inline void Update(const std::list<boost::uuids::uuid>& x) {
    std::list<boost::uuids::uuid>::const_iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), CYCLUS_UUID_SIZE);
  }

  inline void Update(const std::pair<int, int>& x) {
    hash_.process_bytes(&(x.first), sizeof(int));
    hash_.process_bytes(&(x.second), sizeof(int));
  }

  inline void Update(const std::pair<int, std::string>& x) {
    hash_.process_bytes(&(x.first), sizeof(int));
    hash_.process_bytes(x.second.c_str(), x.second.size());
  }

  inline void Update(const std::map<int, int>& x) {
    std::map<int, int>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), sizeof(int));
      hash_.process_bytes(&(it->second), sizeof(int));
    }
  }

  inline void Update(const std::map<int, bool>& x) {
    std::map<int, bool>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), sizeof(int));
      hash_.process_bytes(&(it->second), sizeof(bool));
    }
  }

  inline void Update(const std::map<int, double>& x) {
    std::map<int, double>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), sizeof(int));
      hash_.process_bytes(&(it->second), sizeof(double));
    }
  }

  inline void Update(const std::map<int, float>& x) {
    std::map<int, float>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), sizeof(int));
      hash_.process_bytes(&(it->second), sizeof(float));
    }
  }

  inline void Update(const std::map<int, cyclus::Blob>& x) {
    std::map<int, cyclus::Blob>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), sizeof(int));
      hash_.process_bytes(it->second.str().c_str(), it->second.str().size());
    }
  }

  inline void Update(const std::map<int, boost::uuids::uuid>& x) {
    std::map<int, boost::uuids::uuid>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), sizeof(int));
      hash_.process_bytes(&(it->second), CYCLUS_UUID_SIZE);
    }
  }

  inline void Update(const std::map<int, std::string>& x) {
    std::map<int, std::string>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), sizeof(int));
      hash_.process_bytes(it->second.c_str(), it->second.size());
    }
  }

  inline void Update(const std::map<std::string, int>& x) {
    std::map<std::string, int>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      hash_.process_bytes(&(it->second), sizeof(int));
    }
  }

  inline void Update(const std::map<std::string, double>& x) {
    std::map<std::string, double>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      hash_.process_bytes(&(it->second), sizeof(double));
    }
  }

  inline void Update(const std::map<std::string, float>& x) {
    std::map<std::string, float>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      hash_.process_bytes(&(it->second), sizeof(float));
    }
  }

  inline void Update(const std::map<std::string, bool>& x) {
    std::map<std::string, bool>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      hash_.process_bytes(&(it->second), sizeof(bool));
    }
  }

  inline void Update(const std::map<std::string, cyclus::Blob>& x) {
    std::map<std::string, cyclus::Blob>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      hash_.process_bytes(it->second.str().c_str(), it->second.str().size());
    }
  }

  inline void Update(const std::map<std::string, boost::uuids::uuid>& x) {
    std::map<std::string, boost::uuids::uuid>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      hash_.process_bytes(&(it->second), CYCLUS_UUID_SIZE);
    }
  }

  inline void Update(const std::map<std::string, std::string>& x) {
    std::map<std::string, std::string>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      hash_.process_bytes(it->second.c_str(), it->second.size());
    }
  }

  inline void Update(const std::map<std::pair<int, std::string>, double>& x) {
    std::map<std::pair<int, std::string>, double>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first.first), sizeof(int));
      hash_.process_bytes(it->first.second.c_str(), it->first.second.size());
      hash_.process_bytes(&(it->second), sizeof(double));
    }
  }

  inline void Update(const std::map<std::pair<std::string, std::string>, int>& x) {
    std::map<std::pair<std::string, std::string>, int>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.first.c_str(), it->first.first.size());
      hash_.process_bytes(it->first.second.c_str(), it->first.second.size());
      hash_.process_bytes(&(it->second), sizeof(int));
    }
  }

  inline void Update(const std::map<std::string, std::vector<double>>& x) {
    std::map<std::string, std::vector<double>>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      Update(it->second);
    }
  }

  inline void Update(const std::map<std::string, std::map<int, double>>& x) {
    std::map<std::string, std::map<int, double>>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      Update(it->second);
    }
  }

  inline void Update(const std::map<int, std::map<std::string, double>>& x) {
    std::map<int, std::map<std::string, double>>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), sizeof(int));
      Update(it->second);
    }
  }

  inline void Update(const std::pair<double, std::map<int, double>>& x) {
    hash_.process_bytes(&(x.first), sizeof(double));
    Update(x.second);
  }

  inline void Update(const std::map<std::string, std::pair<double, std::map<int, double>>>& x) {
    std::map<std::string, std::pair<double, std::map<int, double>>>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), it->first.size());
      Update(it->second);
    }
  }

  inline void Update(const std::pair<int, std::pair<std::string, std::string>>& x) {
    hash_.process_bytes(&(x.first), sizeof(int));
    hash_.process_bytes(x.second.first.c_str(), x.second.first.size());
    hash_.process_bytes(x.second.second.c_str(), x.second.second.size());
  }

  inline void Update(const std::vector<std::pair<int, std::pair<std::string, std::string>>>& x) {
    std::vector<std::pair<int, std::pair<std::string, std::string>>>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      Update(*it);
    }
  }

  inline void Update(const std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>& x) {
    std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string>>>>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      Update(it->second);
    }
  }

  inline void Update(const std::list<std::pair<int, int>>& x){
    std::list<std::pair<int, int>>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      Update(*it);
    }
  }

  inline void Update(const std::pair<std::string, std::vector<double>>& x) {
    hash_.process_bytes(x.first.c_str(), x.first.size());
    Update(x.second);
  }

  inline void Update(const std::map<std::string, std::pair<std::string, std::vector<double>>>& x) {
    std::map<std::string, std::pair<std::string, std::vector<double>>>::const_iterator it = x.begin();
    for(; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      Update(it->second);
    }
  }

  inline void Update(const std::map<std::string, std::map<std::string, int>>& x) {
    std::map<std::string, std::map<std::string, int>>::const_iterator it = x.begin();
    for(; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      Update(it->second);
    }
  }

  inline void Update(const std::pair<double, double>& x) {
    hash_.process_bytes(&(x.first), sizeof(double));
    hash_.process_bytes(&(x.second), sizeof(double));
  }

  inline void Update(const std::pair<std::pair<double, double>, std::map<std::string, double>>& x) {
    Update(x.first);
    Update(x.second);
  }

  inline void Update(const std::vector<std::pair<std::pair<double, double>, std::map<std::string, double>>>& x) {
    std::vector<std::pair<std::pair<double, double>, std::map<std::string, double>>>::const_iterator it = x.begin();
    for(; it != x.end(); ++it) {
      Update(*it);
    }
  }

  /// \}

  Digest digest() {
    Digest d;
    hash_.get_digest(d.val);
    return d;
  }

 private:
  boost::uuids::detail::sha1 hash_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_QUERY_BACKEND_H_
