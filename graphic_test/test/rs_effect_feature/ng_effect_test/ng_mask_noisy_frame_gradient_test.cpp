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

#include "ng_filter_test_utils.h"
#include "ng_sdf_test_utils.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

const std::vector<float> COLOR_GRADIENT_COLORS = {
    0.05f,
    0.35f,
    1.0f,
    1.0f,
    0.0f,
    0.85f,
    0.75f,
    1.0f,
    1.0f,
    0.35f,
    0.7f,
    1.0f,
    1.0f,
    0.85f,
    0.2f,
    1.0f,
};

const std::vector<float> COLOR_GRADIENT_POSITIONS = {
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    1.0f,
};

const std::vector<float> COLOR_GRADIENT_STRENGTHS = { 2.8f, 2.8f, 2.8f, 2.8f };

const Vector4f DEFAULT_GRADIENT_BEZIER = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);

const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";

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

void InitNoisyFrameGradientMaskRect(const std::shared_ptr<RSNGNoisyFrameGradientMask>& mask, int width, int height)
{
    if (!mask) {
        return;
    }
    mask->Setter<NoisyFrameGradientMaskRRectWHTag>(Vector2f(static_cast<float>(width), static_cast<float>(height)));
    mask->Setter<NoisyFrameGradientMaskRRectPosTag>(Vector2f(0.0f, 0.0f));
}

const std::vector<float> cornerRadiusValues = { 0.0f, 10.0f, 25.0f, 50.0f };

const std::vector<Vector2f> innerFrameWidthValues = { Vector2f(5.0f, 5.0f), Vector2f(10.0f, 10.0f),
    Vector2f(15.0f, 15.0f), Vector2f(20.0f, 20.0f) };

const std::vector<Vector2f> middleFrameWidthValues = { Vector2f(8.0f, 8.0f), Vector2f(12.0f, 12.0f),
    Vector2f(18.0f, 18.0f), Vector2f(25.0f, 25.0f) };

const std::vector<float> progressValues = { 0.0f, 0.3f, 0.5f, 0.8f, 1.0f };

const std::vector<float> slopeValues = { 0.0f, 0.5f, 1.0f, 2.0f };

const std::vector<float> extremeProgressValues = { -0.5f, 1.5f, 100.0f };

const std::vector<float> extremeCornerRadiusValues = { -10.0f, 100.0f, std::numeric_limits<float>::quiet_NaN() };
} // namespace

class NGMaskNoisyFrameGradientTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        backgroundPath_ = GetAvailableBackgroundPath();
    }

    void AddSceneBase()
    {
        auto sceneNode = RSCanvasNode::Create();
        sceneNode->SetBounds({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        sceneNode->SetFrame({ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
        sceneNode->SetBackgroundColor(0xFF0F1A2E);
        GetRootNode()->AddChild(sceneNode);
        RegisterNode(sceneNode);
    }

    void AddTileBase(int x, int y, int width, int height)
    {
        auto tileNode = RSCanvasNode::Create();
        tileNode->SetBounds({ x, y, width, height });
        tileNode->SetFrame({ x, y, width, height });
        tileNode->SetBackgroundColor(0xFF1C2F4A);
        GetRootNode()->AddChild(tileNode);
        RegisterNode(tileNode);
    }

    std::string backgroundPath_ = BG_PATH;

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
};

GRAPHIC_TEST(NGMaskNoisyFrameGradientTest, EFFECT_TEST, Set_Noisy_Frame_Gradient_Mask_CornerRadius_Test)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
        auto noisyFrameMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        noisyFrameMask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsTag>(DEFAULT_GRADIENT_BEZIER);
        noisyFrameMask->Setter<NoisyFrameGradientMaskInnerFrameWidthTag>(Vector2f(10.0f, 10.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskMiddleFrameWidthTag>(Vector2f(15.0f, 15.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskOutsideFrameWidthTag>(Vector2f(20.0f, 20.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskCornerRadiusTag>(cornerRadiusValues[i]);
        noisyFrameMask->Setter<NoisyFrameGradientMaskSlopeTag>(1.0f);
        noisyFrameMask->Setter<NoisyFrameGradientMaskProgressTag>(0.5f);
        InitNoisyFrameGradientMaskRect(noisyFrameMask, x, y);

        // Create color gradient filter
        auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
        auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
        std::vector<float> colors = { colorGradientParams[i][0], colorGradientParams[i][1], colorGradientParams[i][2],
            colorGradientParams[i][3] };
        std::vector<float> positions = { colorGradientParams[i][4], colorGradientParams[i][5] };
        std::vector<float> strengths = { colorGradientParams[i][6] };
        colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
        colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
        colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
        colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGMaskNoisyFrameGradientTest, EFFECT_TEST, Set_Noisy_Frame_Gradient_Mask_FrameWidth_Test)
{
    int columnCount = 2;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
        auto noisyFrameMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        noisyFrameMask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsTag>(DEFAULT_GRADIENT_BEZIER);
        noisyFrameMask->Setter<NoisyFrameGradientMaskInnerFrameWidthTag>(innerFrameWidthValues[i]);
        noisyFrameMask->Setter<NoisyFrameGradientMaskMiddleFrameWidthTag>(middleFrameWidthValues[i]);
        noisyFrameMask->Setter<NoisyFrameGradientMaskOutsideFrameWidthTag>(Vector2f(25.0f, 25.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskCornerRadiusTag>(20.0f);
        noisyFrameMask->Setter<NoisyFrameGradientMaskSlopeTag>(1.0f);
        noisyFrameMask->Setter<NoisyFrameGradientMaskProgressTag>(0.5f);
        InitNoisyFrameGradientMaskRect(noisyFrameMask, x, y);

        // Create color gradient filter
        auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
        auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
        std::vector<float> colors = { colorGradientParams[i][0], colorGradientParams[i][1], colorGradientParams[i][2],
            colorGradientParams[i][3] };
        std::vector<float> positions = { colorGradientParams[i][4], colorGradientParams[i][5] };
        std::vector<float> strengths = { colorGradientParams[i][6] };
        colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
        colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
        colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
        colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGMaskNoisyFrameGradientTest, EFFECT_TEST, Set_Noisy_Frame_Gradient_Mask_Progress_Test)
{
    int columnCount = 5;
    int rowCount = 5;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
        auto noisyFrameMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        noisyFrameMask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsTag>(DEFAULT_GRADIENT_BEZIER);
        noisyFrameMask->Setter<NoisyFrameGradientMaskInnerFrameWidthTag>(Vector2f(10.0f, 10.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskMiddleFrameWidthTag>(Vector2f(15.0f, 15.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskOutsideFrameWidthTag>(Vector2f(20.0f, 20.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskCornerRadiusTag>(20.0f);
        noisyFrameMask->Setter<NoisyFrameGradientMaskSlopeTag>(1.0f);
        noisyFrameMask->Setter<NoisyFrameGradientMaskProgressTag>(progressValues[i]);
        InitNoisyFrameGradientMaskRect(noisyFrameMask, x, y);

        // Create color gradient filter
        auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
        auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
        std::vector<float> colors = { colorGradientParams[i][0], colorGradientParams[i][1], colorGradientParams[i][2],
            colorGradientParams[i][3] };
        std::vector<float> positions = { colorGradientParams[i][4], colorGradientParams[i][5] };
        std::vector<float> strengths = { colorGradientParams[i][6] };
        colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
        colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
        colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
        colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGMaskNoisyFrameGradientTest, EFFECT_TEST, Set_Noisy_Frame_Gradient_Mask_Slope_Test)
{
    int columnCount = 4;
    int rowCount = 4;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
        auto noisyFrameMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        noisyFrameMask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsTag>(DEFAULT_GRADIENT_BEZIER);
        noisyFrameMask->Setter<NoisyFrameGradientMaskInnerFrameWidthTag>(Vector2f(10.0f, 10.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskMiddleFrameWidthTag>(Vector2f(15.0f, 15.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskOutsideFrameWidthTag>(Vector2f(20.0f, 20.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskCornerRadiusTag>(20.0f);
        noisyFrameMask->Setter<NoisyFrameGradientMaskSlopeTag>(slopeValues[i]);
        noisyFrameMask->Setter<NoisyFrameGradientMaskProgressTag>(0.5f);
        InitNoisyFrameGradientMaskRect(noisyFrameMask, x, y);

        // Create color gradient filter
        auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
        auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
        std::vector<float> colors = { colorGradientParams[i][0], colorGradientParams[i][1], colorGradientParams[i][2],
            colorGradientParams[i][3] };
        std::vector<float> positions = { colorGradientParams[i][4], colorGradientParams[i][5] };
        std::vector<float> strengths = { colorGradientParams[i][6] };
        colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
        colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
        colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
        colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGMaskNoisyFrameGradientTest, EFFECT_TEST, Set_Noisy_Frame_Gradient_Mask_Complete_Test)
{
    int nodeWidth = 920;
    int nodeHeight = 1500;
    int startX = 140;
    int startY = 200;

    auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
    auto noisyFrameGradientMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);

    noisyFrameGradientMask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsTag>(
        Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    noisyFrameGradientMask->Setter<NoisyFrameGradientMaskCornerRadiusTag>(25.0f);
    noisyFrameGradientMask->Setter<NoisyFrameGradientMaskInnerFrameWidthTag>(Vector2f(12.0f, 12.0f));
    noisyFrameGradientMask->Setter<NoisyFrameGradientMaskMiddleFrameWidthTag>(Vector2f(18.0f, 18.0f));
    noisyFrameGradientMask->Setter<NoisyFrameGradientMaskOutsideFrameWidthTag>(Vector2f(24.0f, 24.0f));
    noisyFrameGradientMask->Setter<NoisyFrameGradientMaskRRectWHTag>(
        Vector2f(static_cast<float>(nodeWidth), static_cast<float>(nodeHeight)));
    noisyFrameGradientMask->Setter<NoisyFrameGradientMaskRRectPosTag>(Vector2f(0.0f, 0.0f));
    noisyFrameGradientMask->Setter<NoisyFrameGradientMaskSlopeTag>(1.5f);
    noisyFrameGradientMask->Setter<NoisyFrameGradientMaskProgressTag>(0.6f);

    // Create color gradient filter
    auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
    auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
    colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

    auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 0, 0, startX, startY });
    backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
    GetRootNode()->AddChild(backgroundTestNode);
    RegisterNode(backgroundTestNode);
}

GRAPHIC_TEST(NGMaskNoisyFrameGradientTest, EFFECT_TEST, Set_Noisy_Frame_Gradient_Mask_Progress_Extreme_Values_Test)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
        auto noisyFrameMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        noisyFrameMask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsTag>(DEFAULT_GRADIENT_BEZIER);
        noisyFrameMask->Setter<NoisyFrameGradientMaskInnerFrameWidthTag>(Vector2f(10.0f, 10.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskMiddleFrameWidthTag>(Vector2f(15.0f, 15.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskOutsideFrameWidthTag>(Vector2f(20.0f, 20.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskCornerRadiusTag>(20.0f);
        noisyFrameMask->Setter<NoisyFrameGradientMaskSlopeTag>(1.0f);
        noisyFrameMask->Setter<NoisyFrameGradientMaskProgressTag>(extremeProgressValues[i]);
        InitNoisyFrameGradientMaskRect(noisyFrameMask, x, y);

        // Create color gradient filter
        auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
        auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
        std::vector<float> colors = { colorGradientParams[i][0], colorGradientParams[i][1], colorGradientParams[i][2],
            colorGradientParams[i][3] };
        std::vector<float> positions = { colorGradientParams[i][4], colorGradientParams[i][5] };
        std::vector<float> strengths = { colorGradientParams[i][6] };
        colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
        colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
        colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
        colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGMaskNoisyFrameGradientTest, EFFECT_TEST, Set_Noisy_Frame_Gradient_Mask_CornerRadius_Extreme_Values_Test)
{
    int columnCount = 3;
    int rowCount = 3;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < rowCount; i++) {
        auto mask = CreateMask(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
        auto noisyFrameMask = std::static_pointer_cast<RSNGNoisyFrameGradientMask>(mask);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        noisyFrameMask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsTag>(DEFAULT_GRADIENT_BEZIER);
        noisyFrameMask->Setter<NoisyFrameGradientMaskInnerFrameWidthTag>(Vector2f(10.0f, 10.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskMiddleFrameWidthTag>(Vector2f(15.0f, 15.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskOutsideFrameWidthTag>(Vector2f(20.0f, 20.0f));
        noisyFrameMask->Setter<NoisyFrameGradientMaskCornerRadiusTag>(extremeCornerRadiusValues[i]);
        noisyFrameMask->Setter<NoisyFrameGradientMaskSlopeTag>(1.0f);
        noisyFrameMask->Setter<NoisyFrameGradientMaskProgressTag>(0.5f);
        InitNoisyFrameGradientMaskRect(noisyFrameMask, x, y);

        // Create color gradient filter
        auto filter = CreateFilter(RSNGEffectType::COLOR_GRADIENT);
        auto colorGradientFilter = std::static_pointer_cast<RSNGColorGradientFilter>(filter);
        std::vector<float> colors = { colorGradientParams[i][0], colorGradientParams[i][1], colorGradientParams[i][2],
            colorGradientParams[i][3] };
        std::vector<float> positions = { colorGradientParams[i][4], colorGradientParams[i][5] };
        std::vector<float> strengths = { colorGradientParams[i][6] };
        colorGradientFilter->Setter<ColorGradientColorsTag>(colors);
        colorGradientFilter->Setter<ColorGradientPositionsTag>(positions);
        colorGradientFilter->Setter<ColorGradientStrengthsTag>(strengths);
        colorGradientFilter->Setter<ColorGradientMaskTag>(mask);

        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}
} // namespace OHOS::Rosen