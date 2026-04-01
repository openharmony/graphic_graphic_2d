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

void InitSpatialPointLight(std::shared_ptr<RSNGSpatialPointLight>& spatialPointLight)
{
    if (!spatialPointLight) {
        return;
    }
    spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(1.0f);
    spatialPointLight->Setter<SpatialPointLightLightPositionTag>(Vector3f{0.5f, 0.5f, 1.0f});
    spatialPointLight->Setter<SpatialPointLightAttenuationTag>(0.5f);
    spatialPointLight->Setter<SpatialPointLightLightColorTag>(Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
}

namespace {
const std::string TEST_IMAGE_PATH = "/data/local/tmp/Images/backGroundImage.jpg";
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 2000;

const std::vector<float> lightIntensities = {0.0f, 0.5f, 1.0f, 2.0f};

const std::vector<Vector3f> lightPositions = {
    Vector3f{0.0f, 0.0f, 0.0f},
    Vector3f{0.5f, 0.5f, 0.5f},
    Vector3f{1.0f, 1.0f, 1.0f}
};

const std::vector<float> attenuations = {0.0f, 0.3f, 0.5f, 1.0f};

const std::vector<Vector4f> lightColors = {
    Vector4f{1.0f, 0.0f, 0.0f, 1.0f},
    Vector4f{0.0f, 1.0f, 0.0f, 1.0f},
    Vector4f{0.0f, 0.0f, 1.0f, 1.0f},
    Vector4f{1.0f, 1.0f, 1.0f, 1.0f}
};
}

class NGShaderPointLightTest : public RRSGraphicTest {
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

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, {x, y, sizeX, sizeY});
        testNode->SetBackgroundNGShader(spatialPointLight);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
};

GRAPHIC_TEST(NGShaderPointLightTest, EFFECT_TEST, Set_Point_Light_Intensity_Boundary_Test)
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

GRAPHIC_TEST(NGShaderPointLightTest, EFFECT_TEST, Set_Point_Light_Position_Test)
{
    const size_t columnCount = 3;
    const size_t rowCount = 1;

    for (size_t i = 0; i < lightPositions.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightPositionTag>(lightPositions[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

GRAPHIC_TEST(NGShaderPointLightTest, EFFECT_TEST, Set_Point_Light_Attenuation_Boundary_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;

    for (size_t i = 0; i < attenuations.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightAttenuationTag>(attenuations[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

GRAPHIC_TEST(NGShaderPointLightTest, EFFECT_TEST, Set_Point_Light_Color_Test)
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

GRAPHIC_TEST(NGShaderPointLightTest, EFFECT_TEST, Set_Point_Light_Extreme_Values_Test)
{
    const size_t columnCount = 4;
    const size_t rowCount = 1;
    const std::vector<float> extremeValues = {-1.0f, -10.0f, 9999.0f, 1e10f};
    
    for (size_t i = 0; i < extremeValues.size(); i++) {
        auto spatialPointLight = std::make_shared<RSNGSpatialPointLight>();
        InitSpatialPointLight(spatialPointLight);
        spatialPointLight->Setter<SpatialPointLightLightIntensityTag>(extremeValues[i]);

        SetUpTestNode(i, columnCount, rowCount, spatialPointLight);
    }
}

} // namespace OHOS::Rosen