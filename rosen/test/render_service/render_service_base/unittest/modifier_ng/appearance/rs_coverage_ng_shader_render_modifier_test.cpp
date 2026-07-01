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

#include "common/rs_vector4.h"
#include "effect/rs_render_shader_base.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "property/rs_properties.h"
#include "modifier_ng/appearance/rs_coverage_ng_shader_render_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;
namespace OHOS::Rosen {
class RSCoverageNGShaderRenderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCoverageNGShaderRenderModifierNGTypeTest::SetUpTestCase() {}
void RSCoverageNGShaderRenderModifierNGTypeTest::TearDownTestCase() {}
void RSCoverageNGShaderRenderModifierNGTypeTest::SetUp() {}
void RSCoverageNGShaderRenderModifierNGTypeTest::TearDown() {}

/**
 * @tc.name: RSCoverageNGShaderRenderModifierTest
 * @tc.desc:GetType & ResetProperties
 * @tc.type:FUNC
 */
HWTEST_F(RSCoverageNGShaderRenderModifierNGTypeTest, RSCoverageNGShaderRenderModifierTest, TestSize.Level1)
{
    RSCoverageNGShaderRenderModifier modifier;
    RSProperties properties;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::COVERAGE_NG_SHADER);
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetLightIntensity(), 0.0f);
    EXPECT_EQ(properties.GetLightColor(), RgbPalette::White());
    EXPECT_EQ(properties.GetLightPosition(), Vector4f());
    EXPECT_EQ(properties.GetIlluminatedBorderWidth(), 0.0f);
    EXPECT_EQ(properties.GetIlluminatedType(), 0);
    EXPECT_EQ(properties.GetBloom(), 0.0f);
}

/**
 * @tc.name: RSCoverageNGShaderRenderModifierResetTest001
 * @tc.desc: ResetProperties test with preset values
 * @tc.type:FUNC
 */
HWTEST_F(RSCoverageNGShaderRenderModifierNGTypeTest, RSCoverageNGShaderRenderModifierResetTest001, TestSize.Level1)
{
    RSCoverageNGShaderRenderModifier modifier;
    RSProperties properties;
    
    properties.SetLightIntensity(5.0f);
    properties.SetLightColor(Color(255, 128, 64, 255));
    properties.SetLightPosition(Vector4f(10.0f, 20.0f, 30.0f, 40.0f));
    properties.SetIlluminatedBorderWidth(3.5f);
    properties.SetIlluminatedType(2);
    properties.SetBloom(1.5f);
    
    EXPECT_EQ(properties.GetLightIntensity(), 5.0f);
    EXPECT_EQ(properties.GetLightColor(), Color(255, 128, 64, 255));
    
    modifier.ResetProperties(properties);
    
    EXPECT_EQ(properties.GetLightIntensity(), 5.0f);  // skip while reset call
    EXPECT_EQ(properties.GetLightColor(), RgbPalette::White());
    EXPECT_EQ(properties.GetLightPosition(), Vector4f());
    EXPECT_EQ(properties.GetIlluminatedBorderWidth(), 0.0f);
    EXPECT_EQ(properties.GetIlluminatedType(), 2);
    EXPECT_EQ(properties.GetBloom(), 0.0f);
}

/**
 * @tc.name: RSCoverageNGShaderRenderModifierResetTest002
 * @tc.desc: ResetProperties test with extreme values
 * @tc.type:FUNC
 */
HWTEST_F(RSCoverageNGShaderRenderModifierNGTypeTest, RSCoverageNGShaderRenderModifierResetTest002, TestSize.Level1)
{
    RSCoverageNGShaderRenderModifier modifier;
    RSProperties properties;
    
    constexpr float maxFloat = std::numeric_limits<float>::max();
    properties.SetLightIntensity(maxFloat);
    properties.SetLightPosition(Vector4f(maxFloat, maxFloat, maxFloat, maxFloat));
    properties.SetIlluminatedBorderWidth(maxFloat);
    properties.SetBloom(maxFloat);
    
    modifier.ResetProperties(properties);
    
    EXPECT_EQ(properties.GetLightIntensity(), maxFloat); // skip while reset call
    EXPECT_EQ(properties.GetLightPosition(), Vector4f());
    EXPECT_EQ(properties.GetIlluminatedBorderWidth(), 0.0f);
    EXPECT_EQ(properties.GetBloom(), 0.0f);
}

/**
 * @tc.name: RSCoverageNGShaderRenderModifierGetTypeTest001
 * @tc.desc: GetType test multiple times
 * @tc.type:FUNC
 */
HWTEST_F(RSCoverageNGShaderRenderModifierNGTypeTest, RSCoverageNGShaderRenderModifierGetTypeTest001, TestSize.Level1)
{
    RSCoverageNGShaderRenderModifier modifier;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::COVERAGE_NG_SHADER);
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::COVERAGE_NG_SHADER);
    EXPECT_EQ(modifier.GetType(), RSCoverageNGShaderRenderModifier::Type);
}

