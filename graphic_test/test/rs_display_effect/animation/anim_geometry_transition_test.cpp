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

std::shared_ptr<Rosen::RSCanvasNode> CreateCanvasNode()
{
    return RSCanvasNode::Create();
}

// sharedTransition baseline
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, WO_Set_Transition_Pair_Test)
{
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = CreateCanvasNode();
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
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = CreateCanvasNode();
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
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = CreateCanvasNode();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto highLevelParentNode = CreateCanvasNode();

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
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = CreateCanvasNode();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto lowLevelParentNode = CreateCanvasNode();

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
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = CreateCanvasNode();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto lowLevelParentNode = CreateCanvasNode();
    auto highLevelParentNode = CreateCanvasNode();

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
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = CreateCanvasNode();
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
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = CreateCanvasNode();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto highLevelParentNode = CreateCanvasNode();

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
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = CreateCanvasNode();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto lowLevelParentNode = CreateCanvasNode();

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
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto lowLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsLow{100, 300, 300, 300};
    lowLevelNode->SetBounds(boundsLow);
    lowLevelNode->SetFrame(boundsLow);
    lowLevelNode->SetBackgroundColor(0xFF0000FF); // blue
    auto highLevelNode = CreateCanvasNode();
    Rosen::Vector4f boundsHigh{300, 300, 300, 300};
    highLevelNode->SetBounds(boundsHigh);
    highLevelNode->SetFrame(boundsHigh);
    highLevelNode->SetBackgroundColor(0xFF00FF00); // green
    auto idleNode = CreateCanvasNode();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000); // red

    auto lowLevelParentNode = CreateCanvasNode();
    auto highLevelParentNode = CreateCanvasNode();

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

// RegisterTransitionPair: inNodeId == outNodeId (same node)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Register_Same_Node_As_Pair_Test)
{
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto node = CreateCanvasNode();
    Rosen::Vector4f boundsNode{200, 200, 300, 300};
    node->SetBounds(boundsNode);
    node->SetFrame(boundsNode);
    node->SetBackgroundColor(0xFF0000FF);

    rootNode->AddChild(node);

    RSNode::RegisterTransitionPair(GetRSUIContext(), node->GetId(), node->GetId(), true);
    RegisterNode(rootNode);
    RegisterNode(node);
    GetRootNode()->AddChild(rootNode);
}

// RegisterTransitionPair: non-existent node id
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Register_NonExistent_Node_Test)
{
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto existNode = CreateCanvasNode();
    Rosen::Vector4f boundsExist{200, 200, 300, 300};
    existNode->SetBounds(boundsExist);
    existNode->SetFrame(boundsExist);
    existNode->SetBackgroundColor(0xFF0000FF);

    rootNode->AddChild(existNode);

    constexpr NodeId nonExistentId = 99999999;
    RSNode::RegisterTransitionPair(GetRSUIContext(), existNode->GetId(), nonExistentId, true);
    RSNode::RegisterTransitionPair(GetRSUIContext(), nonExistentId, existNode->GetId(), true);
    RSNode::RegisterTransitionPair(GetRSUIContext(), nonExistentId, nonExistentId, true);

    RegisterNode(rootNode);
    RegisterNode(existNode);
    GetRootNode()->AddChild(rootNode);
}

// UnregisterTransitionPair: normal unregister after register
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Unregister_Normal_Pair_Test)
{
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto inNode = CreateCanvasNode();
    Rosen::Vector4f boundsIn{100, 300, 300, 300};
    inNode->SetBounds(boundsIn);
    inNode->SetFrame(boundsIn);
    inNode->SetBackgroundColor(0xFF0000FF);
    auto outNode = CreateCanvasNode();
    Rosen::Vector4f boundsOut{300, 300, 300, 300};
    outNode->SetBounds(boundsOut);
    outNode->SetFrame(boundsOut);
    outNode->SetBackgroundColor(0xFF00FF00);
    auto idleNode = CreateCanvasNode();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000);

    rootNode->AddChild(inNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(outNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), inNode->GetId(), outNode->GetId(), true);
    RSNode::UnregisterTransitionPair(GetRSUIContext(), inNode->GetId(), outNode->GetId());

    RegisterNode(rootNode);
    RegisterNode(inNode);
    RegisterNode(idleNode);
    RegisterNode(outNode);
    GetRootNode()->AddChild(rootNode);
}

