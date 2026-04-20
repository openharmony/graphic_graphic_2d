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

void InitSpatialPointLight(std::shared_ptr<RSNGSpatialPointLight>& spatialPointLight)
{
    if (!spatialPointLight) {
        return;
    }
    spatialPointLight->Setter<SpatialPointLightLightPositionTag>(Vector3f{600.0f, 1000.0f, 100.0f});
    spatialPointLight->Setter<SpatialPointLightLightColorTag>(Vector4f{1.0f, 0.5f, 0.0f, 1.0f}); // Orange color
    spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(2.0f);
    spatialPointLight->Setter<SpatialPointLightAttenuationTag>(5.0f);
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
const std::vector<float> lightIntensities = {0.0f, 1.0f, 2.0f, 5.0f};

// Extended intensity values
const std::vector<float> extendedIntensities = {0.5f, 1.5f, 3.0f, 10.0f};

// Attenuation boundary values
const std::vector<float> lightAttenuations = {1.0f, 5.0f, 10.0f, 50.0f};

// Extended attenuation values
const std::vector<float> extendedAttenuations = {2.0f, 3.0f, 20.0f, 100.0f};

// Light Z depth values (height above surface)
const std::vector<float> lightZDepths = {10.0f, 50.0f, 100.0f, 500.0f};

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
    {Vector4f{1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 5.0f},    // Red, low intensity, normal attenuation
    {Vector4f{0.0f, 1.0f, 0.0f, 1.0f}, 3.0f, 10.0f},   // Green, high intensity, high attenuation
    {Vector4f{0.0f, 0.0f, 1.0f, 1.0f}, 5.0f, 2.0f}     // Blue, very high intensity, low attenuation
};

// Grid layout configurations for multi-light testing
const std::vector<std::pair<size_t, size_t>> gridLayouts = {
    {2, 2},  // 4 lights in 2x2 grid
    {3, 2},  // 6 lights in 3x2 grid
    {4, 3},  // 12 lights in 4x3 grid
    {2, 4}   // 8 lights in 2x4 grid
};

// Radial gradient mask radius variations
const std::vector<float> radialGradientRadiuses = {0.1f, 0.3f, 0.5f, 0.8f};

// Ripple mask parameter variations
const std::vector<std::tuple<float, float, float>> rippleMaskParams = {
    {50.0f, 20.0f, 0.0f},   // Small radius, thin width
    {100.0f, 50.0f, 0.0f},  // Medium radius, medium width
    {200.0f, 80.0f, 0.0f},  // Large radius, thick width
    {150.0f, 60.0f, 50.0f}  // Medium with offset
};

// Linear gradient mask direction variations
const std::vector<Vector2f> linearGradientDirections = {
    Vector2f{0.0f, 0.0f},   // Top-left to bottom-right
    Vector2f{1.0f, 0.0f},   // Top-right to bottom-left
    Vector2f{0.0f, 1.0f},   // Bottom-left to top-right
    Vector2f{1.0f, 1.0f}    // Bottom-right to top-left
};
}

class NGShaderSpatialPointLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGSpatialPointLight>& spatialPointLight, float zDepth = 100.0f)
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
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, zDepth});

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
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, lightZDepths[i]});

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
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, extendedZDepths[i]});

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
            Vector3f{x + sizeX / 2.0f + lightXOffsets[i], y + sizeY / 2.0f, 100.0f});

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
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f + lightYOffsets[i], 100.0f});

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
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, extremeZDepths[i]});

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
        radialMask->Setter<RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
        radialMask->Setter<RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});

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
 * Test spatial point light with linear gradient mask
 * Tests linear gradient mask integration
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_LinearGradient_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < linearGradientDirections.size(); i++) {
        auto linearMask = std::make_shared<RSNGLinearGradientMask>();
        linearMask->Setter<LinearGradientMaskStartPointTag>(linearGradientDirections[i]);
        linearMask->Setter<LinearGradientMaskEndPointTag>(
            Vector2f{1.0f - linearGradientDirections[i].x, 1.0f - linearGradientDirections[i].y});
        linearMask->Setter<LinearGradientMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
        linearMask->Setter<LinearGradientMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(linearMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with frame gradient mask
 * Tests frame gradient mask integration
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_FrameGradient_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < columnCount * rowCount; i++) {
        auto frameMask = std::make_shared<RSNGFrameGradientMask>();
        frameMask->Setter<FrameGradientMaskInsetTag>(Vector4f{0.1f, 0.1f, 0.1f, 0.1f});
        frameMask->Setter<FrameGradientMaskColorsTag>(std::vector<float>{1.0f, 0.5f, 0.0f});
        frameMask->Setter<FrameGradientMaskPositionsTag>(std::vector<float>{0.0f, 0.5f, 1.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(frameMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with wave gradient mask
 * Tests wave gradient mask integration
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_WaveGradient_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < columnCount * rowCount; i++) {
        auto waveMask = std::make_shared<RSNGWaveGradientMask>();
        waveMask->Setter<WaveGradientMaskWaveHeightTag>(20.0f);
        waveMask->Setter<WaveGradientMaskWaveCountTag>(3.0f);
        waveMask->Setter<WaveGradientMaskDirectionTag>(0.0f);
        waveMask->Setter<WaveGradientMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
        waveMask->Setter<WaveGradientMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(waveMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with double ripple mask
 * Tests double ripple mask effect
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_DoubleRipple_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < columnCount * rowCount; i++) {
        auto doubleRippleMask = std::make_shared<RSNGDoubleRippleMask>();
        doubleRippleMask->Setter<DoubleRippleMaskCenter1Tag>(Vector2f{0.3f, 0.5f});
        doubleRippleMask->Setter<DoubleRippleMaskCenter2Tag>(Vector2f{0.7f, 0.5f});
        doubleRippleMask->Setter<DoubleRippleMaskRadius1Tag>(100.0f);
        doubleRippleMask->Setter<DoubleRippleMaskRadius2Tag>(100.0f);
        doubleRippleMask->Setter<DoubleRippleMaskWidthTag>(30.0f);
        doubleRippleMask->Setter<DoubleRippleMaskOffsetTag>(0.0f);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(doubleRippleMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light with wave disturb mask
 * Tests wave disturb mask effect
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_WaveDisturb_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < columnCount * rowCount; i++) {
        auto waveDisturbMask = std::make_shared<RSNGWaveDisturbMask>();
        waveDisturbMask->Setter<WaveDisturbMaskWaveAmplitudeTag>(10.0f);
        waveDisturbMask->Setter<WaveDisturbMaskWaveFrequencyTag>(2.0f);
        waveDisturbMask->Setter<WaveDisturbMaskWavePhaseTag>(0.0f);
        waveDisturbMask->Setter<WaveDisturbMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
        waveDisturbMask->Setter<WaveDisturbMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(waveDisturbMask));

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
        radialMask->Setter<RadialGradientMaskRadiusXTag>(0.4f);
        radialMask->Setter<RadialGradientMaskRadiusYTag>(0.4f);
        radialMask->Setter<RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
        radialMask->Setter<RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});

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
        rippleMask->Setter<RippleMaskRadiusTag>(100.0f);
        rippleMask->Setter<RippleMaskWidthTag>(50.0f);
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
    borderLight->Setter<BorderLightIntensityTag>(0.5f);
    borderLight->Setter<BorderLightWidthTag>(20.0f);
    borderLight->Setter<BorderLightRotationAngleTag>(Vector3f{0.0f, 0.0f, 0.0f});
    borderLight->Setter<BorderLightCornerRadiusTag>(15.0f);
}

void InitCircleFlowlightForCascade(std::shared_ptr<RSNGCircleFlowlight>& circleFlowlight)
{
    if (!circleFlowlight) {
        return;
    }
    circleFlowlight->Setter<CircleFlowlightColor0Tag>(Vector4f{1.0f, 0.5f, 0.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor1Tag>(Vector4f{0.0f, 1.0f, 0.5f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor2Tag>(Vector4f{0.5f, 0.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightColor3Tag>(Vector4f{1.0f, 1.0f, 0.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightRotationFrequencyTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightRotationAmplitudeTag>(Vector4f{0.5f, 0.5f, 0.5f, 0.5f});
    circleFlowlight->Setter<CircleFlowlightRotationSeedTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightGradientXTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightGradientYTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightStrengthTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    circleFlowlight->Setter<CircleFlowlightDistortStrengthTag>(1.0f);
    circleFlowlight->Setter<CircleFlowlightBlendGradientTag>(1.0f);
    circleFlowlight->Setter<CircleFlowlightProgressTag>(0.5f);
}

void InitAuroraNoiseForCascade(std::shared_ptr<RSNGAuroraNoiseShader>& auroraNoise)
{
    if (!auroraNoise) {
        return;
    }
    auroraNoise->Setter<AuroraNoiseColor1Tag>(Vector4f{0.2f, 0.4f, 0.8f, 1.0f});
    auroraNoise->Setter<AuroraNoiseColor2Tag>(Vector4f{0.8f, 0.2f, 0.4f, 1.0f});
    auroraNoise->Setter<AuroraNoiseColor3Tag>(Vector4f{0.4f, 0.8f, 0.2f, 1.0f});
    auroraNoise->Setter<AuroraNoiseColor4Tag>(Vector4f{0.6f, 0.6f, 0.6f, 1.0f});
    auroraNoise->Setter<AuroraNoiseFrequency1Tag>(3.0f);
    auroraNoise->Setter<AuroraNoiseFrequency2Tag>(5.0f);
    auroraNoise->Setter<AuroraNoiseFrequency3Tag>(7.0f);
    auroraNoise->Setter<AuroraNoiseAmplitude1Tag>(0.1f);
    auroraNoise->Setter<AuroraNoiseAmplitude2Tag>(0.05f);
    auroraNoise->Setter<AuroraNoiseAmplitude3Tag>(0.02f);
    auroraNoise->Setter<AuroraNoiseSpeedTag>(1.0f);
    auroraNoise->Setter<AuroraNoiseProgressTag>(0.5f);
}

void InitColorGradientEffectForCascade(std::shared_ptr<RSNGColorGradientEffect>& colorGradient)
{
    if (!colorGradient) {
        return;
    }
    colorGradient->Setter<ColorGradientEffectColor0Tag>(Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
    colorGradient->Setter<ColorGradientEffectColor1Tag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
    colorGradient->Setter<ColorGradientEffectColor2Tag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
    colorGradient->Setter<ColorGradientEffectColor3Tag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    colorGradient->Setter<ColorGradientEffectPosition0Tag>(Vector2f{0.0f, 0.5f});
    colorGradient->Setter<ColorGradientEffectPosition1Tag>(Vector2f{0.5f, 0.0f});
    colorGradient->Setter<ColorGradientEffectPosition2Tag>(Vector2f{1.0f, 0.5f});
    colorGradient->Setter<ColorGradientEffectPosition3Tag>(Vector2f{0.5f, 1.0f});
    colorGradient->Setter<ColorGradientEffectStrength0Tag>(3.0f);
    colorGradient->Setter<ColorGradientEffectStrength1Tag>(3.0f);
    colorGradient->Setter<ColorGradientEffectStrength2Tag>(3.0f);
    colorGradient->Setter<ColorGradientEffectStrength3Tag>(3.0f);
    colorGradient->Setter<ColorGradientEffectColorNumberTag>(4.0f);
    colorGradient->Setter<ColorGradientEffectBlendTag>(4.0f);
    colorGradient->Setter<ColorGradientEffectBlendKTag>(10.0f);
}

void InitParticleCircularHaloForCascade(std::shared_ptr<RSNGParticleCircularHalo>& particleHalo)
{
    if (!particleHalo) {
        return;
    }
    particleHalo->Setter<ParticleCircularHaloRadiusTag>(0.5f);
    particleHalo->Setter<ParticleCircularHaloColorTag>(Vector4f{1.0f, 0.8f, 0.0f, 1.0f});
    particleHalo->Setter<ParticleCircularHaloIntensityTag>(1.0f);
    particleHalo->Setter<ParticleCircularHaloParticleCountTag>(20.0f);
    particleHalo->Setter<ParticleCircularHaloParticleSizeTag>(5.0f);
    particleHalo->Setter<ParticleCircularHaloRotationSpeedTag>(2.0f);
    particleHalo->Setter<ParticleCircularHaloProgressTag>(0.5f);
}

void InitWavyRippleLightForCascade(std::shared_ptr<RSNGWavyRippleLight>& wavyRipple)
{
    if (!wavyRipple) {
        return;
    }
    wavyRipple->Setter<WavyRippleLightColorTag>(Vector4f{0.0f, 1.0f, 0.8f, 1.0f});
    wavyRipple->Setter<WavyRippleLightIntensityTag>(1.0f);
    wavyRipple->Setter<WavyRippleLightWaveCountTag>(3.0f);
    wavyRipple->Setter<WavyRippleLightWaveAmplitudeTag>(10.0f);
    wavyRipple->Setter<WavyRippleLightWaveFrequencyTag>(1.0f);
    wavyRipple->Setter<WavyRippleLightProgressTag>(0.5f);
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
            Vector4f{lightColors[i].x, lightColors[i].y, lightColors[i].z, 0.5f});

        spatialPointLight->Append(borderLight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// SpatialPointLight + BorderLight cascade - second order
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_Border_Light_Second_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);
        borderLight->Setter<BorderLightColorTag>(lightColors[i]);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(
            Vector4f{lightColors[i].x * 0.5f, lightColors[i].y * 0.5f, lightColors[i].z * 0.5f, 1.0f});

        borderLight->Append(spatialPointLight);
        SetUpTestNode(i, columnCount, rowCount, borderLight);
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

// SpatialPointLight + CircleFlowlight cascade - second order
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_Circle_Flowlight_Second_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlightForCascade(circleFlowlight);
        circleFlowlight->Setter<CircleFlowlightColor0Tag>(lightColors[i]);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(1.5f);

        circleFlowlight->Append(spatialPointLight);
        SetUpTestNode(i, columnCount, rowCount, circleFlowlight);
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

        auto auroraNoise = std::make_shared<RSNGAuroraNoiseShader>();
        InitAuroraNoiseForCascade(auroraNoise);

        spatialPointLight->Append(auroraNoise);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// AuroraNoise + SpatialPointLight cascade (reverse order)
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Aurora_Noise_Append_Spatial_Point_Light_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto auroraNoise = std::make_shared<RSNGAuroraNoiseShader>();
        InitAuroraNoiseForCascade(auroraNoise);
        auroraNoise->Setter<AuroraNoiseColor1Tag>(lightColors[i]);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(2.0f);

        auroraNoise->Append(spatialPointLight);
        SetUpTestNode(i, columnCount, rowCount, auroraNoise);
    }
}

// SpatialPointLight + ColorGradientEffect cascade
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_Color_Gradient_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        InitColorGradientEffectForCascade(colorGradient);
        colorGradient->Setter<ColorGradientEffectColor0Tag>(lightColors[i]);

        spatialPointLight->Append(colorGradient);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// ColorGradientEffect + SpatialPointLight cascade (reverse order)
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Color_Gradient_Append_Spatial_Point_Light_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        InitColorGradientEffectForCascade(colorGradient);
        colorGradient->Setter<ColorGradientEffectColor0Tag>(lightColors[i]);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(3.0f);

        colorGradient->Append(spatialPointLight);
        SetUpTestNode(i, columnCount, rowCount, colorGradient);
    }
}

// SpatialPointLight + ParticleCircularHalo cascade
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_Particle_Halo_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto particleHalo = std::make_shared<RSNGParticleCircularHalo>();
        InitParticleCircularHaloForCascade(particleHalo);
        particleHalo->Setter<ParticleCircularHaloColorTag>(lightColors[i]);

        spatialPointLight->Append(particleHalo);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// ParticleCircularHalo + SpatialPointLight cascade (reverse order)
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Particle_Halo_Append_Spatial_Point_Light_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto particleHalo = std::make_shared<RSNGParticleCircularHalo>();
        InitParticleCircularHaloForCascade(particleHalo);
        particleHalo->Setter<ParticleCircularHaloColorTag>(lightColors[i]);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(2.0f);

        particleHalo->Append(spatialPointLight);
        SetUpTestNode(i, columnCount, rowCount, particleHalo);
    }
}

// SpatialPointLight + WavyRippleLight cascade
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_Wavy_Ripple_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto wavyRipple = std::make_shared<RSNGWavyRippleLight>();
        InitWavyRippleLightForCascade(wavyRipple);
        wavyRipple->Setter<WavyRippleLightColorTag>(lightColors[i]);

        spatialPointLight->Append(wavyRipple);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// WavyRippleLight + SpatialPointLight cascade (reverse order)
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Wavy_Ripple_Append_Spatial_Point_Light_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto wavyRipple = std::make_shared<RSNGWavyRippleLight>();
        InitWavyRippleLightForCascade(wavyRipple);
        wavyRipple->Setter<WavyRippleLightColorTag>(lightColors[i]);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(2.5f);

        wavyRipple->Append(spatialPointLight);
        SetUpTestNode(i, columnCount, rowCount, wavyRipple);
    }
}

// Triple cascade: SpatialPointLight + BorderLight + CircleFlowlight
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Triple_Cascade_1_Test)
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

// Triple cascade: CircleFlowlight + SpatialPointLight + BorderLight
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Triple_Cascade_2_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlightForCascade(circleFlowlight);
        circleFlowlight->Setter<CircleFlowlightColor0Tag>(lightColors[i]);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);

        circleFlowlight->Append(spatialPointLight);
        spatialPointLight->Append(borderLight);
        SetUpTestNode(i, columnCount, rowCount, circleFlowlight);
    }
}

