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

// Ripple mask parameter variations (Radius and Width range: 0-10)
const std::vector<std::tuple<float, float, float>> rippleMaskParams = {
    {2.5f, 2.5f, 0.0f},   // Small radius, thin width
    {5.0f, 5.0f, 0.0f},   // Medium radius, medium width
    {7.5f, 7.5f, 0.0f},   // Large radius, thick width
    {10.0f, 10.0f, 0.5f}  // Max radius, max width with offset
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
        waveGradientMask->Setter<WaveGradientMaskWaveWidthTag>(2.5f);
        waveGradientMask->Setter<WaveGradientMaskPropagationRadiusTag>(5.0f);
        waveGradientMask->Setter<WaveGradientMaskBlurRadiusTag>(waveBlurRadii[i]);
        waveGradientMask->Setter<WaveGradientMaskTurbulenceStrengthTag>(0.5f);

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

void InitAuroraNoiseForCascade(std::shared_ptr<RSNGAuroraNoise>& auroraNoise)
{
    if (!auroraNoise) {
        return;
    }
    auroraNoise->Setter<AuroraNoiseNoiseTag>(0.5f);
    auroraNoise->Setter<AuroraNoiseFreqXTag>(3.0f);
    auroraNoise->Setter<AuroraNoiseFreqYTag>(5.0f);
}

void InitParticleCircularHaloForCascade(std::shared_ptr<RSNGParticleCircularHalo>& particleHalo)
{
    if (!particleHalo) {
        return;
    }
    particleHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{0.5f, 0.5f});
    particleHalo->Setter<ParticleCircularHaloRadiusTag>(50.0f);
    particleHalo->Setter<ParticleCircularHaloNoiseTag>(0.5f);
}

