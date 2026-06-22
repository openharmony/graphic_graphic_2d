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

#include "modifier_ng/appearance/rs_use_union_render_modifier.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSUseUnionRenderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F(RSUseUnionRenderModifierNGTypeTest, GetTypeTest, TestSize.Level1)
{
    ModifierNG::RSUseUnionRenderModifier modifier;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::USE_UNION);
    EXPECT_NE(modifier.GetType(), ModifierNG::RSModifierType::INVALID);
    EXPECT_NE(modifier.GetType(), ModifierNG::RSModifierType::BOUNDS);
    EXPECT_NE(modifier.GetType(), ModifierNG::RSModifierType::ALPHA);
    EXPECT_NE(modifier.GetType(), ModifierNG::RSModifierType::FRAME);
}

HWTEST_F(RSUseUnionRenderModifierNGTypeTest, ResetPropertiesTest, TestSize.Level1)
{
    ModifierNG::RSUseUnionRenderModifier modifier;
    RSProperties properties;

    // Set all Union-related properties to non-default values
    properties.SetUseUnion(true);
    properties.SetUnionSpacing(0.5f);
    properties.SetSDFUnionMode(1);
    properties.SetGravityPullCenterFlag(true);
    properties.SetGravityPullStrength(0.5f);
    properties.SetGravityHotZone(0.5f);

    // Verify they are set
    EXPECT_EQ(properties.GetUseUnion(), true);
    EXPECT_EQ(properties.GetUnionSpacing(), 0.5f);
    EXPECT_EQ(properties.GetSDFUnionMode(), 1);
    EXPECT_EQ(properties.GetGravityPullCenterFlag(), true);
    EXPECT_FLOAT_EQ(properties.GetGravityPullStrength(), 0.5f);
    EXPECT_FLOAT_EQ(properties.GetGravityHotZone(), 0.5f);

    // Reset and verify defaults
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetUseUnion(), false);
    EXPECT_FLOAT_EQ(properties.GetUnionSpacing(), 0.f);
    EXPECT_EQ(properties.GetSDFUnionMode(), 0);
    EXPECT_EQ(properties.GetGravityPullCenterFlag(), false);
    EXPECT_FLOAT_EQ(properties.GetGravityPullStrength(), 0.f);
    EXPECT_FLOAT_EQ(properties.GetGravityHotZone(), 0.f);
}

HWTEST_F(RSUseUnionRenderModifierNGTypeTest, ResetPropertiesDefaultTest, TestSize.Level1)
{
    ModifierNG::RSUseUnionRenderModifier modifier;
    RSProperties properties;

    // Properties start at default values, reset should keep them at defaults
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetUseUnion(), false);
    EXPECT_FLOAT_EQ(properties.GetUnionSpacing(), 0.f);
    EXPECT_EQ(properties.GetSDFUnionMode(), 0);
    EXPECT_EQ(properties.GetGravityPullCenterFlag(), false);
    EXPECT_FLOAT_EQ(properties.GetGravityPullStrength(), 0.f);
    EXPECT_FLOAT_EQ(properties.GetGravityHotZone(), 0.f);
}
} // namespace OHOS::Rosen