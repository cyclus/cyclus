#if !defined _TESTCONVERTER_H_
#define _TESTCONVERTER_H_

#include "converter_model.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Converter, for testing
class TestConverter: public cyclus::ConverterModel {
 public:
  TestConverter(){};
  virtual ~TestConverter(){};
};

#endif
