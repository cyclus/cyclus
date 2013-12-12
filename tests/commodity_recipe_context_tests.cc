#include <gtest/gtest.h>

#include "commodity_recipe_context.h"

namespace cyclus {

TEST(CommodRecCtx, All) {
  std::string in_c = "in_c";
  std::string out_c = "out_c";
  std::string in_r = "in_r";
  std::string out_r = "out_r";

  std::string in_r2 = "in_r2";
  
  Material::Ptr mat = Material::CreateBlank(42);

  CommodityRecipeContext ctx;
  ctx.AddInCommod(in_c, in_r, out_c, out_r);
  EXPECT_EQ(ctx.out_commod(in_c), out_c);
  EXPECT_EQ(ctx.in_recipe(in_c), in_r);
  EXPECT_EQ(ctx.out_recipe(in_r), out_r);

  ctx.UpdateInRec(in_c, in_r2);
  EXPECT_EQ(ctx.in_recipe(in_c), in_r2);
  EXPECT_EQ(ctx.out_recipe(in_r), out_r);
  EXPECT_EQ(ctx.out_recipe(in_r2), out_r);

  ctx.AddMat(in_c, mat);
  EXPECT_EQ(ctx.commod(mat), in_c);
  ctx.UpdateMat(out_c, mat);
  EXPECT_EQ(ctx.commod(mat), out_c);
  ctx.RemoveMat(mat);
  EXPECT_EQ(ctx.commod(mat), "");
  
}

} // namespace cyclus
