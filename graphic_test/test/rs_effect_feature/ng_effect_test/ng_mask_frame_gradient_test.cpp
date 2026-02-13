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
const std::vector<float> COLOR_GRADIENT_COLORS = {
    0.05f, 0.35f, 1.0f, 1.0f,
    0.0f, 0.85f, 0.75f, 1.0f,
    1.0f, 0.35f, 0.7f, 1.0f,
    1.0f, 0.85f, 0.2f, 1.0f,
};
const std::vector<float> COLOR_GRADIENT_POSITIONS = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
};
const std::vector<float> COLOR_GRADIENT_STRENGTHS = {2.8f, 2.8f, 2.8f, 2.8f};
const Vector4f DEFAULT_INNER_BEZIER = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
const Vector4f DEFAULT_OUTER_BEZIER = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);

std::string GetAvailableBackgroundPath()
{
    const std::vector<std::string> candidates = {
        BG_PATH,
        APPEARANCE_TEST_JPG_PATH,
        FG_TEST_JPG_PATH,
    };
    for (const auto& path : candidates) {
        auto pixelMap = DecodePixelMap(path, Media::AllocatorType::SHARE_MEM_ALLOC);
        if (pixelMap != nullptr) {
            return path;
        }
    }
    return BG_PATH;
}

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
        backgroundPath_ = GetAvailableBackgroundPath();
    }

    void AddSceneBase()
    {
        auto sceneNode = RSCanvasNode::Create();
        sceneNode->SetBounds({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        sceneNode->SetFrame({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
        sceneNode->SetBackgroundColor(0xFF0F1A2E);
        GetRootNode()->AddChild(sceneNode);
        RegisterNode(sceneNode);
    }

    void AddTileBase(int x, int y, int width, int height)
    {
        auto tileNode = RSCanvasNode::Create();
        tileNode->SetBounds({x, y, width, height});
        tileNode->SetFrame({x, y, width, height});
        tileNode->SetBackgroundColor(0xFF1C2F4A);
        GetRootNode()->AddChild(tileNode);
        RegisterNode(tileNode);
    }

    std::string backgroundPath_ = BG_PATH;
};

// Test FrameGradientMask with CornerRadius property
GRAPHIC_TEST(NGMaskFrameGradientTest, EFFECT_TEST, Set_NG_Mask_Frame_Gradient_CornerRadius_Test)
{
    AddSceneBase();
    int nodeWidth = 460;
    int nodeHeight = 420;
    int startX = 120;
    int startY = 120;
    int gapX = 40;
    int gapY = 60;
    int row = 4;
    int col = 2;
    constexpr float innerFrameWidth = 12.0f;
    constexpr float outerFrameWidth = 24.0f;

    const std::vector<float> cornerRadiusValues = {0.0f, 10.0f, 20.0f, 50.0f};

    for (int i = 0; i < row; i++) {
        int x = startX + (i % col) * (nodeWidth + gapX);
        int y = startY + (i / col) * (nodeHeight + gapY);
        AddTileBase(x, y, nodeWidth, nodeHeight);
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetBackgroundColor(0xFF22324A);

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGFrameGradientMask>();
        mask->Setter<FrameGradientMaskInnerBezierTag>(DEFAULT_INNER_BEZIER);
        mask->Setter<FrameGradientMaskOuterBezierTag>(DEFAULT_OUTER_BEZIER);
        mask->Setter<FrameGradientMaskInnerFrameWidthTag>(innerFrameWidth);
        mask->Setter<FrameGradientMaskOuterFrameWidthTag>(outerFrameWidth);
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
    AddSceneBase();
    int nodeWidth = 460;
    int nodeHeight = 420;
    int startX = 120;
    int startY = 120;
    int gapX = 40;
    int gapY = 60;
    int row = 4;
    int col = 2;

    const std::vector<float> innerFrameWidthValues = {5.0f, 10.0f, 15.0f, 20.0f};
    const std::vector<float> outerFrameWidthValues = {10.0f, 15.0f, 20.0f, 25.0f};

    for (int i = 0; i < row; i++) {
        int x = startX + (i % col) * (nodeWidth + gapX);
        int y = startY + (i / col) * (nodeHeight + gapY);
        AddTileBase(x, y, nodeWidth, nodeHeight);
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetBackgroundColor(0xFF22324A);

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGFrameGradientMask>();
        mask->Setter<FrameGradientMaskInnerBezierTag>(DEFAULT_INNER_BEZIER);
        mask->Setter<FrameGradientMaskOuterBezierTag>(DEFAULT_OUTER_BEZIER);
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
    AddSceneBase();
    int nodeWidth = 920;
    int nodeHeight = 1500;
    int startX = 140;
    int startY = 200;

    // Create FrameGradientMask with all properties
    auto frameGradientMask = std::make_shared<RSNGFrameGradientMask>();
    frameGradientMask->Setter<FrameGradientMaskInnerBezierTag>(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    frameGradientMask->Setter<FrameGradientMaskOuterBezierTag>(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
    frameGradientMask->Setter<FrameGradientMaskCornerRadiusTag>(25.0f);
    frameGradientMask->Setter<FrameGradientMaskInnerFrameWidthTag>(15.0f);
    frameGradientMask->Setter<FrameGradientMaskOuterFrameWidthTag>(20.0f);
    frameGradientMask->Setter<FrameGradientMaskRectWHTag>(
        Vector2f(static_cast<float>(nodeWidth), static_cast<float>(nodeHeight)));
    frameGradientMask->Setter<FrameGradientMaskRectPosTag>(Vector2f(0.0f, 0.0f));

    // Create effect with mask and gradient colors
    auto colorGradientEffect = std::make_shared<RSNGColorGradientFilter>();
    InitColorGradientFilter(colorGradientEffect);
    colorGradientEffect->Setter<ColorGradientMaskTag>(
        std::static_pointer_cast<RSNGMaskBase>(frameGradientMask));

    auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
        {startX, startY, nodeWidth, nodeHeight});
    backgroundNode->SetBackgroundColor(0xFF22324A);
    backgroundNode->SetBackgroundNGFilter(colorGradientEffect);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

} // namespace OHOS::Rosen
