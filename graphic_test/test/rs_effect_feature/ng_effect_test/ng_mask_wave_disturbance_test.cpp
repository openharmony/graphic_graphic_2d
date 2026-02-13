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

void InitColorGradientFilter(const std::shared_ptr<RSNGColorGradientFilter>& filter,
    const std::shared_ptr<RSNGMaskBase>& mask)
{
    if (!filter || !mask) {
        return;
    }
    filter->Setter<ColorGradientColorsTag>(COLOR_GRADIENT_COLORS);
    filter->Setter<ColorGradientPositionsTag>(COLOR_GRADIENT_POSITIONS);
    filter->Setter<ColorGradientStrengthsTag>(COLOR_GRADIENT_STRENGTHS);
    filter->Setter<ColorGradientMaskTag>(mask);
}

void InitWaveDisturbanceMask(const std::shared_ptr<RSNGWaveDisturbanceMask>& mask, int width, int height)
{
    if (!mask) {
        return;
    }
    mask->Setter<WaveDisturbanceMaskProgressTag>(0.35f);
    mask->Setter<WaveDisturbanceMaskClickPosTag>(
        Vector2f(static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f));
    mask->Setter<WaveDisturbanceMaskWaveRDTag>(Vector2f(6.0f, 0.3f));
    mask->Setter<WaveDisturbanceMaskWaveLWHTag>(Vector3f(220.0f, 60.0f, 35.0f));
}
} // namespace

class NGMaskWaveDisturbanceTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        backgroundPath_ = GetAvailableBackgroundPath();
    }

    std::string backgroundPath_ = BG_PATH;
};

