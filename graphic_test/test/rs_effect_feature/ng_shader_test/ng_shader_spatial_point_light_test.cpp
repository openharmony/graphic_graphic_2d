/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include "rs_graphic_test.h"
#include "rs_graphic_test_img.h"

#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int NODE_SIZE_350 = 350;
constexpr int NODE_SIZE_400 = 400;
constexpr int NODE_SIZE_250 = 250;
constexpr int NODE_SIZE_200 = 200;
constexpr int NODE_SIZE_150 = 150;
constexpr int NODE_SIZE_100 = 100;
constexpr int NODE_SIZE_50 = 50;
constexpr int OFFSET_10 = 10;
constexpr int OFFSET_50 = 50;
constexpr int OFFSET_100 = 100;
constexpr int OFFSET_175 = 175;
constexpr int OFFSET_200 = 200;
constexpr int OFFSET_240 = 240;
constexpr int OFFSET_280 = 280;
constexpr int OFFSET_380 = 380;
constexpr int OFFSET_510 = 510;
constexpr float INTENSITY_0_5F = 0.5f;
constexpr float INTENSITY_1_0F = 1.0f;
constexpr float INTENSITY_1_5F = 1.5f;
constexpr float INTENSITY_2_0F = 2.0f;
constexpr float ATTENUATION_0_1F = 0.1f;
constexpr float ATTENUATION_0_5F = 0.5f;
constexpr uint32_t COLOR_WHITE = 0xffffffff;
constexpr uint32_t COLOR_RED = 0xffff0000;
constexpr uint32_t COLOR_GREEN = 0xff00ff00;
constexpr uint32_t COLOR_BLUE = 0xff0000ff;
const std::string TEST_IMAGE_PATH = "/data/local/tmp/fg_test.jpg";

Vector4f ColorToVector4f(uint32_t color)
{
    return Vector4f(
        ((color >> 16) & 0xff) / 255.0f,
        ((color >> 8) & 0xff) / 255.0f,
        (color & 0xff) / 255.0f,
        ((color >> 24) & 0xff) / 255.0f
    );
}
}

class SpatialPointLightShaderTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        SetSurfaceColor(RSColor(0xff000000));
    }
};

/* Basic Test: single node with default shader params */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Basic_Test_001)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundColor(COLOR_RED);
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: shader with background image */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Basic_Test_002)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: multiple nodes with same shader params */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Basic_Test_003)
{
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * OFFSET_510;
        int y = (i / 2) * OFFSET_510;
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_200, OFFSET_200, 50.0f));
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { x + OFFSET_10, y + OFFSET_10, NODE_SIZE_400, NODE_SIZE_400 });
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Intensity Test: intensity gradient (0.5, 1.0, 1.5, 2.0) */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Intensity_Test_001)
{
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    for (int i = 0; i < 4; i++) {
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(intensityList[i]);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_200, OFFSET_200, 50.0f));
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_280 + OFFSET_50, OFFSET_100, NODE_SIZE_250, NODE_SIZE_250 });
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Intensity Test: intensity x color matrix (4x3) */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Intensity_Test_002)
{
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    uint32_t colorList[] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE };
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 3; col++) {
            auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
            shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(intensityList[row]);
            shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
                Vector3f(OFFSET_175, OFFSET_175, 50.0f));
            shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(colorList[col]));
            shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

            auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
                { col * OFFSET_280 + OFFSET_50, row * OFFSET_280 + OFFSET_50,
                  NODE_SIZE_250, NODE_SIZE_250 });
            testNode->SetBackgroundNGShader(shader);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Intensity Test: intensity = 0 */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Intensity_Test_003)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(0.0f);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Intensity Test: large intensity (100) */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Intensity_Test_004)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(100.0f);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Position Test: corner positions */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Position_Test_001)
{
    Vector3f positions[] = {
        Vector3f(0, 0, 50.0f),
        Vector3f(NODE_SIZE_400, 0, 50.0f),
        Vector3f(0, NODE_SIZE_400, 50.0f),
        Vector3f(NODE_SIZE_400, NODE_SIZE_400, 50.0f)
    };
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * OFFSET_510;
        int y = (i / 2) * OFFSET_510;
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(positions[i]);
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { x + OFFSET_10, y + OFFSET_10, NODE_SIZE_400, NODE_SIZE_400 });
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Position Test: position grid (3x3) */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Position_Test_002)
{
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
            shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
            shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
                Vector3f(OFFSET_175 + x * OFFSET_100, OFFSET_175 + y * OFFSET_100, 50.0f));
            shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
            shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

            auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
                { x * OFFSET_380 + OFFSET_50, y * OFFSET_380 + OFFSET_50,
                  NODE_SIZE_350, NODE_SIZE_350 });
            testNode->SetBackgroundNGShader(shader);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Position Test: Z-axis depth (0, 50, 100, 200) */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Position_Test_003)
{
    float zList[] = { 0.0f, 50.0f, 100.0f, 200.0f };
    for (int i = 0; i < 4; i++) {
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_175, OFFSET_175, zList[i]));
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_280 + OFFSET_50, OFFSET_100, NODE_SIZE_250, NODE_SIZE_250 });
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Color Test: RGB primary colors */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Color_Test_001)
{
    uint32_t colorList[] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE };
    for (int i = 0; i < 3; i++) {
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_175, OFFSET_175, 50.0f));
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(colorList[i]));
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_380 + OFFSET_50, OFFSET_100, NODE_SIZE_350, NODE_SIZE_350 });
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Color Test: color matrix (6 colors) */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Color_Test_002)
{
    uint32_t colorList[] = { COLOR_WHITE, 0xff000000, COLOR_RED, COLOR_GREEN, COLOR_BLUE,
        0x7fffffff };
    for (int i = 0; i < 6; i++) {
        int x = (i % 3) * OFFSET_380;
        int y = (i / 3) * OFFSET_380;
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_175, OFFSET_175, 50.0f));
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(colorList[i]));
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { x + OFFSET_50, y + OFFSET_50, NODE_SIZE_350, NODE_SIZE_350 });
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Color Test: color x intensity matrix (4x4) */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Color_Test_003)
{
    uint32_t colorList[] = { COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE };
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
            shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(intensityList[row]);
            shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
                Vector3f(OFFSET_175, OFFSET_175, 50.0f));
            shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(colorList[col]));
            shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

            auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
                { col * OFFSET_280 + OFFSET_50, row * OFFSET_280 + OFFSET_50,
                  NODE_SIZE_250, NODE_SIZE_250 });
            testNode->SetBackgroundNGShader(shader);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Attenuation Test: attenuation gradient (0.1, 0.5, 1.0, 2.0) */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Attenuation_Test_001)
{
    float attenuationList[] = { ATTENUATION_0_1F, ATTENUATION_0_5F, INTENSITY_1_0F, INTENSITY_2_0F };
    for (int i = 0; i < 4; i++) {
        auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
        shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_175, OFFSET_175, 50.0f));
        shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
        shader->Setter<Rosen::SpatialPointLightAttenuationTag>(attenuationList[i]);

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_280 + OFFSET_50, OFFSET_100, NODE_SIZE_250, NODE_SIZE_250 });
        testNode->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Attenuation Test: attenuation x intensity matrix (4x4) */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Attenuation_Test_002)
{
    float attenuationList[] = { ATTENUATION_0_1F, ATTENUATION_0_5F, INTENSITY_1_0F, INTENSITY_2_0F };
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
            shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(intensityList[row]);
            shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
                Vector3f(OFFSET_175, OFFSET_175, 50.0f));
            shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
            shader->Setter<Rosen::SpatialPointLightAttenuationTag>(attenuationList[col]);

            auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
                { col * OFFSET_280 + OFFSET_50, row * OFFSET_280 + OFFSET_50,
                  NODE_SIZE_250, NODE_SIZE_250 });
            testNode->SetBackgroundNGShader(shader);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Boundary Test: all zero parameters */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Boundary_Test_001)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(0.0f);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(Vector3f(0, 0, 0));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(Vector4f(0, 0, 0, 0));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(0.0f);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Boundary Test: max parameters */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Boundary_Test_002)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(100.0f);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(Vector3f(400, 400, 100));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(10.0f);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Boundary Test: negative intensity */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Boundary_Test_003)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(-1.0f);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Boundary Test: negative attenuation */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Boundary_Test_004)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(-ATTENUATION_0_5F);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Combination Test: shader + border */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Combination_Border_Test_001)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBorderWidth(20);
    testNode->SetBorderColor(COLOR_GREEN);
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Combination Test: shader + alpha */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Combination_Alpha_Test_001)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetAlpha(0.5f);
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Combination Test: shader + corner radius */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Combination_CornerRadius_Test_001)
{
    auto shader = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetCornerRadius(50);
    testNode->SetBackgroundNGShader(shader);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Cascade Test: SpatialPointLight + BorderLight cascade */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Cascade_BorderLight_Test_001)
{
    auto shader1 = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader1->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader1->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader1->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader1->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto shader2 = std::make_shared<Rosen::RSNGBorderLight>();
    shader2->Setter<Rosen::BorderLightPositionTag>(Vector3f(0.5f, 0.5f, 0.0f));
    shader2->Setter<Rosen::BorderLightColorTag>(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
    shader2->Setter<Rosen::BorderLightIntensityTag>(INTENSITY_1_0F);
    shader2->Setter<Rosen::BorderLightWidthTag>(5.0f);
    shader2->Setter<Rosen::BorderLightCornerRadiusTag>(50.0f);

    shader1->Append(shader2);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader1);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Cascade Test: SpatialPointLight + AuroraNoise cascade */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Cascade_AuroraNoise_Test_001)
{
    auto shader1 = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader1->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader1->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader1->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader1->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto shader2 = std::make_shared<Rosen::RSNGAuroraNoise>();
    shader2->Setter<Rosen::AuroraNoiseNoiseTag>(0.5f);
    shader2->Setter<Rosen::AuroraNoiseFreqXTag>(0.01f);
    shader2->Setter<Rosen::AuroraNoiseFreqYTag>(0.01f);

    shader1->Append(shader2);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader1);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Cascade Test: Two SpatialPointLight shaders cascade */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Cascade_DoubleLight_Test_001)
{
    auto shader1 = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader1->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader1->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_100, OFFSET_100, 50.0f));
    shader1->Setter<Rosen::SpatialPointLightLightColorTag>(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
    shader1->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto shader2 = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader2->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader2->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader2->Setter<Rosen::SpatialPointLightLightColorTag>(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
    shader2->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    shader1->Append(shader2);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader1);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Cascade Test: Three-layer shader cascade */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Cascade_ThreeLayer_Test_001)
{
    auto shader1 = std::make_shared<Rosen::RSNGSpatialPointLight>();
    shader1->Setter<Rosen::SpatialPointLightLightIntensityTag>(INTENSITY_1_0F);
    shader1->Setter<Rosen::SpatialPointLightLightPositionTag>(
        Vector3f(OFFSET_200, OFFSET_200, 50.0f));
    shader1->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
    shader1->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

    auto shader2 = std::make_shared<Rosen::RSNGBorderLight>();
    shader2->Setter<Rosen::BorderLightPositionTag>(Vector3f(0.5f, 0.5f, 0.0f));
    shader2->Setter<Rosen::BorderLightColorTag>(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
    shader2->Setter<Rosen::BorderLightIntensityTag>(0.8f);
    shader2->Setter<Rosen::BorderLightWidthTag>(3.0f);
    shader2->Setter<Rosen::BorderLightCornerRadiusTag>(30.0f);

    auto shader3 = std::make_shared<Rosen::RSNGAuroraNoise>();
    shader3->Setter<Rosen::AuroraNoiseNoiseTag>(0.3f);
    shader3->Setter<Rosen::AuroraNoiseFreqXTag>(0.005f);
    shader3->Setter<Rosen::AuroraNoiseFreqYTag>(0.005f);

    shader1->Append(shader2);
    shader2->Append(shader3);

    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
        { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundNGShader(shader1);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Cascade Test: cascade with different intensities */
GRAPHIC_TEST(SpatialPointLightShaderTest, CONTENT_DISPLAY_TEST,
    SpatialPointLightShader_Cascade_Intensity_Test_001)
{
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    for (int i = 0; i < 4; i++) {
        auto shader1 = std::make_shared<Rosen::RSNGSpatialPointLight>();
        shader1->Setter<Rosen::SpatialPointLightLightIntensityTag>(intensityList[i]);
        shader1->Setter<Rosen::SpatialPointLightLightPositionTag>(
            Vector3f(OFFSET_175, OFFSET_175, 50.0f));
        shader1->Setter<Rosen::SpatialPointLightLightColorTag>(ColorToVector4f(COLOR_WHITE));
        shader1->Setter<Rosen::SpatialPointLightAttenuationTag>(ATTENUATION_0_5F);

        auto shader2 = std::make_shared<Rosen::RSNGBorderLight>();
        shader2->Setter<Rosen::BorderLightPositionTag>(Vector3f(0.5f, 0.5f, 0.0f));
        shader2->Setter<Rosen::BorderLightColorTag>(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
        shader2->Setter<Rosen::BorderLightIntensityTag>(intensityList[i]);
        shader2->Setter<Rosen::BorderLightWidthTag>(5.0f);
        shader2->Setter<Rosen::BorderLightCornerRadiusTag>(30.0f);

        shader1->Append(shader2);

        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_280 + OFFSET_50, OFFSET_100, NODE_SIZE_250, NODE_SIZE_250 });
        testNode->SetBackgroundNGShader(shader1);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

} // namespace OHOS::Rosen