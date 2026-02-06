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

void InitRadialGradientMask(std::shared_ptr<RSNGRadialGradientMask>& radialGradientMask)
{
    if (!radialGradientMask) {
        return;
    }
    // Center: center position
    radialGradientMask->Setter<RadialGradientMaskCenterTag>(Vector2f{0.5f, 0.5f});
    // RadiusX: horizontal radius
    radialGradientMask->Setter<RadialGradientMaskRadiusXTag>(0.5f);
    // RadiusY: vertical radius
    radialGradientMask->Setter<RadialGradientMaskRadiusYTag>(0.5f);
    // Colors: gradient colors (white to black)
    std::vector<float> colors = {1.0f, 0.5f, 0.0f};
    radialGradientMask->Setter<RadialGradientMaskColorsTag>(colors);
    // Positions: color stop positions
    std::vector<float> positions = {0.0f, 0.5f, 1.0f};
    radialGradientMask->Setter<RadialGradientMaskPositionsTag>(positions);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Center position variations
const std::vector<Vector2f> radialGradientCenters = {
    Vector2f{0.0f, 0.0f},
    Vector2f{0.5f, 0.5f},
    Vector2f{1.0f, 1.0f}
};

// Radius variations
const std::vector<float> radialGradientRadii = {0.0f, 0.25f, 0.5f, 1.0f};

// Colors and positions
const std::vector<float> radialGradientColors = {1.0f, 0.5f, 0.0f};
const std::vector<float> radialGradientPositions = {0.0f, 0.5f, 1.0f};

// Extreme values
const std::vector<float> radialGradientExtremeRadii = {
    -1.0f,         // Negative
    -10.0f,        // Large negative
    2.0f,          // Above max
    100.0f,        // Extremely large
    9999.0f        // Astronomical
};
}

class NGMaskRadialGradientTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const std::shared_ptr<RSNGRadialGradientMask>& radialGradientMask)
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
        harmoniumEffect->Setter<HarmoniumEffectMaskTag>(std::static_pointer_cast<RSNGMaskBase>(radialGradientMask));
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
 * Test radial gradient mask with different centers and radii
 */
GRAPHIC_TEST(NGMaskRadialGradientTest, EFFECT_TEST, Set_Radial_Gradient_Mask_Center_Radius_Test)
{
    for (size_t i = 0; i < radialGradientCenters.size(); i++) {
        auto radialGradientMask = std::make_shared<RSNGRadialGradientMask>();
        InitRadialGradientMask(radialGradientMask);
        radialGradientMask->Setter<RadialGradientMaskCenterTag>(radialGradientCenters[i]);
        radialGradientMask->Setter<RadialGradientMaskRadiusXTag>(radialGradientRadii[i]);
        radialGradientMask->Setter<RadialGradientMaskRadiusYTag>(radialGradientRadii[i]);
        radialGradientMask->Setter<RadialGradientMaskColorsTag>(radialGradientColors);
        radialGradientMask->Setter<RadialGradientMaskPositionsTag>(radialGradientPositions);

        SetUpTestNode(radialGradientMask);
    }
}

/*
 * Test radial gradient mask with extreme radius values
 */
GRAPHIC_TEST(NGMaskRadialGradientTest, EFFECT_TEST, Set_Radial_Gradient_Mask_Radius_Extreme_Values_Test)
{
    for (size_t i = 0; i < radialGradientExtremeRadii.size(); i++) {
        auto radialGradientMask = std::make_shared<RSNGRadialGradientMask>();
        InitRadialGradientMask(radialGradientMask);
        radialGradientMask->Setter<RadialGradientMaskCenterTag>(Vector2f{0.5f, 0.5f});
        radialGradientMask->Setter<RadialGradientMaskRadiusXTag>(radialGradientExtremeRadii[i]);
        radialGradientMask->Setter<RadialGradientMaskRadiusYTag>(radialGradientExtremeRadii[i]);
        radialGradientMask->Setter<RadialGradientMaskColorsTag>(radialGradientColors);
        radialGradientMask->Setter<RadialGradientMaskPositionsTag>(radialGradientPositions);

        SetUpTestNode(radialGradientMask);
    }
}

} // namespace OHOS::Rosen
