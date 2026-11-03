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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "property/rs_properties.h"
#include "modifier_ng/appearance/rs_blend_render_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;
namespace OHOS::Rosen {
class RSBlendRenderModifierNGTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBlendRenderModifierNGTest::SetUpTestCase() {}
void RSBlendRenderModifierNGTest::TearDownTestCase() {}
void RSBlendRenderModifierNGTest::SetUp() {}
void RSBlendRenderModifierNGTest::TearDown() {}

/**
 * @tc.name: RSBlendRenderModifierTest
 * @tc.desc:GetType & ResetProperties
 * @tc.type:FUNC
 */
HWTEST_F(RSBlendRenderModifierNGTest, RSBlendRenderModifierTest, TestSize.Level1)
{
    RSBlendRenderModifier modifier;
    RSProperties properties;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::BLENDER);
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetColorBlendMode(), 0);
    EXPECT_EQ(properties.GetColorBlendApplyType(), 0);
    EXPECT_EQ(properties.GetFgBrightnessRates(), Vector4f());
    EXPECT_EQ(properties.GetFgBrightnessSaturation(), 0.0f);
    EXPECT_EQ(properties.GetFgBrightnessPosCoeff(), Vector4f());
    EXPECT_EQ(properties.GetFgBrightnessNegCoeff(), Vector4f());
    EXPECT_EQ(properties.GetFgBrightnessFract(), 1.f);
    EXPECT_EQ(properties.GetFgBrightnessHdr(), false);
    EXPECT_EQ(properties.GetShadowBlenderParams(), std::nullopt);
    EXPECT_EQ(properties.GetHdrDarkenBlenderParams(), std::nullopt);
    // ResetProperties now also clears ColorfulBrightnessBlender params (added by ColorfulBrightnessBlender feature)
    EXPECT_EQ(properties.GetColorfulBrightnessBlenderParams(), nullptr);
    EXPECT_FALSE(properties.IsColorfulBrightnessBlenderValid());
}

/**
 * @tc.name: RSBlendRenderModifierColorfulResetTest
 * @tc.desc: verify ResetProperties clears previously-set ColorfulBrightnessBlender params.
 *           Covers the SetColorfulBrightnessBlenderParams(std::nullopt) call added to ResetProperties.
 * @tc.type:FUNC
 */
HWTEST_F(RSBlendRenderModifierNGTest, RSBlendRenderModifierColorfulResetTest, TestSize.Level1)
{
    RSBlendRenderModifier modifier;
    RSProperties properties;
    // set colorful params, then reset and verify cleared
    auto params = std::make_unique<RSColorfulBrightnessBlenderPara>(1.0f, 0.5f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f,
        Vector3f(0.6f, 0.7f, 0.8f), Vector3f(0.9f, 0.1f, 0.2f), 0.33f, 0.88f);
    properties.SetColorfulBrightnessBlenderParams(std::move(params));
    EXPECT_TRUE(properties.IsColorfulBrightnessBlenderValid());
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetColorfulBrightnessBlenderParams(), nullptr);
    EXPECT_FALSE(properties.IsColorfulBrightnessBlenderValid());
}
} // namespace OHOS::Rosen