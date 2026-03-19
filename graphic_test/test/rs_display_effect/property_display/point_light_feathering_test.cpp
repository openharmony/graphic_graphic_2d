/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable lawver agreed to in writing, software
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

class PointLightFeatheringTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        SetSurfaceColor(RSColor(0xff000000));
    }
};

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Basic_Test_1)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 100, 100, 400, 400 });
    testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
    testNode->SetClipToBounds(true);
    testNode->SetLightIntensity(1);
    testNode->SetLightColor(0xffffffff);
    testNode->SetIlluminatedType(6);
    testNode->SetLightPosition(200, 200, 0);
    testNode->SetIlluminatedBorderWidth(5);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Different_Intensity)
{
    float intensityList[] = { 0.5f, 1.0f, 2.0f, 5.0f };
    for (int i = 0; i < 12; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        if (i % 3 == 1) {
            testNode->SetLightIntensity(intensityList[i / 3]);
            testNode->SetLightColor(0xffffffff);
            testNode->SetIlluminatedType(6);
            testNode->SetLightPosition(200, 200, 0);
            testNode->SetIlluminatedBorderWidth(5);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Different_Color)
{
    uint32_t colorList[] = { 0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00, 0xff00ffff };
    for (int i = 0; i < 18; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 380;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y = 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        if (i % 3 == 1) {
            testNode->SetLightIntensity(1);
            testNode->SetLightColor(colorList[i / 3]);
            testNode->SetIlluminatedType(6);
            testNode->SetLightPosition(200, 200, 0);
            testNode->SetIlluminatedBorderWidth(5);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Different_BorderWidth)
{
    float borderWidthList[] = { 2.0f, 5.0f, 10.0f, 20.0f };
    for (int i = 0; i < 12; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        if (i % 3 == 1) {
            testNode->SetLightIntensity(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetIlluminatedType(6);
            testNode->SetLightPosition(200, 200, 0);
            testNode->SetIlluminatedBorderWidth(borderWidthList[i / 3]);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Different_Position)
{
    float positionList[] = { 100.0f, 200.0f, 300.0f };
    float zList[] = { 0.0f, 0.5f, 1.0f };
    for (int i = 0; i < 9; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        if (i % 3 == 1) {
            testNode->SetLightIntensity(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetIlluminatedType(6);
            testNode->SetLightPosition(positionList[i / 3], positionList[i / 3], zList[i / 3]);
            testNode->SetIlluminatedBorderWidth(5);
        }
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Extreme_Intensity)
{
    float extremeIntensities[] = { 0.0f, 0.1f, 1.0f, 10.0f, 50.0f, 100.0f };
    for (int i = 0; i < 6; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        testNode->SetLightIntensity(extremeIntensities[i]);
        testNode->SetLightColor(0xffffffff);
        testNode->SetIlluminatedType(6);
        testNode->SetLightPosition(200, 200, 0);
        testNode->SetIlluminatedBorderWidth(5);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Extreme_BorderWidth)
{
    float extremeBorderWidths[] = { 0.0f, 1.0f, 5.0f, 20.0f, 50.0f, 100.0f };
    for (int i = 0; i < 6; i++) {
        int x = (i % 3) * 420;
        int y = (i / 3) * 420;
        auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x + 10, y + 10, 400, 400 });
        testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
        testNode->SetClipToBounds(true);
        testNode->SetLightIntensity(1);
        testNode->SetLightColor(0xffffffff);
        testNode->SetIlluminatedType(6);
        testNode->SetLightPosition(200, 200, 0);
        testNode->SetIlluminatedBorderWidth(extremeBorderWidths[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Matrix_Intensity_Color)
{
    float intensityList[] = { 0.5f, 1.0f, 2.0f };
    uint32_t colorList[] = { 0xffffffff, 0xffff0000, 0xff00ff00 };
    
    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < 3; col++) {
            int x = col * 380 + 50;
            int y = row * 380 + 50;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 350, 350 });
            testNode->SetClipRRect({ 0, 0, 350, 350 }, { 175, 175, 175, 175 });
            testNode->SetClipToBounds(true);
            testNode->SetLightIntensity(intensityList[row]);
            testNode->SetLightColor(colorList[col]);
            testNode->SetIlluminatedType(6);
            testNode->SetLightPosition(175, 175, 0);
            testNode->SetIlluminatedBorderWidth(5);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathing_Matrix_BorderWidth_Z)
{
    float borderWidthList[] = { 2.0f, 5.0f, 10.0f };
    float zList[] = { 0.0f, 0.5f, 1.0f };
    
    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < 3; col++) {
            int x = col * 380 + 50;
            int y = row * 380 + 50;
            auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { x, y, 350, 350 });
            testNode->SetClipRRect({ 0, 0, 350, 350 }, { 175, 175, 175, 175 });
            testNode->SetClipToBounds(true);
            testNode->SetLightIntensity(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetIlluminatedType(6);
            testNode->SetLightPosition(175, 175, zList[row]);
            testNode->SetIlluminatedBorderWidth(borderWidthList[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Multiple_Nodes)
{
    auto parentNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 100, 100, 800, 800 });
    parentNode->SetClipToBounds(true);
    GetRootNode()->AddChild(parentNode);
    
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * 400;
        int y = (i / 2) * 400;
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ x + 50, y + 50, 300, 300 });
        childNode->SetClipRRect({ 0, 0, 300, 300 }, { 150, 150, 150, 150 });
        childNode->SetClipToBounds(true);
        childNode->SetLightIntensity(1);
        childNode->SetLightColor(0xffffffff);
        childNode->SetIlluminatedType(6);
        childNode->SetLightPosition(150, 150, 0);
        childNode->SetIlluminatedBorderWidth(5);
        parentNode->AddChild(childNode);
    }
    RegisterNode(parentNode);
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathing_Edge_Positions)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 100, 100, 600, 600 });
    testNode->SetClipToBounds(true);
    GetRootNode()->AddChild(testNode);
    
    std::vector<std::pair<float, float>> positions = {
        { 0.0f, 0.0f }, { 600.0f, 0.0f }, { 0.0f, 600.0f }, { 600.0f, 600.0f }, { 300.0f, 300.0f }
    };
    
    for (size_t i = 0; i < positions.size(); i++) {
        auto childNode = RSCanvasNode::Create();
        childNode->SetBounds({ 0, 0, 600, 600 });
        childNode->SetClipRRect({ 0, 0, 600, 600 }, { 300, 300, 300, 300 });
        childNode->SetClipToBounds(true);
        childNode->SetLightIntensity(1);
        childNode->SetLightColor(0xffffffff);
        childNode->SetIlluminatedType(6);
        childNode->SetLightPosition(positions[i].first, positions[i].second, 0);
        childNode->SetIlluminatedBorderWidth(5);
        testNode->AddChild(childNode);
    }
    RegisterNode(testNode);
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathing_Zero_Intensity)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 100, 100, 400, 400 });
    testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
    testNode->SetClipToBounds(true);
    testNode->SetLightIntensity(0);
    testNode->SetLightColor(0xffffffff);
    testNode->SetIlluminatedType(6);
    testNode->SetLightPosition(200, 200, 0);
    testNode->SetIlluminatedBorderWidth(5);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathing_Zero_BorderWidth)
{
    auto testNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", { 100, 100, 400, 400 });
    testNode->SetClipRRect({ 0, 0, 400, 400 }, { 200, 200, 200, 200 });
    testNode->SetClipToBounds(true);
    testNode->SetLightIntensity(1);
    testNode->SetLightColor(0xffffffff);
    testNode->SetIlluminatedType(6);
    testNode->SetLightPosition(200, 200, 0);
    testNode->SetIlluminatedBorderWidth(0);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

} // namespace OHOS::Rosen
