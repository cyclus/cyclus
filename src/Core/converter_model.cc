// converter_model.cc
// Implements the ConverterModel class

#include "converter_model.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ConverterModel::ConverterModel() {
  SetModelType("Converter");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Ptr ConverterModel::Convert(Message::Ptr convMsg,
                                     Message::Ptr refMsg) {
  // converters should override this method, unless they're very naiive.
  // generally, a converter's convert behavior should be to
  // return the amount of the commodity in the convMsg to the units
  // of the commodity in the refMsg. Perhaps it will need to reference
  // the information (like the composition of materials) in each message.
  // by default, it will return an unconverted convMsg
  return convMsg;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string ConverterModel::str() {
  return Model::str();
};

} // namespace cyclus
