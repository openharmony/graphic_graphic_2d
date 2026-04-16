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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

namespace {
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int NODE_SIZE_350 = 350;
constexpr int NODE_SIZE_400 = 400;
constexpr int NODE_SIZE_500 = 500;
constexpr int NODE_SIZE_600 = 600;
constexpr int NODE_SIZE_250 = 250;
constexpr int NODE_SIZE_200 = 200;
constexpr int NODE_SIZE_150 = 150;
constexpr int NODE_SIZE_120 = 120;
constexpr int NODE_SIZE_100 = 100;
constexpr int NODE_SIZE_50 = 50;
constexpr int OFFSET_10 = 10;
constexpr int OFFSET_50 = 50;
constexpr int OFFSET_100 = 100;
constexpr int OFFSET_160 = 160;
constexpr int OFFSET_175 = 175;
constexpr int OFFSET_200 = 200;
constexpr int OFFSET_240 = 240;
constexpr int OFFSET_280 = 280;
constexpr int OFFSET_380 = 380;
constexpr int OFFSET_500 = 500;
constexpr int OFFSET_510 = 510;
constexpr int OFFSET_600 = 600;
constexpr int LIGHT_POSITION_25 = 25;
constexpr int LIGHT_POSITION_75 = 75;
constexpr int LIGHT_POSITION_100 = 100;
constexpr int LIGHT_POSITION_125 = 125;
constexpr int LIGHT_POSITION_200 = 200;
constexpr int LIGHT_POSITION_250 = 250;
constexpr int LIGHT_POSITION_300 = 300;
constexpr int LIGHT_POSITION_400 = 400;
constexpr int LIGHT_DEPTH_10 = 10;
constexpr int LIGHT_DEPTH_30 = 30;
constexpr int LIGHT_DEPTH_50 = 50;
constexpr int LIGHT_DEPTH_100 = 100;
constexpr int BORDER_WIDTH_10 = 10;
constexpr int BORDER_WIDTH_20 = 20;
constexpr int BORDER_WIDTH_40 = 40;
constexpr int BORDER_WIDTH_80 = 80;
constexpr float INTENSITY_0_5F = 0.5f;
constexpr float INTENSITY_1_0F = 1.0f;
constexpr float INTENSITY_1_5F = 1.5f;
constexpr float INTENSITY_2_0F = 2.0f;
constexpr float ATTENUATION_0_1F = 0.1f;
constexpr float ATTENUATION_0_5F = 0.5f;
constexpr float ALPHA_0_3F = 0.3f;
constexpr float ALPHA_0_5F = 0.5f;
constexpr float ALPHA_0_7F = 0.7f;
constexpr float ALPHA_1_0F = 1.0f;
constexpr uint32_t COLOR_WHITE = 0xffffffff;
constexpr uint32_t COLOR_BLACK = 0xff000000;
constexpr uint32_t COLOR_RED = 0xffff0000;
constexpr uint32_t COLOR_GREEN = 0xff00ff00;
constexpr uint32_t COLOR_BLUE = 0xff0000ff;
constexpr uint32_t COLOR_HALF_WHITE = 0x7fffffff;
constexpr uint32_t COLOR_TRANSPARENT = 0x00000000;
constexpr int ILLUMINATED_TYPE = 1;
const std::string TEST_IMAGE_PATH = "/data/local/tmp/fg_test.jpg";
const std::string GEOM_TEST_PATH = "/data/local/tmp/geom_test.jpg";
}

class SpatialPointLightTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
        SetSurfaceColor(RSColor(0xff000000));
    }
};

