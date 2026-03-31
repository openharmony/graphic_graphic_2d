/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_graphic_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class PointLightFeatheringTest : public RSGraphicTest {
private:
    const int32_t screenWidth = 1200;
    const int32_t screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        SetSurfaceColor(RSColor(0xff000000));
    }
};

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Basic_Test_1)
{
    auto testNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{ 100, 100, 400, 400 };
    testNode->SetBounds(bounds);
    testNode->SetFrame(bounds);
    testNode->SetBackgroundColor(0xFF888888);
    testNode->SetClipToBounds(true);
    testNode->SetLightIntensity(1);
    testNode->SetLightColor(0xffffffff);
    testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
    testNode->SetLightPosition(200, 200, 300);
    testNode->SetIlluminatedBorderWidth(10);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Different_Intensity)
{
    float intensityList[] = { 0.5f, 1.0f, 2.0f, 5.0f };
    for (size_t i = 0; i < 4; i++) {
        int x = (i % 2) * 550 + 50;
        int y = (i / 2) * 550 + 50;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{ x, y, 500, 500 };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888);
        testNode->SetClipToBounds(true);
        testNode->SetLightIntensity(intensityList[i]);
        testNode->SetLightColor(0xffffffff);
        testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
        testNode->SetLightPosition(250, 250, 300);
        testNode->SetIlluminatedBorderWidth(10);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Different_Color)
{
    uint32_t colorList[] = { 0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00, 0xff00ffff };
    for (size_t i = 0; i < 6; i++) {
        int x = (i % 3) * 380 + 50;
        int y = (i / 3) * 380 + 50;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{ x, y, 350, 350 };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888);
        testNode->SetClipToBounds(true);
        testNode->SetLightIntensity(1);
        testNode->SetLightColor(colorList[i]);
        testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
        testNode->SetLightPosition(175, 175, 300);
        testNode->SetIlluminatedBorderWidth(10);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Different_BorderWidth)
{
    float borderWidthList[] = { 4.0f, 10.0f, 20.0f, 40.0f };
    for (size_t i = 0; i < 4; i++) {
        int x = (i % 2) * 550 + 50;
        int y = (i / 2) * 550 + 50;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{ x, y, 500, 500 };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888);
        testNode->SetClipToBounds(true);
        testNode->SetLightIntensity(1);
        testNode->SetLightColor(0xffffffff);
        testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
        testNode->SetLightPosition(250, 250, 300);
        testNode->SetIlluminatedBorderWidth(borderWidthList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Different_Position)
{
    float positionList[] = { 100.0f, 200.0f, 300.0f };
    float zList[] = { 0.0f, 150.0f, 300.0f };
    for (size_t i = 0; i < 3; i++) {
        int x = (i % 3) * 380 + 50;
        int y = 50;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{ x, y, 350, 350 };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888);
        testNode->SetClipToBounds(true);
        testNode->SetLightIntensity(1);
        testNode->SetLightColor(0xffffffff);
        testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
        testNode->SetLightPosition(positionList[i], positionList[i], zList[i]);
        testNode->SetIlluminatedBorderWidth(10);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Extreme_Intensity)
{
    float extremeIntensities[] = { 0.0f, 0.1f, 1.0f, 10.0f, 50.0f, 100.0f };
    for (size_t i = 0; i < 6; i++) {
        int x = (i % 3) * 380 + 50;
        int y = (i / 3) * 380 + 50;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{ x, y, 350, 350 };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888);
        testNode->SetClipToBounds(true);
        testNode->SetLightIntensity(extremeIntensities[i]);
        testNode->SetLightColor(0xffffffff);
        testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
        testNode->SetLightPosition(175, 175, 300);
        testNode->SetIlluminatedBorderWidth(10);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Extreme_BorderWidth)
{
    float extremeBorderWidths[] = { 0.0f, 2.0f, 10.0f, 40.0f, 100.0f, 200.0f };
    for (size_t i = 0; i < 6; i++) {
        int x = (i % 3) * 380 + 50;
        int y = (i / 3) * 380 + 50;
        auto testNode = RSCanvasNode::Create();
        Rosen::Vector4f bounds{ x, y, 350, 350 };
        testNode->SetBounds(bounds);
        testNode->SetFrame(bounds);
        testNode->SetBackgroundColor(0xFF888888);
        testNode->SetClipToBounds(true);
        testNode->SetLightIntensity(1);
        testNode->SetLightColor(0xffffffff);
        testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
        testNode->SetLightPosition(175, 175, 300);
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
            auto testNode = RSCanvasNode::Create();
            Rosen::Vector4f bounds{ x, y, 350, 350 };
            testNode->SetBounds(bounds);
            testNode->SetFrame(bounds);
            testNode->SetBackgroundColor(0xFF888888);
            testNode->SetClipToBounds(true);
            testNode->SetLightIntensity(intensityList[row]);
            testNode->SetLightColor(colorList[col]);
            testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
            testNode->SetLightPosition(175, 175, 300);
            testNode->SetIlluminatedBorderWidth(10);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Matrix_BorderWidth_Z)
{
    float borderWidthList[] = { 4.0f, 10.0f, 20.0f };
    float zList[] = { 0.0f, 150.0f, 300.0f };
    
    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < 3; col++) {
            int x = col * 380 + 50;
            int y = row * 380 + 50;
            auto testNode = RSCanvasNode::Create();
            Rosen::Vector4f bounds{ x, y, 350, 350 };
            testNode->SetBounds(bounds);
            testNode->SetFrame(bounds);
            testNode->SetBackgroundColor(0xFF888888);
            testNode->SetClipToBounds(true);
            testNode->SetLightIntensity(1);
            testNode->SetLightColor(0xffffffff);
            testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
            testNode->SetLightPosition(175, 175, zList[row]);
            testNode->SetIlluminatedBorderWidth(borderWidthList[col]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Multiple_Nodes)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f parentBounds{ 100, 100, 800, 800 };
    parentNode->SetBounds(parentBounds);
    parentNode->SetFrame(parentBounds);
    parentNode->SetBackgroundColor(0xFF888888);
    parentNode->SetClipToBounds(true);
    GetRootNode()->AddChild(parentNode);
    
    for (int i = 0; i < 4; i++) {
        int x = (i % 2) * 400;
        int y = (i / 2) * 400;
        auto childNode = RSCanvasNode::Create();
        Rosen::Vector4f childBounds{ x + 50, y + 50, 300, 300 };
        childNode->SetBounds(childBounds);
        childNode->SetFrame(childBounds);
        childNode->SetBackgroundColor(0xFF888888);
        childNode->SetClipToBounds(true);
        childNode->SetLightIntensity(1);
        childNode->SetLightColor(0xffffffff);
        childNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
        childNode->SetLightPosition(150, 150, 300);
        childNode->SetIlluminatedBorderWidth(10);
        parentNode->AddChild(childNode);
    }
    RegisterNode(parentNode);
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Edge_Positions)
{
    auto testNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{ 100, 100, 600, 600 };
    testNode->SetBounds(bounds);
    testNode->SetFrame(bounds);
    testNode->SetBackgroundColor(0xFF888888);
    testNode->SetClipToBounds(true);
    GetRootNode()->AddChild(testNode);
    
    std::vector<std::pair<float, float>> positions = {
        { 0.0f, 0.0f }, { 600.0f, 0.0f }, { 0.0f, 600.0f }, { 600.0f, 600.0f }, { 300.0f, 300.0f }
    };
    
    for (size_t i = 0; i < positions.size(); i++) {
        auto childNode = RSCanvasNode::Create();
        Rosen::Vector4f childBounds{ 0, 0, 600, 600 };
        childNode->SetBounds(childBounds);
        childNode->SetFrame(childBounds);
        childNode->SetBackgroundColor(0xFF888888);
        childNode->SetClipToBounds(true);
        childNode->SetLightIntensity(1);
        childNode->SetLightColor(0xffffffff);
        childNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
        childNode->SetLightPosition(positions[i].first, positions[i].second, 300);
        childNode->SetIlluminatedBorderWidth(10);
        testNode->AddChild(childNode);
    }
    RegisterNode(testNode);
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Zero_Intensity)
{
    auto testNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{ 100, 100, 400, 400 };
    testNode->SetBounds(bounds);
    testNode->SetFrame(bounds);
    testNode->SetBackgroundColor(0xFF888888);
    testNode->SetClipToBounds(true);
    testNode->SetLightIntensity(0);
    testNode->SetLightColor(0xffffffff);
    testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
    testNode->SetLightPosition(200, 200, 300);
    testNode->SetIlluminatedBorderWidth(10);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

GRAPHIC_TEST(PointLightFeatheringTest, CONTENT_DISPLAY_TEST, PointLight_Feathering_Zero_BorderWidth)
{
    auto testNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{ 100, 100, 400, 400 };
    testNode->SetBounds(bounds);
    testNode->SetFrame(bounds);
    testNode->SetBackgroundColor(0xFF888888);
    testNode->SetClipToBounds(true);
    testNode->SetLightIntensity(1);
    testNode->SetLightColor(0xffffffff);
    testNode->SetIlluminatedType(static_cast<uint32_t>(IlluminatedType::FEATHERING_BORDER));
    testNode->SetLightPosition(200, 200, 300);
    testNode->SetIlluminatedBorderWidth(0);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

} // namespace OHOS::Rosen
