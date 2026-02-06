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
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui/rs_effect_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

void InitWaveGradientMask(std::shared_ptr<RSNGWaveGradientMask>& waveGradientMask)
{
    if (!waveGradientMask) {
        return;
    }
    // WaveCenter: wave center position
    waveGradientMask->Setter<WaveGradientMaskWaveCenterTag>(Vector2f{0.5f, 0.5f});
    // WaveWidth: wave width
    waveGradientMask->Setter<WaveGradientMaskWaveWidthTag>(2.5f);
    // PropagationRadius: propagation radius
    waveGradientMask->Setter<WaveGradientMaskPropagationRadiusTag>(5.0f);
    // BlurRadius: blur radius
    waveGradientMask->Setter<WaveGradientMaskBlurRadiusTag>(2.5f);
    // TurbulenceStrength: turbulence strength
    waveGradientMask->Setter<WaveGradientMaskTurbulenceStrengthTag>(0.5f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Boundary values
const std::vector<float> waveBlurRadii = {0.0f, 1.25f, 2.5f, 5.0f};
const std::vector<float> waveWidths = {0.0f, 1.25f, 2.5f, 5.0f};
const std::vector<float> wavePropagationRadii = {0.0f, 2.5f, 5.0f, 10.0f};
const std::vector<float> waveTurbulenceStrengths = {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f};

// Extreme values for robustness testing
const std::vector<float> waveExtremeRadii = {
    -1.0f,         // Negative
    -10.0f,        // Large negative
    6.0f,          // Above max (5.0)
    100.0f,        // Extremely large
    9999.0f        // Astronomical
};

const std::vector<float> waveExtremeStrengths = {
    -2.0f,         // Below min (-1.0)
    -10.0f,        // Large negative
    2.0f,          // Above max (1.0)
    100.0f,        // Extremely large
    std::numeric_limits<float>::quiet_NaN()  // NaN
};
}

class NGMaskWaveGradientTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const std::shared_ptr<RSNGWaveGradientMask>& waveGradientMask)
    {
        const size_t sizeX = SCREEN_WIDTH;
        const size_t sizeY = SCREEN_HEIGHT;

        auto parentNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, sizeX, sizeY});
        parentNode->SetFrame({0, 0, sizeX, sizeY});
        GetRootNode()->AddChild(parentNode);
        RegisterNode(parentNode);

        auto effectNode = RSEffectNode::Create();
        effectNode->SetBounds({0, 0, sizeX, sizeY});
        effectNode->SetFrame({0, 0, sizeX, sizeY});
        effectNode->SetBackgroundColor(0xffff0000);
        parentNode->AddChild(effectNode);
        RegisterNode(effectNode);

        auto harmoniumEffect = std::make_shared<RSNGHarmoniumEffect>();
        harmoniumEffect->Setter<HarmoniumEffectMaskTag>(std::static_pointer_cast<RSNGMaskBase>(waveGradientMask));
        harmoniumEffect->Setter<HarmoniumEffectDistortProgressTag>(1.0f);

        auto effectChildNode = RSCanvasNode::Create();
        effectChildNode->SetBounds({0, 0, sizeX, sizeY});
        effectChildNode->SetFrame({0, 0, sizeX, sizeY});
        effectChildNode->SetBackgroundNGShader(harmoniumEffect);
        effectNode->AddChild(effectChildNode);
        RegisterNode(effectChildNode);
    }
};

/*
 * Test wave gradient mask with boundary values
 */
