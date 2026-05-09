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
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui/rs_ui_context_manager.h"

namespace OHOS::Rosen {
class GeometryTransitionTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
};

static std::shared_ptr<RSUIContext> GetRSUIContext()
{
    static std::shared_ptr<RSUIContext> rsUIContext = nullptr;
    if (rsUIContext == nullptr) {
        OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
        rsUIContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    }
    return rsUIContext;
}

// sharedTransition baseline
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, WO_Set_Transition_Pair_Test)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(highLevelNode);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    RegisterNode(idleNode);
    GetRootNode()->AddChild(rootNode);
}

// direct_low, direct_high
// in_same_window
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Transition_Pair_Same_Window_Test_1)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), lowLevelNode->GetId(), highLevelNode->GetId(), true);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    RegisterNode(idleNode);
    GetRootNode()->AddChild(rootNode);
}

// direct_low, no_direct_high
// in_same_window
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Transition_Pair_Same_Window_Test_2)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto highLevelParentNode = RSCanvasNode::Create();

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(highLevelParentNode);
    highLevelParentNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), lowLevelNode->GetId(), highLevelNode->GetId(), true);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    RegisterNode(highLevelParentNode);
    RegisterNode(idleNode);
    GetRootNode()->AddChild(rootNode);
}

// no_direct_low, direct_high
// in_same_window
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Transition_Pair_Same_Window_Test_3)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto lowLevelParentNode = RSCanvasNode::Create();

    rootNode->AddChild(lowLevelParentNode);
    lowLevelParentNode->AddChild(lowLevelNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), lowLevelNode->GetId(), highLevelNode->GetId(), true);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    RegisterNode(lowLevelParentNode);
    RegisterNode(idleNode);
    GetRootNode()->AddChild(rootNode);
}

// no_direct_low, no_direct_high
// in_same_window
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Transition_Pair_Same_Window_Test_4)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto lowLevelParentNode = RSCanvasNode::Create();
    auto highLevelParentNode = RSCanvasNode::Create();

    rootNode->AddChild(lowLevelParentNode);
    lowLevelParentNode->AddChild(lowLevelNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(highLevelParentNode);
    highLevelParentNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), lowLevelNode->GetId(), highLevelNode->GetId(), true);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    RegisterNode(lowLevelParentNode);
    RegisterNode(highLevelParentNode);
    RegisterNode(idleNode);
    GetRootNode()->AddChild(rootNode);
}

// direct_low, direct_high
// in_different_window
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Transition_Pair_Different_Window_Test_1)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), lowLevelNode->GetId(), highLevelNode->GetId(), false);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    RegisterNode(idleNode);
    GetRootNode()->AddChild(rootNode);
}

// direct_low, no_direct_high
// in_different_window
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Transition_Pair_Different_Window_Test_2)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto highLevelParentNode = RSCanvasNode::Create();

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(highLevelParentNode);
    highLevelParentNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), lowLevelNode->GetId(), highLevelNode->GetId(), false);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    RegisterNode(highLevelParentNode);
    RegisterNode(idleNode);
    GetRootNode()->AddChild(rootNode);
}

// no_direct_low, direct_high
// in_different_window
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Transition_Pair_Different_Window_Test_3)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto lowLevelParentNode = RSCanvasNode::Create();

    rootNode->AddChild(lowLevelParentNode);
    lowLevelParentNode->AddChild(lowLevelNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), lowLevelNode->GetId(), highLevelNode->GetId(), false);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    RegisterNode(lowLevelParentNode);
    RegisterNode(idleNode);
    GetRootNode()->AddChild(rootNode);
}

// no_direct_low, no_direct_high
// in_different_window
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Transition_Pair_Different_Window_Test_4)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto lowLevelParentNode = RSCanvasNode::Create();
    auto highLevelParentNode = RSCanvasNode::Create();

    rootNode->AddChild(lowLevelParentNode);
    lowLevelParentNode->AddChild(lowLevelNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(highLevelParentNode);
    highLevelParentNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), lowLevelNode->GetId(), highLevelNode->GetId(), false);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    RegisterNode(lowLevelParentNode);
    RegisterNode(highLevelParentNode);
    RegisterNode(idleNode);
    GetRootNode()->AddChild(rootNode);
}

// sandbox
// baseline: without set sandbox
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, WO_Set_Sandbox_Test)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000); // red
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF); // blue

    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}

// set sandbox nullptr
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Nullptr_Test)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000); // red
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF); // blue

    childNode->SetSandBox(std::nullopt);

    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}

// sandbox relative to surfaceNode 0, 0
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_0_0_Test)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000); // red
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF); // blue

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(0, 0)));

    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}

// sandbox relative to surfaceNode 100, 100
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_100_100_Test)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000); // red
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF); // blue

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(100, 100)));

    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}