// Triple cascade: AuroraNoise + SpatialPointLight + ColorGradientEffect
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Triple_Cascade_3_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto auroraNoise = std::make_shared<RSNGAuroraNoiseShader>();
        InitAuroraNoiseForCascade(auroraNoise);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        InitColorGradientEffectForCascade(colorGradient);

        auroraNoise->Append(spatialPointLight);
        spatialPointLight->Append(colorGradient);
        SetUpTestNode(i, columnCount, rowCount, auroraNoise);
    }
}

// Quad cascade: SpatialPointLight + BorderLight + CircleFlowlight + AuroraNoise
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Quad_Cascade_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;

    const std::vector<Vector4f> quadColors = {
        Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
        Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 0.0f, 1.0f}
    };

    for (size_t i = 0; i < quadColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(quadColors[i]);

        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);

        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlightForCascade(circleFlowlight);

        auto auroraNoise = std::make_shared<RSNGAuroraNoiseShader>();
        InitAuroraNoiseForCascade(auroraNoise);

        spatialPointLight->Append(borderLight);
        borderLight->Append(circleFlowlight);
        circleFlowlight->Append(auroraNoise);
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
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, 100.0f});

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
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, 100.0f});

        auto testNode = SetUpNodeBgImage(FOREGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetForegroundShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// SpatialPointLight as foreground shader with attenuation variations
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Foreground_Attenuation_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightAttenuations.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightAttenuationTag>(lightAttenuations[i]);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, 100.0f});

        auto testNode = SetUpNodeBgImage(FOREGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetForegroundShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// SpatialPointLight as foreground shader with Z depth variations
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Foreground_Z_Depth_Test)
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
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, lightZDepths[i]});

        auto testNode = SetUpNodeBgImage(FOREGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetForegroundShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// SpatialPointLight as foreground shader with mask
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Foreground_Mask_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < radialGradientRadiuses.size(); i++) {
        auto radialMask = std::make_shared<RSNGRadialGradientMask>();
        radialMask->Setter<RadialGradientMaskCenterTag>(Vector2f{0.5f, 0.5f});
        radialMask->Setter<RadialGradientMaskRadiusXTag>(radialGradientRadiuses[i]);
        radialMask->Setter<RadialGradientMaskRadiusYTag>(radialGradientRadiuses[i]);
        radialMask->Setter<RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
        radialMask->Setter<RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(radialMask));

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, 100.0f});

        auto testNode = SetUpNodeBgImage(FOREGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetForegroundShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// Foreground cascade: SpatialPointLight + BorderLight
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Foreground_Cascade_Border_Test)
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
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, 100.0f});

        auto testNode = SetUpNodeBgImage(FOREGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetForegroundShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// Foreground cascade: SpatialPointLight + CircleFlowlight
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Foreground_Cascade_Flowlight_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlightForCascade(circleFlowlight);

        spatialPointLight->Append(circleFlowlight);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, 100.0f});

        auto testNode = SetUpNodeBgImage(FOREGROUND_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetForegroundShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// Foreground triple cascade: SpatialPointLight + BorderLight + AuroraNoise
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Foreground_Triple_Cascade_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);

        auto auroraNoise = std::make_shared<RSNGAuroraNoiseShader>();
        InitAuroraNoiseForCascade(auroraNoise);

        spatialPointLight->Append(borderLight);
        borderLight->Append(auroraNoise);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;

        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, 100.0f});

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
        spatialPointLight1->Setter<SpatialPointLightLightIntensityTag>(1.5f);

        auto spatialPointLight2 = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight2);
        spatialPointLight2->Setter<SpatialPointLightLightColorTag>(
            Vector4f{1.0f - lightColors[i].x, 1.0f - lightColors[i].y, 1.0f - lightColors[i].z, 0.5f});
        spatialPointLight2->Setter<SpatialPointLightLightIntensityTag>(1.0f);

        spatialPointLight1->Append(spatialPointLight2);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight1);
    }
}

