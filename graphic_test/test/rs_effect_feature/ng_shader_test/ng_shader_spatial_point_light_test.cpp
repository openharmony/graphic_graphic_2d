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
#include "ui_effect/property/include/rs_ui_mask_base.h"
#include <limits>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// Default SpatialPointLight parameters
constexpr float DEFAULT_LIGHT_X = 600.0f;
constexpr float DEFAULT_LIGHT_Y = 1000.0f;
constexpr float DEFAULT_LIGHT_Z = 100.0f;
constexpr float DEFAULT_LIGHT_INTENSITY = 2.0f;
constexpr float DEFAULT_LIGHT_ATTENUATION = 5.0f;
constexpr Vector4f DEFAULT_LIGHT_COLOR = Vector4f{1.0f, 0.5f, 0.0f, 1.0f}; // Orange

// Default shader cascade parameters
constexpr float DEFAULT_BORDER_LIGHT_INTENSITY = 0.5f;
constexpr float DEFAULT_BORDER_LIGHT_WIDTH = 20.0f;
constexpr float DEFAULT_BORDER_LIGHT_CORNER_RADIUS = 15.0f;
constexpr float DEFAULT_AURORA_NOISE_VALUE = 0.5f;
constexpr float DEFAULT_AURORA_FREQ_X = 3.0f;
constexpr float DEFAULT_AURORA_FREQ_Y = 5.0f;
constexpr float DEFAULT_HALO_RADIUS = 50.0f;
constexpr float DEFAULT_HALO_NOISE = 0.5f;
constexpr float DEFAULT_RIPPLE_RADIUS = 50.0f;
constexpr float DEFAULT_RIPPLE_THICKNESS = 10.0f;
constexpr float DEFAULT_WAVE_WIDTH = 2.5f;
constexpr float DEFAULT_PROPAGATION_RADIUS = 5.0f;
constexpr float DEFAULT_TURBULENCE_STRENGTH = 0.5f;

// FrameGradientMask default parameters
constexpr float DEFAULT_FRAME_WIDTH = 300.0f;
constexpr float DEFAULT_FRAME_HEIGHT = 500.0f;
constexpr float DEFAULT_FRAME_CORNER_RADIUS = 25.0f;

// Cascade intensity values
constexpr float PRIMARY_LIGHT_INTENSITY = 1.5f;
constexpr float SECONDARY_LIGHT_INTENSITY = 1.0f;
constexpr float SECONDARY_LIGHT_ALPHA = 0.5f;

void InitSpatialPointLight(std::shared_ptr<RSNGSpatialPointLight>& spatialPointLight)
{
    if (!spatialPointLight) {
        return;
    }
    spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
        Vector3f{DEFAULT_LIGHT_X, DEFAULT_LIGHT_Y, DEFAULT_LIGHT_Z});
    spatialPointLight->Setter<SpatialPointLightLightColorTag>(DEFAULT_LIGHT_COLOR);
    spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(DEFAULT_LIGHT_INTENSITY);
    spatialPointLight->Setter<SpatialPointLightAttenuationTag>(DEFAULT_LIGHT_ATTENUATION);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

// Basic color variations (RGBA)
const std::vector<Vector4f> lightColors = {
    Vector4f{1.0f, 0.0f, 0.0f, 1.0f},   // Red
    Vector4f{0.0f, 1.0f, 0.0f, 1.0f},   // Green
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f},   // Blue
    Vector4f{1.0f, 1.0f, 1.0f, 1.0f}    // White
};

// Extended color variations including mixed colors
const std::vector<Vector4f> extendedLightColors = {
    Vector4f{1.0f, 0.5f, 0.0f, 1.0f},   // Orange
    Vector4f{0.5f, 0.0f, 1.0f, 1.0f},   // Purple
    Vector4f{0.0f, 1.0f, 0.5f, 1.0f},   // Cyan-green
    Vector4f{1.0f, 1.0f, 0.0f, 1.0f}    // Yellow
};

// Color alpha variations
const std::vector<Vector4f> lightColorsWithAlpha = {
    Vector4f{1.0f, 0.0f, 0.0f, 0.25f},  // Red, low alpha
    Vector4f{1.0f, 0.0f, 0.0f, 0.5f},   // Red, medium alpha
    Vector4f{1.0f, 0.0f, 0.0f, 0.75f},  // Red, high alpha
    Vector4f{1.0f, 0.0f, 0.0f, 1.0f}    // Red, full alpha
};

