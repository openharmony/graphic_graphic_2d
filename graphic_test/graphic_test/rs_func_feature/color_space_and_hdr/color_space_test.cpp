/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "rs_graphic_test_text.h"
#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class ColorSpaceTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
    const std::vector<std::string> pathList = { "/data/local/tmp/Images/color_space_srgb_1.jpg",
        "/data/local/tmp/Images/color_space_p3_1.jpg", "/data/local/tmp/Images/color_space_srgb_2.jpg",
        "/data/local/tmp/Images/color_space_p3_2.jpg" };

    void CreateTestSurfaceNode(bool isOpenP3)
    {
        RSSurfaceNodeConfig surfaceNodeConfig;
        surfaceNodeConfig.isSync = true;
        surfaceNodeConfig.SurfaceNodeName = isOpenP3 ? "P3WindowSurface" : "NotP3WindowSurface";
        auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig, RSSurfaceNodeType::APP_WINDOW_NODE);
        surfaceNode->SetBounds({ 0, 0, screenWidth, screenHeight });
        surfaceNode->SetFrame({ 0, 0, screenWidth, screenHeight });
        surfaceNode->SetBackgroundColor(0xFFFFFFFF);
        if (isOpenP3) {
            surfaceNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
        }
        GetRootNode()->SetTestSurface(surfaceNode);
    }

    Drawing::Color4f CreateDrawingColor4f(Drawing::ColorQuad colorQuad)
    {
        Drawing::Color color(colorQuad);
        return color.GetColor4f();
    }

    std::string GetColorSpaceName(GraphicCM_ColorSpaceType colorSpace)
    {
        const static std::map<GraphicCM_ColorSpaceType, std::string> colorSpaceNameMap = {
            { GRAPHIC_CM_COLORSPACE_NONE, "GRAPHIC_CM_COLORSPACE_NONE" },
            { GRAPHIC_CM_BT601_EBU_FULL, "GRAPHIC_CM_BT601_EBU_FULL" },
            { GRAPHIC_CM_BT601_SMPTE_C_FULL, "GRAPHIC_CM_BT601_SMPTE_C_FULL" },
            { GRAPHIC_CM_BT709_FULL, "GRAPHIC_CM_BT709_FULL" },
            { GRAPHIC_CM_BT2020_HLG_FULL, "GRAPHIC_CM_BT2020_HLG_FULL" },
            { GRAPHIC_CM_BT2020_PQ_FULL, "GRAPHIC_CM_BT2020_PQ_FULL" },
            { GRAPHIC_CM_BT601_EBU_LIMIT, "GRAPHIC_CM_BT601_EBU_LIMIT" },
            { GRAPHIC_CM_BT601_SMPTE_C_LIMIT, "GRAPHIC_CM_BT601_SMPTE_C_LIMIT" },
            { GRAPHIC_CM_P3_FULL, "GRAPHIC_CM_P3_FULL" }, { GRAPHIC_CM_SRGB_FULL, "GRAPHIC_CM_SRGB_FULL" },
            { GRAPHIC_CM_P3_HLG_FULL, "GRAPHIC_CM_P3_HLG_FULL" }, { GRAPHIC_CM_P3_PQ_FULL, "GRAPHIC_CM_P3_PQ_FULL" },
            { GRAPHIC_CM_BT709_LIMIT, "GRAPHIC_CM_BT709_LIMIT" },
            { GRAPHIC_CM_BT2020_HLG_LIMIT, "GRAPHIC_CM_BT2020_HLG_LIMIT" },
            { GRAPHIC_CM_BT2020_PQ_LIMIT, "GRAPHIC_CM_BT2020_PQ_LIMIT" },
            { GRAPHIC_CM_ADOBERGB_FULL, "GRAPHIC_CM_ADOBERGB_FULL" },
            { GRAPHIC_CM_SRGB_LIMIT, "GRAPHIC_CM_SRGB_LIMIT" }, { GRAPHIC_CM_P3_LIMIT, "GRAPHIC_CM_P3_LIMIT" },
            { GRAPHIC_CM_P3_HLG_LIMIT, "GRAPHIC_CM_P3_HLG_LIMIT" }
        };
        auto iter = colorSpaceNameMap.find(colorSpace);
        return iter != colorSpaceNameMap.end() ? iter->second : "UNKNOWN_COLOR_SPACE";
    }
};

