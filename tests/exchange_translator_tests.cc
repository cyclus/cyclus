#include <gtest/gtest.h>

#include "exchange_translator.h"
#include "material.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, Truth) {
  cyclus::ExchangeContext<cyclus::Material> ctx;
  cyclus::ExchangeTranslator<cyclus::Material> xlator(&ctx);
}
