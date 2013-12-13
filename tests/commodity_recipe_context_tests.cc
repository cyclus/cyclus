#include <gtest/gtest.h>

#include "material.h"

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

  ctx.AddRsrc(in_c, mat);
  EXPECT_EQ(ctx.commod(mat), in_c);
  ctx.UpdateRsrc(out_c, mat);
  EXPECT_EQ(ctx.commod(mat), out_c);
  ctx.RemoveRsrc(mat);
  EXPECT_EQ(ctx.commod(mat), "");

  CommodityRecipeContext one;
  one.AddInCommod(in_c, in_r, out_c, out_r);
  one.AddInCommod("yabba", "dabba", "dooo", "oooo");
  CommodityRecipeContext other;
  other.AddInCommod("yabba", "dabba", "dooo", "oooo");
  other.AddInCommod(in_c, in_r, out_c, out_r);
  EXPECT_EQ(one, other);

  CommodityRecipeContext other_one;
  other_one.AddInCommod(in_c, in_r, out_c, out_r);
  EXPECT_NE(one, other_one);
}

} // namespace cyclus
