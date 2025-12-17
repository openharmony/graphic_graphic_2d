/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "font_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int FONT_WEIGHT_W100 = 100;
constexpr int FONT_WEIGHT_W200 = 200;
constexpr int FONT_WEIGHT_W400 = 400;
constexpr int FONT_WEIGHT_W700 = 700;
constexpr int FONT_WEIGHT_W900 = 900;
constexpr int FONT_WEIGHT_ENUM_W400 = 3;
constexpr int FONT_WEIGHT_ENUM_W900 = 8;
constexpr int FONT_WEIGHT_ENUM_W700 = 6;
constexpr int INVALID_FONT_WEIGHT1 = 50;
constexpr int INVALID_FONT_WEIGHT2 = 350;
constexpr int INVALID_FONT_WEIGHT3 = 1000;
constexpr int INVALID_FONT_WEIGHT4 = 975;
constexpr int INVALID_FONT_WEIGHT5 = 725;
} // namespace
class MlbFontUtilsTest : public testing::Test {};
/**
 * @tc.name: MlbRegularWeightTest
 * @tc.desc: test regular weight by diffierent weight value
 * @tc.type: FUNC
 */
HWTEST_F(MlbFontUtilsTest, MlbRegularWeightTest, TestSize.Level0)
{
    // Test invalid font weight: Below minimum → clamp to 100
    EXPECT_EQ(MLB::RegularWeight(INVALID_FONT_WEIGHT1), FONT_WEIGHT_W100);
    EXPECT_EQ(MLB::RegularWeight(0), FONT_WEIGHT_W100);
    // Test invalid font weight: Above maximum → clamp to 900
    EXPECT_EQ(MLB::RegularWeight(INVALID_FONT_WEIGHT3), FONT_WEIGHT_W900);
    // Test invalid font weight: Above maximum with rounding
    EXPECT_EQ(MLB::RegularWeight(INVALID_FONT_WEIGHT4), FONT_WEIGHT_W900);
    // Test invalid font weight: not a multiple of 100, rounding up
    EXPECT_EQ(MLB::RegularWeight(INVALID_FONT_WEIGHT2), FONT_WEIGHT_W400);
    // Test invalid font weight: not a multiple of 100, rounding down
    EXPECT_EQ(MLB::RegularWeight(INVALID_FONT_WEIGHT5), FONT_WEIGHT_W700);
    // Test valid font weights: Exact value preserved
    EXPECT_EQ(MLB::RegularWeight(FONT_WEIGHT_W400), FONT_WEIGHT_W400);
    // Test valid font weights: Minimum boundary
    EXPECT_EQ(MLB::RegularWeight(FONT_WEIGHT_W100), FONT_WEIGHT_W100);
    // Test valid font weights: Maximum boundary
    EXPECT_EQ(MLB::RegularWeight(FONT_WEIGHT_W900), FONT_WEIGHT_W900);
}

/**
 * @tc.name: MlbFindFontWeightTest
 * @tc.desc: Test finding font weight enum by actual weight value
 * @tc.type: FUNC
 */
HWTEST_F(MlbFontUtilsTest, MlbFindFontWeightTest, TestSize.Level0)
{
    int result;
    // Valid actual weights → corresponding enum values
    EXPECT_TRUE(MLB::FindFontWeight(FONT_WEIGHT_W100, result));
    EXPECT_EQ(result, 0);

    EXPECT_TRUE(MLB::FindFontWeight(FONT_WEIGHT_W400, result));
    EXPECT_EQ(result, FONT_WEIGHT_ENUM_W400);

    EXPECT_TRUE(MLB::FindFontWeight(FONT_WEIGHT_W900, result));
    EXPECT_EQ(result, FONT_WEIGHT_ENUM_W900);

    EXPECT_TRUE(MLB::FindFontWeight(MLB::RegularWeight(INVALID_FONT_WEIGHT2), result));
    EXPECT_EQ(result, FONT_WEIGHT_ENUM_W400);

    EXPECT_TRUE(MLB::FindFontWeight(MLB::RegularWeight(INVALID_FONT_WEIGHT4), result));
    EXPECT_EQ(result, FONT_WEIGHT_ENUM_W900);

    EXPECT_TRUE(MLB::FindFontWeight(MLB::RegularWeight(INVALID_FONT_WEIGHT5), result));
    EXPECT_EQ(result, FONT_WEIGHT_ENUM_W700);

    // Invalid actual weights → returns false
    EXPECT_FALSE(MLB::FindFontWeight(0, result));
    EXPECT_FALSE(MLB::FindFontWeight(INVALID_FONT_WEIGHT1, result));
    EXPECT_FALSE(MLB::FindFontWeight(INVALID_FONT_WEIGHT2, result));
    EXPECT_FALSE(MLB::FindFontWeight(INVALID_FONT_WEIGHT3, result));
}

/**
 * @tc.name: MlbFindFontWeightEnumTest
 * @tc.desc: Test finding actual weight value by font weight enum
 * @tc.type: FUNC
 */
HWTEST_F(MlbFontUtilsTest, MlbFindFontWeightEnumTest, TestSize.Level0)
{
    int result;

    // Valid enum values → corresponding actual weights
    EXPECT_TRUE(MLB::FindFontWeightEnum(0, result));
    EXPECT_EQ(result, FONT_WEIGHT_W100);

    EXPECT_TRUE(MLB::FindFontWeightEnum(1, result));
    EXPECT_EQ(result, FONT_WEIGHT_W200);

    EXPECT_TRUE(MLB::FindFontWeightEnum(FONT_WEIGHT_ENUM_W400, result));
    EXPECT_EQ(result, FONT_WEIGHT_W400);

    EXPECT_TRUE(MLB::FindFontWeightEnum(FONT_WEIGHT_ENUM_W900, result));
    EXPECT_EQ(result, FONT_WEIGHT_W900);

    // Invalid enum values: below valid range
    EXPECT_FALSE(MLB::FindFontWeightEnum(-1, result));
    // Invalid enum values: above valid range
    EXPECT_FALSE(MLB::FindFontWeightEnum(9, result));
    // Invalid enum values: Wrong value type
    EXPECT_FALSE(MLB::FindFontWeightEnum(100, result));
}

} // namespace Rosen
} // namespace OHOS