// Test WaveDisturbanceMask with Progress property
GRAPHIC_TEST(NGMaskWaveDisturbanceTest, EFFECT_TEST, Set_NG_Mask_Wave_Disturbance_Progress_Test)
{
    int nodeWidth = 460;
    int nodeHeight = 420;
    int startX = 120;
    int startY = 120;
    int gapX = 40;
    int gapY = 60;
    int row = 4;
    int col = 2;

    const std::vector<float> progressValues = {0.1f, 0.35f, 0.6f, 0.9f};

    for (int i = 0; i < row; i++) {
        int x = startX + (i % col) * (nodeWidth + gapX);
        int y = startY + (i / col) * (nodeHeight + gapY);
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetBackgroundColor(0xFF22324A);

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGWaveDisturbanceMask>();
        InitWaveDisturbanceMask(mask, nodeWidth, nodeHeight);
        mask->Setter<WaveDisturbanceMaskProgressTag>(progressValues[i]);

        // Create effect for each iteration
        auto filter = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(filter, std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetBackgroundNGFilter(filter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test WaveDisturbanceMask with ClickPos property
GRAPHIC_TEST(NGMaskWaveDisturbanceTest, EFFECT_TEST, Set_NG_Mask_Wave_Disturbance_ClickPos_Test)
{
    int nodeWidth = 460;
    int nodeHeight = 420;
    int startX = 120;
    int startY = 120;
    int gapX = 40;
    int gapY = 60;
    int row = 4;
    int col = 2;

    const std::vector<Vector2f> clickPosValues = {
        Vector2f(nodeWidth * 0.25f, nodeHeight * 0.25f),
        Vector2f(nodeWidth * 0.75f, nodeHeight * 0.25f),
        Vector2f(nodeWidth * 0.25f, nodeHeight * 0.75f),
        Vector2f(nodeWidth * 0.75f, nodeHeight * 0.75f)
    };

    for (int i = 0; i < row; i++) {
        int x = startX + (i % col) * (nodeWidth + gapX);
        int y = startY + (i / col) * (nodeHeight + gapY);
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetBackgroundColor(0xFF22324A);

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGWaveDisturbanceMask>();
        InitWaveDisturbanceMask(mask, nodeWidth, nodeHeight);
        mask->Setter<WaveDisturbanceMaskClickPosTag>(clickPosValues[i]);

        // Create effect for each iteration
        auto filter = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(filter, std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetBackgroundNGFilter(filter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test WaveDisturbanceMask with WaveRD property
GRAPHIC_TEST(NGMaskWaveDisturbanceTest, EFFECT_TEST, Set_NG_Mask_Wave_Disturbance_WaveRD_Test)
{
    int nodeWidth = 460;
    int nodeHeight = 420;
    int startX = 120;
    int startY = 120;
    int gapX = 40;
    int gapY = 60;
    int row = 4;
    int col = 2;

    const std::vector<Vector2f> waveRDValues = {
        Vector2f(1.0f, 0.1f),
        Vector2f(6.0f, 0.3f),
        Vector2f(12.0f, 0.5f),
        Vector2f(18.0f, 0.7f)
    };

    for (int i = 0; i < row; i++) {
        int x = startX + (i % col) * (nodeWidth + gapX);
        int y = startY + (i / col) * (nodeHeight + gapY);
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetBackgroundColor(0xFF22324A);

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGWaveDisturbanceMask>();
        InitWaveDisturbanceMask(mask, nodeWidth, nodeHeight);
        mask->Setter<WaveDisturbanceMaskWaveRDTag>(waveRDValues[i]);

        // Create effect for each iteration
        auto filter = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(filter, std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetBackgroundNGFilter(filter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test WaveDisturbanceMask with WaveLWH property
GRAPHIC_TEST(NGMaskWaveDisturbanceTest, EFFECT_TEST, Set_NG_Mask_Wave_Disturbance_WaveLWH_Test)
{
    int nodeWidth = 460;
    int nodeHeight = 420;
    int startX = 120;
    int startY = 120;
    int gapX = 40;
    int gapY = 60;
    int row = 4;
    int col = 2;

    const std::vector<Vector3f> waveLWHValues = {
        Vector3f(160.0f, 40.0f, 20.0f),
        Vector3f(220.0f, 60.0f, 35.0f),
        Vector3f(300.0f, 90.0f, 50.0f),
        Vector3f(380.0f, 110.0f, 70.0f)
    };

    for (int i = 0; i < row; i++) {
        int x = startX + (i % col) * (nodeWidth + gapX);
        int y = startY + (i / col) * (nodeHeight + gapY);
        auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
            {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetBackgroundColor(0xFF22324A);

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGWaveDisturbanceMask>();
        InitWaveDisturbanceMask(mask, nodeWidth, nodeHeight);
        mask->Setter<WaveDisturbanceMaskWaveLWHTag>(waveLWHValues[i]);

        // Create effect for each iteration
        auto filter = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(filter, std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetBackgroundNGFilter(filter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test WaveDisturbanceMask with all properties together
GRAPHIC_TEST(NGMaskWaveDisturbanceTest, EFFECT_TEST, Set_NG_Mask_Wave_Disturbance_Complete_Test)
{
    // Create WaveDisturbanceMask with all properties
    auto waveDisturbanceMask = std::make_shared<RSNGWaveDisturbanceMask>();
    int nodeWidth = 920;
    int nodeHeight = 1500;
    InitWaveDisturbanceMask(waveDisturbanceMask, nodeWidth, nodeHeight);
    waveDisturbanceMask->Setter<WaveDisturbanceMaskProgressTag>(0.6f);
    waveDisturbanceMask->Setter<WaveDisturbanceMaskClickPosTag>(
        Vector2f(static_cast<float>(nodeWidth) * 0.5f, static_cast<float>(nodeHeight) * 0.5f));
    waveDisturbanceMask->Setter<WaveDisturbanceMaskWaveRDTag>(Vector2f(12.0f, 0.55f));
    waveDisturbanceMask->Setter<WaveDisturbanceMaskWaveLWHTag>(Vector3f(360.0f, 100.0f, 65.0f));

    // Create effect with mask
    auto colorGradientFilter = std::make_shared<RSNGColorGradientFilter>();
    InitColorGradientFilter(colorGradientFilter, std::static_pointer_cast<RSNGMaskBase>(waveDisturbanceMask));

    int startX = 140;
    int startY = 200;

    auto backgroundNode = SetUpNodeBgImage(backgroundPath_,
        {startX, startY, nodeWidth, nodeHeight});
    backgroundNode->SetBackgroundColor(0xFF22324A);
    backgroundNode->SetBackgroundNGFilter(colorGradientFilter);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

} // namespace OHOS::Rosen
