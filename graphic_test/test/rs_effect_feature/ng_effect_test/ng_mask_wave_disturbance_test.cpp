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

void InitWaveDisturbanceMask(const std::shared_ptr<RSNGWaveDisturbanceMask>& mask)
{
    if (!mask) {
        return;
    }
    mask->Setter<WaveDisturbanceMaskProgressTag>(0.5f);
    mask->Setter<WaveDisturbanceMaskClickPosTag>(Vector2f(0.5f, 0.5f));
    mask->Setter<WaveDisturbanceMaskWaveRDTag>(Vector2f(5.0f, 0.5f));
    mask->Setter<WaveDisturbanceMaskWaveLWHTag>(Vector3f(120.0f, 40.0f, 30.0f));
}
} // namespace

class NGMaskWaveDisturbanceTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

// Test WaveDisturbanceMask with Progress property
GRAPHIC_TEST(NGMaskWaveDisturbanceTest, EFFECT_TEST, Set_NG_Mask_Wave_Disturbance_Progress_Test)
{
    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<float> progressValues = {0.0f, 0.3f, 0.5f, 1.0f};

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth, nodeHeight});

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGWaveDisturbanceMask>();
        InitWaveDisturbanceMask(mask);
        mask->Setter<WaveDisturbanceMaskProgressTag>(progressValues[i]);

        // Create effect for each iteration
        auto effect = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(effect);
        effect->Setter<ColorGradientMaskTag>(std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetForegroundNGFilter(effect);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test WaveDisturbanceMask with ClickPos property
GRAPHIC_TEST(NGMaskWaveDisturbanceTest, EFFECT_TEST, Set_NG_Mask_Wave_Disturbance_ClickPos_Test)
{
    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<Vector2f> clickPosValues = {
        Vector2f(0.0f, 0.0f),
        Vector2f(0.5f, 0.5f),
        Vector2f(1.0f, 0.0f),
        Vector2f(0.5f, 1.0f)
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth, nodeHeight});

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGWaveDisturbanceMask>();
        InitWaveDisturbanceMask(mask);
        mask->Setter<WaveDisturbanceMaskClickPosTag>(clickPosValues[i]);

        // Create effect for each iteration
        auto effect = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(effect);
        effect->Setter<ColorGradientMaskTag>(std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetForegroundNGFilter(effect);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test WaveDisturbanceMask with WaveRD property
GRAPHIC_TEST(NGMaskWaveDisturbanceTest, EFFECT_TEST, Set_NG_Mask_Wave_Disturbance_WaveRD_Test)
{
    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<Vector2f> waveRDValues = {
        Vector2f(1.0f, 0.0f),
        Vector2f(5.0f, 0.3f),
        Vector2f(10.0f, 0.6f),
        Vector2f(20.0f, 1.0f)
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth, nodeHeight});

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGWaveDisturbanceMask>();
        InitWaveDisturbanceMask(mask);
        mask->Setter<WaveDisturbanceMaskWaveRDTag>(waveRDValues[i]);

        // Create effect for each iteration
        auto effect = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(effect);
        effect->Setter<ColorGradientMaskTag>(std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetForegroundNGFilter(effect);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test WaveDisturbanceMask with WaveLWH property
GRAPHIC_TEST(NGMaskWaveDisturbanceTest, EFFECT_TEST, Set_NG_Mask_Wave_Disturbance_WaveLWH_Test)
{
    int nodeWidth = 200;
    int nodeHeight = 200;
    int start = 50;
    int row = 4;

    const std::vector<Vector3f> waveLWHValues = {
        Vector3f(20.0f, 20.0f, 20.0f),
        Vector3f(40.0f, 40.0f, 40.0f),
        Vector3f(60.0f, 60.0f, 60.0f),
        Vector3f(80.0f, 80.0f, 80.0f)
    };

    for (int i = 0; i < row; i++) {
        auto backgroundNode = SetUpNodeBgImage(BG_PATH,
            {start, start + (start + nodeWidth) * i, nodeWidth, nodeHeight});

        // Create mask for each iteration
        auto mask = std::make_shared<RSNGWaveDisturbanceMask>();
        InitWaveDisturbanceMask(mask);
        mask->Setter<WaveDisturbanceMaskWaveLWHTag>(waveLWHValues[i]);

        // Create effect for each iteration
        auto effect = std::make_shared<RSNGColorGradientFilter>();
        InitColorGradientFilter(effect);
        effect->Setter<ColorGradientMaskTag>(std::static_pointer_cast<RSNGMaskBase>(mask));

        backgroundNode->SetForegroundNGFilter(effect);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Test WaveDisturbanceMask with all properties together
GRAPHIC_TEST(NGMaskWaveDisturbanceTest, EFFECT_TEST, Set_NG_Mask_Wave_Disturbance_Complete_Test)
{
    // Create WaveDisturbanceMask with all properties
    auto waveDisturbanceMask = std::make_shared<RSNGWaveDisturbanceMask>();
    InitWaveDisturbanceMask(waveDisturbanceMask);
    waveDisturbanceMask->Setter<WaveDisturbanceMaskProgressTag>(0.6f);
    waveDisturbanceMask->Setter<WaveDisturbanceMaskClickPosTag>(Vector2f(0.5f, 0.5f));
    waveDisturbanceMask->Setter<WaveDisturbanceMaskWaveRDTag>(Vector2f(12.0f, 0.8f));
    waveDisturbanceMask->Setter<WaveDisturbanceMaskWaveLWHTag>(Vector3f(60.0f, 60.0f, 60.0f));

    // Create effect with mask
    auto colorGradientEffect = std::make_shared<RSNGColorGradientFilter>();
    InitColorGradientFilter(colorGradientEffect);
    colorGradientEffect->Setter<ColorGradientMaskTag>(
        std::static_pointer_cast<RSNGMaskBase>(waveDisturbanceMask));

    int nodeWidth = 300;
    int nodeHeight = 300;
    int start = 100;

    auto backgroundNode = SetUpNodeBgImage(BG_PATH,
        {start, start, nodeWidth, nodeHeight});
    backgroundNode->SetForegroundNGFilter(colorGradientEffect);
    GetRootNode()->AddChild(backgroundNode);
    RegisterNode(backgroundNode);
}

} // namespace OHOS::Rosen