// sandbox relative to surfaceNode 200, 200
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_200_200_Test)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000); // red
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF); // blue

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(200, 200)));

    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}

// sandbox relative to surfaceNode -100, -100
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Negative_Test)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000);
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF);

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(-100, -100)));

    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}

// SetSandbox - Multiple nodes with different sandbox offsets
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Multiple_Nodes_Test)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000);
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);

    auto childNode1 = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild1{50, 50, 150, 150};
    childNode1->SetBounds(boundsChild1);
    childNode1->SetFrame(boundsChild1);
    childNode1->SetBackgroundColor(0xFF0000FF);

    auto childNode2 = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild2{250, 50, 150, 150};
    childNode2->SetBounds(boundsChild2);
    childNode2->SetFrame(boundsChild2);
    childNode2->SetBackgroundColor(0xFF00FF00);

    auto childNode3 = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild3{150, 250, 150, 150};
    childNode3->SetBounds(boundsChild3);
    childNode3->SetFrame(boundsChild3);
    childNode3->SetBackgroundColor(0xFFFFFFFF);

    childNode1->SetSandBox(std::make_optional(Rosen::Vector2f(0, 0)));
    childNode2->SetSandBox(std::make_optional(Rosen::Vector2f(150, 50)));
    childNode3->SetSandBox(std::make_optional(Rosen::Vector2f(100, 300)));

    parentNode->AddChild(childNode1);
    parentNode->AddChild(childNode2);
    parentNode->AddChild(childNode3);
    RegisterNode(parentNode);
    RegisterNode(childNode1);
    RegisterNode(childNode2);
    RegisterNode(childNode3);
    GetRootNode()->AddChild(parentNode);
}

// Sandbox node has foreground filter (offscreen)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Foreground_Filter_Test)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000);
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);

    auto bezierWarpFilter = std::make_shared<RSNGBezierWarpFilter>();
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f(0.1f, 0.1f));
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f(0.2f, 0.15f));
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f(0.4f, 0.15f));
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f(0.5f, 0.1f));
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f(0.1f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f(0.3f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f(0.45f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f(0.5f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f(0.6f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f(0.75f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f(0.85f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f(0.9f, 0.9f));

    auto childNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF);

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(100, 100)));
    childNode->SetForegroundNGFilter(bezierWarpFilter);

    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}

// Sandbox's parent node has foreground filter (offscreen)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Parent_Foreground_Filter_Test)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000);
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);

    auto bezierWarpFilter = std::make_shared<RSNGBezierWarpFilter>();
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f(0.1f, 0.1f));
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f(0.2f, 0.15f));
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f(0.4f, 0.15f));
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f(0.5f, 0.1f));
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f(0.1f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f(0.3f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f(0.45f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f(0.5f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f(0.6f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f(0.75f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f(0.85f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f(0.9f, 0.9f));

    parentNode->SetForegroundNGFilter(bezierWarpFilter);

    auto childNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF);

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(100, 100)));

    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}

// Sandbox's parent node has two foreground filters (offscreen)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Parent_Two_Foreground_Filters_Test)
{
    auto parentNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000);
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);

    auto bezierWarpFilter = std::make_shared<RSNGBezierWarpFilter>();
    bezierWarpFilter->Setter<BezierWarpControlPoint0Tag>(Vector2f(0.1f, 0.1f));
    bezierWarpFilter->Setter<BezierWarpControlPoint1Tag>(Vector2f(0.2f, 0.15f));
    bezierWarpFilter->Setter<BezierWarpControlPoint2Tag>(Vector2f(0.4f, 0.15f));
    bezierWarpFilter->Setter<BezierWarpControlPoint3Tag>(Vector2f(0.5f, 0.1f));
    bezierWarpFilter->Setter<BezierWarpControlPoint4Tag>(Vector2f(0.1f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint5Tag>(Vector2f(0.3f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint6Tag>(Vector2f(0.45f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint7Tag>(Vector2f(0.5f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint8Tag>(Vector2f(0.6f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint9Tag>(Vector2f(0.75f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint10Tag>(Vector2f(0.85f, 0.4f));
    bezierWarpFilter->Setter<BezierWarpControlPoint11Tag>(Vector2f(0.9f, 0.9f));

    parentNode->SetForegroundNGFilter(bezierWarpFilter);

    auto parentNode2 = RSCanvasNode::Create();
    Rosen::Vector4f bounds2{100, 100, 500, 500};
    parentNode2->SetBackgroundColor(0xFF00FF00);
    parentNode2->SetBounds(bounds2);
    parentNode2->SetFrame(bounds2);
    parentNode2->SetForegroundNGFilter(bezierWarpFilter);

    auto childNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF);

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(100, 100)));

    parentNode->AddChild(parentNode2);
    parentNode2->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(parentNode2);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}
} // namespace OHOS::Rosen