/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "modifier_ng/appearance/rs_use_union_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSUseUnionModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F(RSUseUnionModifierNGTypeTest, GetTypeTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSUseUnionModifier>();
    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::USE_UNION);
    EXPECT_NE(modifier->GetType(), ModifierNG::RSModifierType::INVALID);
    EXPECT_NE(modifier->GetType(), ModifierNG::RSModifierType::BOUNDS);
    EXPECT_NE(modifier->GetType(), ModifierNG::RSModifierType::ALPHA);
    EXPECT_NE(modifier->GetType(), ModifierNG::RSModifierType::FRAME);
}

HWTEST_F(RSUseUnionModifierNGTypeTest, SetGetUseUnionTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSUseUnionModifier>();

    EXPECT_EQ(modifier->GetUseUnion(), false);

    modifier->SetUseUnion(true);
    EXPECT_EQ(modifier->GetUseUnion(), true);

    modifier->SetUseUnion(false);
    EXPECT_EQ(modifier->GetUseUnion(), false);
}

HWTEST_F(RSUseUnionModifierNGTypeTest, SetGetUnionSpacingTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSUseUnionModifier>();

    EXPECT_FLOAT_EQ(modifier->GetUnionSpacing(), 0.f);

    modifier->SetUnionSpacing(0.5f);
    EXPECT_FLOAT_EQ(modifier->GetUnionSpacing(), 0.5f);

    modifier->SetUnionSpacing(1.0f);
    EXPECT_FLOAT_EQ(modifier->GetUnionSpacing(), 1.0f);

    modifier->SetUnionSpacing(0.f);
    EXPECT_FLOAT_EQ(modifier->GetUnionSpacing(), 0.f);
}

HWTEST_F(RSUseUnionModifierNGTypeTest, SetGetSDFUnionModeTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSUseUnionModifier>();

    EXPECT_EQ(modifier->GetSDFUnionMode(), 0);

    modifier->SetSDFUnionMode(1);
    EXPECT_EQ(modifier->GetSDFUnionMode(), 1);

    modifier->SetSDFUnionMode(0);
    EXPECT_EQ(modifier->GetSDFUnionMode(), 0);

    modifier->SetSDFUnionMode(2);
    EXPECT_EQ(modifier->GetSDFUnionMode(), 2);
}

HWTEST_F(RSUseUnionModifierNGTypeTest, SetGetGravityPullCenterFlagTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSUseUnionModifier>();

    EXPECT_EQ(modifier->GetGravityPullCenterFlag(), false);

    modifier->SetGravityPullCenterFlag(true);
    EXPECT_EQ(modifier->GetGravityPullCenterFlag(), true);

    modifier->SetGravityPullCenterFlag(false);
    EXPECT_EQ(modifier->GetGravityPullCenterFlag(), false);
}

HWTEST_F(RSUseUnionModifierNGTypeTest, SetGetGravityPullStrengthTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSUseUnionModifier>();

    EXPECT_FLOAT_EQ(modifier->GetGravityPullStrength(), 0.f);

    modifier->SetGravityPullStrength(0.5f);
    EXPECT_FLOAT_EQ(modifier->GetGravityPullStrength(), 0.5f);

    modifier->SetGravityPullStrength(1.0f);
    EXPECT_FLOAT_EQ(modifier->GetGravityPullStrength(), 1.0f);

    modifier->SetGravityPullStrength(0.f);
    EXPECT_FLOAT_EQ(modifier->GetGravityPullStrength(), 0.f);
}

HWTEST_F(RSUseUnionModifierNGTypeTest, SetGetGravityHotZoneTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSUseUnionModifier>();

    EXPECT_FLOAT_EQ(modifier->GetGravityHotZone(), 0.f);

    modifier->SetGravityHotZone(0.5f);
    EXPECT_FLOAT_EQ(modifier->GetGravityHotZone(), 0.5f);

    modifier->SetGravityHotZone(1.0f);
    EXPECT_FLOAT_EQ(modifier->GetGravityHotZone(), 1.0f);

    modifier->SetGravityHotZone(0.f);
    EXPECT_FLOAT_EQ(modifier->GetGravityHotZone(), 0.f);
}

/**
 * @tc.name: SetGetUnionSpacingBoundaryTest
 * @tc.desc: Test boundary values for UnionSpacing
 * @tc.type: FUNC
 */
HWTEST_F(RSUseUnionModifierNGTypeTest, SetGetUnionSpacingBoundaryTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSUseUnionModifier>();

    modifier->SetUnionSpacing(-1.0f);
    EXPECT_FLOAT_EQ(modifier->GetUnionSpacing(), -1.0f);

    modifier->SetUnionSpacing(100.0f);
    EXPECT_FLOAT_EQ(modifier->GetUnionSpacing(), 100.0f);
}

/**
 * @tc.name: SetGetGravityPullStrengthBoundaryTest
 * @tc.desc: Test boundary values for GravityPullStrength
 * @tc.type: FUNC
 */
HWTEST_F(RSUseUnionModifierNGTypeTest, SetGetGravityPullStrengthBoundaryTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSUseUnionModifier>();

    modifier->SetGravityPullStrength(-0.5f);
    EXPECT_FLOAT_EQ(modifier->GetGravityPullStrength(), -0.5f);

    modifier->SetGravityPullStrength(50.0f);
    EXPECT_FLOAT_EQ(modifier->GetGravityPullStrength(), 50.0f);
}

/**
 * @tc.name: SetGetGravityHotZoneBoundaryTest
 * @tc.desc: Test boundary values for GravityHotZone
 * @tc.type: FUNC
 */
HWTEST_F(RSUseUnionModifierNGTypeTest, SetGetGravityHotZoneBoundaryTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSUseUnionModifier>();

    modifier->SetGravityHotZone(-1.0f);
    EXPECT_FLOAT_EQ(modifier->GetGravityHotZone(), -1.0f);

    modifier->SetGravityHotZone(200.0f);
    EXPECT_FLOAT_EQ(modifier->GetGravityHotZone(), 200.0f);
}
} // namespace OHOS::Rosen