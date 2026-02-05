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

// Threshold (SpreadFactor) variations
const std::vector<float> sdfThresholdParams = {0.0f, 0.3f, 0.5f, 0.7f};

// Smoothness (BloomFalloffPow) variations
const std::vector<float> sdfSmoothnessParams = {1.0f, 2.0f, 5.0f};

// Boundary values for bloom intensity
const std::vector<float> sdfBloomIntensityParams = {0.0f, 0.5f, 1.0f};

// Extreme values for robustness testing
const std::vector<float> sdfThresholdExtremeValues = {-0.5f, 1.5f};

// Parameter combinations for interaction testing
const std::vector<std::pair<float, float>> sdfThresholdSmoothnessCombinations = {
    {0.0f, 1.0f},  // Min threshold, min smoothness
    {0.5f, 5.0f},  // Mid threshold, max smoothness
    {1.0f, 2.0f}   // Max threshold, mid smoothness
};
}

class NGFilterSDFEdgeLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGSDFEdgeLightFilter>& sdfEdgeLightFilter)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGFilter(sdfEdgeLightFilter);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test SDF edge light with different threshold values
 * Tests 4 threshold values: 0.0, 0.3, 0.5, 0.7
 */
GRAPHIC_TEST(NGFilterSDFEdgeLightTest, EFFECT_TEST, Set_SDF_Edge_Light_Threshold_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < sdfThresholdParams.size(); i++) {
        auto sdfEdgeLightFilter = std::make_shared<RSNGSDFEdgeLightFilter>();
        InitSDFEdgeLight(sdfEdgeLightFilter);
        sdfEdgeLightFilter->Setter<SDFEdgeLightSpreadFactorTag>(sdfThresholdParams[i]);

        SetUpTestNode(i, columnCount, rowCount, sdfEdgeLightFilter);
    }
}

/*
 * Test SDF edge light with different smoothness values
 * Tests 3 smoothness (falloff power) values
 */
GRAPHIC_TEST(NGFilterSDFEdgeLightTest, EFFECT_TEST, Set_SDF_Edge_Light_Smoothness_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < sdfSmoothnessParams.size(); i++) {
        auto sdfEdgeLightFilter = std::make_shared<RSNGSDFEdgeLightFilter>();
        InitSDFEdgeLight(sdfEdgeLightFilter);
        sdfEdgeLightFilter->Setter<SDFEdgeLightBloomFalloffPowTag>(sdfSmoothnessParams[i]);

        SetUpTestNode(i, columnCount, rowCount, sdfEdgeLightFilter);
    }
}

/*
 * Test SDF edge light with boundary bloom intensity values
 * Tests 3 intensity levels: 0.0, 0.5, 1.0
 */
GRAPHIC_TEST(NGFilterSDFEdgeLightTest, EFFECT_TEST, Set_SDF_Edge_Light_Bloom_Intensity_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < sdfBloomIntensityParams.size(); i++) {
        auto sdfEdgeLightFilter = std::make_shared<RSNGSDFEdgeLightFilter>();
        InitSDFEdgeLight(sdfEdgeLightFilter);
        sdfEdgeLightFilter->Setter<SDFEdgeLightMaxBloomIntensityTag>(sdfBloomIntensityParams[i]);

        SetUpTestNode(i, columnCount, rowCount, sdfEdgeLightFilter);
    }
}

/*
 * Test SDF edge light with extreme threshold values
 * Tests negative and out-of-range threshold values
 * Expected: System should handle gracefully (clamp or ignore)
 */
GRAPHIC_TEST(NGFilterSDFEdgeLightTest, EFFECT_TEST, Set_SDF_Edge_Light_Threshold_Extreme_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < sdfThresholdExtremeValues.size(); i++) {
        auto sdfEdgeLightFilter = std::make_shared<RSNGSDFEdgeLightFilter>();
        InitSDFEdgeLight(sdfEdgeLightFilter);
        sdfEdgeLightFilter->Setter<SDFEdgeLightSpreadFactorTag>(sdfThresholdExtremeValues[i]);

        SetUpTestNode(i, columnCount, rowCount, sdfEdgeLightFilter);
    }
}

/*
 * Test SDF edge light with threshold and smoothness combinations
 * Tests interaction between threshold and smoothness parameters
 */
GRAPHIC_TEST(NGFilterSDFEdgeLightTest, EFFECT_TEST, Set_SDF_Edge_Light_Threshold_Smoothness_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < sdfThresholdSmoothnessCombinations.size(); i++) {
        auto sdfEdgeLightFilter = std::make_shared<RSNGSDFEdgeLightFilter>();
        InitSDFEdgeLight(sdfEdgeLightFilter);
        sdfEdgeLightFilter->Setter<SDFEdgeLightSpreadFactorTag>(sdfThresholdSmoothnessCombinations[i].first);
        sdfEdgeLightFilter->Setter<SDFEdgeLightBloomFalloffPowTag>(sdfThresholdSmoothnessCombinations[i].second);

        SetUpTestNode(i, columnCount, rowCount, sdfEdgeLightFilter);
    }
}

} // namespace OHOS::Rosen
