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

void InitWavyRippleLight(std::shared_ptr<RSNGWavyRippleLight>& wavyRippleLight)
{
    if (!wavyRippleLight) {
        return;
    }
    // Center
    wavyRippleLight->Setter<WavyRippleLightCenterTag>(Vector2f{0.5f, 0.5f});
    // Radius
    wavyRippleLight->Setter<WavyRippleLightRadiusTag>(50.0f);
    // Thickness
    wavyRippleLight->Setter<WavyRippleLightThicknessTag>(10.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Center position variations (Vector2f: x, y)
const std::vector<Vector2f> wavyRippleCenters = {
    Vector2f{0.0f, 0.0f},       // Top-left corner
    Vector2f{600.0f, 1000.0f},  // Center
    Vector2f{1200.0f, 2000.0f}  // Bottom-right corner
};

// Radius boundary values and variations
const std::vector<float> wavyRippleRadii = {0.0f, 50.0f, 200.0f, 500.0f};

// Thickness variations
const std::vector<float> wavyRippleThicknesses = {0.0f, 5.0f, 10.0f, 20.0f};

// Parameter combinations for interaction testing
const std::vector<std::pair<float, float>> wavyRippleRadiusThicknessCombinations = {
    {50.0f, 5.0f},    // Small radius, thin
    {200.0f, 10.0f},  // Medium radius, medium thickness
    {500.0f, 20.0f}   // Large radius, thick
};

// Extreme and invalid values for robustness testing
const std::vector<float> wavyRippleExtremeRadii = {
    -100.0f,       // Negative radius
    -1.0f,         // Small negative
    9999.0f,       // Extremely large
    1e10f          // astronomical value
};

const std::vector<float> wavyRippleExtremeThicknesses = {
    -50.0f,        // Negative thickness
    -0.1f,         // Small negative
    1000.0f,       // Extremely large
    std::numeric_limits<float>::quiet_NaN(),  // NaN
    std::numeric_limits<float>::infinity()    // Infinity
};
}

class NGShaderWavyRippleLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGWavyRippleLight>& wavyRippleLight)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(wavyRippleLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test wavy ripple light with different center positions
 * Tests 3 different center positions including corners and center
 */
GRAPHIC_TEST(NGShaderWavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Center_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < wavyRippleCenters.size(); i++) {
        auto wavyRippleLight = std::make_shared<RSNGWavyRippleLight>();
        InitWavyRippleLight(wavyRippleLight);
        wavyRippleLight->Setter<WavyRippleLightCenterTag>(wavyRippleCenters[i]);
        wavyRippleLight->Setter<WavyRippleLightRadiusTag>(200.0f);
        wavyRippleLight->Setter<WavyRippleLightThicknessTag>(10.0f);

        SetUpTestNode(i, columnCount, rowCount, wavyRippleLight);
    }
}

/*
 * Test wavy ripple light with boundary radius values
 * Tests 4 radius values: 0 (no effect), small, medium, large
 */
GRAPHIC_TEST(NGShaderWavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Radius_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < wavyRippleRadii.size(); i++) {
        auto wavyRippleLight = std::make_shared<RSNGWavyRippleLight>();
        InitWavyRippleLight(wavyRippleLight);
        wavyRippleLight->Setter<WavyRippleLightCenterTag>(Vector2f{600.0f, 1000.0f});
        wavyRippleLight->Setter<WavyRippleLightRadiusTag>(wavyRippleRadii[i]);
        wavyRippleLight->Setter<WavyRippleLightThicknessTag>(10.0f);

        SetUpTestNode(i, columnCount, rowCount, wavyRippleLight);
    }
}

/*
 * Test wavy ripple light with different thickness values
 * Tests 4 thickness values
 */
GRAPHIC_TEST(NGShaderWavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Thickness_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < wavyRippleThicknesses.size(); i++) {
        auto wavyRippleLight = std::make_shared<RSNGWavyRippleLight>();
        InitWavyRippleLight(wavyRippleLight);
        wavyRippleLight->Setter<WavyRippleLightCenterTag>(Vector2f{600.0f, 1000.0f});
        wavyRippleLight->Setter<WavyRippleLightRadiusTag>(200.0f);
        wavyRippleLight->Setter<WavyRippleLightThicknessTag>(wavyRippleThicknesses[i]);

        SetUpTestNode(i, columnCount, rowCount, wavyRippleLight);
    }
}

/*
 * Test wavy ripple light with radius and thickness combinations
 * Tests interaction between radius and thickness parameters
 */
GRAPHIC_TEST(NGShaderWavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Radius_Thickness_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < wavyRippleRadiusThicknessCombinations.size(); i++) {
        auto wavyRippleLight = std::make_shared<RSNGWavyRippleLight>();
        InitWavyRippleLight(wavyRippleLight);
        wavyRippleLight->Setter<WavyRippleLightCenterTag>(Vector2f{600.0f, 1000.0f});
        wavyRippleLight->Setter<WavyRippleLightRadiusTag>(wavyRippleRadiusThicknessCombinations[i].first);
        wavyRippleLight->Setter<WavyRippleLightThicknessTag>(wavyRippleRadiusThicknessCombinations[i].second);

        SetUpTestNode(i, columnCount, rowCount, wavyRippleLight);
    }
}

/*
 * Test wavy ripple light with extreme and invalid radius values
 * Tests malicious inputs: negative values, extremely large values
 * Ensures system robustness against invalid parameters
 */
GRAPHIC_TEST(NGShaderWavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Radius_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < wavyRippleExtremeRadii.size(); i++) {
        auto wavyRippleLight = std::make_shared<RSNGWavyRippleLight>();
        InitWavyRippleLight(wavyRippleLight);
        wavyRippleLight->Setter<WavyRippleLightCenterTag>(Vector2f{600.0f, 1000.0f});
        wavyRippleLight->Setter<WavyRippleLightRadiusTag>(wavyRippleExtremeRadii[i]);
        wavyRippleLight->Setter<WavyRippleLightThicknessTag>(10.0f);

        SetUpTestNode(i, columnCount, rowCount, wavyRippleLight);
    }
}

/*
 * Test wavy ripple light with extreme and invalid thickness values
 * Tests malicious inputs: negative values, extremely large values, NaN, Infinity
 * Ensures system robustness against invalid parameters
 */
GRAPHIC_TEST(NGShaderWavyRippleLightTest, EFFECT_TEST, Set_Wavy_Ripple_Light_Thickness_Extreme_Values_Test)
{
    const size_t columnCount = 5;
    const size_t rowCount = 1;

    for (size_t i = 0; i < wavyRippleExtremeThicknesses.size(); i++) {
        auto wavyRippleLight = std::make_shared<RSNGWavyRippleLight>();
        InitWavyRippleLight(wavyRippleLight);
        wavyRippleLight->Setter<WavyRippleLightCenterTag>(Vector2f{600.0f, 1000.0f});
        wavyRippleLight->Setter<WavyRippleLightRadiusTag>(200.0f);
        wavyRippleLight->Setter<WavyRippleLightThicknessTag>(wavyRippleExtremeThicknesses[i]);

        SetUpTestNode(i, columnCount, rowCount, wavyRippleLight);
    }
}

} // namespace OHOS::Rosen
