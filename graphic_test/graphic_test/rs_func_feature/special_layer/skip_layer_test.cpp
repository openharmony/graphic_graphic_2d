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
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_utils.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
constexpr uint32_t COLOR_GREEN = 0xFFFF00FF;
constexpr uint32_t COLOR_GRAY = 0xFF888888;
constexpr uint32_t FORTY = 40;
constexpr uint32_t FIFTY = 50;
Vector4f DEFAULT_RECT1 = {0, 0, 1000, 1000};
Vector4f DEFAULT_RECT2 = {400, 400, 100, 100};
Vector4f DEFAULT_RECT3 = {0, 0, 200, 200};
Vector4f IMAGE_BOUNDS = {100, 100, 500, 500};
Vector4f IMAGE_RECT = {100, 100, -200, -200};
Vector4f BORDER_WIDTH = {5, 5, 5, 5};

class SkipLayerTest : public RSGraphicTest  {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        SetSurfaceColor(RSColor(COLOR_RED));
    }

    // called after each tests
    void AfterEach() override {}
};
} //namespace

/*
 * @tc.name: Set_Skip_Layer_Test01
 * @tc.desc: test the skipLayer has two child nodes
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SkipLayerTest, CONTENT_DISPLAY_TEST, Set_Skip_Layer_Test01)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config);
    testSurface->SetBounds(DEFAULT_RECT1);
    testSurface->SetBackgroundColor(COLOR_BLUE);
    testSurface->SetSkipLayer(true);
    RegisterNode(testSurface);
    GetRootNode()->SetTestSurface(testSurface);

    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds(DEFAULT_RECT2);
    canvasNode0->SetBackgroundColor(COLOR_YELLOW);
    RegisterNode(canvasNode0);
    GetRootNode()->AddChild(canvasNode0);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds(DEFAULT_RECT3);
    canvasNode1->SetBackgroundColor(COLOR_RED);
    RegisterNode(canvasNode1);
    GetRootNode()->AddChild(canvasNode1);
}

/*
 * @tc.name: Set_Skip_Layer_Test02
 * @tc.desc: test the non-skipLayer has two child nodes
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SkipLayerTest, CONTENT_DISPLAY_TEST, Set_Skip_Layer_Test02)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config);
    testSurface->SetBounds(DEFAULT_RECT1);
    testSurface->SetBackgroundColor(COLOR_BLUE);
    testSurface->SetSkipLayer(false);
    RegisterNode(testSurface);
    GetRootNode()->SetTestSurface(testSurface);

    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds(DEFAULT_RECT2);
    canvasNode0->SetBackgroundColor(COLOR_YELLOW);
    RegisterNode(canvasNode0);
    GetRootNode()->AddChild(canvasNode0);

    auto canvasNode1 = RSCanvasNode::Create();
    canvasNode1->SetBounds(DEFAULT_RECT3);
    canvasNode1->SetBackgroundColor(COLOR_RED);
    RegisterNode(canvasNode1);
    GetRootNode()->AddChild(canvasNode1);
}

/*
 * @tc.name: Set_Skip_Layer_Test03
 * @tc.desc: Test add a shadow to skipLayer
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SkipLayerTest, CONTENT_DISPLAY_TEST, Set_Skip_Layer_Test03)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config);
    testSurface->SetBounds(DEFAULT_RECT1);
    testSurface->SetShadowColor(COLOR_BLUE);
    testSurface->SetShadowRadius(FIFTY);
    testSurface->SetBackgroundColor(COLOR_BLUE);
    testSurface->SetSkipLayer(false);
    RegisterNode(testSurface);
    GetRootNode()->SetTestSurface(testSurface);

    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds(DEFAULT_RECT2);
    canvasNode0->SetBackgroundColor(COLOR_YELLOW);
    RegisterNode(canvasNode0);
    GetRootNode()->AddChild(canvasNode0);
}

/*
 * @tc.name: Set_Skip_Layer_Test04
 * @tc.desc: Test Add a shadow to non-skipLayer
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SkipLayerTest, CONTENT_DISPLAY_TEST, Set_Skip_Layer_Test04)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config);
    testSurface->SetBounds(DEFAULT_RECT1);
    testSurface->SetShadowColor(COLOR_BLUE);
    testSurface->SetShadowRadius(FIFTY);
    testSurface->SetBackgroundColor(COLOR_BLUE);
    testSurface->SetSkipLayer(true);
    RegisterNode(testSurface);
    GetRootNode()->SetTestSurface(testSurface);

    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds(DEFAULT_RECT2);
    canvasNode0->SetBackgroundColor(COLOR_YELLOW);
    RegisterNode(canvasNode0);
    GetRootNode()->AddChild(canvasNode0);
}

/*
 * @tc.name: Set_Skip_Layer_Test05
 * @tc.desc: Test compare skipLayer and non-skiPlayer
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SkipLayerTest, CONTENT_DISPLAY_TEST, Set_Skip_Layer_Test05)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config);
    testSurface->SetBounds(DEFAULT_RECT1);
    testSurface->SetShadowColor(COLOR_BLUE);
    testSurface->SetShadowRadius(FIFTY);
    testSurface->SetBackgroundColor(COLOR_BLUE);
    testSurface->SetSkipLayer(true);
    RegisterNode(testSurface);
    GetRootNode()->SetTestSurface(testSurface);

    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds(DEFAULT_RECT2);
    canvasNode0->SetBackgroundColor(COLOR_YELLOW);
    RegisterNode(canvasNode0);
    GetRootNode()->AddChild(canvasNode0);

    config.SurfaceNodeName = "TestSurface2";
    auto testSurface2 = RSSurfaceNode::Create(config);
    testSurface2->SetBounds(DEFAULT_RECT3);
    testSurface2->SetShadowColor(COLOR_GREEN);
    testSurface2->SetShadowRadius(FIFTY);
    testSurface2->SetBackgroundColor(COLOR_GRAY);
    testSurface2->SetSkipLayer(false);
    RegisterNode(testSurface2);
    GetRootNode()->SetTestSurface(testSurface2);
}

/*
 * @tc.name: Set_Skip_Layer_Test06
 * @tc.desc: Test compare skipLayer and non-skiPlayer
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SkipLayerTest, CONTENT_DISPLAY_TEST, Set_Skip_Layer_Test06)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config);
    testSurface->SetBounds(DEFAULT_RECT1);
    testSurface->SetShadowColor(COLOR_BLUE);
    testSurface->SetShadowRadius(FIFTY);
    testSurface->SetBackgroundColor(COLOR_BLUE);
    testSurface->SetSkipLayer(false);
    RegisterNode(testSurface);
    GetRootNode()->SetTestSurface(testSurface);

    auto canvasNode0 = RSCanvasNode::Create();
    canvasNode0->SetBounds(DEFAULT_RECT2);
    canvasNode0->SetBackgroundColor(COLOR_YELLOW);
    RegisterNode(canvasNode0);
    GetRootNode()->AddChild(canvasNode0);

    config.SurfaceNodeName = "TestSurface2";
    auto testSurface2 = RSSurfaceNode::Create(config);
    testSurface2->SetBounds(DEFAULT_RECT3);
    testSurface2->SetShadowColor(COLOR_GREEN);
    testSurface2->SetShadowRadius(FIFTY);
    testSurface2->SetBackgroundColor(COLOR_GRAY);
    testSurface2->SetSkipLayer(true);
    RegisterNode(testSurface2);
    GetRootNode()->SetTestSurface(testSurface2);
}

/*
 * @tc.name: Set_Skip_Layer_Test07
 * @tc.desc: Test skipLayer whih img child
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SkipLayerTest, CONTENT_DISPLAY_TEST, Set_Skip_Layer_Test07)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config);
    testSurface->SetBounds(DEFAULT_RECT1);
    testSurface->SetCornerRadius(FORTY);
    testSurface->SetBackgroundColor(COLOR_BLUE);
    testSurface->SetSkipLayer(false);
    RegisterNode(testSurface);
    GetRootNode()->SetTestSurface(testSurface);

    Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
    auto canvasNode0 =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", IMAGE_BOUNDS);
    canvasNode0->SetBorderStyle(style);
    canvasNode0->SetBorderWidth(BORDER_WIDTH);
    canvasNode0->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    canvasNode0->SetBgImageInnerRect(IMAGE_RECT);
    RegisterNode(canvasNode0);
    GetRootNode()->AddChild(canvasNode0);
}

/*
 * @tc.name: Set_Skip_Layer_Test08
 * @tc.desc: Test non-skipLayer whih img child
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SkipLayerTest, CONTENT_DISPLAY_TEST, Set_Skip_Layer_Test08)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSurface";
    auto testSurface = RSSurfaceNode::Create(config);
    testSurface->SetBounds(DEFAULT_RECT1);
    testSurface->SetCornerRadius(FORTY);
    testSurface->SetBackgroundColor(COLOR_BLUE);
    testSurface->SetSkipLayer(true);
    RegisterNode(testSurface);
    GetRootNode()->SetTestSurface(testSurface);

    Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
    auto canvasNode0 =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", IMAGE_BOUNDS);
    canvasNode0->SetBorderStyle(style);
    canvasNode0->SetBorderWidth(BORDER_WIDTH);
    canvasNode0->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    canvasNode0->SetBgImageInnerRect(IMAGE_RECT);
    RegisterNode(canvasNode0);
    GetRootNode()->AddChild(canvasNode0);
}
} //namespace