// Intensity boundary values
const std::vector<float> lightIntensities = {0.0f, 1.0f, DEFAULT_LIGHT_INTENSITY, 5.0f};

// Extended intensity values
const std::vector<float> extendedIntensities = {0.5f, 1.5f, 3.0f, 10.0f};

// Attenuation boundary values
const std::vector<float> lightAttenuations = {1.0f, DEFAULT_LIGHT_ATTENUATION, 10.0f, 50.0f};

// Extended attenuation values
const std::vector<float> extendedAttenuations = {DEFAULT_LIGHT_INTENSITY, 3.0f, 20.0f, 100.0f};

// Light Z depth values (height above surface)
const std::vector<float> lightZDepths = {10.0f, 50.0f, DEFAULT_LIGHT_Z, 500.0f};

// Extended Z depth values
const std::vector<float> extendedZDepths = {25.0f, 75.0f, 200.0f, 1000.0f};

// Light X position variations (horizontal offset from center)
const std::vector<float> lightXOffsets = {-100.0f, -50.0f, 50.0f, 100.0f};

// Light Y position variations (vertical offset from center)
const std::vector<float> lightYOffsets = {-100.0f, -50.0f, 50.0f, 100.0f};

// Extreme intensity values for robustness testing
const std::vector<float> extremeIntensities = {-1.0f, -10.0f, 100.0f, 9999.0f};

// Extreme attenuation values
const std::vector<float> extremeAttenuations = {-1.0f, -10.0f, 1000.0f, 1e10f};

// Extreme Z depth values
const std::vector<float> extremeZDepths = {-100.0f, 0.0f, 10000.0f, 1e6f};

// Invalid values including NaN and infinity
const std::vector<float> invalidValues = {
    std::numeric_limits<float>::quiet_NaN(),
    std::numeric_limits<float>::infinity(),
    -std::numeric_limits<float>::infinity(),
    std::numeric_limits<float>::denorm_min()
};

// Parameter combinations for interaction testing
const std::vector<std::tuple<Vector4f, float, float>> colorIntensityAttenuationCombinations = {
    {Vector4f{1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, DEFAULT_LIGHT_ATTENUATION},  // Red, low intensity, normal attenuation
    {Vector4f{0.0f, 1.0f, 0.0f, 1.0f}, 3.0f, 10.0f},                       // Green, high intensity, high attenuation
    {Vector4f{0.0f, 0.0f, 1.0f, 1.0f}, 5.0f, DEFAULT_LIGHT_INTENSITY}      // Blue, very high intensity, low attenuation
};

// Radial gradient mask radius variations
const std::vector<float> radialGradientRadiuses = {0.1f, 0.3f, 0.5f, 0.8f};

// Ripple mask parameter variations (Radius and Width range: 0-10)
const std::vector<std::tuple<float, float, float>> rippleMaskParams = {
    {2.5f, 2.5f, 0.0f},   // Small radius, thin width
    {5.0f, 5.0f, 0.0f},   // Medium radius, medium width
    {7.5f, 7.5f, 0.0f},   // Large radius, thick width
    {10.0f, 10.0f, 0.5f}  // Max radius, max width with offset
};

// Frame gradient mask width variations
const std::vector<float> innerFrameWidthValues = {5.0f, 10.0f, 15.0f, 20.0f};
const std::vector<float> outerFrameWidthValues = {10.0f, 15.0f, 20.0f, 25.0f};
const std::vector<float> cornerRadiusValues = {0.0f, 10.0f, 20.0f, 50.0f};

constexpr float INNER_FRAME_WIDTH = 12.0f;
constexpr float OUTER_FRAME_WIDTH = 24.0f;

}

class NGShaderSpatialPointLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGSpatialPointLight>& spatialPointLight, float zDepth = DEFAULT_LIGHT_Z)
    {
        if (columnCount == 0 || rowCount == 0) {
            return;
        }
        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        // Set light position to center of current node
        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2, y + sizeY / 2, zDepth});

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