// Three SpatialPointLights cascade with different Z depths
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Triple_Spatial_Point_Light_Cascade_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    const std::vector<float> cascadeZDepths = {50.0f, 100.0f, 200.0f};

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight1 = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight1);
        spatialPointLight1->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto spatialPointLight2 = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight2);
        spatialPointLight2->Setter<SpatialPointLightLightColorTag>(extendedLightColors[i]);
        spatialPointLight2->Setter<SpatialPointLightAttenuationTag>(cascadeZDepths[0]);

        auto spatialPointLight3 = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight3);
        spatialPointLight3->Setter<SpatialPointLightLightIntensityTag>(0.5f);
        spatialPointLight3->Setter<SpatialPointLightAttenuationTag>(cascadeZDepths[1]);

        spatialPointLight1->Append(spatialPointLight2);
        spatialPointLight2->Append(spatialPointLight3);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight1);
    }
}

// Multiple SpatialPointLights with mask cascade
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_Cascade_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto radialMask = std::make_shared<RSNGRadialGradientMask>();
        radialMask->Setter<RadialGradientMaskCenterTag>(Vector2f{0.5f, 0.5f});
        radialMask->Setter<RadialGradientMaskRadiusXTag>(0.4f);
        radialMask->Setter<RadialGradientMaskRadiusYTag>(0.4f);
        radialMask->Setter<RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
        radialMask->Setter<RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});

        auto spatialPointLight1 = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight1);
        spatialPointLight1->Setter<SpatialPointLightLightColorTag>(lightColors[i]);
        spatialPointLight1->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(radialMask));

        auto spatialPointLight2 = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight2);
        spatialPointLight2->Setter<SpatialPointLightLightIntensityTag>(0.5f);

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
        borderLight->Setter<BorderLightIntensityTag>(
            (extremeIntensities[i] > 0.0f) ? extremeIntensities[i] : 0.0f);

        spatialPointLight->Append(borderLight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// SpatialPointLight + CircleFlowlight with extreme attenuation
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Cascade_Extreme_Attenuation_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < extremeAttenuations.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightAttenuationTag>(extremeAttenuations[i]);

        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlightForCascade(circleFlowlight);
        circleFlowlight->Setter<CircleFlowlightDistortStrengthTag>(
            (extremeAttenuations[i] > 0.0f) ? 1.0f : 0.0f);

        spatialPointLight->Append(circleFlowlight);
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
// Complex combination tests
// ============================================================================

// SpatialPointLight + BorderLight + CircleFlowlight + AuroraNoise with mask
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Complex_Mask_Cascade_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 2;

    for (size_t i = 0; i < columnCount * rowCount; i++) {
        auto radialMask = std::make_shared<RSNGRadialGradientMask>();
        radialMask->Setter<RadialGradientMaskCenterTag>(Vector2f{0.5f, 0.5f});
        radialMask->Setter<RadialGradientMaskRadiusXTag>(0.5f);
        radialMask->Setter<RadialGradientMaskRadiusYTag>(0.5f);
        radialMask->Setter<RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
        radialMask->Setter<RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i % lightColors.size()]);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(radialMask));

        auto borderLight = std::make_shared<RSNGBorderLight>();
        InitBorderLightForCascade(borderLight);

        auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
        InitCircleFlowlightForCascade(circleFlowlight);

        auto auroraNoise = std::make_shared<RSNGAuroraNoiseShader>();
        InitAuroraNoiseForCascade(auroraNoise);

        spatialPointLight->Append(borderLight);
        borderLight->Append(circleFlowlight);
        circleFlowlight->Append(auroraNoise);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// Grid test with cascade effects
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Grid_Cascade_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 3;
    const size_t totalNodes = columnCount * rowCount;

    for (size_t i = 0; i < totalNodes; i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i % lightColors.size()]);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(lightIntensities[i % lightIntensities.size()]);
        spatialPointLight->Setter<SpatialPointLightAttenuationTag>(lightAttenuations[i % lightAttenuations.size()]);

        // Add BorderLight to every other node
        if (i % 2 == 0) {
            auto borderLight = std::make_shared<RSNGBorderLight>();
            InitBorderLightForCascade(borderLight);
            spatialPointLight->Append(borderLight);
        }

        // Add CircleFlowlight to every third node
        if (i % 3 == 0) {
            auto circleFlowlight = std::make_shared<RSNGCircleFlowlight>();
            InitCircleFlowlightForCascade(circleFlowlight);
            spatialPointLight->Append(circleFlowlight);
        }

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

