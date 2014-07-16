#ifndef CYCLUS_SRC_DATUM_H_
#define CYCLUS_SRC_DATUM_H_

#include <list>
#include <string>
#include <vector>

#include "any.hpp"
#include "recorder.h"

namespace cyclus {

/// Used to specify and send a collection of key-value pairs to the
/// Recorder for recording.
class Datum {
  friend class Recorder;

 public:
  typedef std::pair<const char*, boost::spirit::hold_any> Entry;
  typedef std::vector<Entry> Vals;
  typedef std::vector<int> Shape;
  typedef std::vector<Shape> Shapes;

  virtual ~Datum();

  /// Add an arbitrary field-value pair to the datum.
  ///
  /// @param field a label or key for a value. Loosely analogous to a column
  /// label.
  ///
  /// @param val a value of any type (int, bool, string, vector) supported by
  /// the backends being used.
  ///
  /// @param shape a pointer to a vector of ints that represents the maximum
  /// shape for this field. This is only useful for variable length data types
  /// such as string and blob. If a shape is provided, this field and value
  /// is interpreted as having a fixed length (of the value given). If the
  /// pointer is NULL or the entry is less than one (<1), the field is interpreted
  /// as inherently variable length, which may affect persistance. This is a
  /// vector of ints (a shape) rather than an int (a length) to accomodate
  /// nested data types, such as a vector of vectors of doubles or a map of
  /// strings to ints.  Use NULL as the shape for fixed length data types such
  /// as int, float, double, etc.
  ///
  /// @warning for the val argument - what variable types are supported
  /// depends on what the backend(s) in use are designed to handle.
  Datum* AddVal(const char* field, boost::spirit::hold_any val,
                std::vector<int>* shape = NULL);

  /// Record this datum to its Recorder. Recorded Datum objects of the same
  /// title (e.g. same table) must not contain any fields that were not
  /// present in the first datum recorded of that title.
  void Record();

  /// Returns the datum's title as specified during the datum's creation.
  std::string title();

  /// Returns a vector of all field-value pairs that have been added to this datum.
  const Vals& vals();

  /// Returns a vector of all shapes (pointers to vectors of ints) that have been
  /// added to this datum. The length of shapes must match the length of vals.
  const Shapes& shapes();

  static void* operator new(size_t size);
  static void operator delete(void* rawMemory) throw();

 private:
  /// Datum objects should generally not be created using a constructor (i.e.
  /// use the recorder interface).
  Datum(Recorder* m, std::string title);

  Recorder* manager_;
  std::string title_;
  Vals vals_;
  Shapes shapes_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_DATUM_H_
