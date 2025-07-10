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
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
constexpr uint32_t COLOR_GREEN = 0xFFFF00FF;
constexpr uint32_t FORTY = 40;
constexpr uint32_t FIFTY = 50;
Vector4f DEFAULT_RECT1 = {100, 100, 500, 400};
Vector4f DEFAULT_RECT2 = {150, 150, 500, 400};
Vector4f IMAGE_BOUNDS = {100, 100, 500, 500};
Vector4f IMAGE_RECT = {100, 100, -200, -200};
Vector4f BORDER_WIDTH = {5, 5, 5, 5};

class SecurityLayerTest : public RSGraphicTest  {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
        SetSurfaceColor(RSColor(COLOR_YELLOW));
    }
};
} //namespace

/*
 * @tc.name: Set_Security_Layer_Test01
 * @tc.desc: test set security layer true
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SecurityLayerTest, CONTENT_DISPLAY_TEST, Set_Security_Layer_Test01)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSecuritySurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> securitySurfaceNode = RSSurfaceNode::Create(config);
    securitySurfaceNode->SetBounds(DEFAULT_RECT1);
    securitySurfaceNode->SetBackgroundColor(COLOR_BLUE);
    securitySurfaceNode->SetSecurityLayer(true);

    GetRootNode()->AddChild(securitySurfaceNode);
    RegisterNode(securitySurfaceNode);
}

/*
 * @tc.name: Set_Security_Layer_Test02
 * @tc.desc: test set security layer false
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SecurityLayerTest, CONTENT_DISPLAY_TEST, Set_Security_Layer_Test02)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "TestSecuritySurface02";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> securitySurfaceNode = RSSurfaceNode::Create(config);
    securitySurfaceNode->SetBounds(DEFAULT_RECT1);
    securitySurfaceNode->SetBackgroundColor(COLOR_BLUE);
    securitySurfaceNode->SetSecurityLayer(false);

    GetRootNode()->AddChild(securitySurfaceNode);
    RegisterNode(securitySurfaceNode);
}

/*
 * @tc.name: Set_Security_Layer_Test03
 * @tc.desc: Test the effect after overcovering the layer with the Skip attribute on the layer with
 *           the Security attribute set
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SecurityLayerTest, CONTENT_DISPLAY_TEST, Set_Security_Layer_Test03)
{
    RSSurfaceNodeConfig securityConfig;
    securityConfig.SurfaceNodeName = "TestSecuritySurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> securitySurfaceNode = RSSurfaceNode::Create(securityConfig);
    securitySurfaceNode->SetBounds(DEFAULT_RECT1);
    securitySurfaceNode->SetBackgroundColor(COLOR_BLUE);
    securitySurfaceNode->SetSecurityLayer(true);
    GetRootNode()->AddChild(securitySurfaceNode);
    RegisterNode(securitySurfaceNode);

    RSSurfaceNodeConfig skipConfig;
    skipConfig.SurfaceNodeName = "TestSkipSurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> skipSurfaceNode = RSSurfaceNode::Create(skipConfig);
    skipSurfaceNode->SetBounds(DEFAULT_RECT2);
    skipSurfaceNode->SetBackgroundColor(COLOR_RED);
    skipSurfaceNode->SetSkipLayer(true);
    GetRootNode()->AddChild(skipSurfaceNode);
    RegisterNode(skipSurfaceNode);
}

/*
 * @tc.name: Set_Security_Layer_Test04
 * @tc.desc: Test the effect after covering the layer with the SnapshotSkip property on top of the layer with
 *           the Security property set
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SecurityLayerTest, CONTENT_DISPLAY_TEST, Set_Security_Layer_Test04)
{
    RSSurfaceNodeConfig securityConfig;
    securityConfig.SurfaceNodeName = "TestSecuritySurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> securitySurfaceNode = RSSurfaceNode::Create(securityConfig);
    securitySurfaceNode->SetBounds(DEFAULT_RECT1);
    securitySurfaceNode->SetBackgroundColor(COLOR_BLUE);
    securitySurfaceNode->SetSecurityLayer(true);
    GetRootNode()->AddChild(securitySurfaceNode);
    RegisterNode(securitySurfaceNode);

    RSSurfaceNodeConfig skipConfig;
    skipConfig.SurfaceNodeName = "TestSkipSurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> skipSurfaceNode = RSSurfaceNode::Create(skipConfig);
    skipSurfaceNode->SetBounds(DEFAULT_RECT2);
    skipSurfaceNode->SetBackgroundColor(COLOR_RED);
    skipSurfaceNode->SetSnapshotSkipLayer(true);
    GetRootNode()->AddChild(skipSurfaceNode);
    RegisterNode(skipSurfaceNode);
}

/*
 * @tc.name: Set_Security_Layer_Test05
 * @tc.desc: Test the effect after covering the shaded Security layer with a shaded SnapshotSkip layer
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SecurityLayerTest, CONTENT_DISPLAY_TEST, Set_Security_Layer_Test05)
{
    RSSurfaceNodeConfig securityConfig;
    securityConfig.SurfaceNodeName = "TestSecuritySurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> securitySurfaceNode = RSSurfaceNode::Create(securityConfig);
    securitySurfaceNode->SetBounds(DEFAULT_RECT1);
    securitySurfaceNode->SetShadowColor(COLOR_BLUE);
    securitySurfaceNode->SetShadowRadius(FIFTY);
    securitySurfaceNode->SetBackgroundColor(COLOR_BLUE);
    securitySurfaceNode->SetSecurityLayer(true);
    GetRootNode()->AddChild(securitySurfaceNode);
    RegisterNode(securitySurfaceNode);

    RSSurfaceNodeConfig skipConfig;
    skipConfig.SurfaceNodeName = "TestSkipSurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> skipSurfaceNode = RSSurfaceNode::Create(skipConfig);
    skipSurfaceNode->SetBounds(DEFAULT_RECT2);
    skipSurfaceNode->SetShadowColor(COLOR_BLUE);
    skipSurfaceNode->SetShadowRadius(FIFTY);
    skipSurfaceNode->SetBackgroundColor(COLOR_RED);
    skipSurfaceNode->SetSnapshotSkipLayer(true);
    GetRootNode()->AddChild(skipSurfaceNode);
    RegisterNode(skipSurfaceNode);
}

/*
 * @tc.name: Set_Security_Layer_Test06
 * @tc.desc: Test the effect after covering a shaded non-Security layer with a shaded non-Snapshotskip layer
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SecurityLayerTest, CONTENT_DISPLAY_TEST, Set_Security_Layer_Test06)
{
    RSSurfaceNodeConfig securityConfig;
    securityConfig.SurfaceNodeName = "TestSecuritySurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> securitySurfaceNode = RSSurfaceNode::Create(securityConfig);
    securitySurfaceNode->SetBounds(DEFAULT_RECT1);
    securitySurfaceNode->SetShadowColor(COLOR_GREEN);
    securitySurfaceNode->SetShadowRadius(FORTY);
    securitySurfaceNode->SetBackgroundColor(COLOR_BLUE);
    securitySurfaceNode->SetSecurityLayer(false);
    GetRootNode()->AddChild(securitySurfaceNode);
    RegisterNode(securitySurfaceNode);

    RSSurfaceNodeConfig skipConfig;
    skipConfig.SurfaceNodeName = "TestSkipSurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> skipSurfaceNode = RSSurfaceNode::Create(skipConfig);
    skipSurfaceNode->SetBounds(DEFAULT_RECT2);
    skipSurfaceNode->SetShadowColor(COLOR_BLUE);
    skipSurfaceNode->SetShadowRadius(FIFTY);
    skipSurfaceNode->SetBackgroundColor(COLOR_RED);
    skipSurfaceNode->SetSnapshotSkipLayer(false);
    GetRootNode()->AddChild(skipSurfaceNode);
    RegisterNode(skipSurfaceNode);
}

/*
 * @tc.name: Set_Security_Layer_Test07
 * @tc.desc: TEST the securityLayer and its child nodes are images
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SecurityLayerTest, CONTENT_DISPLAY_TEST, Set_Security_Layer_Test07)
{
    RSSurfaceNodeConfig securityConfig;
    securityConfig.SurfaceNodeName = "TestSecuritySurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> securitySurfaceNode = RSSurfaceNode::Create(securityConfig);
    securitySurfaceNode->SetBounds(DEFAULT_RECT1);
    securitySurfaceNode->SetBackgroundColor(COLOR_BLUE);
    securitySurfaceNode->SetSecurityLayer(true);
    GetRootNode()->SetTestSurface(securitySurfaceNode);
    RegisterNode(securitySurfaceNode);

    Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", IMAGE_BOUNDS);
    testNodeBackGround->SetBorderStyle(style);
    testNodeBackGround->SetBorderWidth(BORDER_WIDTH);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageInnerRect(IMAGE_RECT);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}

/*
 * @tc.name: Set_Security_Layer_Test08
 * @tc.desc: Test the non-securityLayer and its child nodes are images
 * @tc.type: FUNC
 * @tc.require: issueICF7KO
 */
