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
#include "ng_filter_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;
}

class NGFilterHeatDistortionTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

// Basic function test - Verify basic functionality of heat distortion effect
GRAPHIC_TEST(NGFilterHeatDistortionTest, EFFECT_TEST, Set_NG_Filter_Heat_Distortion_Test)
{
    const int columnCount = 2;
    const int nodeWidth = SCREEN_WIDTH / columnCount;
    const int nodeHeight = SCREEN_HEIGHT / 2;

    for (int i = 0; i < 4; i++) {
        auto heatDistortionPtr = CreateFilter(RSNGEffectType::HEAT_DISTORTION);
        auto heatDistortionFilter = std::static_pointer_cast<RSNGHeatDistortionFilter>(heatDistortionPtr);

        // Set standard parameters
        heatDistortionFilter->Setter<HeatDistortionIntensityTag>(1.0f);
        heatDistortionFilter->Setter<HeatDistortionNoiseScaleTag>(1.0f);
        heatDistortionFilter->Setter<HeatDistortionRiseWeightTag>(0.2f);
        heatDistortionFilter->Setter<HeatDistortionProgressTag>(0.5f);

        const int x = (i % columnCount) * nodeWidth;
        const int y = (i / columnCount) * nodeHeight;

        auto backgroundNode = SetUpNodeBgImage(BG_PATH, {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetForegroundNGFilter(heatDistortionFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Intensity parameter test - Test different intensity values
GRAPHIC_TEST(NGFilterHeatDistortionTest, EFFECT_TEST, Set_NG_Filter_Heat_Distortion_Intensity_Test)
{
    const int columnCount = 2;
    const int nodeWidth = SCREEN_WIDTH / columnCount;
    const int nodeHeight = SCREEN_HEIGHT / 2;
    std::vector<float> intensityVec = {0.2f, 0.8f, 1.2f, 1.8f};

    for (int i = 0; i < 4; i++) {
        auto heatDistortionPtr = CreateFilter(RSNGEffectType::HEAT_DISTORTION);
        auto heatDistortionFilter = std::static_pointer_cast<RSNGHeatDistortionFilter>(heatDistortionPtr);

        heatDistortionFilter->Setter<HeatDistortionIntensityTag>(intensityVec[i]);
        heatDistortionFilter->Setter<HeatDistortionNoiseScaleTag>(1.0f);
        heatDistortionFilter->Setter<HeatDistortionRiseWeightTag>(0.2f);
        heatDistortionFilter->Setter<HeatDistortionProgressTag>(0.5f);

        const int x = (i % columnCount) * nodeWidth;
        const int y = (i / columnCount) * nodeHeight;

        auto backgroundNode = SetUpNodeBgImage(BG_PATH, {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetForegroundNGFilter(heatDistortionFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Noise scale parameter test - Test different noise scale values
GRAPHIC_TEST(NGFilterHeatDistortionTest, EFFECT_TEST, Set_NG_Filter_Heat_Distortion_NoiseScale_Test)
{
    const int columnCount = 2;
    const int nodeWidth = SCREEN_WIDTH / columnCount;
    const int nodeHeight = SCREEN_HEIGHT / 2;
    std::vector<float> noiseScaleVec = {0.3f, 0.8f, 1.3f, 1.8f};

    for (int i = 0; i < 4; i++) {
        auto heatDistortionPtr = CreateFilter(RSNGEffectType::HEAT_DISTORTION);
        auto heatDistortionFilter = std::static_pointer_cast<RSNGHeatDistortionFilter>(heatDistortionPtr);

        heatDistortionFilter->Setter<HeatDistortionIntensityTag>(1.0f);
        heatDistortionFilter->Setter<HeatDistortionNoiseScaleTag>(noiseScaleVec[i]);
        heatDistortionFilter->Setter<HeatDistortionRiseWeightTag>(0.2f);
        heatDistortionFilter->Setter<HeatDistortionProgressTag>(0.5f);

        const int x = (i % columnCount) * nodeWidth;
        const int y = (i / columnCount) * nodeHeight;

        auto backgroundNode = SetUpNodeBgImage(BG_PATH, {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetForegroundNGFilter(heatDistortionFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Rise weight parameter test - Test different rise weight values
GRAPHIC_TEST(NGFilterHeatDistortionTest, EFFECT_TEST, Set_NG_Filter_Heat_Distortion_RiseWeight_Test)
{
    const int columnCount = 2;
    const int nodeWidth = SCREEN_WIDTH / columnCount;
    const int nodeHeight = SCREEN_HEIGHT / 2;
    std::vector<float> riseWeightVec = {0.0f, 0.2f, 0.5f, 0.8f};

    for (int i = 0; i < 4; i++) {
        auto heatDistortionPtr = CreateFilter(RSNGEffectType::HEAT_DISTORTION);
        auto heatDistortionFilter = std::static_pointer_cast<RSNGHeatDistortionFilter>(heatDistortionPtr);

        heatDistortionFilter->Setter<HeatDistortionIntensityTag>(1.0f);
        heatDistortionFilter->Setter<HeatDistortionNoiseScaleTag>(1.0f);
        heatDistortionFilter->Setter<HeatDistortionRiseWeightTag>(riseWeightVec[i]);
        heatDistortionFilter->Setter<HeatDistortionProgressTag>(0.5f);

        const int x = (i % columnCount) * nodeWidth;
        const int y = (i / columnCount) * nodeHeight;

        auto backgroundNode = SetUpNodeBgImage(BG_PATH, {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetForegroundNGFilter(heatDistortionFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}

// Progress parameter test - Test different progress values
GRAPHIC_TEST(NGFilterHeatDistortionTest, EFFECT_TEST, Set_NG_Filter_Heat_Distortion_Progress_Test)
{
    const int columnCount = 2;
    const int nodeWidth = SCREEN_WIDTH / columnCount;
    const int nodeHeight = SCREEN_HEIGHT / 2;
    std::vector<float> progressVec = {0.0f, 0.3f, 0.6f, 0.9f};

    for (int i = 0; i < 4; i++) {
        auto heatDistortionPtr = CreateFilter(RSNGEffectType::HEAT_DISTORTION);
        auto heatDistortionFilter = std::static_pointer_cast<RSNGHeatDistortionFilter>(heatDistortionPtr);

        heatDistortionFilter->Setter<HeatDistortionIntensityTag>(1.0f);
        heatDistortionFilter->Setter<HeatDistortionNoiseScaleTag>(1.0f);
        heatDistortionFilter->Setter<HeatDistortionRiseWeightTag>(0.2f);
        heatDistortionFilter->Setter<HeatDistortionProgressTag>(progressVec[i]);

        const int x = (i % columnCount) * nodeWidth;
        const int y = (i / columnCount) * nodeHeight;

        auto backgroundNode = SetUpNodeBgImage(BG_PATH, {x, y, nodeWidth, nodeHeight});
        backgroundNode->SetForegroundNGFilter(heatDistortionFilter);
        GetRootNode()->AddChild(backgroundNode);
        RegisterNode(backgroundNode);
    }
}
} // namespace OHOS::Rosen