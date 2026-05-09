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

#include "ui/rs_effect_node.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

void InitDupoliNoiseMask(std::shared_ptr<RSNGDupoliNoiseMask>& dupoliNoiseMask)
{
    if (!dupoliNoiseMask) {
        return;
    }
    dupoliNoiseMask->Setter<DupoliNoiseMaskProgressTag>(0.5f);
    dupoliNoiseMask->Setter<DupoliNoiseMaskGranularityTag>(1.0f);
    dupoliNoiseMask->Setter<DupoliNoiseMaskVerticalMoveDistanceTag>(10.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

const std::vector<float> progressValues = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };

const std::vector<float> granularityValues = { 0.0f, 0.5f, 1.0f, 2.0f, 5.0f };

const std::vector<float> verticalMoveDistanceValues = { 0.0f, 5.0f, 10.0f, 20.0f, 50.0f };

const std::vector<std::tuple<float, float, float>> parameterCombinations = { { 0.25f, 0.5f, 5.0f },
    { 0.5f, 1.0f, 10.0f }, { 0.75f, 2.0f, 20.0f } };

const std::vector<float> extremeProgressValues = { -0.5f, 1.5f, 100.0f };

const std::vector<float> extremeGranularityValues = { -1.0f, 100.0f, std::numeric_limits<float>::quiet_NaN() };
} // namespace

class NGMaskDupoliNoiseTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const std::shared_ptr<RSNGDupoliNoiseMask>& dupoliNoiseMask)
    {
        const size_t sizeX = SCREEN_WIDTH;
        const size_t sizeY = SCREEN_HEIGHT;

        auto parentNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { 0, 0, sizeX, sizeY });
        parentNode->SetFrame({ 0, 0, sizeX, sizeY });
        GetRootNode()->AddChild(parentNode);
        RegisterNode(parentNode);

        auto effectNode = RSEffectNode::Create();
        effectNode->SetBounds({ 0, 0, sizeX, sizeY });
        effectNode->SetFrame({ 0, 0, sizeX, sizeY });
        effectNode->SetBackgroundColor(0xffff0000);
        parentNode->AddChild(effectNode);
        RegisterNode(effectNode);

        auto harmoniumEffect = std::make_shared<RSNGHarmoniumEffect>();
        harmoniumEffect->Setter<HarmoniumEffectMaskTag>(std::static_pointer_cast<RSNGMaskBase>(dupoliNoiseMask));
        harmoniumEffect->Setter<HarmoniumEffectDistortProgressTag>(1.0f);

        auto effectChildNode = RSCanvasNode::Create();
        effectChildNode->SetBounds({ 0, 0, sizeX, sizeY });
        effectChildNode->SetFrame({ 0, 0, sizeX, sizeY });
        effectChildNode->SetBackgroundNGShader(harmoniumEffect);
        effectNode->AddChild(effectChildNode);
        RegisterNode(effectChildNode);
    }

    const int screenWidth = 1200;
    const int screenHeight = 2000;
};

GRAPHIC_TEST(NGMaskDupoliNoiseTest, EFFECT_TEST, Set_Dupoli_Noise_Mask_Progress_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(progressValues.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (int i = 0; i < progressValues.size(); i++) {
        // Create double ripple mask
        auto mask = CreateMask(RSNGEffectType::DUPOLI_NOISE_MASK);
        auto dupoliNoiseMask = std::static_pointer_cast<RSNGDupoliNoiseMask>(mask);
        dupoliNoiseMask->Setter<DupoliNoiseMaskProgressTag>(0.5f);
        dupoliNoiseMask->Setter<DupoliNoiseMaskGranularityTag>(1.0f);
        dupoliNoiseMask->Setter<DupoliNoiseMaskVerticalMoveDistanceTag>(10.0f);
        dupoliNoiseMask->Setter<DupoliNoiseMaskProgressTag>(progressValues[i]);

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

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGMaskDupoliNoiseTest, EFFECT_TEST, Set_Dupoli_Noise_Mask_Granularity_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(granularityValues.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < granularityValues.size(); i++) {
        auto mask = CreateMask(RSNGEffectType::DUPOLI_NOISE_MASK);
        auto dupoliNoiseMask = std::static_pointer_cast<RSNGDupoliNoiseMask>(mask);
        InitDupoliNoiseMask(dupoliNoiseMask);
        dupoliNoiseMask->Setter<DupoliNoiseMaskGranularityTag>(granularityValues[i]);

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

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGMaskDupoliNoiseTest, EFFECT_TEST, Set_Dupoli_Noise_Mask_VerticalMoveDistance_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(verticalMoveDistanceValues.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < verticalMoveDistanceValues.size(); i++) {
        auto mask = CreateMask(RSNGEffectType::DUPOLI_NOISE_MASK);
        auto dupoliNoiseMask = std::static_pointer_cast<RSNGDupoliNoiseMask>(mask);
        InitDupoliNoiseMask(dupoliNoiseMask);
        dupoliNoiseMask->Setter<DupoliNoiseMaskVerticalMoveDistanceTag>(verticalMoveDistanceValues[i]);

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

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGMaskDupoliNoiseTest, EFFECT_TEST, Set_Dupoli_Noise_Mask_Parameter_Combination_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(parameterCombinations.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < parameterCombinations.size(); i++) {
        auto mask = CreateMask(RSNGEffectType::DUPOLI_NOISE_MASK);
        auto dupoliNoiseMask = std::static_pointer_cast<RSNGDupoliNoiseMask>(mask);
        InitDupoliNoiseMask(dupoliNoiseMask);
        dupoliNoiseMask->Setter<DupoliNoiseMaskProgressTag>(std::get<0>(parameterCombinations[i]));
        dupoliNoiseMask->Setter<DupoliNoiseMaskGranularityTag>(std::get<1>(parameterCombinations[i]));
        dupoliNoiseMask->Setter<DupoliNoiseMaskVerticalMoveDistanceTag>(std::get<2>(parameterCombinations[i]));

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

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGMaskDupoliNoiseTest, EFFECT_TEST, Set_Dupoli_Noise_Mask_Progress_Extreme_Values_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(extremeProgressValues.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < extremeProgressValues.size(); i++) {
        auto mask = CreateMask(RSNGEffectType::DUPOLI_NOISE_MASK);
        auto dupoliNoiseMask = std::static_pointer_cast<RSNGDupoliNoiseMask>(mask);
        InitDupoliNoiseMask(dupoliNoiseMask);
        dupoliNoiseMask->Setter<DupoliNoiseMaskProgressTag>(extremeProgressValues[i]);

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

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

GRAPHIC_TEST(NGMaskDupoliNoiseTest, EFFECT_TEST, Set_Dupoli_Noise_Mask_Granularity_Extreme_Values_Test)
{
    int columnCount = 2;
    int rowCount = static_cast<int>(extremeGranularityValues.size());
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight * columnCount / rowCount;
    for (size_t i = 0; i < extremeGranularityValues.size(); i++) {
        auto mask = CreateMask(RSNGEffectType::DUPOLI_NOISE_MASK);
        auto dupoliNoiseMask = std::static_pointer_cast<RSNGDupoliNoiseMask>(mask);
        InitDupoliNoiseMask(dupoliNoiseMask);
        dupoliNoiseMask->Setter<DupoliNoiseMaskGranularityTag>(extremeGranularityValues[i]);

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

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto backgroundTestNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, sizeX, sizeY });
        backgroundTestNode->SetBackgroundNGFilter(colorGradientFilter);
        GetRootNode()->AddChild(backgroundTestNode);
        RegisterNode(backgroundTestNode);
    }
}

} // namespace OHOS::Rosen