/* Basic Test: single node with default light */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Basic_Test_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBackgroundColor(COLOR_RED);
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: single node with specified light position */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Basic_Test_002)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: single node with background image */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Basic_Test_003)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(OFFSET_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Basic Test: multiple nodes with same light parameters */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Basic_Test_004)
{
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * OFFSET_510;
        int y = (i / 2) * OFFSET_510;
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { x + OFFSET_10, y + OFFSET_10, NODE_SIZE_400, NODE_SIZE_400 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Basic Test: multiple nodes with different light positions */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Basic_Test_005)
{
    std::vector<std::pair<int, int>> positions = {
        { OFFSET_100, OFFSET_100 },
        { LIGHT_POSITION_300, OFFSET_100 },
        { OFFSET_100, LIGHT_POSITION_300 },
        { LIGHT_POSITION_300, LIGHT_POSITION_300 }
    };
    for (size_t i = 0; i < positions.size(); i++) {
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { positions[i].first, positions[i].second, NODE_SIZE_200, NODE_SIZE_200 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(LIGHT_POSITION_100, LIGHT_POSITION_100, LIGHT_DEPTH_50);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Light Intensity Test: intensity = 0 */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Intensity_Test_001)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(0.0f);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Light Intensity Test: intensity gradient (0.5, 1.0, 1.5, 2.0) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Intensity_Test_002)
{
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * OFFSET_510;
        int y = (i / 2) * OFFSET_510;
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { x + OFFSET_10, y + OFFSET_10, NODE_SIZE_400, NODE_SIZE_400 });
        testNode->SetLightIntensity(intensityList[i]);
        testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Light Intensity Test: intensity x color matrix (4x3) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Intensity_Test_003)
{
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    uint32_t colorList[] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE };
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 3; col++) {
            auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
                { col * OFFSET_280 + OFFSET_50, row * OFFSET_280 + OFFSET_50, NODE_SIZE_250, NODE_SIZE_250 });
            testNode->SetLightIntensity(intensityList[row]);
            testNode->SetLightPosition(LIGHT_POSITION_125, LIGHT_POSITION_125, LIGHT_DEPTH_50);
            testNode->SetLightColor(colorList[col]);
            testNode->SetIlluminatedType(ILLUMINATED_TYPE);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Light Intensity Test: large intensity (100) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Intensity_Test_004)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(100.0f);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Light Intensity Test: boundary intensity matrix (0, 0.5, 1, 10) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Intensity_Test_005)
{
    float intensityList[] = { 0.0f, INTENSITY_0_5F, INTENSITY_1_0F, 10.0f };
    uint32_t colorList[] = { COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE };
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
                { col * OFFSET_280 + OFFSET_50, row * OFFSET_280 + OFFSET_50, NODE_SIZE_250, NODE_SIZE_250 });
            testNode->SetLightIntensity(intensityList[row]);
            testNode->SetLightPosition(LIGHT_POSITION_125, LIGHT_POSITION_125, LIGHT_DEPTH_50);
            testNode->SetLightColor(colorList[col]);
            testNode->SetIlluminatedType(ILLUMINATED_TYPE);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Light Position Test: center position */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Position_Test_001)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Light Position Test: corner positions */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Position_Test_002)
{
    std::vector<std::pair<int, int>> cornerPositions = {
        { 0, 0 },
        { NODE_SIZE_400, 0 },
        { 0, NODE_SIZE_400 },
        { NODE_SIZE_400, NODE_SIZE_400 }
    };
    for (size_t i = 0; i < cornerPositions.size(); i++) {
        int x = (i % 2) * OFFSET_510;
        int y = (i / 2) * OFFSET_510;
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { x + OFFSET_10, y + OFFSET_10, NODE_SIZE_400, NODE_SIZE_400 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(cornerPositions[i].first, cornerPositions[i].second, LIGHT_DEPTH_50);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Light Position Test: position grid (3x3) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Position_Test_003)
{
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
                { x * OFFSET_380 + OFFSET_50, y * OFFSET_380 + OFFSET_50, NODE_SIZE_350, NODE_SIZE_350 });
            testNode->SetLightIntensity(INTENSITY_1_0F);
            testNode->SetLightPosition(OFFSET_175 + x * OFFSET_100, OFFSET_175 + y * OFFSET_100, LIGHT_DEPTH_50);
            testNode->SetLightColor(COLOR_WHITE);
            testNode->SetIlluminatedType(ILLUMINATED_TYPE);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Light Position Test: Z-axis depth (0, 50, 100, 200) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Position_Test_004)
{
    float zList[] = { 0, LIGHT_DEPTH_50, LIGHT_DEPTH_100, 200 };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_280 + OFFSET_50, OFFSET_100, NODE_SIZE_250, NODE_SIZE_250 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(LIGHT_POSITION_125, LIGHT_POSITION_125, zList[i]);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Light Position Test: out-of-bounds position */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Position_Test_005)
{
    std::vector<std::tuple<int, int, int>> outOfBoundsPositions = {
        { -OFFSET_100, -OFFSET_100, 0 },
        { OFFSET_500, OFFSET_500, 0 },
        { -OFFSET_100, LIGHT_POSITION_200, LIGHT_DEPTH_50 },
        { LIGHT_POSITION_200, OFFSET_500, LIGHT_DEPTH_50 }
    };
    for (size_t i = 0; i < outOfBoundsPositions.size(); i++) {
        int x = (i % 2) * OFFSET_510;
        int y = (i / 2) * OFFSET_510;
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { x + OFFSET_10, y + OFFSET_10, NODE_SIZE_400, NODE_SIZE_400 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(std::get<0>(outOfBoundsPositions[i]),
            std::get<1>(outOfBoundsPositions[i]), std::get<2>(outOfBoundsPositions[i]));
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Light Position Test: extreme position matrix (x, y, z 3x3x3) - simplified to 9 combinations */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Position_Test_006)
{
    int xList[] = { 0, LIGHT_POSITION_125, NODE_SIZE_250 };
    int yList[] = { 0, LIGHT_POSITION_125, NODE_SIZE_250 };
    int zList[] = { 0, LIGHT_DEPTH_50, LIGHT_DEPTH_100 };
    for (int i = 0; i < 9; i++) {
        int xi = i % 3;
        int yi = (i / 3) % 3;
        int zi = i / 3;
        int posX = (i % 3) * OFFSET_380 + OFFSET_50;
        int posY = (i / 3) * OFFSET_380 + OFFSET_50;
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { posX, posY, NODE_SIZE_350, NODE_SIZE_350 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(xList[xi], yList[yi], zList[zi]);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Light Color Test: white light */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Color_Test_001)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Light Color Test: red light */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Color_Test_002)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_RED);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Light Color Test: RGB primary colors */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Color_Test_003)
{
    uint32_t colorList[] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE };
    for (int i = 0; i < 3; i++) {
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_380 + OFFSET_50, OFFSET_100, NODE_SIZE_350, NODE_SIZE_350 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(OFFSET_175, OFFSET_175, LIGHT_DEPTH_50);
        testNode->SetLightColor(colorList[i]);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Light Color Test: color matrix (6 colors) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Color_Test_004)
{
    uint32_t colorList[] = { COLOR_WHITE, COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_HALF_WHITE };
    for (int i = 0; i < 6; i++) {
        int x = (i % 3) * OFFSET_380;
        int y = (i / 3) * OFFSET_380;
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { x + OFFSET_50, y + OFFSET_50, NODE_SIZE_350, NODE_SIZE_350 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(OFFSET_175, OFFSET_175, LIGHT_DEPTH_50);
        testNode->SetLightColor(colorList[i]);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Light Color Test: color x intensity matrix (4x4) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Color_Test_005)
{
    uint32_t colorList[] = { COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE };
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
                { col * OFFSET_280 + OFFSET_50, row * OFFSET_280 + OFFSET_50, NODE_SIZE_250, NODE_SIZE_250 });
            testNode->SetLightIntensity(intensityList[row]);
            testNode->SetLightPosition(LIGHT_POSITION_125, LIGHT_POSITION_125, LIGHT_DEPTH_50);
            testNode->SetLightColor(colorList[col]);
            testNode->SetIlluminatedType(ILLUMINATED_TYPE);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Light Color Test: color alpha test */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Color_Test_006)
{
    uint32_t alphaColorList[] = { 0xffffffff, 0x7fffffff, 0x3fffffff, 0x1fffffff };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_280 + OFFSET_50, OFFSET_100, NODE_SIZE_250, NODE_SIZE_250 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(LIGHT_POSITION_125, LIGHT_POSITION_125, LIGHT_DEPTH_50);
        testNode->SetLightColor(alphaColorList[i]);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Attenuation Test: attenuation = 0 */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Attenuation_Test_001)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    testNode->SetBloom(0.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Attenuation Test: attenuation gradient (0.1, 0.5, 1.0, 2.0) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Attenuation_Test_002)
{
    float attenuationList[] = { ATTENUATION_0_1F, ATTENUATION_0_5F, INTENSITY_1_0F, INTENSITY_2_0F };
    for (int i = 0; i < 4; i++) {
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_280 + OFFSET_50, OFFSET_100, NODE_SIZE_250, NODE_SIZE_250 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(LIGHT_POSITION_125, LIGHT_POSITION_125, LIGHT_DEPTH_50);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        testNode->SetBloom(attenuationList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Attenuation Test: high attenuation (32, 64) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Attenuation_Test_003)
{
    float highAttenuationList[] = { 32.0f, 64.0f };
    for (int i = 0; i < 2; i++) {
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_510 + OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        testNode->SetBloom(highAttenuationList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Attenuation Test: attenuation x intensity matrix (4x4) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Attenuation_Test_004)
{
    float attenuationList[] = { ATTENUATION_0_1F, ATTENUATION_0_5F, INTENSITY_1_0F, INTENSITY_2_0F };
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
                { col * OFFSET_280 + OFFSET_50, row * OFFSET_280 + OFFSET_50, NODE_SIZE_250, NODE_SIZE_250 });
            testNode->SetLightIntensity(intensityList[row]);
            testNode->SetLightPosition(LIGHT_POSITION_125, LIGHT_POSITION_125, LIGHT_DEPTH_50);
            testNode->SetLightColor(COLOR_WHITE);
            testNode->SetIlluminatedType(ILLUMINATED_TYPE);
            testNode->SetBloom(attenuationList[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Boundary Test: all zero parameters */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Boundary_Test_001)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(0.0f);
    testNode->SetLightPosition(0, 0, 0);
    testNode->SetLightColor(COLOR_TRANSPARENT);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Boundary Test: max parameters */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Boundary_Test_002)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(100.0f);
    testNode->SetLightPosition(LIGHT_POSITION_400, LIGHT_POSITION_400, LIGHT_DEPTH_100);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    testNode->SetBloom(10.0f);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Boundary Test: negative intensity */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Boundary_Test_003)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(-1.0f);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Boundary Test: negative position */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Boundary_Test_004)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(-OFFSET_100, -OFFSET_100, -LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Boundary Test: negative attenuation */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Boundary_Test_005)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    testNode->SetBloom(-ATTENUATION_0_5F);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Boundary Test: large size node (500x500) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Boundary_Test_006)
{
    auto largeNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_50, OFFSET_50, NODE_SIZE_500, NODE_SIZE_500 });
    largeNode->SetLightIntensity(INTENSITY_1_0F);
    largeNode->SetLightPosition(LIGHT_POSITION_250, LIGHT_POSITION_250, LIGHT_DEPTH_100);
    largeNode->SetLightColor(COLOR_WHITE);
    largeNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(largeNode);
    RegisterNode(largeNode);
    
    auto smallNode = RSCanvasNode::Create();
    smallNode->SetBounds({ OFFSET_600, OFFSET_50, NODE_SIZE_50, NODE_SIZE_50 });
    smallNode->SetBackgroundColor(COLOR_RED);
    smallNode->SetLightIntensity(INTENSITY_1_0F);
    smallNode->SetLightPosition(LIGHT_POSITION_25, LIGHT_POSITION_25, LIGHT_DEPTH_10);
    smallNode->SetLightColor(COLOR_WHITE);
    smallNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(smallNode);
    RegisterNode(smallNode);
}

/* Boundary Test: small size node (50x50) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Boundary_Test_007)
{
    for (int i = 0; i < 4; i++) {
        auto smallNode = RSCanvasNode::Create();
        smallNode->SetBounds({ i * OFFSET_100 + OFFSET_50, OFFSET_100, NODE_SIZE_50, NODE_SIZE_50 });
        smallNode->SetBackgroundColor(COLOR_RED);
        smallNode->SetLightIntensity(INTENSITY_1_0F);
        smallNode->SetLightPosition(LIGHT_POSITION_25, LIGHT_POSITION_25, LIGHT_DEPTH_10);
        smallNode->SetLightColor(COLOR_WHITE);
        smallNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(smallNode);
        RegisterNode(smallNode);
    }
}

/* Boundary Test: non-square node (300x100) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Boundary_Test_008)
{
    std::vector<std::pair<int, int>> sizes = { { 300, 100 }, { 100, 300 }, { 400, 200 }, { 200, 400 } };
    for (size_t i = 0; i < sizes.size(); i++) {
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
            { i * OFFSET_280 + OFFSET_50, OFFSET_100, sizes[i].first, sizes[i].second });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(sizes[i].first / 2, sizes[i].second / 2, LIGHT_DEPTH_50);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Node Combination Test: light + border */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Node_Border_Test_001)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetBorderWidth(BORDER_WIDTH_20);
    testNode->SetBorderColor(COLOR_GREEN);
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Node Combination Test: light + border matrix (border width x intensity 5x4) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Node_Border_Test_002)
{
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    int borderWidthList[] = { 0, BORDER_WIDTH_10, BORDER_WIDTH_20, BORDER_WIDTH_40, BORDER_WIDTH_80 };
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 4; col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ col * OFFSET_240 + OFFSET_50, row * OFFSET_160 + OFFSET_50, NODE_SIZE_200, NODE_SIZE_120 });
            testNode->SetBackgroundColor(COLOR_RED);
            testNode->SetBorderWidth(borderWidthList[row]);
            testNode->SetBorderColor(COLOR_GREEN);
            testNode->SetLightIntensity(intensityList[col]);
            testNode->SetLightPosition(LIGHT_POSITION_100, LIGHT_DEPTH_30, LIGHT_DEPTH_30);
            testNode->SetLightColor(COLOR_WHITE);
            testNode->SetIlluminatedType(ILLUMINATED_TYPE);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Node Combination Test: light + shadow */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Node_Shadow_Test_001)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetShadowColor(COLOR_BLACK);
    testNode->SetShadowRadius(40);
    testNode->SetShadowOffset(30, 30);
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Node Combination Test: light + alpha */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Node_Alpha_Test_001)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetAlpha(ALPHA_0_5F);
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Node Combination Test: light + alpha matrix (alpha x intensity 4x4) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Node_Alpha_Test_002)
{
    float alphaList[] = { ALPHA_0_3F, ALPHA_0_5F, ALPHA_0_7F, ALPHA_1_0F };
    float intensityList[] = { INTENSITY_0_5F, INTENSITY_1_0F, INTENSITY_1_5F, INTENSITY_2_0F };
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH,
                { col * OFFSET_280 + OFFSET_50, row * OFFSET_280 + OFFSET_50, NODE_SIZE_250, NODE_SIZE_250 });
            testNode->SetAlpha(alphaList[row]);
            testNode->SetLightIntensity(intensityList[col]);
            testNode->SetLightPosition(LIGHT_POSITION_125, LIGHT_POSITION_125, LIGHT_DEPTH_50);
            testNode->SetLightColor(COLOR_WHITE);
            testNode->SetIlluminatedType(ILLUMINATED_TYPE);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/* Node Combination Test: light + corner radius clipping */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Node_ClipRRect_Test_001)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetCornerRadius(50);
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Node Combination Test: light + ClipToBounds */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_Node_ClipToBounds_Test_001)
{
    auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    testNode->SetClipToBounds(true);
    testNode->SetLightIntensity(INTENSITY_1_0F);
    testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    testNode->SetLightColor(COLOR_WHITE);
    testNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/* Multi-node Test: 2x2 grid */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_MultiNode_Test_001)
{
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * OFFSET_510;
        int y = (i / 2) * OFFSET_510;
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { x + OFFSET_10, y + OFFSET_10, NODE_SIZE_400, NODE_SIZE_400 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Multi-node Test: 3x3 grid */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_MultiNode_Test_002)
{
    for (int i = 0; i < 9; i++) {
        int x = (i % 3) * OFFSET_380 + OFFSET_50;
        int y = (i / 3) * OFFSET_380 + OFFSET_50;
        auto testNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { x, y, NODE_SIZE_350, NODE_SIZE_350 });
        testNode->SetLightIntensity(INTENSITY_1_0F);
        testNode->SetLightPosition(OFFSET_175, OFFSET_175, LIGHT_DEPTH_50);
        testNode->SetLightColor(COLOR_WHITE);
        testNode->SetIlluminatedType(ILLUMINATED_TYPE);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/* Multi-node Test: parent-child - parent has light */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_MultiNode_Test_003)
{
    auto parentNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_600, NODE_SIZE_600 });
    parentNode->SetLightIntensity(INTENSITY_0_5F);
    parentNode->SetLightPosition(LIGHT_POSITION_300, LIGHT_POSITION_300, LIGHT_DEPTH_100);
    parentNode->SetLightColor(COLOR_RED);
    parentNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(parentNode);
    
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * OFFSET_200 + OFFSET_100;
        int y = (i / 2) * OFFSET_200 + OFFSET_100;
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ x, y, NODE_SIZE_150, NODE_SIZE_150 });
        childNode->SetBackgroundColor(COLOR_WHITE);
        parentNode->AddChild(childNode);
        RegisterNode(childNode);
    }
    RegisterNode(parentNode);
}