/*
 * @tc.name: ColorSpace_ImageInsRGBWindow_Test_01
 * @tc.desc: Test srgb image
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ImageInsRGBWindow_Test_01)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    auto totalSize = columnCount * rowCount;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "sRGBWindowSurface";
    auto sRGBWindowSurface = RSSurfaceNode::Create(surfaceNodeConfig, RSSurfaceNodeType::APP_WINDOW_NODE);
    sRGBWindowSurface->SetBounds({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetFrame({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetBackgroundColor(0xffffffff);
    RegisterNode(sRGBWindowSurface);
    GetRootNode()->AddChild(sRGBWindowSurface);
    sRGBWindowSurface->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    for (int i = 0; i < static_cast<int>(pathList.size()) && i < totalSize; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround, -1);
        RegisterNode(testNodeBackGround);
    }
}

/*
 * @tc.name: ColorSpace_ImageInsRGBWindow_Test_02
 * @tc.desc: Test rgb image
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ImageInsRGBWindow_Test_02)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    auto totalSize = columnCount * rowCount;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "sRGBWindowSurface";
    auto sRGBWindowSurface = RSSurfaceNode::Create(surfaceNodeConfig, RSSurfaceNodeType::APP_WINDOW_NODE);
    sRGBWindowSurface->SetBounds({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetFrame({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetBackgroundColor(0xffffffff);
    RegisterNode(sRGBWindowSurface);
    GetRootNode()->AddChild(sRGBWindowSurface);
    sRGBWindowSurface->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    for (int i = 0; i < static_cast<int>(pathList.size()) && i < totalSize; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround, -1);
        RegisterNode(testNodeBackGround);
    }
}

/*
 * @tc.name: ColorSpace_ImageInsRGBWindow_Test_03
 * @tc.desc: Test p3 image
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ImageInsRGBWindow_Test_03)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    auto totalSize = columnCount * rowCount;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "sRGBWindowSurface";
    auto sRGBWindowSurface = RSSurfaceNode::Create(surfaceNodeConfig, RSSurfaceNodeType::APP_WINDOW_NODE);
    sRGBWindowSurface->SetBounds({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetFrame({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetBackgroundColor(0xffffffff);
    RegisterNode(sRGBWindowSurface);
    GetRootNode()->AddChild(sRGBWindowSurface);
    sRGBWindowSurface->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    for (int i = 0; i < static_cast<int>(pathList.size()) && i < totalSize; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround, -1);
        RegisterNode(testNodeBackGround);
    }
}

/*
 * @tc.name: ColorSpace_ImageInP3Window_Test_01
 * @tc.desc: Test p3 color space
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ImageInP3Window_Test_01)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    auto totalSize = columnCount * rowCount;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "sRGBWindowSurface";
    auto sRGBWindowSurface = RSSurfaceNode::Create(surfaceNodeConfig, RSSurfaceNodeType::APP_WINDOW_NODE);
    sRGBWindowSurface->SetBounds({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetFrame({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetBackgroundColor(0xffffffff);
    RegisterNode(sRGBWindowSurface);
    GetRootNode()->AddChild(sRGBWindowSurface);
    sRGBWindowSurface->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    for (int i = 0; i < static_cast<int>(pathList.size()) && i < totalSize; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround, -1);
        RegisterNode(testNodeBackGround);
    }
}

/*
 * @tc.name: ColorSpace_ImageInP3Window_Test_02
 * @tc.desc: Test p3 color space
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ImageInP3Window_Test_02)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    auto totalSize = columnCount * rowCount;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "sRGBWindowSurface";
    auto sRGBWindowSurface = RSSurfaceNode::Create(surfaceNodeConfig, RSSurfaceNodeType::APP_WINDOW_NODE);
    sRGBWindowSurface->SetBounds({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetFrame({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetBackgroundColor(0xffffffff);
    RegisterNode(sRGBWindowSurface);
    GetRootNode()->AddChild(sRGBWindowSurface);
    sRGBWindowSurface->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    for (int i = 0; i < static_cast<int>(pathList.size()) && i < totalSize; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround, -1);
        RegisterNode(testNodeBackGround);
    }
}

/*
 * @tc.name: ColorSpace_ImageInP3Window_Test_03
 * @tc.desc: Test p3 color space
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ImageInP3Window_Test_03)
{
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    auto totalSize = columnCount * rowCount;

    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.isSync = true;
    surfaceNodeConfig.SurfaceNodeName = "sRGBWindowSurface";
    auto sRGBWindowSurface = RSSurfaceNode::Create(surfaceNodeConfig, RSSurfaceNodeType::APP_WINDOW_NODE);
    sRGBWindowSurface->SetBounds({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetFrame({ 0, 0, 100, 100 });
    sRGBWindowSurface->SetBackgroundColor(0xffffffff);
    RegisterNode(sRGBWindowSurface);
    GetRootNode()->AddChild(sRGBWindowSurface);
    sRGBWindowSurface->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    for (int i = 0; i < static_cast<int>(pathList.size()) && i < totalSize; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNodeBackGround = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNodeBackGround->SetBorderStyle(0, 0, 0, 0);
        testNodeBackGround->SetBorderWidth(5, 5, 5, 5);
        testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNodeBackGround, -1);
        RegisterNode(testNodeBackGround);
    }
}

/*
 * @tc.name: ColorSpace_SetAndGetScreenColoorSpace_Test_01
 * @tc.desc: Test GetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetAndGetScreenColoorSpace_Test_01)
{
    std::string testText = "TEST";
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL;
    int ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpace);
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        testText = "SCREEN_NOT_FOUND";
    } else {
        testText = GetColorSpaceName(colorSpace);
    }

    auto testNodeImg = RSCanvasNode::Create();
    testNodeImg->SetBounds({ 0, 400, 800, 400 });
    testNodeImg->SetTranslate(0, 100, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetText(testText);
    testNodeImg->AddModifier(textModifier);
    GetRootNode()->AddChild(testNodeImg);
    RegisterNode(testNodeImg);
}

/*
 * @tc.name: ColorSpace_SetAndGetScreenColoorSpace_Test_02
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetAndGetScreenColoorSpace_Test_02)
{
    std::string testText = "TEST";
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL;
    RSInterfaces::GetInstance().SetScreenColorSpace(0, GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL);
    int ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpace);
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        testText = "SCREEN_NOT_FOUND";
    } else {
        testText = GetColorSpaceName(colorSpace);
    }

    auto testNodeImg = RSCanvasNode::Create();
    testNodeImg->SetBounds({ 0, 400, 800, 400 });
    testNodeImg->SetTranslate(0, 100, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetText(testText);
    testNodeImg->AddModifier(textModifier);
    GetRootNode()->AddChild(testNodeImg);
    RegisterNode(testNodeImg);
}

/*
 * @tc.name: ColorSpace_SetAndGetScreenColoorSpace_Test_03
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetAndGetScreenColoorSpace_Test_03)
{
    std::string testText = "TEST";
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL;
    RSInterfaces::GetInstance().SetScreenColorSpace(0, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL);
    int ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpace);
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        testText = "SCREEN_NOT_FOUND";
    } else {
        testText = GetColorSpaceName(colorSpace);
    }

    auto testNodeImg = RSCanvasNode::Create();
    testNodeImg->SetBounds({ 0, 400, 800, 400 });
    testNodeImg->SetTranslate(0, 100, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetText(testText);
    testNodeImg->AddModifier(textModifier);
    GetRootNode()->AddChild(testNodeImg);
    RegisterNode(testNodeImg);
}

/*
 * @tc.name: ColorSpace_SetAndGetScreenColoorSpace_Test_04
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetAndGetScreenColoorSpace_Test_04)
{
    std::string testText = "TEST";
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL;
    RSInterfaces::GetInstance().SetScreenColorSpace(0, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_HLG_LIMIT);
    int ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpace);
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        testText = "SCREEN_NOT_FOUND";
    } else {
        testText = GetColorSpaceName(colorSpace);
    }

    auto testNodeImg = RSCanvasNode::Create();
    testNodeImg->SetBounds({ 0, 400, 800, 400 });
    testNodeImg->SetTranslate(0, 100, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetText(testText);
    testNodeImg->AddModifier(textModifier);
    GetRootNode()->AddChild(testNodeImg);
    RegisterNode(testNodeImg);
}

/*
 * @tc.name: ColorSpace_SetAndGetScreenColoorSpace_Test_05
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetAndGetScreenColoorSpace_Test_05)
{
    std::string testText = "TEST";
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL;
    RSInterfaces::GetInstance().SetScreenColorSpace(0, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT709_FULL);
    int ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpace);
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        testText = "SCREEN_NOT_FOUND";
    } else {
        testText = GetColorSpaceName(colorSpace);
    }

    auto testNodeImg = RSCanvasNode::Create();
    testNodeImg->SetBounds({ 0, 400, 800, 400 });
    testNodeImg->SetTranslate(0, 100, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetText(testText);
    testNodeImg->AddModifier(textModifier);
    GetRootNode()->AddChild(testNodeImg);
    RegisterNode(testNodeImg);
}

/*
 * @tc.name: ColorSpace_SetAndGetScreenColoorSpace_Test_06
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetAndGetScreenColoorSpace_Test_06)
{
    std::string testText = "TEST";
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL;
    RSInterfaces::GetInstance().SetScreenColorSpace(0, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL);
    int ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpace);
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        testText = "SCREEN_NOT_FOUND";
    } else {
        testText = GetColorSpaceName(colorSpace);
    }

    auto testNodeImg = RSCanvasNode::Create();
    testNodeImg->SetBounds({ 0, 400, 800, 400 });
    testNodeImg->SetTranslate(0, 100, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetText(testText);
    testNodeImg->AddModifier(textModifier);
    GetRootNode()->AddChild(testNodeImg);
    RegisterNode(testNodeImg);
}

/*
 * @tc.name: ColorSpace_SetAndGetScreenColoorSpace_Test_07
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetAndGetScreenColoorSpace_Test_07)
{
    std::string testText = "TEST";
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL;
    RSInterfaces::GetInstance().SetScreenColorSpace(0, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT2020_PQ_FULL);
    int ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpace);
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        testText = "SCREEN_NOT_FOUND";
    } else {
        testText = GetColorSpaceName(colorSpace);
    }

    auto testNodeImg = RSCanvasNode::Create();
    testNodeImg->SetBounds({ 0, 400, 800, 400 });
    testNodeImg->SetTranslate(0, 100, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetText(testText);
    testNodeImg->AddModifier(textModifier);
    GetRootNode()->AddChild(testNodeImg);
    RegisterNode(testNodeImg);
}

/*
 * @tc.name: ColorSpace_SetAndGetScreenColoorSpace_Test_08
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetAndGetScreenColoorSpace_Test_08)
{
    std::string testText = "TEST";
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL;
    RSInterfaces::GetInstance().SetScreenColorSpace(0, GraphicCM_ColorSpaceType::GRAPHIC_CM_ADOBERGB_FULL);
    int ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpace);
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        testText = "SCREEN_NOT_FOUND";
    } else {
        testText = GetColorSpaceName(colorSpace);
    }

    auto testNodeImg = RSCanvasNode::Create();
    testNodeImg->SetBounds({ 0, 400, 800, 400 });
    testNodeImg->SetTranslate(0, 100, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetText(testText);
    testNodeImg->AddModifier(textModifier);
    GetRootNode()->AddChild(testNodeImg);
    RegisterNode(testNodeImg);
}

/*
 * @tc.name: ColorSpace_SetAndGetScreenColoorSpace_Test_09
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetAndGetScreenColoorSpace_Test_09)
{
    std::string testText = "TEST";
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL;
    RSInterfaces::GetInstance().SetScreenColorSpace(0, GraphicCM_ColorSpaceType::GRAPHIC_CM_BT709_LIMIT);
    int ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpace);
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        testText = "SCREEN_NOT_FOUND";
    } else {
        testText = GetColorSpaceName(colorSpace);
    }

    auto testNodeImg = RSCanvasNode::Create();
    testNodeImg->SetBounds({ 0, 400, 800, 400 });
    testNodeImg->SetTranslate(0, 100, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetText(testText);
    testNodeImg->AddModifier(textModifier);
    GetRootNode()->AddChild(testNodeImg);
    RegisterNode(testNodeImg);
}

/*
 * @tc.name: ColorSpace_SetAndGetScreenColoorSpace_Test_10
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetAndGetScreenColoorSpace_Test_10)
{
    std::string testText = "TEST";
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_P3_FULL;
    RSInterfaces::GetInstance().SetScreenColorSpace(0, GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_LIMIT);
    int ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpace);
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        testText = "SCREEN_NOT_FOUND";
    } else {
        testText = GetColorSpaceName(colorSpace);
    }

    auto testNodeImg = RSCanvasNode::Create();
    testNodeImg->SetBounds({ 0, 400, 800, 400 });
    testNodeImg->SetTranslate(0, 100, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetText(testText);
    testNodeImg->AddModifier(textModifier);
    GetRootNode()->AddChild(testNodeImg);
    RegisterNode(testNodeImg);
}

/*
 * @tc.name: ColorSpace_SetWideColorGamut_Test_01
 * @tc.desc: Test SetColorGamut
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetWideColorGamut_Test_01)
{
    CreateTestSurfaceNode(true);
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    int totalSize = columnCount * rowCount;
    for (int i = 0; i < static_cast<int>(pathList.size()) && i < totalSize; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetColorGamut(3); // 3 is DISPLAY_P3
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ColorSpace_SetWideColorGamut_Test_02
 * @tc.desc: Test SetColorGamut
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetWideColorGamut_Test_02)
{
    CreateTestSurfaceNode(true);
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    int totalSize = columnCount * rowCount;
    for (int i = 0; i < static_cast<int>(pathList.size()) && i < totalSize; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetColorGamut(4); // 4 is SRGB
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ColorSpace_SetWideColorGamut_Test_03
 * @tc.desc: Test SetColorGamut
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetWideColorGamut_Test_03)
{
    CreateTestSurfaceNode(false);
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    int totalSize = columnCount * rowCount;
    for (int i = 0; i < static_cast<int>(pathList.size()) && i < totalSize; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetColorGamut(3); // 3 is DISPLAY_P3
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ColorSpace_SetWideColorGamut_Test_04
 * @tc.desc: Test SetColorGamut
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetWideColorGamut_Test_04)
{
    CreateTestSurfaceNode(true);
    int columnCount = 2;
    int rowCount = 2;
    auto sizeX = screenWidth / columnCount;
    auto sizeY = screenHeight / rowCount;
    int totalSize = columnCount * rowCount;
    for (int i = 0; i < static_cast<int>(pathList.size()) && i < totalSize; i++) {
        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;
        auto testNode = SetUpNodeBgImage(pathList[i], { x, y, sizeX - 10, sizeY - 10 });
        testNode->SetColorGamut(4); // 4 is SRGB
        testNode->SetBorderStyle(0, 0, 0, 0);
        testNode->SetBorderWidth(5, 5, 5, 5);
        testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: ColorSpace_SetBackgroundColor_Test_01
 * @tc.desc: Test SetBackgroundColor
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetBackgroundColor_Test_01)
{
    CreateTestSurfaceNode(true);
    auto p3Color = RSColor(0xFF00FFFF);
    p3Color.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundColor(p3Color);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_SetBackgroundColor_Test_02
 * @tc.desc: Test SetBackgroundColor
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetBackgroundColor_Test_02)
{
    CreateTestSurfaceNode(false);
    auto p3Color = RSColor(0xFF00FFFF);
    p3Color.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundColor(p3Color);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_SetBackgroundColor_Test_03
 * @tc.desc: Test SetBackgroundColor
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetBackgroundColor_Test_03)
{
    CreateTestSurfaceNode(true);
    auto srgbColor = RSColor(0xFF00FFFF);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundColor(srgbColor);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_SetBackgroundColor_Test_04
 * @tc.desc: Test SetBackgroundColor
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_SetBackgroundColor_Test_04)
{
    CreateTestSurfaceNode(false);
    auto srgbColor = RSColor(0xFF00FFFF);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundColor(srgbColor);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_ShaderEffect_Test_01
 * @tc.desc: Test ShaderEffect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ShaderEffect_Test_01)
{
    CreateTestSurfaceNode(true);
    auto p3ColorSpace =
        Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
    auto color = CreateDrawingColor4f(Drawing::Color::COLOR_CYAN);
    auto shaderEffect = OHOS::Rosen::Drawing::ShaderEffect::CreateColorSpaceShader(color, p3ColorSpace);
    auto shader = RSShader::CreateRSShader(shaderEffect);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundShader(shader);
    testNode->SetBorderStyle(0);
    testNode->SetBorderWidth(5);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_ShaderEffect_Test_02
 * @tc.desc: Test ShaderEffect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ShaderEffect_Test_02)
{
    CreateTestSurfaceNode(true);
    auto p3ColorSpace =
        Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
    auto shaderEffect = OHOS::Rosen::Drawing::ShaderEffect::CreateLinearGradient({ 10, 10 }, { 100, 100 },
        { CreateDrawingColor4f(Drawing::Color::COLOR_GREEN), CreateDrawingColor4f(Drawing::Color::COLOR_BLUE),
            CreateDrawingColor4f(Drawing::Color::COLOR_RED) },
        p3ColorSpace, { 0.0, 0.5, 1.0 }, Drawing::TileMode::MIRROR);
    auto shader = RSShader::CreateRSShader(shaderEffect);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundShader(shader);
    testNode->SetBorderStyle(0);
    testNode->SetBorderWidth(5);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_ShaderEffect_Test_03
 * @tc.desc: Test ShaderEffect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ShaderEffect_Test_03)
{
    CreateTestSurfaceNode(true);
    auto p3ColorSpace =
        Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
    auto shaderEffect = OHOS::Rosen::Drawing::ShaderEffect::CreateRadialGradient({ 10, 10 }, 100,
        { CreateDrawingColor4f(Drawing::Color::COLOR_GREEN), CreateDrawingColor4f(Drawing::Color::COLOR_BLUE),
            CreateDrawingColor4f(Drawing::Color::COLOR_RED) },
        p3ColorSpace, { 0.0, 0.5, 1.0 }, Drawing::TileMode::MIRROR);
    auto shader = RSShader::CreateRSShader(shaderEffect);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundShader(shader);
    testNode->SetBorderStyle(0);
    testNode->SetBorderWidth(5);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_ShaderEffect_Test_04
 * @tc.desc: Test ShaderEffect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ShaderEffect_Test_04)
{
    CreateTestSurfaceNode(true);
    auto p3ColorSpace =
        Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
    auto shaderEffect = OHOS::Rosen::Drawing::ShaderEffect::CreateTwoPointConical({ 10, 10 }, 100, { 100, 100 }, 100,
        { CreateDrawingColor4f(Drawing::Color::COLOR_GREEN), CreateDrawingColor4f(Drawing::Color::COLOR_BLUE),
            CreateDrawingColor4f(Drawing::Color::COLOR_RED) },
        p3ColorSpace, { 0.0, 0.5, 1.0 }, Drawing::TileMode::MIRROR);
    auto shader = RSShader::CreateRSShader(shaderEffect);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundShader(shader);
    testNode->SetBorderStyle(0);
    testNode->SetBorderWidth(5);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_ShaderEffect_Test_05
 * @tc.desc: Test ShaderEffect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ShaderEffect_Test_05)
{
    CreateTestSurfaceNode(true);
    auto p3ColorSpace =
        Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
    auto shaderEffect = OHOS::Rosen::Drawing::ShaderEffect::CreateSweepGradient({ 10, 10 },
        { CreateDrawingColor4f(Drawing::Color::COLOR_GREEN), CreateDrawingColor4f(Drawing::Color::COLOR_BLUE),
            CreateDrawingColor4f(Drawing::Color::COLOR_RED) },
        p3ColorSpace, { 0.0, 0.5, 1.0 }, Drawing::TileMode::MIRROR, 0, 90);
    auto shader = RSShader::CreateRSShader(shaderEffect);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundShader(shader);
    testNode->SetBorderStyle(0);
    testNode->SetBorderWidth(5);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_ShaderEffect_Test_06
 * @tc.desc: Test ShaderEffect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ShaderEffect_Test_06)
{
    CreateTestSurfaceNode(true);
    auto p3ColorSpace =
        Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
    auto shaderEffect = OHOS::Rosen::Drawing::ShaderEffect::CreateSweepGradient({ 10, 10 },
        { CreateDrawingColor4f(Drawing::Color::COLOR_GREEN), CreateDrawingColor4f(Drawing::Color::COLOR_BLUE),
            CreateDrawingColor4f(Drawing::Color::COLOR_RED) },
        p3ColorSpace, { 0.0, 0.5, 1.0 }, Drawing::TileMode::MIRROR, 90, 180);
    auto shader = RSShader::CreateRSShader(shaderEffect);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundShader(shader);
    testNode->SetBorderStyle(0);
    testNode->SetBorderWidth(5);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_ShaderEffect_Test_07
 * @tc.desc: Test ShaderEffect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_ShaderEffect_Test_07)
{
    CreateTestSurfaceNode(true);
    auto p3ColorSpace =
        Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
    auto shaderEffect = OHOS::Rosen::Drawing::ShaderEffect::CreateSweepGradient({ 10, 10 },
        { CreateDrawingColor4f(Drawing::Color::COLOR_GREEN), CreateDrawingColor4f(Drawing::Color::COLOR_BLUE),
            CreateDrawingColor4f(Drawing::Color::COLOR_RED) },
        p3ColorSpace, { 0.0, 0.5, 1.0 }, Drawing::TileMode::MIRROR, 0, 180);
    auto shader = RSShader::CreateRSShader(shaderEffect);
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetBackgroundShader(shader);
    testNode->SetBorderStyle(0);
    testNode->SetBorderWidth(5);
    testNode->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_Screen_Test_01
 * @tc.desc: Test GetScreenColorSpace
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_Screen_Test_01)
{
    CreateTestSurfaceNode(true);
    GraphicCM_ColorSpaceType colorSpaceType = GraphicCM_ColorSpaceType::GRAPHIC_CM_COLORSPACE_NONE;
    auto ret = RSInterfaces::GetInstance().GetScreenColorSpace(0, colorSpaceType);
    std::string text = "";
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        text = "SCREEN_NOT_FOUND";
    } else {
        text = GetColorSpaceName(colorSpaceType);
    }
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetTranslate(0, 200, 0);
    auto textModifier = std::make_shared<TextCustomModifier>();
    textModifier->SetFontSize(100);
    textModifier->SetPosition({ 0, 100 });
    textModifier->SetText(text);
    testNode->AddModifier(textModifier);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: ColorSpace_Screen_Test_02
 * @tc.desc: Test GetScreenSupportedColorSpaces
 * @tc.type: FUNC
 */
