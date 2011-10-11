// ConverterModel.cpp
// Implements the ConverterModel class

#include "ConverterModel.h"
#include "Logician.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConverterModel::init(xmlNodePtr cur) {
  Model::init(cur);
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConverterModel::copy(ConverterModel* src) { 
  Model::copy(src); 

  // don't copy conv_name to new instance
  conv_name_ = "";
  LI->addModel(this, CONVERTER);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConverterModel::handleTick(int time) {
  // converters might someday override this method
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ConverterModel::handleTock(int time) {
  // converters might someday override this method
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message* ConverterModel::convert(Message* convMsg, Message* refMsg) {
  // converters should override this method, unless they're very naiive.
  // generally, a converter's convert behavior should be to 
  // return the amount of the commodity in the convMsg to the units
  // of the commodity in the refMsg. Perhaps it will need to reference
  // the information (like the composition of materials) in each message.
  // by default, it will return an unconverted convMsg
  return convMsg;
}