void InitWavyRippleLightForCascade(std::shared_ptr<RSNGWavyRippleLight>& wavyRipple)
{
    if (!wavyRipple) {
        return;
    }
    wavyRipple->Setter<WavyRippleLightCenterTag>(Vector2f{0.5f, 0.5f});
    wavyRipple->Setter<WavyRippleLightRadiusTag>(50.0f);
    wavyRipple->Setter<WavyRippleLightThicknessTag>(10.0f);
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
            Vector4f{lightColors[i][0], lightColors[i][1], lightColors[i][2], 0.5f});

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
    const std::vector<float> noiseValues = {0.0f, 0.3f, 0.5f, 1.0f};

    for (size_t i = 0; i < noiseValues.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        auto particleHalo = std::make_shared<RSNGParticleCircularHalo>();
        particleHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{0.5f, 0.5f});
        particleHalo->Setter<ParticleCircularHaloRadiusTag>(50.0f);
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
    const std::vector<std::tuple<Vector2f, float, float>> wavyRippleParams = {
        {Vector2f{0.5f, 0.5f}, 50.0f, 10.0f},
        {Vector2f{0.3f, 0.3f}, 100.0f, 15.0f},
        {Vector2f{0.7f, 0.7f}, 200.0f, 20.0f},
        {Vector2f{0.0f, 0.0f}, 300.0f, 5.0f}
    };

    for (size_t i = 0; i < wavyRippleParams.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        auto wavyRipple = std::make_shared<RSNGWavyRippleLight>();
        wavyRipple->Setter<WavyRippleLightCenterTag>(std::get<0>(wavyRippleParams[i]));
        wavyRipple->Setter<WavyRippleLightRadiusTag>(std::get<1>(wavyRippleParams[i]));
        wavyRipple->Setter<WavyRippleLightThicknessTag>(std::get<2>(wavyRippleParams[i]));

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
            Vector4f{1.0f - lightColors[i][0], 1.0f - lightColors[i][1], 1.0f - lightColors[i][2], 0.5f});
        spatialPointLight2->Setter<SpatialPointLightLightIntensityTag>(1.0f);

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
 * FrameGradientMask: InnerFrameWidth and OuterFrameWidth are float types
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_FrameGradient_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> cornerRadii = {0.0f, 10.0f, 20.0f, 50.0f};
    constexpr float innerFrameWidth = 12.0f;
    constexpr float outerFrameWidth = 24.0f;

    for (size_t i = 0; i < cornerRadii.size(); i++) {
        auto frameGradientMask = std::make_shared<RSNGFrameGradientMask>();
        frameGradientMask->Setter<FrameGradientMaskInnerBezierTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        frameGradientMask->Setter<FrameGradientMaskOuterBezierTag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        frameGradientMask->Setter<FrameGradientMaskCornerRadiusTag>(cornerRadii[i]);
        frameGradientMask->Setter<FrameGradientMaskInnerFrameWidthTag>(innerFrameWidth);
        frameGradientMask->Setter<FrameGradientMaskOuterFrameWidthTag>(outerFrameWidth);
        frameGradientMask->Setter<FrameGradientMaskRectWHTag>(Vector2f{300.0f, 500.0f});
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
 * FrameGradientMask: InnerFrameWidth and OuterFrameWidth are float types
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_FrameGradient_Width_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> innerFrameWidthValues = {5.0f, 10.0f, 15.0f, 20.0f};
    const std::vector<float> outerFrameWidthValues = {10.0f, 15.0f, 20.0f, 25.0f};

    for (size_t i = 0; i < innerFrameWidthValues.size(); i++) {
        auto frameGradientMask = std::make_shared<RSNGFrameGradientMask>();
        frameGradientMask->Setter<FrameGradientMaskInnerBezierTag>(Vector4f{0.0f, 0.0f, 1.0f, 1.0f});
        frameGradientMask->Setter<FrameGradientMaskOuterBezierTag>(Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
        frameGradientMask->Setter<FrameGradientMaskCornerRadiusTag>(25.0f);
        frameGradientMask->Setter<FrameGradientMaskInnerFrameWidthTag>(innerFrameWidthValues[i]);
        frameGradientMask->Setter<FrameGradientMaskOuterFrameWidthTag>(outerFrameWidthValues[i]);
        frameGradientMask->Setter<FrameGradientMaskRectWHTag>(Vector2f{300.0f, 500.0f});
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
            Vector3f{x + sizeX / 2.0f + xyOffsets[i].first,
                    y + sizeY / 2.0f + xyOffsets[i].second, 100.0f});

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
        {100.0f, 50.0f, 1.0f},   // Low Z, low intensity
        {100.0f, 200.0f, 2.0f},  // Medium Z, medium intensity
        {100.0f, 500.0f, 3.0f},  // High Z, high intensity
        {100.0f, 1000.0f, 5.0f}  // Very high Z, very high intensity
    };

    for (size_t i = 0; i < positionIntensityParams.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);

        const size_t sizeX = SCREEN_WIDTH / columnCount;
        const size_t sizeY = SCREEN_HEIGHT / rowCount;
        const size_t x = (i % columnCount) * sizeX;
        const size_t y = (i / columnCount) * sizeY;
        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, std::get<1>(positionIntensityParams[i])});
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
 * WavyRippleLight only has: Center, Radius, Thickness
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_WavyRippleLight_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> thicknessValues = {5.0f, 10.0f, 15.0f, 20.0f};

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto wavyRippleLight = std::make_shared<RSNGWavyRippleLight>();
        wavyRippleLight->Setter<WavyRippleLightCenterTag>(Vector2f{0.5f, 0.5f});
        wavyRippleLight->Setter<WavyRippleLightRadiusTag>(50.0f);
        wavyRippleLight->Setter<WavyRippleLightThicknessTag>(thicknessValues[i]);

        spatialPointLight->Append(wavyRippleLight);
        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

/*
 * Test spatial point light + ParticleCircularHalo cascade
 * ParticleCircularHalo only has: Center, Radius, Noise
 */
GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Append_ParticleCircularHalo_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> noiseValues = {0.0f, 0.3f, 0.5f, 1.0f};

    for (size_t i = 0; i < lightColors.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightColorTag>(lightColors[i]);

        auto particleCircularHalo = std::make_shared<RSNGParticleCircularHalo>();
        particleCircularHalo->Setter<ParticleCircularHaloCenterTag>(Vector2f{0.5f, 0.5f});
        particleCircularHalo->Setter<ParticleCircularHaloRadiusTag>(50.0f);
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