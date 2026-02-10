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
#include "render_service_client/core/ui/rs_canvas_callback_router.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class CanvasCallbackRouterTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

/*
 * @tc.name: CanvasCallbackRouterTest_001
 * @tc.desc: Test GetInstance and RegisterNode
 * @tc.type: FUNC
 * @tc.require: issueI7N7L6
 */
GRAPHIC_TEST(CanvasCallbackRouterTest, CONTENT_DISPLAY_TEST, CanvasCallbackRouterTest_001)
{
    auto router = RSCanvasCallbackRouter::GetInstance();
    ASSERT_NE(router, nullptr);

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Register node with callback router
    router->RegisterNode(testNode->GetId(), testNode);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: CanvasCallbackRouterTest_002
 * @tc.desc: Test UnregisterNode
 * @tc.type: FUNC
 * @tc.require: issueI7N7L6
 */
GRAPHIC_TEST(CanvasCallbackRouterTest, CONTENT_DISPLAY_TEST, CanvasCallbackRouterTest_002)
{
    auto router = RSCanvasCallbackRouter::GetInstance();

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Register node
    router->RegisterNode(testNode->GetId(), testNode);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Unregister node
    router->UnregisterNode(testNode->GetId());
}

/*
 * @tc.name: CanvasCallbackRouterTest_003
 * @tc.desc: Test RegisterNode multiple nodes (3x4 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7L6
 */
GRAPHIC_TEST(CanvasCallbackRouterTest, CONTENT_DISPLAY_TEST, CanvasCallbackRouterTest_003)
{
    auto router = RSCanvasCallbackRouter::GetInstance();

    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaList[col]);

            // Register each node
            router->RegisterNode(testNode->GetId(), testNode);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: CanvasCallbackRouterTest_004
 * @tc.desc: Test RegisterNode with different node types
 * @tc.type: FUNC
 * @tc.require: issueI7N7L6
 */
GRAPHIC_TEST(CanvasCallbackRouterTest, CONTENT_DISPLAY_TEST, CanvasCallbackRouterTest_004)
{
    auto router = RSCanvasCallbackRouter::GetInstance();

    // CanvasNode
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 50, 50, 300, 300 });
    canvasNode->SetBackgroundColor(0xffff0000);
    router->RegisterNode(canvasNode->GetId(), canvasNode);
    GetRootNode()->AddChild(canvasNode);
    RegisterNode(canvasNode);

    // SurfaceNode
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 400, 50, 300, 300 });
    router->RegisterNode(surfaceNode->GetId(), surfaceNode);
    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);

    // DisplayNode
    auto displayNode = RSDisplayNode::Create();
    displayNode->SetBounds({ 750, 50, 300, 300 });
    router->RegisterNode(displayNode->GetId(), displayNode);
    GetRootNode()->AddChild(displayNode);
    RegisterNode(displayNode);
}

/*
 * @tc.name: CanvasCallbackRouterTest_005
 * @tc.desc: Test RouteToNode
 * @tc.type: FUNC
 * @tc.require: issueI7N7L6
 */
GRAPHIC_TEST(CanvasCallbackRouterTest, CONTENT_DISPLAY_TEST, CanvasCallbackRouterTest_005)
{
    auto router = RSCanvasCallbackRouter::GetInstance();

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    router->RegisterNode(testNode->GetId(), testNode);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Route to node
    router->RouteToNode(testNode->GetId());
}

/*
 * @tc.name: CanvasCallbackRouterTest_006
 * @tc.desc: Test Register/Unregister/Register cycle
 * @tc.type: FUNC
 * @tc.require: issueI7N7L6
 */
GRAPHIC_TEST(CanvasCallbackRouterTest, CONTENT_DISPLAY_TEST, CanvasCallbackRouterTest_006)
{
    auto router = RSCanvasCallbackRouter::GetInstance();

    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    // Register
    router->RegisterNode(testNode->GetId(), testNode);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);

    // Unregister
    router->UnregisterNode(testNode->GetId());

    // Register again
    router->RegisterNode(testNode->GetId(), testNode);

    // Unregister again
    router->UnregisterNode(testNode->GetId());
}

/*
 * @tc.name: CanvasCallbackRouterTest_007
 * @tc.desc: Test RegisterNode with rotation (3x3 matrix)
 * @tc.type: FUNC
 * @tc.require: issueI7N7L6
 */
GRAPHIC_TEST(CanvasCallbackRouterTest, CONTENT_DISPLAY_TEST, CanvasCallbackRouterTest_007)
{
    auto router = RSCanvasCallbackRouter::GetInstance();

    std::vector<float> rotationList = { 0, 45, 90 };

    for (size_t row = 0; row < 3; row++) {
        for (size_t col = 0; col < rotationList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 380 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetRotation(rotationList[col]);

            router->RegisterNode(testNode->GetId(), testNode);

            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: CanvasCallbackRouterTest_008
 * @tc.desc: Test RegisterNode with parent-child hierarchy
 * @tc.type: FUNC
 * @tc.require: issueI7N7L6
 */
GRAPHIC_TEST(CanvasCallbackRouterTest, CONTENT_DISPLAY_TEST, CanvasCallbackRouterTest_008)
{
    auto router = RSCanvasCallbackRouter::GetInstance();

    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 600, 600 });
    parent->SetBackgroundColor(0xffff0000);

    router->RegisterNode(parent->GetId(), parent);

    // Add children
    for (int i = 0; i < 4; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + (i % 2) * 300, 50 + (i / 2) * 300, 200, 200 });
        child->SetBackgroundColor(0xff00ff00);

        router->RegisterNode(child->GetId(), child);

        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
}

/*
 * @tc.name: CanvasCallbackRouterTest_009
 * @tc.desc: Test RegisterNode with scale
 * @tc.type: FUNC
 * @tc.require: issueI7N7L6
 */
GRAPHIC_TEST(CanvasCallbackRouterTest, CONTENT_DISPLAY_TEST, CanvasCallbackRouterTest_009)
{
    auto router = RSCanvasCallbackRouter::GetInstance();

    std::vector<std::pair<float, float>> scaleList = {
        { 0.5f, 0.5f },
        { 1.0f, 1.0f },
        { 1.5f, 1.5f }
    };

    for (size_t i = 0; i < scaleList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 300, 300 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetScale(scaleList[i].first, scaleList[i].second);

        router->RegisterNode(testNode->GetId(), testNode);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: CanvasCallbackRouterTest_010
 * @tc.desc: Test RouteToNode with multiple nodes
 * @tc.type: FUNC
 * @tc.require: issueI7N7L6
 */
GRAPHIC_TEST(CanvasCallbackRouterTest, CONTENT_DISPLAY_TEST, CanvasCallbackRouterTest_010)
{
    auto router = RSCanvasCallbackRouter::GetInstance();

    std::vector<NodeId> nodeIds;

    for (int i = 0; i < 5; i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        testNode->SetBackgroundColor(0xffff0000 - i * 0x00200000);

        router->RegisterNode(testNode->GetId(), testNode);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);

        nodeIds.push_back(testNode->GetId());
    }

    // Route to each node
    for (auto nodeId : nodeIds) {
        router->RouteToNode(nodeId);
    }
}

} // namespace OHOS::Rosen