/**
 * @tc.name: RSCoverageNGShaderRenderModifierResetTest003
 * @tc.desc: ResetProperties test with negative values
 * @tc.type:FUNC
 */
HWTEST_F(RSCoverageNGShaderRenderModifierNGTypeTest, RSCoverageNGShaderRenderModifierResetTest003, TestSize.Level1)
{
    RSCoverageNGShaderRenderModifier modifier;
    RSProperties properties;
    
    properties.SetLightIntensity(-5.0f);
    properties.SetLightPosition(Vector4f(-10.0f, -20.0f, -30.0f, -40.0f));
    properties.SetIlluminatedBorderWidth(-3.5f);
    properties.SetIlluminatedType(-1);
    properties.SetBloom(-1.5f);
    
    modifier.ResetProperties(properties);
    
    EXPECT_EQ(properties.GetLightIntensity(), -5.0f); // skip while reset call
    EXPECT_EQ(properties.GetLightPosition(), Vector4f());
    EXPECT_EQ(properties.GetIlluminatedBorderWidth(), 0.0f);
    EXPECT_EQ(properties.GetIlluminatedType(), 0);
    EXPECT_EQ(properties.GetBloom(), 0.0f);
}

/**
 * @tc.name: RSCoverageNGShaderRenderModifierCoverageShaderTest001
 * @tc.desc: ResetProperties test with coverage shader
 * @tc.type:FUNC
 */
HWTEST_F(RSCoverageNGShaderRenderModifierNGTypeTest,
    RSCoverageNGShaderRenderModifierCoverageShaderTest001, TestSize.Level1)
{
    RSCoverageNGShaderRenderModifier modifier;
    RSProperties properties;
    
    auto coverageShader = std::make_shared<RSNGRenderAIBarRectHalo>();
    EXPECT_NE(coverageShader, nullptr);
    
    properties.SetCoverageNGShader(coverageShader);
    EXPECT_EQ(properties.GetCoverageNGShader(), coverageShader);
    
    modifier.ResetProperties(properties);
    
    EXPECT_EQ(properties.GetCoverageNGShader(), nullptr);
}

/**
 * @tc.name: RSCoverageNGShaderRenderModifierCoverageShaderTest002
 * @tc.desc: ResetProperties test with multiple coverage shader operations
 * @tc.type:FUNC
 */
HWTEST_F(RSCoverageNGShaderRenderModifierNGTypeTest,
    RSCoverageNGShaderRenderModifierCoverageShaderTest002, TestSize.Level1)
{
    RSCoverageNGShaderRenderModifier modifier;
    RSProperties properties;
    
    auto shader1 = std::make_shared<RSNGRenderAIBarRectHalo>();
    auto shader2 = std::make_shared<RSNGRenderAIBarRectHalo>();
    
    properties.SetCoverageNGShader(shader1);
    EXPECT_EQ(properties.GetCoverageNGShader(), shader1);
    
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetCoverageNGShader(), nullptr);
    
    properties.SetCoverageNGShader(shader2);
    EXPECT_EQ(properties.GetCoverageNGShader(), shader2);
    
    modifier.ResetProperties(properties);
    EXPECT_EQ(properties.GetCoverageNGShader(), nullptr);
}

/**
 * @tc.name: RSCoverageNGShaderRenderModifierCombinedResetTest001
 * @tc.desc: ResetProperties test with coverage shader and other properties
 * @tc.type:FUNC
 */
HWTEST_F(RSCoverageNGShaderRenderModifierNGTypeTest,
    RSCoverageNGShaderRenderModifierCombinedResetTest001, TestSize.Level1)
{
    RSCoverageNGShaderRenderModifier modifier;
    RSProperties properties;
    
    auto coverageShader = std::make_shared<RSNGRenderAIBarRectHalo>();
    properties.SetCoverageNGShader(coverageShader);
    properties.SetLightIntensity(5.0f);
    properties.SetLightColor(Color(255, 128, 64, 255));
    properties.SetLightPosition(Vector4f(10.0f, 20.0f, 30.0f, 40.0f));
    properties.SetIlluminatedBorderWidth(3.5f);
    properties.SetIlluminatedType(2);
    properties.SetBloom(1.5f);
    
    modifier.ResetProperties(properties);
    
    EXPECT_EQ(properties.GetCoverageNGShader(), nullptr);
    EXPECT_EQ(properties.GetLightIntensity(), 5.0f); // skip while reset call
    EXPECT_EQ(properties.GetLightColor(), RgbPalette::White());
    EXPECT_EQ(properties.GetLightPosition(), Vector4f());
    EXPECT_EQ(properties.GetIlluminatedBorderWidth(), 0.0f);
    EXPECT_EQ(properties.GetIlluminatedType(), 2);
    EXPECT_EQ(properties.GetBloom(), 0.0f);
}
} // namespace OHOS::Rosen