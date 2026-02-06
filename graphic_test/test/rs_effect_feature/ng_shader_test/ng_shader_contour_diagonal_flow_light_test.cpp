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
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

void InitContourDiagonalFlowLight(std::shared_ptr<RSNGContourDiagonalFlowLight>& contourDiagonalFlowLight)
{
    if (!contourDiagonalFlowLight) {
        return;
    }
    // Contour (vector of Vector2f points)
    std::vector<Vector2f> contour = {
        Vector2f{0.2f, 0.2f},
        Vector2f{0.8f, 0.2f},
        Vector2f{0.8f, 0.8f},
        Vector2f{0.2f, 0.8f}
    };
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightContourTag>(contour);
    // Line1Start
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine1StartTag>(0.0f);
    // Line1Length
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine1LengthTag>(0.5f);
    // Line1Color
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine1ColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Line2Start
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine2StartTag>(0.5f);
    // Line2Length
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine2LengthTag>(0.5f);
    // Line2Color
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLine2ColorTag>(Vector4f{0.8f, 0.8f, 1.0f, 1.0f});
    // Thickness
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightThicknessTag>(2.0f);
    // HaloRadius
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightHaloRadiusTag>(10.0f);
    // LightWeight
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightLightWeightTag>(1.0f);
    // HaloWeight
    contourDiagonalFlowLight->Setter<ContourDiagonalFlowLightHaloWeightTag>(1.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Contour values
const std::vector<std::vector<Vector2f>> contourValues = {
    {Vector2f{0.0f, 0.0f}},
    {Vector2f{0.5f, 0.5f}},
    {Vector2f{1.0f, 1.0f}}
};

// Thickness values
const std::vector<float> thicknessValues = {0.0f, 5.0f, 10.0f, 20.0f};

// Halo radius values
const std::vector<float> haloRadiusValues = {0.0f, 10.0f, 50.0f};

// Light weight values
const std::vector<float> lightWeightValues = {0.0f, 0.5f, 1.0f};
}

class NGShaderContourDiagonalFlowLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGContourDiagonalFlowLight>& contourLight)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(contourLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test contour diagonal flow light with different contour values
 */
GRAPHIC_TEST(NGShaderContourDiagonalFlowLightTest, EFFECT_TEST, Set_Contour_Diagonal_Flow_Light_Contour_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < contourValues.size(); i++) {
        auto contourLight = std::make_shared<RSNGContourDiagonalFlowLight>();
        InitContourDiagonalFlowLight(contourLight);
        contourLight->Setter<ContourDiagonalFlowLightContourTag>(std::vector<Vector2f>{Vector2f{0.5f, 0.5f}});
        contourLight->Setter<ContourDiagonalFlowLightThicknessTag>(10.0f);

        SetUpTestNode(i, columnCount, rowCount, contourLight);
    }
}

/*
 * Test contour diagonal flow light with boundary thickness values
 */
GRAPHIC_TEST(NGShaderContourDiagonalFlowLightTest, EFFECT_TEST, Set_Contour_Diagonal_Flow_Light_Thickness_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < thicknessValues.size(); i++) {
        auto contourLight = std::make_shared<RSNGContourDiagonalFlowLight>();
        InitContourDiagonalFlowLight(contourLight);
        contourLight->Setter<ContourDiagonalFlowLightContourTag>(std::vector<Vector2f>{Vector2f{0.5f, 0.5f}});
        contourLight->Setter<ContourDiagonalFlowLightThicknessTag>(thicknessValues[i]);

        SetUpTestNode(i, columnCount, rowCount, contourLight);
    }
}

/*
 * Test contour diagonal flow light with halo radius and weight combinations
 */
GRAPHIC_TEST(NGShaderContourDiagonalFlowLightTest, EFFECT_TEST, Set_Contour_Diagonal_Flow_Light_Halo_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < haloRadiusValues.size(); i++) {
        auto contourLight = std::make_shared<RSNGContourDiagonalFlowLight>();
        InitContourDiagonalFlowLight(contourLight);
        contourLight->Setter<ContourDiagonalFlowLightContourTag>(std::vector<Vector2f>{Vector2f{0.5f, 0.5f}});
        contourLight->Setter<ContourDiagonalFlowLightThicknessTag>(10.0f);
        contourLight->Setter<ContourDiagonalFlowLightHaloRadiusTag>(haloRadiusValues[i]);
        contourLight->Setter<ContourDiagonalFlowLightLightWeightTag>(lightWeightValues[i]);

        SetUpTestNode(i, columnCount, rowCount, contourLight);
    }
}

/*
 * Test contour diagonal flow light with extreme and invalid values
 * Tests malicious inputs: negative values, extremely large values
 */
GRAPHIC_TEST(NGShaderContourDiagonalFlowLightTest, EFFECT_TEST, Set_Contour_Diagonal_Flow_Light_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};
    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto contourLight = std::make_shared<RSNGContourDiagonalFlowLight>();
        InitContourDiagonalFlowLight(contourLight);
        std::vector<Vector2f> contourVec = {Vector2f{extremeValues[i], extremeValues[i]}};
        contourLight->Setter<ContourDiagonalFlowLightContourTag>(contourVec);
        SetUpTestNode(i, columnCount, rowCount, contourLight);
    }
}

} // namespace OHOS::Rosen
