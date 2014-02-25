// datum.h
#ifndef CYCLUS_SRC_DATUM_H_
#define CYCLUS_SRC_DATUM_H_

#include <list>
#include <string>

#include <boost/pool/singleton_pool.hpp>

#include "any.hpp"
#include "recorder.h"

namespace cyclus {

/*!
Used to specify and send a collection of key-value pairs to the
Recorder for recording.
*/
class Datum {
  friend class Recorder;

 public:
  typedef std::pair<const char*, boost::spirit::hold_any> Entry;
  typedef std::vector<Entry> Vals;

  virtual ~Datum();

  /*!
  Add an arbitrary field-value pair to the datum.

  @param field a label or key for a value. Loosely analogous to a column
  label.

  @param val a value of any type (int, bool, string, vector) supported by
  the backends being used.

  @warning for the val argument - what variable types are supported
  depends on what the backend(s) in use are designed to handle.
  */
  Datum* AddVal(const char* field, boost::spirit::hold_any val);

  /*!
  Record this datum to its Recorder. Recorded Datum objects of the same
  title (e.g. same table) must not contain any fields that were not
  present in the first datum recorded of that title.
  */
  void Record();

  /// Returns the datum's title as specified during the datum's creation.
  std::string title();

  /// Returns a map of all field-value pairs that have been added to this datum.
  const Vals& vals();

  static void* operator new(size_t size);
  static void operator delete(void* rawMemory) throw();

 private:
  /// Datum objects should generally not be created using a constructor (i.e.
  /// use the recorder interface).
  Datum(Recorder* m, std::string title);

  Recorder* manager_;
  std::string title_;
  Vals vals_;
};
}  // namespace cyclus
#endif  // CYCLUS_SRC_DATUM_H_