GRAPHIC_TEST(SecurityLayerTest, CONTENT_DISPLAY_TEST, Set_Security_Layer_Test08)
{
    RSSurfaceNodeConfig securityConfig;
    securityConfig.SurfaceNodeName = "TestSecuritySurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> securitySurfaceNode = RSSurfaceNode::Create(securityConfig);
    securitySurfaceNode->SetBounds(DEFAULT_RECT1);
    securitySurfaceNode->SetBackgroundColor(COLOR_BLUE);
    securitySurfaceNode->SetSecurityLayer(false);
    GetRootNode()->SetTestSurface(securitySurfaceNode);
    RegisterNode(securitySurfaceNode);

    Vector4<BorderStyle> style = Vector4<BorderStyle>(BorderStyle::SOLID);
    auto testNodeBackGround =
        SetUpNodeBgImage("/data/local/tmp/Images/backGroundImage.jpg", IMAGE_BOUNDS);
    testNodeBackGround->SetBorderStyle(style);
    testNodeBackGround->SetBorderWidth(BORDER_WIDTH);
    testNodeBackGround->SetBorderColor(Vector4<Color>(RgbPalette::Red()));
    testNodeBackGround->SetBgImageInnerRect(IMAGE_RECT);
    GetRootNode()->AddChild(testNodeBackGround);
    RegisterNode(testNodeBackGround);
}
}