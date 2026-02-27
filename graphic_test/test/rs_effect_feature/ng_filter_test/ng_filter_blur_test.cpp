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

// Test parameters for blur radius
// Valid range: [0, 100]
const std::vector<float> blurRadiusBasicParams = {10.0f, 20.0f, 30.0f, 40.0f};
const float BLUR_RADIUS_ZERO = 0.0f;
const float BLUR_RADIUS_MAX = 100.0f;

// Anisotropic blur parameters (different X and Y radii)
const std::vector<std::pair<float, float>> anisotropicBlurParams = {
    {10.0f, 20.0f},  // Different radii
    {30.0f, 10.0f},  // Inverted radii
    {50.0f, 25.0f}   // Large X, smaller Y
};

// Extreme values for robustness testing
const std::vector<float> blurRadiusExtremeValues = {-10.0f, 150.0f};

// Parameter combinations for interaction testing
const std::vector<std::pair<float, float>> blurRadiusCombinations = {
    {0.0f, 100.0f},   // Min X, Max Y
    {100.0f, 0.0f},   // Max X, Min Y
    {50.0f, 50.0f}    // Mid values
};
}

class NGFilterBlurTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGBlurFilter>& blurFilter)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGFilter(blurFilter);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test basic blur radius values
 * Tests 4 different radius values: 10, 20, 30, 40
 */
GRAPHIC_TEST(NGFilterBlurTest, EFFECT_TEST, Set_Blur_Basic_Radius_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;

    for (size_t i = 0; i < blurRadiusBasicParams.size(); i++) {
        auto blurFilter = std::make_shared<RSNGBlurFilter>();
        InitBlurFilter(blurFilter);
        blurFilter->Setter<BlurRadiusXTag>(blurRadiusBasicParams[i]);
        blurFilter->Setter<BlurRadiusYTag>(blurRadiusBasicParams[i]);

        SetUpTestNode(i, columnCount, rowCount, blurFilter);
    }
}

/*
 * Test zero blur radius (boundary value)
 * Expected: No blur effect applied
 */
GRAPHIC_TEST(NGFilterBlurTest, EFFECT_TEST, Set_Blur_Zero_Radius_Test)
{
    auto blurFilter = std::make_shared<RSNGBlurFilter>();
    InitBlurFilter(blurFilter);
    blurFilter->Setter<BlurRadiusXTag>(BLUR_RADIUS_ZERO);
    blurFilter->Setter<BlurRadiusYTag>(BLUR_RADIUS_ZERO);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    testNode->SetBackgroundNGFilter(blurFilter);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test maximum blur radius (boundary value)
 * Tests radius = 100 (maximum valid value)
 */
GRAPHIC_TEST(NGFilterBlurTest, EFFECT_TEST, Set_Blur_Max_Radius_Test)
{
    auto blurFilter = std::make_shared<RSNGBlurFilter>();
    InitBlurFilter(blurFilter);
    blurFilter->Setter<BlurRadiusXTag>(BLUR_RADIUS_MAX);
    blurFilter->Setter<BlurRadiusYTag>(BLUR_RADIUS_MAX);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
    testNode->SetBackgroundNGFilter(blurFilter);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * Test anisotropic blur (different X and Y radii)
 * Tests 3 combinations of different RadiusX and RadiusY values
 */
GRAPHIC_TEST(NGFilterBlurTest, EFFECT_TEST, Set_Blur_Anisotropic_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < anisotropicBlurParams.size(); i++) {
        auto blurFilter = std::make_shared<RSNGBlurFilter>();
        InitBlurFilter(blurFilter);
        blurFilter->Setter<BlurRadiusXTag>(anisotropicBlurParams[i].first);
        blurFilter->Setter<BlurRadiusYTag>(anisotropicBlurParams[i].second);

        SetUpTestNode(i, columnCount, rowCount, blurFilter);
    }
}

/*
 * Test blur with extreme and invalid values
 * Tests negative value and value beyond maximum (>100)
 * Expected: System should handle gracefully (clamp or ignore)
 */
GRAPHIC_TEST(NGFilterBlurTest, EFFECT_TEST, Set_Blur_Extreme_Values_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < blurRadiusExtremeValues.size(); i++) {
        auto blurFilter = std::make_shared<RSNGBlurFilter>();
        InitBlurFilter(blurFilter);
        blurFilter->Setter<BlurRadiusXTag>(blurRadiusExtremeValues[i]);
        blurFilter->Setter<BlurRadiusYTag>(blurRadiusExtremeValues[i]);

        SetUpTestNode(i, columnCount, rowCount, blurFilter);
    }
}

/*
 * Test blur with parameter combinations
 * Tests different combinations of RadiusX and RadiusY values
 */
GRAPHIC_TEST(NGFilterBlurTest, EFFECT_TEST, Set_Blur_Parameter_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < blurRadiusCombinations.size(); i++) {
        auto blurFilter = std::make_shared<RSNGBlurFilter>();
        InitBlurFilter(blurFilter);
        blurFilter->Setter<BlurRadiusXTag>(blurRadiusCombinations[i].first);
        blurFilter->Setter<BlurRadiusYTag>(blurRadiusCombinations[i].second);

        SetUpTestNode(i, columnCount, rowCount, blurFilter);
    }
}

} // namespace OHOS::Rosen
