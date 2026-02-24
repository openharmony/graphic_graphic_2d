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

// Color variations (RGBA)
// Alpha range: [0.0, 1.0]
const std::vector<Vector4f> edgeLightColors = {
    Vector4f{1.0f, 0.0f, 0.0f, 1.0f},  // Red
    Vector4f{0.0f, 1.0f, 0.0f, 1.0f},  // Green
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f},  // Blue
    Vector4f{1.0f, 1.0f, 0.0f, 0.8f},  // Yellow with transparency
    Vector4f{1.0f, 1.0f, 1.0f, 1.0f}   // White
};

// Width boundary values
const std::vector<float> edgeLightWidths = {0.0f, 10.0f, 50.0f};

// Intensity variations
const std::vector<float> edgeLightIntensities = {0.5f, 1.0f, 1.5f, 2.0f};

// Parameter combinations for interaction testing
const std::vector<std::pair<Vector4f, float>> edgeLightColorIntensityCombinations = {
    {Vector4f{1.0f, 0.0f, 0.0f, 1.0f}, 0.5f},  // Red, low intensity
    {Vector4f{0.0f, 1.0f, 0.0f, 1.0f}, 1.5f},  // Green, high intensity
    {Vector4f{0.0f, 0.0f, 1.0f, 1.0f}, 2.0f}   // Blue, max intensity
};
}

class NGFilterEdgeLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGEdgeLightFilter>& edgeLightFilter)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGFilter(edgeLightFilter);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test edge light with different colors
 * Tests 5 different color combinations including RGB and transparency
 */
GRAPHIC_TEST(NGFilterEdgeLightTest, EFFECT_TEST, Set_Edge_Light_Color_Test)
{
    const size_t columnCount = 5;
    const size_t rowCount = 1;

    for (size_t i = 0; i < edgeLightColors.size(); i++) {
        auto edgeLightFilter = std::make_shared<RSNGEdgeLightFilter>();
        InitEdgeLight(edgeLightFilter);
        edgeLightFilter->Setter<EdgeLightColorTag>(edgeLightColors[i]);

        SetUpTestNode(i, columnCount, rowCount, edgeLightFilter);
    }
}

/*
 * Test edge light with boundary width values
 * Tests 3 width values: 0 (no effect), 10 (small), 50 (large)
 */
GRAPHIC_TEST(NGFilterEdgeLightTest, EFFECT_TEST, Set_Edge_Light_Width_Boundary_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < edgeLightWidths.size(); i++) {
        auto edgeLightFilter = std::make_shared<RSNGEdgeLightFilter>();
        InitEdgeLight(edgeLightFilter);
        if (edgeLightWidths[i] > 0.0f) {
            edgeLightFilter->Setter<EdgeLightBloomTag>(true);
        }
        edgeLightFilter->Setter<EdgeLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});

        SetUpTestNode(i, columnCount, rowCount, edgeLightFilter);
    }
}

/*
 * Test edge light with different intensity values
 * Tests 4 intensity levels
 */
GRAPHIC_TEST(NGFilterEdgeLightTest, EFFECT_TEST, Set_Edge_Light_Intensity_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < edgeLightIntensities.size(); i++) {
        auto edgeLightFilter = std::make_shared<RSNGEdgeLightFilter>();
        InitEdgeLight(edgeLightFilter);
        Vector4f intensityColor{edgeLightIntensities[i], edgeLightIntensities[i], edgeLightIntensities[i], 1.0f};
        edgeLightFilter->Setter<EdgeLightColorTag>(intensityColor);

        SetUpTestNode(i, columnCount, rowCount, edgeLightFilter);
    }
}

/*
 * Test edge light with color and intensity combinations
 * Tests interaction between color and intensity parameters
 */
GRAPHIC_TEST(NGFilterEdgeLightTest, EFFECT_TEST, Set_Edge_Light_Color_Intensity_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < edgeLightColorIntensityCombinations.size(); i++) {
        auto edgeLightFilter = std::make_shared<RSNGEdgeLightFilter>();
        InitEdgeLight(edgeLightFilter);
        // Apply intensity by scaling the RGB components
        Vector4f scaledColor = edgeLightColorIntensityCombinations[i].first;
        scaledColor[0] *= edgeLightColorIntensityCombinations[i].second;
        scaledColor[1] *= edgeLightColorIntensityCombinations[i].second;
        scaledColor[2] *= edgeLightColorIntensityCombinations[i].second;

        edgeLightFilter->Setter<EdgeLightColorTag>(scaledColor);

        SetUpTestNode(i, columnCount, rowCount, edgeLightFilter);
    }
}

} // namespace OHOS::Rosen
