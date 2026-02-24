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
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ng_filter_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Different blur modes (Radius variations)
const std::vector<float> frostedGlassBlurRadii = {10.0f, 20.0f, 30.0f, 40.0f};

// Boundary values
const std::vector<float> frostedGlassBoundaryRadii = {0.0f, 100.0f};

// Intensity variations (RadiusScaleK)
const std::vector<float> frostedGlassIntensities = {0.5f, 1.0f, 2.0f};

// Parameter combinations for interaction testing
const std::vector<std::pair<float, float>> frostedGlassRadiusIntensityCombinations = {
    {10.0f, 0.5f},  // Small radius, low intensity
    {50.0f, 1.0f},  // Mid radius, mid intensity
    {100.0f, 2.0f}  // Max radius, high intensity
};
}

class NGFilterFrostedGlassBlurTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGFrostedGlassBlurFilter>& frostedGlassBlurFilter)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGFilter(frostedGlassBlurFilter);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test frosted glass blur with different blur radii
 * Tests 4 different radius values
 */
GRAPHIC_TEST(NGFilterFrostedGlassBlurTest, EFFECT_TEST, Set_Frosted_Glass_Blur_Radius_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < frostedGlassBlurRadii.size(); i++) {
        auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
        InitFrostedGlassBlurFilter(frostedGlassBlurFilter);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusTag>(frostedGlassBlurRadii[i]);

        SetUpTestNode(i, columnCount, rowCount, frostedGlassBlurFilter);
    }
}

/*
 * Test frosted glass blur with boundary values
 * Tests radius = 0 and radius = 100
 */
GRAPHIC_TEST(NGFilterFrostedGlassBlurTest, EFFECT_TEST, Set_Frosted_Glass_Blur_Boundary_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < frostedGlassBoundaryRadii.size(); i++) {
        auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
        InitFrostedGlassBlurFilter(frostedGlassBlurFilter);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusTag>(frostedGlassBoundaryRadii[i]);

        SetUpTestNode(i, columnCount, rowCount, frostedGlassBlurFilter);
    }
}

/*
 * Test frosted glass blur with different intensities
 * Tests 3 different intensity scale values
 */
GRAPHIC_TEST(NGFilterFrostedGlassBlurTest, EFFECT_TEST, Set_Frosted_Glass_Blur_Intensity_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < frostedGlassIntensities.size(); i++) {
        auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
        InitFrostedGlassBlurFilter(frostedGlassBlurFilter);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusScaleKTag>(frostedGlassIntensities[i]);

        SetUpTestNode(i, columnCount, rowCount, frostedGlassBlurFilter);
    }
}

/*
 * Test frosted glass blur with radius and intensity combinations
 * Tests interaction between radius and intensity parameters
 */
GRAPHIC_TEST(NGFilterFrostedGlassBlurTest, EFFECT_TEST,
    Set_Frosted_Glass_Blur_Radius_Intensity_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < frostedGlassRadiusIntensityCombinations.size(); i++) {
        auto frostedGlassBlurFilter = std::make_shared<RSNGFrostedGlassBlurFilter>();
        InitFrostedGlassBlurFilter(frostedGlassBlurFilter);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusTag>(frostedGlassRadiusIntensityCombinations[i].first);
        frostedGlassBlurFilter->Setter<FrostedGlassBlurRadiusScaleKTag>(
            frostedGlassRadiusIntensityCombinations[i].second);

        SetUpTestNode(i, columnCount, rowCount, frostedGlassBlurFilter);
    }
}

} // namespace OHOS::Rosen