/* Multi-node Test: parent-child - child has light */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_MultiNode_Test_004)
{
    auto parentNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_600, NODE_SIZE_600 });
    GetRootNode()->AddChild(parentNode);
    
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * OFFSET_200 + OFFSET_100;
        int y = (i / 2) * OFFSET_200 + OFFSET_100;
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ x, y, NODE_SIZE_150, NODE_SIZE_150 });
        childNode->SetBackgroundColor(COLOR_WHITE);
        childNode->SetLightIntensity(INTENSITY_1_0F);
        childNode->SetLightPosition(LIGHT_POSITION_75, LIGHT_POSITION_75, LIGHT_DEPTH_30);
        childNode->SetLightColor(COLOR_GREEN);
        childNode->SetIlluminatedType(ILLUMINATED_TYPE);
        parentNode->AddChild(childNode);
        RegisterNode(childNode);
    }
    RegisterNode(parentNode);
}

/* Multi-node Test: parent-child - both have light */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_MultiNode_Test_005)
{
    auto parentNode = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_600, NODE_SIZE_600 });
    parentNode->SetLightIntensity(INTENSITY_0_5F);
    parentNode->SetLightPosition(LIGHT_POSITION_300, LIGHT_POSITION_300, LIGHT_DEPTH_100);
    parentNode->SetLightColor(COLOR_RED);
    parentNode->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(parentNode);
    
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * OFFSET_200 + OFFSET_100;
        int y = (i / 2) * OFFSET_200 + OFFSET_100;
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ x, y, NODE_SIZE_150, NODE_SIZE_150 });
        childNode->SetBackgroundColor(COLOR_WHITE);
        childNode->SetLightIntensity(INTENSITY_1_0F);
        childNode->SetLightPosition(LIGHT_POSITION_75, LIGHT_POSITION_75, LIGHT_DEPTH_30);
        childNode->SetLightColor(COLOR_GREEN);
        childNode->SetIlluminatedType(ILLUMINATED_TYPE);
        parentNode->AddChild(childNode);
        RegisterNode(childNode);
    }
    RegisterNode(parentNode);
}