// UnregisterTransitionPair: unregister with reversed order (outNodeId, inNodeId)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Unregister_Reversed_Order_Test)
{
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto inNode = CreateCanvasNode();
    Rosen::Vector4f boundsIn{100, 300, 300, 300};
    inNode->SetBounds(boundsIn);
    inNode->SetFrame(boundsIn);
    inNode->SetBackgroundColor(0xFF0000FF);
    auto outNode = CreateCanvasNode();
    Rosen::Vector4f boundsOut{300, 300, 300, 300};
    outNode->SetBounds(boundsOut);
    outNode->SetFrame(boundsOut);
    outNode->SetBackgroundColor(0xFF00FF00);
    auto idleNode = CreateCanvasNode();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000);

    rootNode->AddChild(inNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(outNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), inNode->GetId(), outNode->GetId(), true);
    RSNode::UnregisterTransitionPair(GetRSUIContext(), outNode->GetId(), inNode->GetId());

    RegisterNode(rootNode);
    RegisterNode(inNode);
    RegisterNode(idleNode);
    RegisterNode(outNode);
    GetRootNode()->AddChild(rootNode);
}

// UnregisterTransitionPair: unregister non-existent nodes (should not crash, pair remains)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Unregister_NonExistent_Node_Test)
{
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto inNode = CreateCanvasNode();
    Rosen::Vector4f boundsIn{100, 300, 300, 300};
    inNode->SetBounds(boundsIn);
    inNode->SetFrame(boundsIn);
    inNode->SetBackgroundColor(0xFF0000FF);
    auto outNode = CreateCanvasNode();
    Rosen::Vector4f boundsOut{300, 300, 300, 300};
    outNode->SetBounds(boundsOut);
    outNode->SetFrame(boundsOut);
    outNode->SetBackgroundColor(0xFF00FF00);
    auto idleNode = CreateCanvasNode();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000);

    rootNode->AddChild(inNode);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(outNode);

    RSNode::RegisterTransitionPair(GetRSUIContext(), inNode->GetId(), outNode->GetId(), true);
    constexpr NodeId nonExistentId = 88888888;
    RSNode::UnregisterTransitionPair(GetRSUIContext(), inNode->GetId(), nonExistentId);
    RSNode::UnregisterTransitionPair(GetRSUIContext(), nonExistentId, outNode->GetId());
    RSNode::UnregisterTransitionPair(GetRSUIContext(), nonExistentId, nonExistentId);

    RegisterNode(rootNode);
    RegisterNode(inNode);
    RegisterNode(idleNode);
    RegisterNode(outNode);
    GetRootNode()->AddChild(rootNode);
}

// UnregisterTransitionPair: unregister without prior register (no pair exists)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Unregister_Without_Register_Test)
{
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto nodeA = CreateCanvasNode();
    Rosen::Vector4f boundsA{100, 300, 300, 300};
    nodeA->SetBounds(boundsA);
    nodeA->SetFrame(boundsA);
    nodeA->SetBackgroundColor(0xFF0000FF);
    auto nodeB = CreateCanvasNode();
    Rosen::Vector4f boundsB{300, 300, 300, 300};
    nodeB->SetBounds(boundsB);
    nodeB->SetFrame(boundsB);
    nodeB->SetBackgroundColor(0xFF00FF00);
    auto idleNode = CreateCanvasNode();
    Rosen::Vector4f boundsIdle{200, 200, 200, 200};
    idleNode->SetBounds(boundsIdle);
    idleNode->SetFrame(boundsIdle);
    idleNode->SetBackgroundColor(0xFFFF0000);

    rootNode->AddChild(nodeA);
    rootNode->AddChild(idleNode);
    rootNode->AddChild(nodeB);

    RSNode::UnregisterTransitionPair(GetRSUIContext(), nodeA->GetId(), nodeB->GetId());

    RegisterNode(rootNode);
    RegisterNode(nodeA);
    RegisterNode(idleNode);
    RegisterNode(nodeB);
    GetRootNode()->AddChild(rootNode);
}

// UnregisterTransitionPair: register A-C then unregister A-B (id mismatch, A should keep pair)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Unregister_Id_Mismatch_Test)
{
    auto rootNode = CreateCanvasNode();
    Rosen::Vector4f bounds{0, 0, screenWidth, screenHeight};
    rootNode->SetBounds(bounds);
    rootNode->SetFrame(bounds);
    auto nodeA = CreateCanvasNode();
    Rosen::Vector4f boundsA{100, 300, 300, 300};
    nodeA->SetBounds(boundsA);
    nodeA->SetFrame(boundsA);
    nodeA->SetBackgroundColor(0xFF0000FF);
    auto nodeB = CreateCanvasNode();
    Rosen::Vector4f boundsB{300, 300, 300, 300};
    nodeB->SetBounds(boundsB);
    nodeB->SetFrame(boundsB);
    nodeB->SetBackgroundColor(0xFF00FF00);
    auto nodeC = CreateCanvasNode();
    Rosen::Vector4f boundsC{200, 200, 200, 200};
    nodeC->SetBounds(boundsC);
    nodeC->SetFrame(boundsC);
    nodeC->SetBackgroundColor(0xFFFF0000);

    rootNode->AddChild(nodeA);
    rootNode->AddChild(nodeB);
    rootNode->AddChild(nodeC);

    RSNode::RegisterTransitionPair(GetRSUIContext(), nodeA->GetId(), nodeC->GetId(), true);
    RSNode::UnregisterTransitionPair(GetRSUIContext(), nodeA->GetId(), nodeB->GetId());

    RegisterNode(rootNode);
    RegisterNode(nodeA);
    RegisterNode(nodeB);
    RegisterNode(nodeC);
    GetRootNode()->AddChild(rootNode);
}

