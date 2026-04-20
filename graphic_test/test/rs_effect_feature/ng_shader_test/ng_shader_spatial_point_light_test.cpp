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

#include "draw/color.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "ui_effect/property/include/rs_ui_mask_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int NODE_SIZE = 500;
constexpr int OFFSET_X = 100;
constexpr int OFFSET_Y = 100;
constexpr int GAP = 550;
constexpr int HALF_DIVISOR = 2;

// Reference: render_service_client_spatial_point_light_demo.cpp
constexpr float DEFAULT_INTENSITY = 2.0f;
constexpr float DEFAULT_ATTENUATION = 5.0f;
constexpr float DEFAULT_LIGHT_Z = 100.0f;
}

class SpatialPointLightShaderTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        SetSurfaceColor(RSColor(0xff000000));
    }
};

/* Basic Test: single node without mask */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Basic_001)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(DEFAULT_INTENSITY);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_X + NODE_SIZE / HALF_DIVISOR, OFFSET_Y + NODE_SIZE / HALF_DIVISOR, DEFAULT_LIGHT_Z));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(DEFAULT_ATTENUATION);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ OFFSET_X, OFFSET_Y, NODE_SIZE, NODE_SIZE });
    testNode->SetBackgroundColor(0xFF333333);
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: high intensity light */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Basic_002)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(5.0f);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_X + NODE_SIZE / HALF_DIVISOR, OFFSET_Y + NODE_SIZE / HALF_DIVISOR, DEFAULT_LIGHT_Z));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(Vector4f(1.0f, 0.5f, 0.0f, 1.0f));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(10.0f);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ OFFSET_X + GAP, OFFSET_Y, NODE_SIZE, NODE_SIZE });
    testNode->SetBackgroundColor(0xFF222222);
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Intensity Test: different intensity values */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Intensity_001)
{
    float intensityList[] = { 1.0f, 2.0f, 5.0f, 10.0f };
    for (int i = 0; i < 4; i++) {
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(intensityList[i]);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_X + i * GAP + NODE_SIZE / HALF_DIVISOR, 
                     OFFSET_Y + 600 + NODE_SIZE / HALF_DIVISOR, DEFAULT_LIGHT_Z));
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(DEFAULT_ATTENUATION);

        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ OFFSET_X + i * GAP, OFFSET_Y + 600, NODE_SIZE, NODE_SIZE });
        testNode->SetBackgroundColor(0xFF303030);
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Attenuation Test: different attenuation values */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Attenuation_001)
{
    float attenuationList[] = { 1.0f, 5.0f, 10.0f, 50.0f };
    for (int i = 0; i < 4; i++) {
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(DEFAULT_INTENSITY);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_X + i * GAP + NODE_SIZE / HALF_DIVISOR, 
                     OFFSET_Y + 1200 + NODE_SIZE / HALF_DIVISOR, DEFAULT_LIGHT_Z));
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(attenuationList[i]);

        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ OFFSET_X + i * GAP, OFFSET_Y + 1200, NODE_SIZE, NODE_SIZE });
        testNode->SetBackgroundColor(0xFF303030);
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Color Test: RGB primary colors */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Color_001)
{
    Vector4f colorList[] = {
        Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
        Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
        Vector4f(0.0f, 0.0f, 1.0f, 1.0f),
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f)
    };
    for (int i = 0; i < 4; i++) {
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(DEFAULT_INTENSITY);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_X + i * GAP + NODE_SIZE / HALF_DIVISOR, 
                     OFFSET_Y + 1800 + NODE_SIZE / HALF_DIVISOR, DEFAULT_LIGHT_Z));
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(colorList[i]);
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(DEFAULT_ATTENUATION);

        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ OFFSET_X + i * GAP, OFFSET_Y + 1800, NODE_SIZE, NODE_SIZE });
        testNode->SetBackgroundColor(0xFF202020);
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Mask Test: with RadialGradientMask */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Mask_001)
{
    auto radialMask = std::make_shared<Rosen::RSNGRadialGradientMask>();
    radialMask->Setter<Rosen::RadialGradientMaskCenterTag>(Vector2f(0.5f, 0.5f));
    radialMask->Setter<Rosen::RadialGradientMaskRadiusXTag>(0.4f);
    radialMask->Setter<Rosen::RadialGradientMaskRadiusYTag>(0.4f);
    radialMask->Setter<Rosen::RadialGradientMaskColorsTag>(std::vector<float>{1.0f, 0.0f});
    radialMask->Setter<Rosen::RadialGradientMaskPositionsTag>(std::vector<float>{0.0f, 1.0f});

    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(DEFAULT_INTENSITY);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_X + NODE_SIZE / HALF_DIVISOR, OFFSET_Y + NODE_SIZE / HALF_DIVISOR, DEFAULT_LIGHT_Z));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(DEFAULT_ATTENUATION);
    shader->Setter<Rosen::SpatialPointLightMaskTag>(
        std::static_pointer_cast<Rosen::RSNGMaskBase>(radialMask));

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ OFFSET_X + 700, OFFSET_Y, NODE_SIZE, NODE_SIZE });
    testNode->SetBackgroundColor(0xFF303030);
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Mask Test: with RippleMask */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Mask_002)
{
    auto rippleMask = std::make_shared<Rosen::RSNGRippleMask>();
    rippleMask->Setter<Rosen::RippleMaskCenterTag>(Vector2f(0.5f, 0.5f));
    rippleMask->Setter<Rosen::RippleMaskRadiusTag>(100.0f);
    rippleMask->Setter<Rosen::RippleMaskWidthTag>(50.0f);
    rippleMask->Setter<Rosen::RippleMaskOffsetTag>(0.0f);

    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(DEFAULT_INTENSITY);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_X + NODE_SIZE / HALF_DIVISOR, OFFSET_Y + NODE_SIZE / HALF_DIVISOR, DEFAULT_LIGHT_Z));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(DEFAULT_ATTENUATION);
    shader->Setter<Rosen::SpatialPointLightMaskTag>(
        std::static_pointer_cast<Rosen::RSNGMaskBase>(rippleMask));

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ OFFSET_X + 700, OFFSET_Y + 600, NODE_SIZE, NODE_SIZE });
    testNode->SetBackgroundColor(0xFF303030);
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Z-axis Test: different light heights */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_ZAxis_001)
{
    float zList[] = { 50.0f, 100.0f, 200.0f, 500.0f };
    for (int i = 0; i < 4; i++) {
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(DEFAULT_INTENSITY);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_X + i * GAP + NODE_SIZE / HALF_DIVISOR, 
                     OFFSET_Y + 1200 + NODE_SIZE / HALF_DIVISOR, zList[i]));
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(DEFAULT_ATTENUATION);

        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ OFFSET_X + i * GAP, OFFSET_Y + 1200, NODE_SIZE, NODE_SIZE });
        testNode->SetBackgroundColor(0xFF282828);
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen