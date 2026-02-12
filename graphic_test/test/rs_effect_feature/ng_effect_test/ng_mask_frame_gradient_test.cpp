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
#include "../ng_filter_test/ng_filter_test_utils.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
const std::vector<float> COLOR_GRADIENT_COLORS = {0.5f, 0.6f, 0.9f, 0.9f};
const std::vector<float> COLOR_GRADIENT_POSITIONS = {0.2f, 0.8f};
const std::vector<float> COLOR_GRADIENT_STRENGTHS = {1.5f};

void InitColorGradientFilter(const std::shared_ptr<RSNGColorGradientFilter>& filter)
{
    if (!filter) {
        return;
    }
    filter->Setter<ColorGradientColorsTag>(COLOR_GRADIENT_COLORS);
    filter->Setter<ColorGradientPositionsTag>(COLOR_GRADIENT_POSITIONS);
    filter->Setter<ColorGradientStrengthsTag>(COLOR_GRADIENT_STRENGTHS);
}

void InitFrameGradientMaskRect(const std::shared_ptr<RSNGFrameGradientMask>& mask, int width, int height)
{
    if (!mask) {
        return;
    }
    mask->Setter<FrameGradientMaskRectWHTag>(Vector2f(static_cast<float>(width), static_cast<float>(height)));
    mask->Setter<FrameGradientMaskRectPosTag>(Vector2f(0.0f, 0.0f));
}
} // namespace

class NGMaskFrameGradientTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

// Test FrameGradientMask with InnerBezier and OuterBezier properties
GRAPHIC_TEST(NGMaskFrameGradientTest, EFFECT_TEST, Set_NG_Mask_Frame_Gradient_Bezier_Test)
{
    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<Vector4f> innerBezierValues = {
        Vector4f(0.0f, 0.0f, 0.5f, 0.5f),
        Vector4f(0.0f, 0.5f, 1.0f, 0.5f),
        Vector4f(0.5f, 0.0f, 0.5f, 1.0f),
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f)
    };
    const std::vector<Vector4f> outerBezierValues = {
        Vector4f(0.0f, 0.0f, 0.5f, 0.5f),
        Vector4f(0.0f, 0.5f, 1.0f, 0.5f),
        Vector4f(0.5f, 0.0f, 0.5f, 1.0f),
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f)
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth, nodeHeight});

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGFrameGradientMask>();
        mask->Setter<FrameGradientMaskInnerBezierTag>(innerBezierValues[i]);
        mask->Setter<FrameGradientMaskOuterBezierTag>(outerBezierValues[i]);
        mask->Setter<FrameGradientMaskInnerFrameWidthTag>(12.0f + 2.0f * i);
        mask->Setter<FrameGradientMaskOuterFrameWidthTag>(24.0f + 2.0f * i);
        InitFrameGradientMaskRect(mask, nodeWidth, nodeHeight);

        // Create effect for each iteration with gradient colors
        auto effect = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(effect);
        effect->Setter<ColorGradientMaskTag>(std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetBackgroundNGFilter(effect);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test FrameGradientMask with CornerRadius property
GRAPHIC_TEST(NGMaskFrameGradientTest, EFFECT_TEST, Set_NG_Mask_Frame_Gradient_CornerRadius_Test)
{
    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> cornerRadiusValues = {0.0f, 10.0f, 20.0f, 50.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth, nodeHeight});

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGFrameGradientMask>();
        mask->Setter<FrameGradientMaskCornerRadiusTag>(cornerRadiusValues[i]);
        InitFrameGradientMaskRect(mask, nodeWidth, nodeHeight);

        // Create effect for each iteration with gradient colors
        auto effect = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(effect);
        effect->Setter<ColorGradientMaskTag>(std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetBackgroundNGFilter(effect);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test FrameGradientMask with FrameWidth properties
GRAPHIC_TEST(NGMaskFrameGradientTest, EFFECT_TEST, Set_NG_Mask_Frame_Gradient_FrameWidth_Test)
{
    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> innerFrameWidthValues = {5.0f, 10.0f, 15.0f, 20.0f};
    const std::vector<float> outerFrameWidthValues = {10.0f, 15.0f, 20.0f, 25.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth, nodeHeight});

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGFrameGradientMask>();
        mask->Setter<FrameGradientMaskInnerFrameWidthTag>(innerFrameWidthValues[i]);
        mask->Setter<FrameGradientMaskOuterFrameWidthTag>(outerFrameWidthValues[i]);
        InitFrameGradientMaskRect(mask, nodeWidth, nodeHeight);

        // Create effect for each iteration with gradient colors
        auto effect = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(effect);
        effect->Setter<ColorGradientMaskTag>(std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetBackgroundNGFilter(effect);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test FrameGradientMask with all properties together
GRAPHIC_TEST(NGMaskFrameGradientTest, EFFECT_TEST, Set_NG_Mask_Frame_Gradient_Complete_Test)
{
    // Create FrameGradientMask with all properties
    auto frameGradientMask = std::make_shared<RSNGFrameGradientMask>();
    frameGradientMask->Setter<FrameGradientMaskInnerBezierTag>(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    frameGradientMask->Setter<FrameGradientMaskOuterBezierTag>(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
    frameGradientMask->Setter<FrameGradientMaskCornerRadiusTag>(25.0f);
    frameGradientMask->Setter<FrameGradientMaskInnerFrameWidthTag>(15.0f);
    frameGradientMask->Setter<FrameGradientMaskOuterFrameWidthTag>(20.0f);
    frameGradientMask->Setter<FrameGradientMaskRectWHTag>(Vector2f(300.0f, 300.0f));
    frameGradientMask->Setter<FrameGradientMaskRectPosTag>(Vector2f(50.0f, 50.0f));

    // Create effect with mask and gradient colors
    auto colorGradientEffect = std::make_shared<RSNGColorGradientFilter>();
    InitColorGradientFilter(colorGradientEffect);
    colorGradientEffect->Setter<ColorGradientMaskTag>(
        std::static_pointer_cast<RSNGMaskBase>(frameGradientMask));

    int nodeWidth = 350;
    int nodeHeight = 350;
    int start = 100;

    auto backgroundNode = SetUpNodeBgImage(BG_PATH,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetBackgroundNGFilter(colorGradientEffect);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

} // namespace OHOS::Rosen