// sandbox
// Currently, Sandbox only takes effect when the node registers the transition pair.
// baseline: without set sandbox
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, WO_Set_Sandbox_Test)
{
    auto parentNode = CreateCanvasNode();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000); // red
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = CreateCanvasNode();
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
    auto parentNode = CreateCanvasNode();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000); // red
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = CreateCanvasNode();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF); // blue
    auto siblingNode = CreateCanvasNode();

    childNode->SetSandBox(std::nullopt);

    parentNode->AddChild(siblingNode);
    parentNode->AddChild(childNode);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode->GetId(), siblingNode->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(childNode);
    RegisterNode(siblingNode);
    GetRootNode()->AddChild(parentNode);
}

// sandbox relative to surfaceNode 0, 0
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_0_0_Test)
{
    auto parentNode = CreateCanvasNode();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000); // red
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = CreateCanvasNode();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF); // blue
    auto siblingNode = CreateCanvasNode();

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(0, 0)));

    parentNode->AddChild(siblingNode);
    parentNode->AddChild(childNode);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode->GetId(), siblingNode->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(childNode);
    RegisterNode(siblingNode);
    GetRootNode()->AddChild(parentNode);
}

// sandbox relative to surfaceNode 100, 100
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_100_100_Test)
{
    auto parentNode = CreateCanvasNode();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000); // red
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = CreateCanvasNode();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF); // blue
    auto siblingNode = CreateCanvasNode();

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(100, 100)));

    parentNode->AddChild(siblingNode);
    parentNode->AddChild(childNode);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode->GetId(), siblingNode->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(childNode);
    RegisterNode(siblingNode);
    GetRootNode()->AddChild(parentNode);
}

// sandbox relative to surfaceNode 200, 200
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_200_200_Test)
{
    auto parentNode = CreateCanvasNode();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000); // red
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = CreateCanvasNode();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF); // blue
    auto siblingNode = CreateCanvasNode();

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(200, 200)));

    parentNode->AddChild(siblingNode);
    parentNode->AddChild(childNode);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode->GetId(), siblingNode->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(childNode);
    RegisterNode(siblingNode);
    GetRootNode()->AddChild(parentNode);
}

// sandbox relative to surfaceNode -100, -100
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Negative_Test)
{
    auto parentNode = CreateCanvasNode();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000);
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = CreateCanvasNode();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF);
    auto siblingNode = CreateCanvasNode();

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(-100, -100)));

    parentNode->AddChild(siblingNode);
    parentNode->AddChild(childNode);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode->GetId(), siblingNode->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(childNode);
    RegisterNode(siblingNode);
    GetRootNode()->AddChild(parentNode);
}

// SetSandbox - Multiple nodes with different sandbox offsets
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Multiple_Nodes_Test)
{
    auto parentNode = CreateCanvasNode();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000);
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);

    auto childNode1 = CreateCanvasNode();
    Rosen::Vector4f boundsChild1{50, 50, 150, 150};
    childNode1->SetBounds(boundsChild1);
    childNode1->SetFrame(boundsChild1);
    childNode1->SetBackgroundColor(0xFF0000FF);

    auto childNode2 = CreateCanvasNode();
    Rosen::Vector4f boundsChild2{250, 50, 150, 150};
    childNode2->SetBounds(boundsChild2);
    childNode2->SetFrame(boundsChild2);
    childNode2->SetBackgroundColor(0xFF00FF00);

    auto childNode3 = CreateCanvasNode();
    Rosen::Vector4f boundsChild3{150, 250, 150, 150};
    childNode3->SetBounds(boundsChild3);
    childNode3->SetFrame(boundsChild3);
    childNode3->SetBackgroundColor(0xFFFFFFFF);

    auto siblingNode1 = CreateCanvasNode();
    auto siblingNode2 = CreateCanvasNode();
    auto siblingNode3 = CreateCanvasNode();

    childNode1->SetSandBox(std::make_optional(Rosen::Vector2f(0, 0)));
    childNode2->SetSandBox(std::make_optional(Rosen::Vector2f(150, 50)));
    childNode3->SetSandBox(std::make_optional(Rosen::Vector2f(100, 300)));

    parentNode->AddChild(siblingNode1);
    parentNode->AddChild(childNode1);
    parentNode->AddChild(siblingNode2);
    parentNode->AddChild(childNode2);
    parentNode->AddChild(siblingNode3);
    parentNode->AddChild(childNode3);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode1->GetId(), siblingNode1->GetId(), true);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode2->GetId(), siblingNode2->GetId(), true);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode3->GetId(), siblingNode3->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(childNode1);
    RegisterNode(childNode2);
    RegisterNode(childNode3);
    RegisterNode(siblingNode1);
    RegisterNode(siblingNode2);
    RegisterNode(siblingNode3);
    GetRootNode()->AddChild(parentNode);
}

