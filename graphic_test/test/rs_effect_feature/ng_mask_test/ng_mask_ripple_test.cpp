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

void InitRippleMask(std::shared_ptr<RSNGRippleMask>& rippleMask)
{
    if (!rippleMask) {
        return;
    }
    // Radius: ripple radius
    rippleMask->Setter<RippleMaskRadiusTag>(5.0f);
    // Width: ripple width
    rippleMask->Setter<RippleMaskWidthTag>(5.0f);
    // Center: center position
    rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
    // Offset: width center offset
    rippleMask->Setter<RippleMaskOffsetTag>(0.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
// Center position variations
const std::vector<Vector2f> rippleCenters = {
    Vector2f{0.0f, 0.0f},       // Origin
    Vector2f{0.5f, 0.5f},       // Center
    Vector2f{1.0f, 1.0f}        // Max
};

// Radius boundary values
const std::vector<float> rippleRadii = {0.0f, 2.5f, 5.0f, 10.0f};

// Width boundary values
const std::vector<float> rippleWidths = {0.0f, 2.5f, 5.0f, 10.0f};

// Width center offset values
const std::vector<float> rippleOffsets = {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f};

// Parameter combinations
const std::vector<std::tuple<float, float, float>> rippleRadiusWidthOffsetCombinations = {
    {2.5f, 2.5f, 0.0f},    // Small radius, small width, centered
    {5.0f, 5.0f, 0.5f},    // Medium radius, medium width, offset
    {10.0f, 10.0f, 1.0f}   // Large radius, large width, max offset
};

// Extreme and invalid values for robustness testing
const std::vector<float> rippleExtremeRadii = {
    -1.0f,         // Negative (below range)
    -10.0f,        // Large negative
    11.0f,         // Above max (10.0)
    100.0f,        // Extremely large
    9999.0f        // Astronomical value
};

const std::vector<float> rippleExtremeWidths = {
    -1.0f,         // Negative (below range)
    -10.0f,        // Large negative
    11.0f,         // Above max (10.0)
    100.0f,        // Extremely large
    std::numeric_limits<float>::quiet_NaN()  // NaN
};
}

class NGMaskRippleTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const std::shared_ptr<RSNGRippleMask>& rippleMask)
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
        harmoniumEffect->Setter<HarmoniumEffectMaskTag>(std::static_pointer_cast<RSNGMaskBase>(rippleMask));
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
 * Test ripple mask with different center positions
 */
GRAPHIC_TEST(NGMaskRippleTest, EFFECT_TEST, Set_Ripple_Mask_Center_Test)
{
    for (size_t i = 0; i < rippleCenters.size(); i++) {
        auto rippleMask = std::make_shared<RSNGRippleMask>();
        InitRippleMask(rippleMask);
        rippleMask->Setter<RippleMaskCenterTag>(rippleCenters[i]);
        rippleMask->Setter<RippleMaskRadiusTag>(5.0f);
        rippleMask->Setter<RippleMaskWidthTag>(5.0f);

        SetUpTestNode(i, rippleMask);
    }
}

/*
 * Test ripple mask with boundary radius values
 */
GRAPHIC_TEST(NGMaskRippleTest, EFFECT_TEST, Set_Ripple_Mask_Radius_Boundary_Test)
{
    for (size_t i = 0; i < rippleRadii.size(); i++) {
        auto rippleMask = std::make_shared<RSNGRippleMask>();
        InitRippleMask(rippleMask);
        rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
        rippleMask->Setter<RippleMaskRadiusTag>(rippleRadii[i]);
        rippleMask->Setter<RippleMaskWidthTag>(5.0f);

        SetUpTestNode(i, rippleMask);
    }
}

/*
 * Test ripple mask with boundary width values
 */
GRAPHIC_TEST(NGMaskRippleTest, EFFECT_TEST, Set_Ripple_Mask_Width_Boundary_Test)
{
    for (size_t i = 0; i < rippleWidths.size(); i++) {
        auto rippleMask = std::make_shared<RSNGRippleMask>();
        InitRippleMask(rippleMask);
        rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
        rippleMask->Setter<RippleMaskRadiusTag>(5.0f);
        rippleMask->Setter<RippleMaskWidthTag>(rippleWidths[i]);

        SetUpTestNode(i, rippleMask);
    }
}

/*
 * Test ripple mask with radius, width, and offset combinations
 */
GRAPHIC_TEST(NGMaskRippleTest, EFFECT_TEST, Set_Ripple_Mask_Parameter_Combination_Test)
{
    for (size_t i = 0; i < rippleRadiusWidthOffsetCombinations.size(); i++) {
        auto rippleMask = std::make_shared<RSNGRippleMask>();
        InitRippleMask(rippleMask);
        rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
        rippleMask->Setter<RippleMaskRadiusTag>(std::get<0>(rippleRadiusWidthOffsetCombinations[i]));
        rippleMask->Setter<RippleMaskWidthTag>(std::get<1>(rippleRadiusWidthOffsetCombinations[i]));
        rippleMask->Setter<RippleMaskOffsetTag>(std::get<2>(rippleRadiusWidthOffsetCombinations[i]));

        SetUpTestNode(i, rippleMask);
    }
}

/*
 * Test ripple mask with extreme and invalid radius values
 * Tests malicious inputs: negative values, out-of-range values, extremely large values
 */
GRAPHIC_TEST(NGMaskRippleTest, EFFECT_TEST, Set_Ripple_Mask_Radius_Extreme_Values_Test)
{
    for (size_t i = 0; i < rippleExtremeRadii.size(); i++) {
        auto rippleMask = std::make_shared<RSNGRippleMask>();
        InitRippleMask(rippleMask);
        rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
        rippleMask->Setter<RippleMaskRadiusTag>(rippleExtremeRadii[i]);
        rippleMask->Setter<RippleMaskWidthTag>(5.0f);

        SetUpTestNode(i, rippleMask);
    }
}

/*
 * Test ripple mask with extreme and invalid width values
 * Tests malicious inputs: negative values, out-of-range values, NaN
 */
GRAPHIC_TEST(NGMaskRippleTest, EFFECT_TEST, Set_Ripple_Mask_Width_Extreme_Values_Test)
{
    for (size_t i = 0; i < rippleExtremeWidths.size(); i++) {
        auto rippleMask = std::make_shared<RSNGRippleMask>();
        InitRippleMask(rippleMask);
        rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
        rippleMask->Setter<RippleMaskRadiusTag>(5.0f);
        rippleMask->Setter<RippleMaskWidthTag>(rippleExtremeWidths[i]);

        SetUpTestNode(i, rippleMask);
    }
}

} // namespace OHOS::Rosen