GRAPHIC_TEST(ColorSpaceTest, CONTENT_DISPLAY_TEST, ColorSpace_Screen_Test_02)
{
    CreateTestSurfaceNode(true);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces = {};
    auto ret = RSInterfaces::GetInstance().GetScreenSupportedColorSpaces(0, colorSpaces);
    std::string errorMsg = "";
    if (ret == StatusCode::SCREEN_NOT_FOUND) {
        errorMsg = "SCREEN_NOT_FOUND";
    } else if (colorSpaces.size() == 0) {
        errorMsg = "NOT_HAS_SUPPORTED_COLOR_SPACE";
    }
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
    testNode->SetFrame({ 0, 0, screenWidth, screenHeight });
    testNode->SetTranslate(0, 200, 0);
    int fontSize = 100;
    if (!errorMsg.empty()) {
        auto textModifier = std::make_shared<TextCustomModifier>();
        textModifier->SetFontSize(fontSize);
        textModifier->SetPosition({ 0, fontSize });
        textModifier->SetText(errorMsg);
        testNode->AddModifier(textModifier);
    } else {
        for (int i = 0; i < static_cast<int>(colorSpaces.size()); i++) {
            auto textModifier = std::make_shared<TextCustomModifier>();
            textModifier->SetFontSize(fontSize);
            textModifier->SetPosition({ 0, (i + 1) * fontSize });
            textModifier->SetText(GetColorSpaceName(colorSpaces[i]));
            testNode->AddModifier(textModifier);
        }
    }
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}
} // namespace OHOS::Rosen
