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
// sharedTransition Function
// baseline
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
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Transition_Pair_Test)
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

    RSNode::RegisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId(), true);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    RegisterNode(idleNode);
    GetRootNode()->AddChild(rootNode);
}

// sandbox
// baseline
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

// sandbox
// nullptr
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

// RegisterTransitionPair - Multiple pairs in same window
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Register_Transition_Pair_Multiple_Test)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);

    auto lowLevelNode1 = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow1{100, 300, 200, 200};
    lowLevelNode1->SetBounds(boundsLow1);
    lowLevelNode1->SetFrame(boundsLow1);
    lowLevelNode1->SetBackgroundColor(0xFF0000FF);

    auto highLevelNode1 = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh1{100, 600, 200, 200};
    highLevelNode1->SetBounds(boundsHigh1);
    highLevelNode1->SetFrame(boundsHigh1);
    highLevelNode1->SetBackgroundColor(0xFF00FF00);

    auto lowLevelNode2 = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow2{400, 300, 200, 200};
    lowLevelNode2->SetBounds(boundsLow2);
    lowLevelNode2->SetFrame(boundsLow2);
    lowLevelNode2->SetBackgroundColor(0xFFFF0000);

    auto highLevelNode2 = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh2{400, 600, 200, 200};
    highLevelNode2->SetBounds(boundsHigh2);
    highLevelNode2->SetFrame(boundsHigh2);
    highLevelNode2->SetBackgroundColor(0xFFFFFF00);

    rootNode->AddChild(lowLevelNode1);
    rootNode->AddChild(highLevelNode1);
    rootNode->AddChild(lowLevelNode2);
    rootNode->AddChild(highLevelNode2);

    RSNode::RegisterTransitionPair(lowLevelNode1->GetId(), highLevelNode1->GetId(), true);
    RSNode::RegisterTransitionPair(lowLevelNode2->GetId(), highLevelNode2->GetId(), false);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode1);
    RegisterNode(highLevelNode1);
    RegisterNode(lowLevelNode2);
    RegisterNode(highLevelNode2);
    GetRootNode()->AddChild(rootNode);
}

// RegisterTransitionPair - Register same pair twice
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Register_Transition_Pair_Duplicate_Test)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);

    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF);

    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00);

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId(), true);
    RSNode::RegisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId(), true);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    GetRootNode()->AddChild(rootNode);
}

// RegisterTransitionPair - Register with different flags
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Register_Transition_Pair_Flag_True_Test)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);

    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF);

    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00);

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId(), true);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    GetRootNode()->AddChild(rootNode);
}

GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Register_Transition_Pair_Flag_False_Test)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);

    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF);

    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00);

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId(), false);

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    GetRootNode()->AddChild(rootNode);
}

// UnregisterTransitionPair - Basic unregister
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Unregister_Transition_Pair_Basic_Test)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);

    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF);

    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00);

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId(), true);
    RSNode::UnregisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId());

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    GetRootNode()->AddChild(rootNode);
}

// UnregisterTransitionPair - Unregister non-existent pair
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Unregister_Transition_Pair_NonExistent_Test)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);

    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF);

    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00);

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(highLevelNode);

    RSNode::UnregisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId());

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    GetRootNode()->AddChild(rootNode);
}

// UnregisterTransitionPair - Unregister twice
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Unregister_Transition_Pair_Twice_Test)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);

    auto lowLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF);

    auto highLevelNode = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00);

    rootNode->AddChild(lowLevelNode);
    rootNode->AddChild(highLevelNode);

    RSNode::RegisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId(), true);
    RSNode::UnregisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId());
    RSNode::UnregisterTransitionPair(lowLevelNode->GetId(), highLevelNode->GetId());

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode);
    RegisterNode(highLevelNode);
    GetRootNode()->AddChild(rootNode);
}

// UnregisterTransitionPair - Register and unregister different pairs
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Unregister_Transition_Pair_Multiple_Test)
{
    auto rootNode = RSCanvasNode::Create();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);

    auto lowLevelNode1 = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow1{100, 300, 200, 200};
    lowLevelNode1->SetBounds(boundsLow1);
    lowLevelNode1->SetFrame(boundsLow1);
    lowLevelNode1->SetBackgroundColor(0xFF0000FF);

    auto highLevelNode1 = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh1{100, 600, 200, 200};
    highLevelNode1->SetBounds(boundsHigh1);
    highLevelNode1->SetFrame(boundsHigh1);
    highLevelNode1->SetBackgroundColor(0xFF00FF00);

    auto lowLevelNode2 = RSCanvasNode::Create();
    Rosen::Vector4f boundsLow2{400, 300, 200, 200};
    lowLevelNode2->SetBounds(boundsLow2);
    lowLevelNode2->SetFrame(boundsLow2);
    lowLevelNode2->SetBackgroundColor(0xFFFF0000);

    auto highLevelNode2 = RSCanvasNode::Create();
    Rosen::Vector4f boundsHigh2{400, 600, 200, 200};
    highLevelNode2->SetBounds(boundsHigh2);
    highLevelNode2->SetFrame(boundsHigh2);
    highLevelNode2->SetBackgroundColor(0xFFFFFF00);

    rootNode->AddChild(lowLevelNode1);
    rootNode->AddChild(highLevelNode1);
    rootNode->AddChild(lowLevelNode2);
    rootNode->AddChild(highLevelNode2);

    RSNode::RegisterTransitionPair(lowLevelNode1->GetId(), highLevelNode1->GetId(), true);
    RSNode::RegisterTransitionPair(lowLevelNode2->GetId(), highLevelNode2->GetId(), true);
    RSNode::UnregisterTransitionPair(lowLevelNode1->GetId(), highLevelNode1->GetId());

    RegisterNode(rootNode);
    RegisterNode(lowLevelNode1);
    RegisterNode(highLevelNode1);
    RegisterNode(lowLevelNode2);
    RegisterNode(highLevelNode2);
    GetRootNode()->AddChild(rootNode);
}

// SetSandbox - Negative offset
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

// SetSandbox - Large offset
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Large_Test)
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

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(1000, 1000)));

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
    childNode3->SetBackgroundColor(0xFFFFFF00);

    childNode1->SetSandBox(std::make_optional(Rosen::Vector2f(0, 0)));
    childNode2->SetSandBox(std::make_optional(Rosen::Vector2f(50, 50)));
    childNode3->SetSandBox(std::make_optional(Rosen::Vector2f(100, 100)));

    parentNode->AddChild(childNode1);
    parentNode->AddChild(childNode2);
    parentNode->AddChild(childNode3);
    RegisterNode(parentNode);
    RegisterNode(childNode1);
    RegisterNode(childNode2);
    RegisterNode(childNode3);
    GetRootNode()->AddChild(parentNode);
}

// SetSandbox - Update sandbox offset dynamically
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Update_Test)
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

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(50, 50)));
    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(150, 150)));

    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}

// SetSandbox - One offset zero, other non-zero
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Partial_Zero_Test)
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

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(0, 100)));

    parentNode->AddChild(childNode);
    RegisterNode(parentNode);
    RegisterNode(childNode);
    GetRootNode()->AddChild(parentNode);
}
} // namespace OHOS::Rosen