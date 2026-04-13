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
#include "modifier_ng/appearance/rs_overlay_ng_shader_modifier.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSOverlayNGShaderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSOverlayNGShaderModifierTest
 * @tc.desc: Test Set/Get functions of RSOverlayNGShaderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSOverlayNGShaderModifier> modifier =
        std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::OVERLAY_NG_SHADER);

    modifier->SetLightIntensity(1.0f);
    EXPECT_EQ(modifier->GetLightIntensity(), 1.0f);

    Color lightColor(15, 11, 15, 16);
    modifier->SetLightColor(lightColor);
    EXPECT_EQ(modifier->GetLightColor(), lightColor);

    Vector4f lightPosition(1.0f, 2.0f, 3.0f, 4.0f);
    modifier->SetLightPosition(lightPosition);
    EXPECT_EQ(modifier->GetLightPosition(), lightPosition);

    modifier->SetIlluminatedBorderWidth(1.0f);
    EXPECT_EQ(modifier->GetIlluminatedBorderWidth(), 1.0f);

    modifier->SetIlluminatedType(1);
    EXPECT_EQ(modifier->GetIlluminatedType(), 1);

    modifier->SetBloom(1.0f);
    EXPECT_EQ(modifier->GetBloom(), 1.0f);
}

/**
 * @tc.name: RSOverlayNGShaderModifierConstructorTest001
 * @tc.desc: Test constructor of RSOverlayNGShaderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierConstructorTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);
    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::OVERLAY_NG_SHADER);
}

/**
 * @tc.name: RSOverlayNGShaderModifierOverlayShaderTest001
 * @tc.desc: Test Set/Get OverlayNGShader with valid shader
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierOverlayShaderTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    auto shader = std::make_shared<RSNGAIBarRectHalo>();
    EXPECT_NE(shader, nullptr);
    
    modifier->SetOverlayNGShader(shader);
    EXPECT_EQ(modifier->GetOverlayNGShader(), shader);
}

/**
 * @tc.name: RSOverlayNGShaderModifierOverlayShaderTest002
 * @tc.desc: Test Set/Get OverlayNGShader with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierOverlayShaderTest002, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    modifier->SetOverlayNGShader(nullptr);
    EXPECT_EQ(modifier->GetOverlayNGShader(), nullptr);
}

/**
 * @tc.name: RSOverlayNGShaderModifierOverlayShaderTest003
 * @tc.desc: Test Set/Get OverlayNGShader multiple times
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierOverlayShaderTest003, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    auto shader1 = std::make_shared<RSNGAIBarRectHalo>();
    auto shader2 = std::make_shared<RSNGAIBarRectHalo>();
    
    modifier->SetOverlayNGShader(shader1);
    EXPECT_EQ(modifier->GetOverlayNGShader(), shader1);
    
    modifier->SetOverlayNGShader(shader2);
    EXPECT_EQ(modifier->GetOverlayNGShader(), shader2);
    
    modifier->SetOverlayNGShader(nullptr);
    EXPECT_EQ(modifier->GetOverlayNGShader(), nullptr);
}

/**
 * @tc.name: RSOverlayNGShaderModifierLightIntensityTest001
 * @tc.desc: Test Set/Get LightIntensity with boundary values
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierLightIntensityTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    modifier->SetLightIntensity(0.0f);
    EXPECT_EQ(modifier->GetLightIntensity(), 0.0f);

    modifier->SetLightIntensity(10.0f);
    EXPECT_EQ(modifier->GetLightIntensity(), 10.0f);

    modifier->SetLightIntensity(100.0f);
    EXPECT_EQ(modifier->GetLightIntensity(), 100.0f);
}

/**
 * @tc.name: RSOverlayNGShaderModifierLightColorTest001
 * @tc.desc: Test Set/Get LightColor with different colors
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierLightColorTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    Color whiteColor(255, 255, 255, 255);
    modifier->SetLightColor(whiteColor);
    EXPECT_EQ(modifier->GetLightColor(), whiteColor);

    Color blackColor(0, 0, 0, 255);
    modifier->SetLightColor(blackColor);
    EXPECT_EQ(modifier->GetLightColor(), blackColor);

    Color customColor(128, 64, 32, 16);
    modifier->SetLightColor(customColor);
    EXPECT_EQ(modifier->GetLightColor(), customColor);
}

/**
 * @tc.name: RSOverlayNGShaderModifierLightPositionTest001
 * @tc.desc: Test Set/Get LightPosition with boundary values
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierLightPositionTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    Vector4f pos1(0.0f, 0.0f, 0.0f, 0.0f);
    modifier->SetLightPosition(pos1);
    EXPECT_EQ(modifier->GetLightPosition(), pos1);

    Vector4f pos2(100.0f, 200.0f, 300.0f, 400.0f);
    modifier->SetLightPosition(pos2);
    EXPECT_EQ(modifier->GetLightPosition(), pos2);

    Vector4f pos3(-50.0f, -60.0f, -70.0f, -80.0f);
    modifier->SetLightPosition(pos3);
    EXPECT_EQ(modifier->GetLightPosition(), pos3);
}

/**
 * @tc.name: RSOverlayNGShaderModifierBorderWidthTest001
 * @tc.desc: Test Set/Get IlluminatedBorderWidth with boundary values
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierBorderWidthTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    modifier->SetIlluminatedBorderWidth(0.0f);
    EXPECT_EQ(modifier->GetIlluminatedBorderWidth(), 0.0f);

    modifier->SetIlluminatedBorderWidth(5.5f);
    EXPECT_EQ(modifier->GetIlluminatedBorderWidth(), 5.5f);

    modifier->SetIlluminatedBorderWidth(20.0f);
    EXPECT_EQ(modifier->GetIlluminatedBorderWidth(), 20.0f);
}

/**
 * @tc.name: RSOverlayNGShaderModifierIlluminatedTypeTest001
 * @tc.desc: Test Set/Get IlluminatedType with different types
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierIlluminatedTypeTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    modifier->SetIlluminatedType(0);
    EXPECT_EQ(modifier->GetIlluminatedType(), 0);

    modifier->SetIlluminatedType(1);
    EXPECT_EQ(modifier->GetIlluminatedType(), 1);

    modifier->SetIlluminatedType(2);
    EXPECT_EQ(modifier->GetIlluminatedType(), 2);

    modifier->SetIlluminatedType(10);
    EXPECT_EQ(modifier->GetIlluminatedType(), 10);
}

/**
 * @tc.name: RSOverlayNGShaderModifierBloomTest001
 * @tc.desc: Test Set/Get Bloom with boundary values
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierBloomTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    modifier->SetBloom(0.0f);
    EXPECT_EQ(modifier->GetBloom(), 0.0f);

    modifier->SetBloom(1.0f);
    EXPECT_EQ(modifier->GetBloom(), 1.0f);

    modifier->SetBloom(2.5f);
    EXPECT_EQ(modifier->GetBloom(), 2.5f);

    modifier->SetBloom(100.0f);
    EXPECT_EQ(modifier->GetBloom(), 100.0f);
}

/**
 * @tc.name: RSOverlayNGShaderModifierGetTypeTest001
 * @tc.desc: Test GetType returns correct type consistently
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierGetTypeTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::OVERLAY_NG_SHADER);
    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::OVERLAY_NG_SHADER);
    EXPECT_EQ(modifier->GetType(), ModifierNG::RSOverlayNGShaderModifier::Type);
}

/**
 * @tc.name: RSOverlayNGShaderModifierCombinedTest001
 * @tc.desc: Test setting multiple properties together
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierCombinedTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    EXPECT_NE(modifier, nullptr);

    modifier->SetLightIntensity(5.0f);
    modifier->SetLightColor(Color(255, 128, 64, 255));
    modifier->SetLightPosition(Vector4f(10.0f, 20.0f, 30.0f, 40.0f));
    modifier->SetIlluminatedBorderWidth(3.5f);
    modifier->SetIlluminatedType(2);
    modifier->SetBloom(1.5f);

    auto shader = std::make_shared<RSNGAIBarRectHalo>();
    modifier->SetOverlayNGShader(shader);

    EXPECT_EQ(modifier->GetLightIntensity(), 5.0f);
    EXPECT_EQ(modifier->GetLightColor(), Color(255, 128, 64, 255));
    EXPECT_EQ(modifier->GetLightPosition(), Vector4f(10.0f, 20.0f, 30.0f, 40.0f));
    EXPECT_EQ(modifier->GetIlluminatedBorderWidth(), 3.5f);
    EXPECT_EQ(modifier->GetIlluminatedType(), 2);
    EXPECT_EQ(modifier->GetBloom(), 1.5f);
    EXPECT_EQ(modifier->GetOverlayNGShader(), shader);
}
} // namespace OHOS::Rosen