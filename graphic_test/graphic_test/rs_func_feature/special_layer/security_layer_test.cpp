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
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t COLOR_YELLOW = 0xFFFFFF00;
constexpr uint32_t COLOR_BLUE = 0xFF0000FF;
constexpr uint32_t COLOR_RED = 0xFFFF0000;
Vector4f DEFAULT_RECT1 = {100, 100, 500, 400};
Vector4f DEFAULT_RECT2 = {150, 150, 500, 400};

class SecurityLayerTest : public RSGraphicTest  {
public:
    // called before each tests
    void BeforeEach() override
    {
        auto size = GetScreenSize();
        SetSurfaceBounds({0, 0, size.x_, size.y_});
        SetSurfaceColor(RSColor(COLOR_YELLOW));
    }
};
} //namespace

/*
 * @tc.name: Set_Security_Layer_Test01
 * @tc.desc: test set security layer func
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
 * @tc.desc: test set security layer func
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
 * @tc.desc: test set security layer func
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
    skipConfig.SurfaceNodeName = "TestSecuritySurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> skipSurfaceNode = RSSurfaceNode::Create(skipConfig);
    skipSurfaceNode->SetBounds(DEFAULT_RECT2);
    skipSurfaceNode->SetBackgroundColor(COLOR_RED);
    skipSurfaceNode->SetSkipLayer(true);
    GetRootNode()->AddChild(skipSurfaceNode);
    RegisterNode(skipSurfaceNode);
}

/*
 * @tc.name: Set_Security_Layer_Test04
 * @tc.desc: test set security layer func
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
    skipConfig.SurfaceNodeName = "TestSecuritySurface01";
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> skipSurfaceNode = RSSurfaceNode::Create(skipConfig);
    skipSurfaceNode->SetBounds(DEFAULT_RECT2);
    skipSurfaceNode->SetBackgroundColor(COLOR_RED);
    skipSurfaceNode->SetSnapshotSkipLayer(true);
    GetRootNode()->AddChild(skipSurfaceNode);
    RegisterNode(skipSurfaceNode);
}
}