/*
 * Test spatial point light with basic colors (RGB + White)
 * Tests 4 basic color combinations
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Color_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with extended colors (mixed colors)
 * Tests 4 extended color combinations including orange, purple, cyan-green, yellow
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Extended_Color_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < extendedLightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(extendedLightColors[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with color alpha variations
 * Tests how alpha channel affects light transparency
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Color_Alpha_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColorsWithAlpha.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColorsWithAlpha[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with boundary intensity values
 * Tests 4 intensity levels: 0 (no effect), low, normal, high
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Intensity_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightIntensities.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(lightIntensities[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with extended intensity values
 * Tests additional intensity variations
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Extended_Intensity_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < extendedIntensities.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(extendedIntensities[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with boundary attenuation values
 * Tests how attenuation affects light spread/range
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Attenuation_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightAttenuations.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightAttenuationTag>(lightAttenuations[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with extended attenuation values
 * Tests additional attenuation variations
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Extended_Attenuation_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < extendedAttenuations.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightAttenuationTag>(extendedAttenuations[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with Z depth variations
 * Tests how light height affects the lighting effect
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Z_Depth_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightZDepths.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2, y + sizeY / 2, lightZDepths[i]});

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight, lightZDepths[i]);
    }
}

/*
 * Test spatial point light with extended Z depth values
 * Tests additional Z depth variations
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Extended_Z_Depth_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < extendedZDepths.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2, y + sizeY / 2, extendedZDepths[i]});

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight, extendedZDepths[i]);
    }
}

/*
 * Test spatial point light with X position offset variations
 * Tests horizontal light position offset from node center
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_X_Offset_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightXOffsets.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2 + lightXOffsets[i], y + sizeY / 2, DEFAULT_LIGHT_Z});

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with Y position offset variations
 * Tests vertical light position offset from node center
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Y_Offset_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightYOffsets.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2, y + sizeY / 2 + lightYOffsets[i], DEFAULT_LIGHT_Z});

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with color, intensity, and attenuation combinations
 * Tests parameter interaction effects
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Combination_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < colorIntensityAttenuationCombinations.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(
            std::get<0>(colorIntensityAttenuationCombinations[i]));
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(
            std::get<1>(colorIntensityAttenuationCombinations[i]));
        spatialPointLight->Setter<SpatialPointLightAttenuationTag>(
            std::get<2>(colorIntensityAttenuationCombinations[i]));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with grid layout (2x2)
 * Tests multiple lights in grid arrangement
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Grid_2x2_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;
    const size_t totalNodes = columnCount * rowCount;

    for (size_t i = 0; i < totalNodes; i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        // Vary color based on position
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(
            lightColors[i % lightColors.size()]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with grid layout (3x2)
 * Tests multiple lights in grid arrangement
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Grid_3x2_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 2;
    const size_t totalNodes = columnCount * rowCount;

    for (size_t i = 0; i < totalNodes; i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        // Vary intensity based on position
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(
            lightIntensities[i % lightIntensities.size()]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with extreme intensity values
 * Tests robustness against extreme/malicious inputs
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Intensity_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < extremeIntensities.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(extremeIntensities[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with extreme attenuation values
 * Tests robustness against extreme/malicious inputs
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Attenuation_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < extremeAttenuations.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightAttenuationTag>(extremeAttenuations[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with extreme Z depth values
 * Tests robustness against extreme position inputs
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Z_Depth_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < extremeZDepths.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2, y + sizeY / 2, extremeZDepths[i]});

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with invalid values (NaN, infinity)
 * Tests robustness against special floating-point values
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Intensity_Invalid_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < invalidValues.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(invalidValues[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with radial gradient mask
 * Tests mask integration with point light
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_RadialGradient_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < radialGradientRadiuses.size(); i++) {
        auto radialMask = std::make_shared<RSNGRadialGradientMask>();
        radialMask->Setter<RadialGradientMaskCenterTag>(Vector2f{0.5f, 0.5f});
        radialMask->Setter<RadialGradientMaskRadiusXTag>(radialGradientRadiuses[i]);
        radialMask->Setter<RadialGradientMaskRadiusYTag>(radialGradientRadiuses[i]);
        radialMask->Setter<RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.5f, 0.0f});
        radialMask->Setter<RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 0.5f, 1.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(radialMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with ripple mask variations
 * Tests different ripple mask parameters
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_Ripple_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < rippleMaskParams.size(); i++) {
        auto rippleMask = std::make_shared<RSNGRippleMask>();
        rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
        rippleMask->Setter<RippleMaskRadiusTag>(std::get<0>(rippleMaskParams[i]));
        rippleMask->Setter<RippleMaskWidthTag>(std::get<1>(rippleMaskParams[i]));
        rippleMask->Setter<RippleMaskOffsetTag>(std::get<2>(rippleMaskParams[i]));

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(rippleMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with wave gradient mask
 * Tests wave gradient mask integration
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_WaveGradient_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> waveBlurRadii = {0.0f, 1.25f, 2.5f, 5.0f};

    for (size_t i = 0; i < waveBlurRadii.size(); i++) {
        auto waveGradientMask = std::make_shared<RSNGWaveGradientMask>();
        waveGradientMask->Setter<WaveGradientMaskWaveCenterTag>(Vector2f{0.5f, 0.5f});
        waveGradientMask->Setter<WaveGradientMaskWaveWidthTag>(DEFAULT_WAVE_WIDTH);
        waveGradientMask->Setter<WaveGradientMaskPropagationRadiusTag>(DEFAULT_PROPAGATION_RADIUS);
        waveGradientMask->Setter<WaveGradientMaskBlurRadiusTag>(waveBlurRadii[i]);
        waveGradientMask->Setter<WaveGradientMaskTurbulenceStrengthTag>(DEFAULT_TURBULENCE_STRENGTH);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(waveGradientMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with double ripple mask
 * Tests double ripple mask effect
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_DoubleRipple_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<std::tuple<Vector2f, Vector2f, float, float, float>> doubleRippleParams = {
        {Vector2f{0.3f, 0.5f}, Vector2f{0.7f, 0.5f}, 5.0f, 5.0f, 0.0f},
        {Vector2f{0.2f, 0.3f}, Vector2f{0.8f, 0.7f}, 7.5f, 7.5f, 0.5f},
        {Vector2f{0.4f, 0.4f}, Vector2f{0.6f, 0.6f}, 10.0f, 10.0f, 1.0f},
        {Vector2f{0.0f, 0.0f}, Vector2f{1.0f, 1.0f}, 2.5f, 2.5f, -0.5f}
    };

    for (size_t i = 0; i < doubleRippleParams.size(); i++) {
        auto doubleRippleMask = std::make_shared<RSNGDoubleRippleMask>();
        doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(std::get<0>(doubleRippleParams[i]));
        doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(std::get<1>(doubleRippleParams[i]));
        doubleRippleMask->Setter<DoubleRippleMaskRadiusTag>(std::get<2>(doubleRippleParams[i]));
        doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(std::get<3>(doubleRippleParams[i]));
        doubleRippleMask->Setter<DoubleRippleMaskTurbulenceTag>(std::get<4>(doubleRippleParams[i]));
        doubleRippleMask->Setter<DoubleRippleMaskHaloThicknessTag>(0.0f);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with mask and color combination
 * Tests mask with different light colors
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_Color_Combination_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto radialMask = std::make_shared<RSNGRadialGradientMask>();
        radialMask->Setter<RadialGradientMaskCenterTag>(Vector2f{0.5f, 0.5f});
        radialMask->Setter<RadialGradientMaskRadiusXTag>(0.5f);
        radialMask->Setter<RadialGradientMaskRadiusYTag>(0.5f);
        radialMask->Setter<RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.5f, 0.0f});
        radialMask->Setter<RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 0.5f, 1.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(radialMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with mask and intensity combination
 * Tests mask with different intensity values
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_Intensity_Combination_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightIntensities.size(); i++) {
        auto rippleMask = std::make_shared<RSNGRippleMask>();
        rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
        rippleMask->Setter<RippleMaskRadiusTag>(5.0f);
        rippleMask->Setter<RippleMaskWidthTag>(5.0f);
        rippleMask->Setter<RippleMaskOffsetTag>(0.0f);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(lightIntensities[i]);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(rippleMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// ============================================================================
// Shader Append Cascade Tests - SpatialPointLight with other shaders
// ============================================================================

void InitBorderLightForCascade(std::shared_ptr<RSNGBorderLight>& borderLight)
{
    if (!borderLight) {
        return;
    }
    borderLight->Setter<BorderLightPositionTag>(Vector3f{0.5f, 0.5f, 0.5f});
    borderLight->Setter<BorderLightColorTag>(Vector4f{0.0f, 1.0f, 1.0f, 1.0f}); // Cyan
    borderLight->Setter<BorderLightIntensityTag>(DEFAULT_BORDER_LIGHT_INTENSITY);
    borderLight->Setter<BorderLightWidthTag>(DEFAULT_BORDER_LIGHT_WIDTH);
    borderLight->Setter<BorderLightRotationAngleTag>(Vector3f{0.0f, 0.0f, 0.0f});
    borderLight->Setter<BorderLightCornerRadiusTag>(DEFAULT_BORDER_LIGHT_CORNER_RADIUS);
}

void InitCircleFlowlightForCascade(std::shared_ptr<RSNGCircleFlowlight>& circleFlowlight)
{
    if (!circleFlowlight) {
        return;
    }
    circleFlowlight->Setter<CircleFlowlightColor0Tag>(DEFAULT_LIGHT_COLOR);
    circleFlowlight->Setter<CircleFlowlightColor1Tag>(Vector4f{0.0f, 1.0f, 0.5f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor2Tag>(Vector4f{0.5f, 0.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor3Tag>(Vector4f{1.0f, 1.0f, 0.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightRotationFrequencyTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightRotationAmplitudeTag>(
        Vector4f{0.5f, 0.5f, 0.5f, 0.5f});
    circleFlowlight->Setter<CircleFlowlightRotationSeedTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightGradientXTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightGradientYTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightDistortStrengthTag>(1.0f);
    circleFlowlight->Setter<CircleFlowlightBlendGradientTag>(1.0f);
    circleFlowlight->Setter<CircleFlowlightProgressTag>(DEFAULT_AURORA_NOISE_VALUE);
}

void InitAuroraNoiseForCascade(std::shared_ptr<RSNGAuroraNoise>& auroraNoise)
{
    if (!auroraNoise) {
        return;
    }
    auroraNoise->Setter<AuroraNoiseNoiseTag>(DEFAULT_AURORA_NOISE_VALUE);
    auroraNoise->Setter<AuroraNoiseFreqXTag>(DEFAULT_AURORA_FREQ_X);
    auroraNoise->Setter<AuroraNoiseFreqYTag>(DEFAULT_AURORA_FREQ_Y);
}

// SpatialPointLight + BorderLight cascade - first order
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_Border_Light_First_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);
        borderLight->Setter<BorderLightColorTag>(
            Vector4f{lightColors[i][0], lightColors[i][1], lightColors[i][2], SECONDARY_LIGHT_ALPHA});

        spatialPointLight->Append(borderLight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// SpatialPointLight + CircleFlowlight cascade - first order
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_Circle_Flowlight_First_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlightForCascade(circleFlowlight);
        circleFlowlight->Setter<CircleFlowlightColor0Tag>(lightColors[i]);

        spatialPointLight->Append(circleFlowlight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// SpatialPointLight + AuroraNoise cascade
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_Aurora_Noise_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto auroraNoise = std::make_shared<RSNGAuroraNoise>();
        InitAuroraNoiseForCascade(auroraNoise);

        spatialPointLight->Append(auroraNoise);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// SpatialPointLight + ParticleCircularHalo cascade
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_Particle_Halo_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> noiseValues = {0.0f, 0.3f, DEFAULT_HALO_NOISE, 1.0f};

    for (size_t i = 0; i < noiseValues.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        auto particleHalo = std::make_shared<RSNGParticleCircularHalo>();
        particleHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{0.5f, 0.5f});
        particleHalo->Setter<ParticleCircularHaloRadiusTag>(DEFAULT_HALO_RADIUS);
        particleHalo->Setter<ParticleCircularHaloNoiseTag>(noiseValues[i]);

        spatialPointLight->Append(particleHalo);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// SpatialPointLight + WavyRippleLight cascade
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_Wavy_Ripple_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> thicknessValues = {5.0f, DEFAULT_RIPPLE_THICKNESS, 15.0f, 20.0f};

    for (size_t i = 0; i < thicknessValues.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        auto wavyRipple = std::make_shared<RSNGWavyRippleLight>();
        wavyRipple->Setter<WavyRippleLightCenterTag>(Vector2f{0.5f, 0.5f});
        wavyRipple->Setter<WavyRippleLightRadiusTag>(DEFAULT_RIPPLE_RADIUS);
        wavyRipple->Setter<WavyRippleLightThicknessTag>(thicknessValues[i]);

        spatialPointLight->Append(wavyRipple);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// Triple cascade: SpatialPointLight + BorderLight + CircleFlowlight
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Triple_Cascade_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);

        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlightForCascade(circleFlowlight);

        spatialPointLight->Append(borderLight);
        borderLight->Append(circleFlowlight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// ============================================================================
// Foreground Shader Tests - SpatialPointLight as foreground shader
// ============================================================================

const std::string FOREGROUND_IMAGE_PATH = "/data/local/tmp/Images/fg_test.jpg";

// SpatialPointLight as foreground shader with different colors
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Foreground_Color_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2, y + sizeY / 2, DEFAULT_LIGHT_Z});

        auto testNode = SetUpNodeBgImage(FOREGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetForegroundShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// SpatialPointLight as foreground shader with intensity variations
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Foreground_Intensity_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightIntensities.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(lightIntensities[i]);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2, y + sizeY / 2, DEFAULT_LIGHT_Z});

        auto testNode = SetUpNodeBgImage(FOREGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetForegroundShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// Foreground cascade: SpatialPointLight + BorderLight
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Foreground_Cascade_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);

        spatialPointLight->Append(borderLight);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2, y + sizeY / 2, DEFAULT_LIGHT_Z});

        auto testNode = SetUpNodeBgImage(FOREGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetForegroundShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// ============================================================================
// Multiple SpatialPointLight cascade tests
// ============================================================================

// Two SpatialPointLights cascade
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Double_Spatial_Point_Light_Cascade_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight1 = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight1);
        spatialPointLight1->Setter<SpatialPointLightLightColorTag>(lightColors[i]);
        spatialPointLight1->Setter<SpatialPointLightLightIntensityTag>(PRIMARY_LIGHT_INTENSITY);

        auto spatialPointLight2 = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight2);
        spatialPointLight2->Setter<SpatialPointLightLightColorTag>(
            Vector4f{1.0f - lightColors[i][0], 1.0f - lightColors[i][1],
                    1.0f - lightColors[i][2], SECONDARY_LIGHT_ALPHA});
        spatialPointLight2->Setter<SpatialPointLightLightIntensityTag>(SECONDARY_LIGHT_INTENSITY);

        spatialPointLight1->Append(spatialPointLight2);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight1);
    }
}

// ============================================================================
// Cascade with extreme values robustness tests
// ============================================================================

// SpatialPointLight + BorderLight with extreme intensity
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Cascade_Extreme_Intensity_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < extremeIntensities.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(extremeIntensities[i]);

        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);

        spatialPointLight->Append(borderLight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// Cascade with invalid values (NaN, infinity)
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Cascade_Invalid_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < invalidValues.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(invalidValues[i]);

        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);

        spatialPointLight->Append(borderLight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// ============================================================================
// FrameGradientMask tests
// ============================================================================

/*
 * Test spatial point light with frame gradient mask
 * Tests frame gradient mask with different corner radius
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_FrameGradient_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < cornerRadiusValues.size(); i++) {
        auto frameGradientMask = std::make_shared<RSNGFrameGradientMask>();
        frameGradientMask->Setter<FrameGradientMaskInnerBezierTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        frameGradientMask->Setter<FrameGradientMaskOuterBezierTag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        frameGradientMask->Setter<FrameGradientMaskCornerRadiusTag>(cornerRadiusValues[i]);
        frameGradientMask->Setter<FrameGradientMaskInnerFrameWidthTag>(INNER_FRAME_WIDTH);
        frameGradientMask->Setter<FrameGradientMaskOuterFrameWidthTag>(OUTER_FRAME_WIDTH);
        frameGradientMask->Setter<FrameGradientMaskRectWHTag>(
            Vector2f{DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT});
        frameGradientMask->Setter<FrameGradientMaskRectPosTag>(Vector2f{0.0f, 0.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(frameGradientMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with frame gradient mask and inner/outer frame width
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_FrameGradient_Width_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < innerFrameWidthValues.size(); i++) {
        auto frameGradientMask = std::make_shared<RSNGFrameGradientMask>();
        frameGradientMask->Setter<FrameGradientMaskInnerBezierTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        frameGradientMask->Setter<FrameGradientMaskOuterBezierTag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        frameGradientMask->Setter<FrameGradientMaskCornerRadiusTag>(DEFAULT_FRAME_CORNER_RADIUS);
        frameGradientMask->Setter<FrameGradientMaskInnerFrameWidthTag>(innerFrameWidthValues[i]);
        frameGradientMask->Setter<FrameGradientMaskOuterFrameWidthTag>(outerFrameWidthValues[i]);
        frameGradientMask->Setter<FrameGradientMaskRectWHTag>(
            Vector2f{DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT});
        frameGradientMask->Setter<FrameGradientMaskRectPosTag>(Vector2f{0.0f, 0.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(frameGradientMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// ============================================================================
// Position combination tests
// ============================================================================

/*
 * Test spatial point light with XY position combinations
 * Tests light position offset in both X and Y directions
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_XY_Position_Combination_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<std::pair<float, float>> xyOffsets = {
        {-50.0f, -50.0f},  // Top-left offset
        {50.0f, -50.0f},   // Top-right offset
        {-50.0f, 50.0f},   // Bottom-left offset
        {50.0f, 50.0f}     // Bottom-right offset
    };

    for (size_t i = 0; i < xyOffsets.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2 + xyOffsets[i].first,
                    y + sizeY / 2 + xyOffsets[i].second, DEFAULT_LIGHT_Z});

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with position and intensity combinations
 * Tests different position and intensity parameter combinations
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Position_Intensity_Combination_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<std::tuple<float, float, float>> positionIntensityParams = {
        {100.0f, 50.0f, 1.0f},               // Low Z, low intensity
        {100.0f, 200.0f, DEFAULT_LIGHT_INTENSITY},  // Medium Z, medium intensity
        {100.0f, 500.0f, 3.0f},              // High Z, high intensity
        {100.0f, 1000.0f, 5.0f}              // Very high Z, very high intensity
    };

    for (size_t i = 0; i < positionIntensityParams.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2, y + sizeY / 2, std::get<1>(positionIntensityParams[i])});
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(
            std::get<2>(positionIntensityParams[i]));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// ============================================================================
// Additional cascade combinations
// ============================================================================

/*
 * Test spatial point light + WavyRippleLight cascade
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_WavyRippleLight_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> thicknessValues = {5.0f, DEFAULT_RIPPLE_THICKNESS, 15.0f, 20.0f};

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto wavyRippleLight = std::make_shared<RSNGWavyRippleLight>();
        wavyRippleLight->Setter<WavyRippleLightCenterTag>(Vector2f{0.5f, 0.5f});
        wavyRippleLight->Setter<WavyRippleLightRadiusTag>(DEFAULT_RIPPLE_RADIUS);
        wavyRippleLight->Setter<WavyRippleLightThicknessTag>(thicknessValues[i]);

        spatialPointLight->Append(wavyRippleLight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light + ParticleCircularHalo cascade
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_ParticleCircularHalo_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> noiseValues = {0.0f, 0.3f, DEFAULT_HALO_NOISE, 1.0f};

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto particleCircularHalo = std::make_shared<RSNGParticleCircularHalo>();
        particleCircularHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{0.5f, 0.5f});
        particleCircularHalo->Setter<ParticleCircularHaloRadiusTag>(DEFAULT_HALO_RADIUS);
        particleCircularHalo->Setter<ParticleCircularHaloNoiseTag>(noiseValues[i]);

        spatialPointLight->Append(particleCircularHalo);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test mask + cascade combination: RadialGradientMask + BorderLight
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_Cascade_Border_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto radialMask = std::make_shared<RSNGRadialGradientMask>();
        radialMask->Setter<RadialGradientMaskCenterTag>(Vector2f{0.5f, 0.5f});
        radialMask->Setter<RadialGradientMaskRadiusXTag>(0.5f);
        radialMask->Setter<RadialGradientMaskRadiusYTag>(0.5f);
        radialMask->Setter<RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.5f, 0.0f});
        radialMask->Setter<RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 0.5f, 1.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(radialMask));

        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);

        spatialPointLight->Append(borderLight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test mask + cascade combination: RippleMask + CircleFlowlight
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_Cascade_Flowlight_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto rippleMask = std::make_shared<RSNGRippleMask>();
        rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
        rippleMask->Setter<RippleMaskRadiusTag>(5.0f);
        rippleMask->Setter<RippleMaskWidthTag>(5.0f);
        rippleMask->Setter<RippleMaskOffsetTag>(0.0f);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(rippleMask));

        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlightForCascade(circleFlowlight);

        spatialPointLight->Append(circleFlowlight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

} // namespace OHOS::Rosen