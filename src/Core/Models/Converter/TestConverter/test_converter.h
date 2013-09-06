#if !defined _TESTCONVERTER_H_
#define _TESTCONVERTER_H_

#include "converter_model.h"
#include "context.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Converter, for testing
class TestConverter: public cyclus::ConverterModel {
 public:
  TestConverter(cyclus::Context* ctx) : cyclus::ConverterModel(ctx) {};
  virtual ~TestConverter() {};
};

#endif
