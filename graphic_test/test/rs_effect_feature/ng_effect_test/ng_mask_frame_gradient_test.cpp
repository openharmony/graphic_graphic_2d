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

        // Create effect for each iteration with gradient colors
        auto effect = std::make_shared<RSNGColorGradientFilter>();
        effect->Setter<ColorGradientMaskTag>(std::static_pointer_cast<RSNGMaskBase>(mask));
        // Set gradient colors for rendering
        effect->Setter<ColorGradientEffectColor0Tag>(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));  // Red
        effect->Setter<ColorGradientEffectColor1Tag>(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));  // Green
        effect->Setter<ColorGradientEffectColor2Tag>(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));  // Blue
        effect->Setter<ColorGradientEffectColor3Tag>(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));  // Yellow
        effect->Setter<ColorGradientEffectPosition0Tag>(Vector2f(0.0f, 0.0f));
        effect->Setter<ColorGradientEffectPosition1Tag>(Vector2f(1.0f, 0.0f));
        effect->Setter<ColorGradientEffectPosition2Tag>(Vector2f(0.0f, 1.0f));
        effect->Setter<ColorGradientEffectPosition3Tag>(Vector2f(1.0f, 1.0f));
        effect->Setter<ColorGradientEffectStrength0Tag>(5.0f);
        effect->Setter<ColorGradientEffectStrength1Tag>(5.0f);
        effect->Setter<ColorGradientEffectStrength2Tag>(5.0f);
        effect->Setter<ColorGradientEffectStrength3Tag>(5.0f);
        effect->Setter<ColorGradientEffectColorNumberTag>(4.0f);
        effect->Setter<ColorGradientEffectBlendTag>(1.0f);
        effect->Setter<ColorGradientEffectBlendKTag>(1.0f);

        backgroundNode->SetForegroundNGFilter(effect);
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

        // Create effect for each iteration with gradient colors
        auto effect = std::make_shared<RSNGColorGradientFilter>();
        effect->Setter<ColorGradientMaskTag>(std::static_pointer_cast<RSNGMaskBase>(mask));
        // Set gradient colors for rendering
        effect->Setter<ColorGradientEffectColor0Tag>(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));  // Red
        effect->Setter<ColorGradientEffectColor1Tag>(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));  // Green
        effect->Setter<ColorGradientEffectColor2Tag>(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));  // Blue
        effect->Setter<ColorGradientEffectColor3Tag>(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));  // Yellow
        effect->Setter<ColorGradientEffectPosition0Tag>(Vector2f(0.0f, 0.0f));
        effect->Setter<ColorGradientEffectPosition1Tag>(Vector2f(1.0f, 0.0f));
        effect->Setter<ColorGradientEffectPosition2Tag>(Vector2f(0.0f, 1.0f));
        effect->Setter<ColorGradientEffectPosition3Tag>(Vector2f(1.0f, 1.0f));
        effect->Setter<ColorGradientEffectStrength0Tag>(5.0f);
        effect->Setter<ColorGradientEffectStrength1Tag>(5.0f);
        effect->Setter<ColorGradientEffectStrength2Tag>(5.0f);
        effect->Setter<ColorGradientEffectStrength3Tag>(5.0f);
        effect->Setter<ColorGradientEffectColorNumberTag>(4.0f);
        effect->Setter<ColorGradientEffectBlendTag>(1.0f);
        effect->Setter<ColorGradientEffectBlendKTag>(1.0f);

        backgroundNode->SetForegroundNGFilter(effect);
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

        // Create effect for each iteration with gradient colors
        auto effect = std::make_shared<RSNGColorGradientFilter>();
        effect->Setter<ColorGradientMaskTag>(std::static_pointer_cast<RSNGMaskBase>(mask));
        // Set gradient colors for rendering
        effect->Setter<ColorGradientEffectColor0Tag>(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));  // Red
        effect->Setter<ColorGradientEffectColor1Tag>(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));  // Green
        effect->Setter<ColorGradientEffectColor2Tag>(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));  // Blue
        effect->Setter<ColorGradientEffectColor3Tag>(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));  // Yellow
        effect->Setter<ColorGradientEffectPosition0Tag>(Vector2f(0.0f, 0.0f));
        effect->Setter<ColorGradientEffectPosition1Tag>(Vector2f(1.0f, 0.0f));
        effect->Setter<ColorGradientEffectPosition2Tag>(Vector2f(0.0f, 1.0f));
        effect->Setter<ColorGradientEffectPosition3Tag>(Vector2f(1.0f, 1.0f));
        effect->Setter<ColorGradientEffectStrength0Tag>(5.0f);
        effect->Setter<ColorGradientEffectStrength1Tag>(5.0f);
        effect->Setter<ColorGradientEffectStrength2Tag>(5.0f);
        effect->Setter<ColorGradientEffectStrength3Tag>(5.0f);
        effect->Setter<ColorGradientEffectColorNumberTag>(4.0f);
        effect->Setter<ColorGradientEffectBlendTag>(1.0f);
        effect->Setter<ColorGradientEffectBlendKTag>(1.0f);

        backgroundNode->SetForegroundNGFilter(effect);
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
    colorGradientEffect->Setter<ColorGradientMaskTag>(
        std::static_pointer_cast<RSNGMaskBase>(frameGradientMask));
    // Set gradient colors for rendering
    colorGradientEffect->Setter<ColorGradientEffectColor0Tag>(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));  // Red
    colorGradientEffect->Setter<ColorGradientEffectColor1Tag>(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));  // Green
    colorGradientEffect->Setter<ColorGradientEffectColor2Tag>(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));  // Blue
    colorGradientEffect->Setter<ColorGradientEffectColor3Tag>(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));  // Yellow
    colorGradientEffect->Setter<ColorGradientEffectPosition0Tag>(Vector2f(0.0f, 0.0f));
    colorGradientEffect->Setter<ColorGradientEffectPosition1Tag>(Vector2f(1.0f, 0.0f));
    colorGradientEffect->Setter<ColorGradientEffectPosition2Tag>(Vector2f(0.0f, 1.0f));
    colorGradientEffect->Setter<ColorGradientEffectPosition3Tag>(Vector2f(1.0f, 1.0f));
    colorGradientEffect->Setter<ColorGradientEffectStrength0Tag>(5.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength1Tag>(5.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength2Tag>(5.0f);
    colorGradientEffect->Setter<ColorGradientEffectStrength3Tag>(5.0f);
    colorGradientEffect->Setter<ColorGradientEffectColorNumberTag>(4.0f);
    colorGradientEffect->Setter<ColorGradientEffectBlendTag>(1.0f);
    colorGradientEffect->Setter<ColorGradientEffectBlendKTag>(1.0f);

    int nodeWidth = 350;
    int nodeHeight = 350;
    int start = 100;

    auto backgroundNode = SetUpNodeBgImage(BG_PATH,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetForegroundNGFilter(colorGradientEffect);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

} // namespace OHOS::Rosen
