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

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/AllWhite.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
const int NODE_SIZE = 259;
} // namespace

class NGShaderCosmosRingTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

GRAPHIC_TEST(NGShaderCosmosRingTest, EFFECT_TEST, Set_Cosmos_Ring_Test)
{
    // ---- CosmosRing main ring shader ----
    auto ringShader = std::make_shared<RSNGCosmosRing>();
    ringShader->Setter<CosmosRingRadiusTag>(87.f);
    ringShader->Setter<CosmosRingWidthTag>(30.f);
    ringShader->Setter<CosmosRingFeatherTag>(14.7f);
    ringShader->Setter<CosmosRingNoiseDensityTag>(25.5f);
    ringShader->Setter<CosmosRingNoisePeriodTag>(10);
    ringShader->Setter<CosmosRingNoiseContrastTag>(2.56f);
    ringShader->Setter<CosmosRingNoise2DensityTag>(10.5f);
    ringShader->Setter<CosmosRingNoise2PeriodTag>(7);
    ringShader->Setter<CosmosRingNoise2ContrastTag>(1.28f);
    ringShader->Setter<CosmosRingNoiseSeedTag>(29);
    ringShader->Setter<CosmosRingChromaStrengthTag>(0.69f);
    ringShader->Setter<CosmosRingMidColorTag>(
        Vector4f(0x0B / 255.0f, 0x5C / 255.0f, 0xFF / 255.0f, 1.0f));
    ringShader->Setter<CosmosRingDarkColorTag>(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
    ringShader->Setter<CosmosRingBrightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    ringShader->Setter<CosmosRingHlEnableTag>(true);
    ringShader->Setter<CosmosRingHlOuterRadiusTag>(97.f);
    ringShader->Setter<CosmosRingHlInnerRadiusTag>(78.f);
    ringShader->Setter<CosmosRingHlOuterWidthTag>(8.33f);
    ringShader->Setter<CosmosRingHlInnerWidthTag>(3.75f);
    ringShader->Setter<CosmosRingHlDispersionStrengthTag>(0.32f);
    ringShader->Setter<CosmosRingHlWholeBrightnessTag>(2.26f);
    ringShader->Setter<CosmosRingHlRotationSpeedTag>(100.f);
    ringShader->Setter<CosmosRingHlP1OuterCenterAngleTag>(204.f);
    ringShader->Setter<CosmosRingHlP1OuterArcSpanTag>(220.f);
    ringShader->Setter<CosmosRingHlP1OuterBrightnessTag>(2.f);
    ringShader->Setter<CosmosRingHlP1OuterDispScalesTag>(Vector3f(1.5f, 0.f, 1.5f));
    ringShader->Setter<CosmosRingHlP1InnerCenterAngleTag>(270.f);
    ringShader->Setter<CosmosRingHlP1InnerArcSpanTag>(260.f);
    ringShader->Setter<CosmosRingHlP1InnerBrightnessTag>(1.3f);
    ringShader->Setter<CosmosRingHlP1InnerDispScalesTag>(Vector3f(1.f, 0.f, 1.f));
    ringShader->Setter<CosmosRingHlP2OuterCenterAngleTag>(75.f);
    ringShader->Setter<CosmosRingHlP2OuterArcSpanTag>(210.f);
    ringShader->Setter<CosmosRingHlP2OuterBrightnessTag>(1.4f);
    ringShader->Setter<CosmosRingHlP2OuterDispScalesTag>(Vector3f(1.2f, 0.f, 1.2f));
    ringShader->Setter<CosmosRingHlP2InnerCenterAngleTag>(70.f);
    ringShader->Setter<CosmosRingHlP2InnerArcSpanTag>(190.f);
    ringShader->Setter<CosmosRingHlP2InnerBrightnessTag>(1.4f);
    ringShader->Setter<CosmosRingHlP2InnerDispScalesTag>(Vector3f(1.f, 0.f, 1.f));
    ringShader->Setter<CosmosRingHlRadialFeatherPxTag>(10.f);
    ringShader->Setter<CosmosRingHlTangentFeatherDegTag>(50.f);
    ringShader->Setter<CosmosRingHlSpindleStrengthTag>(0.8f);
    ringShader->Setter<CosmosRingHlSpindlePowerTag>(2.f);
    ringShader->Setter<CosmosRingHlAngularFadeTag>(0.3f);
    ringShader->Setter<CosmosRingGlowRadiusTag>(Vector2f(18.f, 90.f));
    ringShader->Setter<CosmosRingGlowStrengthTag>(Vector2f(0.12f, 0.43f));
    ringShader->Setter<CosmosRingGlowThresholdTag>(Vector2f(-0.26f, 0.f));
    ringShader->Setter<CosmosRingGlowIntensityTag>(0.41f);
    ringShader->Setter<CosmosRingGlowSaturationTag>(1.f);
    ringShader->Setter<CosmosRingGlowExposureTag>(6.59f);
    ringShader->Setter<CosmosRingProgressTag>(0.5f);
    ringShader->Setter<CosmosRingParticleEnableTag>(true);
    ringShader->Setter<CosmosRingParticleCountTag>(65.f);
    ringShader->Setter<CosmosRingParticleSizeTag>(3.7f);
    ringShader->Setter<CosmosRingSizeVariationTag>(0.35f);
    ringShader->Setter<CosmosRingDispersionStrengthTag>(0.01f);
    ringShader->Setter<CosmosRingScaleTag>(1.f);
    ringShader->Setter<CosmosRingYPositionTag>(0.f);
    ringShader->Setter<CosmosRingInnerRadiusTag>(65.f);
    ringShader->Setter<CosmosRingOuterRadiusTag>(113.f);
    ringShader->Setter<CosmosRingSpeedTag>(45.f);
    ringShader->Setter<CosmosRingTintStrengthTag>(0.55f);
    ringShader->Setter<CosmosRingBrightnessTag>(2.04f);
    ringShader->Setter<CosmosRingOpacityTag>(1.f);
    ringShader->Setter<CosmosRingFadePowerTag>(3.f);
    ringShader->Setter<CosmosRingFadeMinTag>(0.3f);
    ringShader->Setter<CosmosRingBirthPeakTag>(0.15f);
    ringShader->Setter<CosmosRingSeedTag>(50.f);

    // Background node (full screen, white image)
    auto bgNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    GetRootNode()->AddChild(bgNode);
    RegisterNode(bgNode);

    // Effect node: 259x259, centered on the background
    const float posX = (SCREEN_WIDTH - NODE_SIZE) * 0.5f;
    const float posY = (SCREEN_HEIGHT - NODE_SIZE) * 0.5f;
    auto effectNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
    effectNode->SetBounds(posX, posY, NODE_SIZE, NODE_SIZE);
    effectNode->SetFrame(posX, posY, NODE_SIZE, NODE_SIZE);
    effectNode->SetBackgroundNGShader(ringShader);
    GetRootNode()->AddChild(effectNode);
    RegisterNode(effectNode);
}
} // namespace OHOS::Rosen