GRAPHIC_TEST(NGMaskWaveGradientTest, EFFECT_TEST, Set_Wave_Gradient_Mask_Boundary_Test)
{
    for (size_t i = 0; i < waveBlurRadii.size(); i++) {
        auto waveGradientMask = std::make_shared<RSNGWaveGradientMask>();
        InitWaveGradientMask(waveGradientMask);
        waveGradientMask->Setter<WaveGradientMaskWaveCenterTag>(Vector2f{0.5f, 0.5f});
        waveGradientMask->Setter<WaveGradientMaskBlurRadiusTag>(waveBlurRadii[i]);
        waveGradientMask->Setter<WaveGradientMaskWaveWidthTag>(2.5f);
        waveGradientMask->Setter<WaveGradientMaskPropagationRadiusTag>(5.0f);
        waveGradientMask->Setter<WaveGradientMaskTurbulenceStrengthTag>(0.5f);

        SetUpTestNode(waveGradientMask);
    }
}

/*
 * Test wave gradient mask with turbulence strength
 */
GRAPHIC_TEST(NGMaskWaveGradientTest, EFFECT_TEST, Set_Wave_Gradient_Mask_Turbulence_Strength_Test)
{
    for (size_t i = 0; i < waveTurbulenceStrengths.size(); i++) {
        auto waveGradientMask = std::make_shared<RSNGWaveGradientMask>();
        InitWaveGradientMask(waveGradientMask);
        waveGradientMask->Setter<WaveGradientMaskWaveCenterTag>(Vector2f{0.5f, 0.5f});
        waveGradientMask->Setter<WaveGradientMaskBlurRadiusTag>(2.5f);
        waveGradientMask->Setter<WaveGradientMaskWaveWidthTag>(2.5f);
        waveGradientMask->Setter<WaveGradientMaskPropagationRadiusTag>(5.0f);
        waveGradientMask->Setter<WaveGradientMaskTurbulenceStrengthTag>(waveTurbulenceStrengths[i]);

        SetUpTestNode(waveGradientMask);
    }
}

/*
 * Test wave gradient mask with extreme radius values
 * Tests malicious inputs: negative, out-of-range, extremely large values
 */
GRAPHIC_TEST(NGMaskWaveGradientTest, EFFECT_TEST, Set_Wave_Gradient_Mask_Radius_Extreme_Values_Test)
{
    for (size_t i = 0; i < waveExtremeRadii.size(); i++) {
        auto waveGradientMask = std::make_shared<RSNGWaveGradientMask>();
        InitWaveGradientMask(waveGradientMask);
        waveGradientMask->Setter<WaveGradientMaskWaveCenterTag>(Vector2f{0.5f, 0.5f});
        waveGradientMask->Setter<WaveGradientMaskBlurRadiusTag>(waveExtremeRadii[i]);
        waveGradientMask->Setter<WaveGradientMaskWaveWidthTag>(2.5f);
        waveGradientMask->Setter<WaveGradientMaskPropagationRadiusTag>(5.0f);
        waveGradientMask->Setter<WaveGradientMaskTurbulenceStrengthTag>(0.5f);

        SetUpTestNode(waveGradientMask);
    }
}

/*
 * Test wave gradient mask with extreme turbulence strength values
 * Tests malicious inputs: below min, above max, extremely large, NaN
 */
GRAPHIC_TEST(NGMaskWaveGradientTest, EFFECT_TEST, Set_Wave_Gradient_Mask_Turbulence_Extreme_Values_Test)
{
    for (size_t i = 0; i < waveExtremeStrengths.size(); i++) {
        auto waveGradientMask = std::make_shared<RSNGWaveGradientMask>();
        InitWaveGradientMask(waveGradientMask);
        waveGradientMask->Setter<WaveGradientMaskWaveCenterTag>(Vector2f{0.5f, 0.5f});
        waveGradientMask->Setter<WaveGradientMaskBlurRadiusTag>(2.5f);
        waveGradientMask->Setter<WaveGradientMaskWaveWidthTag>(2.5f);
        waveGradientMask->Setter<WaveGradientMaskPropagationRadiusTag>(5.0f);
        waveGradientMask->Setter<WaveGradientMaskTurbulenceStrengthTag>(waveExtremeStrengths[i]);

        SetUpTestNode(waveGradientMask);
    }
}

} // namespace OHOS::Rosen
