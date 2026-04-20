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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

void InitSpatialPointLight(std::shared_ptr<RSNGSpatialPointLight>& spatialPointLight)
{
    if (!spatialPointLight) {
        return;
    }
    spatialPointLight->Setter<SpatialPointLightLightPositionTag>(Vector3f{600.0f, 1000.0f, 100.0f});
    spatialPointLight->Setter<SpatialPointLightLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
    spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(2.0f);
    spatialPointLight->Setter<SpatialPointLightAttenuationTag>(5.0f);
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

const std::vector<Vector4f> lightColors = {
    Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
    Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
    Vector4f{1.0f, 1.0f, 1.0f, 1.0f}
};

const std::vector<float> lightIntensities = {0.0f, 1.0f, 2.0f, 5.0f};

const std::vector<float> lightAttenuations = {1.0f, 5.0f, 10.0f, 50.0f};

const std::vector<float> lightZDepths = {50.0f, 100.0f, 200.0f, 500.0f};

const std::vector<float> extremeIntensities = {-1.0f, -10.0f, 100.0f, 9999.0f};

const std::vector<float> extremeAttenuations = {-1.0f, -10.0f, 1000.0f, 1e10f};
}

class NGShaderSpatialPointLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    void SetUpTestNode(const size_t i, const size_t columnCount, const size_t rowCount,
        std::shared_ptr<RSNGSpatialPointLight>& spatialPointLight)
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
            Vector3f{x + sizeX / 2.0f, y + sizeY / 2.0f, 100.0f});

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

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

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

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

GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_RadialGradient_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < columnCount * rowCount; i++) {
        auto radialMask = std::make_shared<RSNGRadialGradientMask>();
        radialMask->Setter<RadialGradientMaskCenterTag>(Vector2f{0.5f, 0.5f});
        radialMask->Setter<RadialGradientMaskRadiusXTag>(0.4f);
        radialMask->Setter<RadialGradientMaskRadiusYTag>(0.4f);
        radialMask->Setter<RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
        radialMask->Setter<RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(radialMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

GRAPHIC_TEST(NGShaderSpatialPointLightTest, EFFECT_TEST, Set_Spatial_Point_Light_Mask_Ripple_Test)
{
    const size_t columnCount = 2;
    const size_t rowCount = 1;

    for (size_t i = 0; i < columnCount * rowCount; i++) {
        auto rippleMask = std::make_shared<RSNGRippleMask>();
        rippleMask->Setter<RippleMaskCenterTag>(Vector2f{0.5f, 0.5f});
        rippleMask->Setter<RippleMaskRadiusTag>(100.0f);
        rippleMask->Setter<RippleMaskWidthTag>(50.0f);
        rippleMask->Setter<RippleMaskOffsetTag>(0.0f);

        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightMaskTag>(
            std::static_pointer_cast<RSNGMaskBase>(rippleMask));

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

} // namespace OHOS::Rosen