// Parameter sweep cascade test
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Param_Sweep_Cascade_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 2;
    const size_t totalNodes = columnCount * rowCount;

    const std::vector<std::tuple<Vector4f, float, float>> sweepParams = {
        {Vector4f{1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 2.0f},
        {Vector4f{0.0f, 1.0f, 0.0f, 1.0f}, 2.0f, 5.0f},
        {Vector4f{0.0f, 0.0f, 1.0f, 1.0f}, 3.0f, 10.0f},
        {Vector4f{1.0f, 1.0f, 0.0f, 1.0f}, 0.5f, 1.0f},
        {Vector4f{1.0f, 0.0f, 1.0f, 1.0f}, 1.5f, 3.0f},
        {Vector4f{0.0f, 1.0f, 1.0f, 1.0f}, 2.5f, 7.0f}
    };

    for (size_t i = 0; i < totalNodes && i < sweepParams.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(std::get<0>(sweepParams[i]));
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(std::get<1>(sweepParams[i]));
        spatialPointLight->Setter<SpatialPointLightAttenuationTag>(std::get<2>(sweepParams[i]));

        auto colorGradient = std::make_shared<RSNGColorGradientEffect>();
        InitColorGradientEffectForCascade(colorGradient);
        colorGradient->Setter<ColorGradientEffectColor0Tag>(std::get<0>(sweepParams[i]));

        spatialPointLight->Append(colorGradient);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

} // namespace OHOS::Rosen