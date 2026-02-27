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

void InitBorderLight(std::shared_ptr<RSNGBorderLight>& borderLight)
{
    if (!borderLight) {
        return;
    }
    // Position (x, y, z)
    borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 1.0f});
    // Color
    borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    // Intensity
    borderLight->Setter<BorderLightIntensityTag>(1.0f);
    // Width
    borderLight->Setter<BorderLightWidthTag>(10.0f);
    // RotationAngle (x, y, z)
    borderLight->Setter<BorderLightRotationAngleTag>(Vector3f{0.0f, 0.0f, 0.0f});
    // CornerRadius
    borderLight->Setter<BorderLightCornerRadiusTag>(10.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Position variations (Vector3f: x, y, z)
const std::vector<Vector3f> borderLightPositions = {
    Vector3f{0.0f, 0.0f, 0.0f},       // Origin
    Vector3f{0.5f, 0.5f, 0.5f},       // Center
    Vector3f{1.0f, 1.0f, 1.0f}        // Max
};

// Color variations (RGBA)
const std::vector<Vector4f> borderLightColors = {
    Vector4f{1.0f, 0.0f, 0.0f, 1.0f},  // Red
    Vector4f{0.0f, 1.0f, 0.0f, 1.0f},  // Green
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f},  // Blue
    Vector4f{1.0f, 1.0f, 1.0f, 1.0f}   // White
};

// Intensity boundary values
const std::vector<float> borderLightIntensities = {0.0f, 0.5f, 1.0f, 1.5f};

// Width boundary values
const std::vector<float> borderLightWidths = {0.0f, 10.0f, 50.0f, 100.0f};

// Rotation angle variations
const std::vector<Vector3f> borderLightRotationAngles = {
    Vector3f{0.0f, 0.0f, 0.0f},
    Vector3f{45.0f, 45.0f, 45.0f},
    Vector3f{90.0f, 90.0f, 90.0f},
    Vector3f{180.0f, 180.0f, 180.0f}
};

// Corner radius variations
const std::vector<float> borderLightCornerRadii = {0.0f, 10.0f, 25.0f, 50.0f};

// Parameter combinations for interaction testing
const std::vector<std::tuple<Vector4f, Vector3f, float, float>> borderLightColorIntensityWidthCombinations = {
    {Vector4f{1.0f, 0.0f, 0.0f, 1.0f}, Vector3f{0.5f, 0.5f, 0.5f}, 0.5f, 10.0f},   // Red, low intensity, thin
    {Vector4f{0.0f, 1.0f, 0.0f, 1.0f}, Vector3f{0.5f, 0.5f, 0.5f}, 1.0f, 50.0f},   // Green, normal intensity, medium
    {Vector4f{0.0f, 0.0f, 1.0f, 1.0f}, Vector3f{0.5f, 0.5f, 0.5f}, 1.5f, 100.0f}   // Blue, high intensity, thick
};

// Extreme and invalid values for robustness testing
const std::vector<float> borderLightExtremeIntensities = {
    -1.0f,         // Negative
    -10.0f,        // Large negative
    100.0f,        // Extremely high
    9999.0f        // astronomical value
};

const std::vector<float> borderLightExtremeWidths = {
    -50.0f,        // Negative width
    -1.0f,         // Small negative
    1000.0f,       // Extremely large
    std::numeric_limits<float>::quiet_NaN()  // NaN
};
}

class NGShaderBorderLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGBorderLight>& borderLight)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;  // Invalid test configuration
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(borderLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test border light with different colors
 * Tests 4 different color combinations including RGB and white
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Color_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < borderLightColors.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(borderLightColors[i]);
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(1.0f);
        borderLight->Setter<BorderLightWidthTag>(50.0f);

        SetUpTestNode(i, columnCount, rowCount, borderLight);
    }
}

/*
 * Test border light with boundary intensity values
 * Tests 4 intensity levels: 0 (no effect), low, normal, high
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Intensity_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < borderLightIntensities.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(borderLightIntensities[i]);
        borderLight->Setter<BorderLightWidthTag>(50.0f);

        SetUpTestNode(i, columnCount, rowCount, borderLight);
    }
}

/*
 * Test border light with boundary width values
 * Tests 4 width values: 0 (no effect), small, medium, large
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Width_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < borderLightWidths.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(1.0f);
        borderLight->Setter<BorderLightWidthTag>(borderLightWidths[i]);

        SetUpTestNode(i, columnCount, rowCount, borderLight);
    }
}

/*
 * Test border light with different rotation angles
 * Tests 4 rotation angles: 0, 45, 90, 180 degrees
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Rotation_Angle_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < borderLightRotationAngles.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(1.0f);
        borderLight->Setter<BorderLightWidthTag>(50.0f);
        borderLight->Setter<BorderLightRotationAngleTag>(borderLightRotationAngles[i]);

        SetUpTestNode(i, columnCount, rowCount, borderLight);
    }
}

/*
 * Test border light with corner radius variations
 * Tests 4 corner radius values
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Corner_Radius_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < borderLightCornerRadii.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(1.0f);
        borderLight->Setter<BorderLightWidthTag>(50.0f);
        borderLight->Setter<BorderLightCornerRadiusTag>(borderLightCornerRadii[i]);

        SetUpTestNode(i, columnCount, rowCount, borderLight);
    }
}

/*
 * Test border light with color, intensity, and width combinations
 * Tests interaction between color, intensity, and width parameters
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Color_Intensity_Width_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < borderLightColorIntensityWidthCombinations.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(std::get<0>(borderLightColorIntensityWidthCombinations[i]));
        borderLight->Setter<BorderLightPositionTag>(std::get<1>(borderLightColorIntensityWidthCombinations[i]));
        borderLight->Setter<BorderLightIntensityTag>(std::get<2>(borderLightColorIntensityWidthCombinations[i]));
        borderLight->Setter<BorderLightWidthTag>(std::get<3>(borderLightColorIntensityWidthCombinations[i]));

        SetUpTestNode(i, columnCount, rowCount, borderLight);
    }
}

/*
 * Test border light with extreme and invalid intensity values
 * Tests malicious inputs: negative values, extremely large values
 * Ensures system robustness against invalid parameters
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Intensity_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < borderLightExtremeIntensities.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(borderLightExtremeIntensities[i]);
        borderLight->Setter<BorderLightWidthTag>(50.0f);

        SetUpTestNode(i, columnCount, rowCount, borderLight);
    }
}

/*
 * Test border light with extreme and invalid width values
 * Tests malicious inputs: negative values, extremely large values, NaN
 * Ensures system robustness against invalid parameters
 */
GRAPHIC_TEST(NGShaderBorderLightTest, EFFECT_TEST, Set_Border_Light_Width_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < borderLightExtremeWidths.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLight(borderLight);
        borderLight->Setter<BorderLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
        borderLight->Setter<BorderLightIntensityTag>(1.0f);
        borderLight->Setter<BorderLightWidthTag>(borderLightExtremeWidths[i]);

        SetUpTestNode(i, columnCount, rowCount, borderLight);
    }
}

} // namespace OHOS::Rosen

