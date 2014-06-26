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
  BOOL = 0,
  INT,
  FLOAT,
  DOUBLE,
  STRING,
  VL_STRING,
  BLOB,
  UUID,
  // vector types
  VECTOR_BOOL,
  VL_VECTOR_BOOL,
  VECTOR_INT,
  VL_VECTOR_INT,
  VECTOR_FLOAT,
  VL_VECTOR_FLOAT,
  VECTOR_DOUBLE,
  VL_VECTOR_DOUBLE,
  VECTOR_STRING,
  VL_VECTOR_STRING,
  VECTOR_VL_STRING,
  VL_VECTOR_VL_STRING,
  VECTOR_BLOB,
  VL_VECTOR_BLOB,
  VECTOR_UUID,
  VL_VECTOR_UUID,
  // set types
  SET_BOOL,
  VL_SET_BOOL,
  SET_INT,
  VL_SET_INT,
  SET_FLOAT,
  VL_SET_FLOAT,
  SET_DOUBLE,
  VL_SET_DOUBLE,
  SET_STRING,
  VL_SET_STRING,
  SET_VL_STRING,
  VL_SET_VL_STRING,
  SET_BLOB,
  VL_SET_BLOB,
  SET_UUID,
  VL_SET_UUID,
  // list types
  LIST_BOOL,
  VL_LIST_BOOL,
  LIST_INT,
  VL_LIST_INT,
  LIST_FLOAT,
  VL_LIST_FLOAT,
  LIST_DOUBLE,
  VL_LIST_DOUBLE,
  LIST_STRING,
  VL_LIST_STRING,
  LIST_VL_STRING,
  VL_LIST_VL_STRING,
  LIST_BLOB,
  VL_LIST_BLOB,
  LIST_UUID,
  VL_LIST_UUID,
  // pairs - not variable length
  PAIR_INT_BOOL,
  PAIR_INT_INT,
  PAIR_INT_FLOAT,
  PAIR_INT_DOUBLE,
  PAIR_INT_STRING,
  PAIR_INT_VL_STRING,
  PAIR_INT_BLOB,
  PAIR_INT_UUID,
  PAIR_STRING_BOOL,
  PAIR_STRING_INT,
  PAIR_STRING_FLOAT,
  PAIR_STRING_DOUBLE,
  PAIR_STRING_STRING,
  PAIR_STRING_VL_STRING,
  PAIR_STRING_BLOB,
  PAIR_STRING_UUID,
  PAIR_VL_STRING_BOOL,
  PAIR_VL_STRING_INT,
  PAIR_VL_STRING_FLOAT,
  PAIR_VL_STRING_DOUBLE,
  PAIR_VL_STRING_STRING,
  PAIR_VL_STRING_VL_STRING,
  PAIR_VL_STRING_BLOB,
  PAIR_VL_STRING_UUID,
  // maps with int keys
  MAP_INT_BOOL,
  VL_MAP_INT_BOOL,
  MAP_INT_INT,
  VL_MAP_INT_INT,
  MAP_INT_FLOAT,
  VL_MAP_INT_FLOAT,
  MAP_INT_DOUBLE,
  VL_MAP_INT_DOUBLE,
  MAP_INT_STRING,
  VL_MAP_INT_STRING,
  MAP_INT_VL_STRING,
  VL_MAP_INT_VL_STRING,
  MAP_INT_BLOB,
  VL_MAP_INT_BLOB,
  MAP_INT_UUID,
  VL_MAP_INT_UUID,
  // maps with fixed-length string keys
  MAP_STRING_BOOL,
  VL_MAP_STRING_BOOL,
  MAP_STRING_INT,
  VL_MAP_STRING_INT,
  MAP_STRING_FLOAT,
  VL_MAP_STRING_FLOAT,
  MAP_STRING_DOUBLE,
  VL_MAP_STRING_DOUBLE,
  MAP_STRING_STRING,
  VL_MAP_STRING_STRING,
  MAP_STRING_VL_STRING,
  VL_MAP_STRING_VL_STRING,
  MAP_STRING_BLOB,
  VL_MAP_STRING_BLOB,
  MAP_STRING_UUID,
  VL_MAP_STRING_UUID,
  // maps with variable length string keys
  MAP_VL_STRING_BOOL,
  VL_MAP_VL_STRING_BOOL,
  MAP_VL_STRING_INT,
  VL_MAP_VL_STRING_INT,
  MAP_VL_STRING_FLOAT,
  VL_MAP_VL_STRING_FLOAT,
  MAP_VL_STRING_DOUBLE,
  VL_MAP_VL_STRING_DOUBLE,
  MAP_VL_STRING_STRING,
  VL_MAP_VL_STRING_STRING,
  MAP_VL_STRING_VL_STRING,
  VL_MAP_VL_STRING_VL_STRING,
  MAP_VL_STRING_BLOB,
  VL_MAP_VL_STRING_BLOB,
  MAP_VL_STRING_UUID,
  VL_MAP_VL_STRING_UUID,
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
    if (row >= rows.size()) {
      throw KeyError("index larger than number of query rows");
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

/// Interface implemented by backends that support rudimentary querying.
class QueryableBackend {
 public:
  virtual ~QueryableBackend() {}

  /// Return a set of rows from the specificed table that match all given
  /// conditions.  Conditions are AND'd together.  conds may be NULL.
  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) = 0;
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

  inline void Update(const std::set<int>& x) {
    std::set<int>::iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(&(*it), sizeof(int));
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

  inline void Update(const std::list<std::string>& x) {
    std::list<std::string>::const_iterator it = x.begin();
    for (; it != x.end(); ++it)
      hash_.process_bytes(it->c_str(), it->size());
  }

  inline void Update(const std::pair<int, int>& x) {
    hash_.process_bytes(&(x.first), sizeof(int));
    hash_.process_bytes(&(x.second), sizeof(int));
  }

  inline void Update(const std::map<int, int>& x) {
    std::map<int, int>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), sizeof(int));
      hash_.process_bytes(&(it->second), sizeof(int));
    }
  }

  inline void Update(const std::map<int, double>& x) {
    std::map<int, double>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(&(it->first), sizeof(int));
      hash_.process_bytes(&(it->second), sizeof(double));
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

  inline void Update(const std::map<std::string, std::string>& x) {
    std::map<std::string, std::string>::const_iterator it = x.begin();
    for (; it != x.end(); ++it) {
      hash_.process_bytes(it->first.c_str(), it->first.size());
      hash_.process_bytes(it->second.c_str(), it->second.size());
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