/* Multi-node Test: nested nodes (3 layers) */
GRAPHIC_TEST(SpatialPointLightTest, CONTENT_DISPLAY_TEST, SpatialPointLight_MultiNode_Test_006)
{
    auto layer1Node = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_50, OFFSET_50, NODE_SIZE_500, NODE_SIZE_500 });
    layer1Node->SetLightIntensity(INTENSITY_0_3F);
    layer1Node->SetLightPosition(LIGHT_POSITION_250, LIGHT_POSITION_250, LIGHT_DEPTH_100);
    layer1Node->SetLightColor(COLOR_WHITE);
    layer1Node->SetIlluminatedType(ILLUMINATED_TYPE);
    GetRootNode()->AddChild(layer1Node);
    
    auto layer2Node = SetUpNodeBgImage(TEST_IMAGE_PATH, { OFFSET_100, OFFSET_100, NODE_SIZE_400, NODE_SIZE_400 });
    layer2Node->SetLightIntensity(INTENSITY_0_5F);
    layer2Node->SetLightPosition(LIGHT_POSITION_200, LIGHT_POSITION_200, LIGHT_DEPTH_50);
    layer2Node->SetLightColor(COLOR_RED);
    layer2Node->SetIlluminatedType(ILLUMINATED_TYPE);
    layer1Node->AddChild(layer2Node);
    
    auto layer3Node = RSCanvasNode::Create();
    layer3Node->SetBounds({ OFFSET_150, OFFSET_150, NODE_SIZE_200, NODE_SIZE_200 });
    layer3Node->SetBackgroundColor(COLOR_WHITE);
    layer3Node->SetLightIntensity(INTENSITY_1_0F);
    layer3Node->SetLightPosition(LIGHT_POSITION_100, LIGHT_POSITION_100, LIGHT_DEPTH_30);
    layer3Node->SetLightColor(COLOR_GREEN);
    layer3Node->SetIlluminatedType(ILLUMINATED_TYPE);
    layer2Node->AddChild(layer3Node);
    
    RegisterNode(layer3Node);
    RegisterNode(layer2Node);
    RegisterNode(layer1Node);
}

} // namespace OHOS::Rosen