// SetSandbox - extremely large values
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Extremely_Large_Test)
{
    auto parentNode = CreateCanvasNode();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000);
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = CreateCanvasNode();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF);
    auto siblingNode = CreateCanvasNode();

    constexpr float largeVal = 1e10f;
    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(largeVal, -largeVal)));

    parentNode->AddChild(siblingNode);
    parentNode->AddChild(childNode);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode->GetId(), siblingNode->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(childNode);
    RegisterNode(siblingNode);
    GetRootNode()->AddChild(parentNode);
}

// SetSandbox - extremely small (near zero) values
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Extremely_Small_Test)
{
    auto parentNode = CreateCanvasNode();
    Rosen::Vector4f bounds{100, 100, 800, 800};
    parentNode->SetBackgroundColor(0xFFFF0000);
    parentNode->SetBounds(bounds);
    parentNode->SetFrame(bounds);
    auto childNode = CreateCanvasNode();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF);
    auto siblingNode = CreateCanvasNode();

    constexpr float smallVal = 1e-10f;
    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(smallVal, -smallVal)));

    parentNode->AddChild(siblingNode);
    parentNode->AddChild(childNode);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode->GetId(), siblingNode->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(childNode);
    RegisterNode(siblingNode);
    GetRootNode()->AddChild(parentNode);
}

// Sandbox node has foreground filter (offscreen)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Foreground_Filter_Test)
{
    auto parentNode = CreateCanvasNode();
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

    auto childNode = CreateCanvasNode();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF);
    auto siblingNode = CreateCanvasNode();

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(100, 100)));
    childNode->SetForegroundNGFilter(bezierWarpFilter);

    parentNode->AddChild(siblingNode);
    parentNode->AddChild(childNode);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode->GetId(), siblingNode->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(childNode);
    RegisterNode(siblingNode);
    GetRootNode()->AddChild(parentNode);
}

// Sandbox's parent node has foreground filter (offscreen)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Parent_Foreground_Filter_Test)
{
    auto parentNode = CreateCanvasNode();
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

    auto childNode = CreateCanvasNode();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF);
    auto siblingNode = CreateCanvasNode();

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(100, 100)));

    parentNode->AddChild(siblingNode);
    parentNode->AddChild(childNode);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode->GetId(), siblingNode->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(childNode);
    RegisterNode(siblingNode);
    GetRootNode()->AddChild(parentNode);
}

// Sandbox's parent node has two foreground filters (offscreen)
GRAPHIC_TEST(GeometryTransitionTest, ANIMATION_TEST, Set_Sandbox_Parent_Two_Foreground_Filters_Test)
{
    auto parentNode = CreateCanvasNode();
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

    auto parentNode2 = CreateCanvasNode();
    Rosen::Vector4f bounds2{100, 100, 500, 500};
    parentNode2->SetBackgroundColor(0xFF00FF00);
    parentNode2->SetBounds(bounds2);
    parentNode2->SetFrame(bounds2);
    parentNode2->SetForegroundNGFilter(bezierWarpFilter);

    auto childNode = CreateCanvasNode();
    Rosen::Vector4f boundsChild{100, 100, 200, 200};
    childNode->SetBounds(boundsChild);
    childNode->SetFrame(boundsChild);
    childNode->SetBackgroundColor(0xFF0000FF);
    auto siblingNode = CreateCanvasNode();

    childNode->SetSandBox(std::make_optional(Rosen::Vector2f(100, 100)));

    parentNode->AddChild(parentNode2);
    parentNode2->AddChild(siblingNode);
    parentNode2->AddChild(childNode);
    RSNode::RegisterTransitionPair(GetRSUIContext(), childNode->GetId(), siblingNode->GetId(), true);

    RegisterNode(parentNode);
    RegisterNode(parentNode2);
    RegisterNode(childNode);
    RegisterNode(siblingNode);
    GetRootNode()->AddChild(parentNode);
}
} // namespace OHOS::Rosen