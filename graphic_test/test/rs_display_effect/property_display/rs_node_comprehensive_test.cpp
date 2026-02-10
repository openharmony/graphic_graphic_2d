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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSNodeComprehensiveTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

// ============================================================================
// Part 1: Node Properties (属性类接口)
// ============================================================================

// Callback function for bounds changed
static void OnBoundsChangedCallback()
{
    // Callback handler
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetBoundsChangedCallback_001
 * @tc.desc: Test SetBoundsChangedCallback with valid/null callback
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetBoundsChangedCallback_001)
{
    // Test with valid callback
    auto testNode1 = RSCanvasNode::Create();
    testNode1->SetBounds({ 50, 50, 400, 400 });
    testNode1->SetBackgroundColor(0xffff0000);
    testNode1->SetBoundsChangedCallback(OnBoundsChangedCallback);
    GetRootNode()->AddChild(testNode1);
    RegisterNode(testNode1);

    // Test with null callback
    auto testNode2 = RSCanvasNode::Create();
    testNode2->SetBounds({ 500, 50, 400, 400 });
    testNode2->SetBackgroundColor(0xffff0000);
    testNode2->SetBoundsChangedCallback(nullptr);
    GetRootNode()->AddChild(testNode2);
    RegisterNode(testNode2);
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetPixelmap_001
 * @tc.desc: Test SetPixelmap with normal/null pixelmap (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetPixelmap_001)
{
    auto pixelMap = DecodePixelMap("/data/local/tmp/appearance_test.jpg",
                                   Media::AllocatorType::SHARE_MEM_ALLOC);

    // Test with pixelmap
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)j * 380 + 50, (int)i * 350 + 50, 300, 300 });
            if (i == 0 && j == 0) {
                testNode->SetPixelMap(nullptr); // null pixelmap
                testNode->SetBackgroundColor(0xffff0000);
            } else {
                testNode->SetPixelMap(pixelMap);
            }
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetDrawNodeType_001
 * @tc.desc: Test SetDrawNodeType with boundary values (3x4 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetDrawNodeType_001)
{
    std::vector<int> typeList = { 0, 1, 2 };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < typeList.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaList[col]);
            testNode->SetDrawNodeType(typeList[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetTextureExport_001
 * @tc.desc: Test SetTextureExport toggle behavior
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetTextureExport_001)
{
    std::vector<bool> exportStates = { true, false, true };

    for (size_t i = 0; i < exportStates.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetTextureExport(exportStates[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_ClearChildren_001
 * @tc.desc: Test ClearChildren removes all children
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_ClearChildren_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 1100, 400 });
    parent->SetBackgroundColor(0xffff0000);

    for (int i = 0; i < 4; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + i * 280, 50, 250, 300 });
        child->SetBackgroundColor(0xff00ff00);
        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
    parent->ClearChildren();
}

/*
 * @tc.name: RSNodeComprehensiveTest_MarkRepaintBoundary_001
 * @tc.desc: Test MarkRepaintBoundary with different states (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_MarkRepaintBoundary_001)
{
    std::vector<bool> boundaryStates = { true, false };
    std::vector<float> alphaList = { 0.3f, 0.6f, 1.0f };

    for (size_t row = 0; row < boundaryStates.size(); row++) {
        for (size_t col = 0; col < alphaList.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 380 + 50, (int)row * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetAlpha(alphaList[col]);
            testNode->MarkRepaintBoundary(boundaryStates[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetInstanceId_001
 * @tc.desc: Test SetInstanceId with boundary values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetInstanceId_001)
{
    std::vector<uint64_t> instanceIds = { 0, 1, 100, 1000, UINT64_MAX };

    for (size_t i = 0; i < instanceIds.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 220 + 50, 50, 200, 200 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetInstanceId(instanceIds[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_MarkSuggestOpincNode_001
 * @tc.desc: Test MarkSuggestOpincNode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_MarkSuggestOpincNode_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->MarkSuggestOpincNode();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensiveTest_MarkUifirstNode_001
 * @tc.desc: Test MarkUifirstNode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_MarkUifirstNode_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->MarkUifirstNode();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetOutOfParent_001
 * @tc.desc: Test SetOutOfParent with various positions
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetOutOfParent_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 600, 600 });
    parent->SetBackgroundColor(0xffff0000);

    std::vector<Vector4f> childBounds = {
        { 100, 100, 100, 100 },     // Inside
        { 400, 400, 300, 300 },      // Partially outside
        { 700, 700, 200, 200 }       // Completely outside
    };

    for (size_t i = 0; i < childBounds.size(); i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ childBounds[i].x_, childBounds[i].y_,
            childBounds[i].z_, childBounds[i].w_ });
        child->SetBackgroundColor(0xff00ff00);
        child->SetOutOfParent(true);
        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
}

// ============================================================================
// Part 2: Node Hierarchy (层次结构接口)
// ============================================================================

/*
 * @tc.name: RSNodeComprehensiveTest_AddChild_001
 * @tc.desc: Test AddChild with single/multiple children
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_AddChild_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 1100, 1100 });
    parent->SetBackgroundColor(0xffff0000);

    for (int i = 0; i < 9; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + (i % 3) * 350, 50 + (i / 3) * 350, 300, 300 });
        child->SetBackgroundColor(0xff00ff00 - i * 0x00100000);
        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
}

/*
 * @tc.name: RSNodeComprehensiveTest_MoveChild_001
 * @tc.desc: Test MoveChild reposition
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_MoveChild_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 1100, 400 });
    parent->SetBackgroundColor(0xffff0000);

    for (int i = 0; i < 4; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + i * 280, 50, 250, 300 });
        child->SetBackgroundColor(0xff00ff00);
        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
    parent->MoveChild(0, 2);
}

/*
 * @tc.name: RSNodeComprehensiveTest_RemoveChildByNodeSelf_001
 * @tc.desc: Test RemoveChildByNodeSelf
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_RemoveChildByNodeSelf_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 600, 600 });
    parent->SetBackgroundColor(0xffff0000);

    for (int i = 0; i < 3; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + i * 180, 50, 150, 150 });
        child->SetBackgroundColor(0xff00ff00);
        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);

    auto firstChild = std::static_pointer_cast<RSCanvasNode>(parent->GetChildren()[0]);
    parent->RemoveChildByNodeSelf(firstChild);
}

/*
 * @tc.name: RSNodeComprehensiveTest_RemoveFromTree_001
 * @tc.desc: Test RemoveFromTree
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_RemoveFromTree_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 500, 500 });
    parent->SetBackgroundColor(0xffff0000);

    auto child = RSCanvasNode::Create();
    child->SetBounds({ 100, 100, 200, 200 });
    child->SetBackgroundColor(0xff00ff00);
    parent->AddChild(child);

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
    child->RemoveFromTree();
}

/*
 * @tc.name: RSNodeComprehensiveTest_AddCompositeNodeChild_001
 * @tc.desc: Test AddCompositeNodeChild
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_AddCompositeNodeChild_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 1100, 400 });
    parent->SetBackgroundColor(0xffff0000);

    for (int i = 0; i < 3; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + i * 350, 50, 300, 300 });
        child->SetBackgroundColor(0xff00ff00);
        parent->AddCompositeNodeChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
}

/*
 * @tc.name: RSNodeComprehensiveTest_AddCrossParentChild_001
 * @tc.desc: Test AddCrossParentChild
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_AddCrossParentChild_001)
{
    auto parent1 = RSCanvasNode::Create();
    parent1->SetBounds({ 50, 50, 500, 500 });
    parent1->SetBackgroundColor(0xffff0000);

    auto parent2 = RSCanvasNode::Create();
    parent2->SetBounds({ 600, 50, 500, 500 });
    parent2->SetBackgroundColor(0xff00ff00);

    auto child = RSCanvasNode::Create();
    child->SetBounds({ 100, 100, 200, 200 });
    child->SetBackgroundColor(0xff0000ff);

    parent1->AddCrossParentChild(child);

    GetRootNode()->AddChild(parent1);
    GetRootNode()->AddChild(parent2);
    RegisterNode(parent1);
    RegisterNode(parent2);
}

/*
 * @tc.name: RSNodeComprehensiveTest_RemoveCrossParentChild_001
 * @tc.desc: Test RemoveCrossParentChild
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_RemoveCrossParentChild_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 500, 500 });
    parent->SetBackgroundColor(0xffff0000);

    auto child = RSCanvasNode::Create();
    child->SetBounds({ 100, 100, 200, 200 });
    child->SetBackgroundColor(0xff00ff00);

    parent->AddCrossParentChild(child);

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
    parent->RemoveCrossParentChild(child);
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetIsCrossNode_001
 * @tc.desc: Test SetIsCrossNode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetIsCrossNode_001)
{
    std::vector<bool> isCrossValues = { true, false, true };

    for (size_t i = 0; i < isCrossValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetIsCrossNode(isCrossValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_AddCrossScreenChild_001
 * @tc.desc: Test AddCrossScreenChild
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_AddCrossScreenChild_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 500, 500 });
    parent->SetBackgroundColor(0xffff0000);

    auto child = RSCanvasNode::Create();
    child->SetBounds({ 100, 100, 200, 200 });
    child->SetBackgroundColor(0xff00ff00);

    parent->AddCrossScreenChild(child);

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
}

/*
 * @tc.name: RSNodeComprehensiveTest_RemoveCrossScreenChild_001
 * @tc.desc: Test RemoveCrossScreenChild
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_RemoveCrossScreenChild_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 500, 500 });
    parent->SetBackgroundColor(0xffff0000);

    auto child = RSCanvasNode::Create();
    child->SetBounds({ 100, 100, 200, 200 });
    child->SetBackgroundColor(0xff00ff00);

    parent->AddCrossScreenChild(child);

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
    parent->RemoveCrossScreenChild(child);
}

/*
 * @tc.name: RSNodeComprehensiveTest_IsInstanceOf_001
 * @tc.desc: Test IsInstanceOf with different node types
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_IsInstanceOf_001)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds({ 50, 50, 300, 300 });
    canvasNode->SetBackgroundColor(0xffff0000);

    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 400, 50, 300, 300 });

    auto displayNode = RSDisplayNode::Create();
    displayNode->SetBounds({ 750, 50, 300, 300 });

    bool isCanvas = canvasNode->IsInstanceOf(RSNodeType::CANVAS_NODE);
    bool isSurface = surfaceNode->IsInstanceOf(RSNodeType::SURFACE_NODE);
    bool isDisplay = displayNode->IsInstanceOf(RSNodeType::DISPLAY_NODE);

    GetRootNode()->AddChild(canvasNode);
    GetRootNode()->AddChild(surfaceNode);
    GetRootNode()->AddChild(displayNode);
    RegisterNode(canvasNode);
    RegisterNode(surfaceNode);
    RegisterNode(displayNode);
}

// ============================================================================
// Part 3: Node Advanced Features (高级特性接口)
// ============================================================================

/*
 * @tc.name: RSNodeComprehensiveTest_SetTakeSurfaceForUIFlag_001
 * @tc.desc: Test SetTakeSurfaceForUIFlag
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetTakeSurfaceForUIFlag_001)
{
    std::vector<bool> flagValues = { true, false, true };

    for (size_t i = 0; i < flagValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetTakeSurfaceForUIFlag(flagValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_RemoveModifier_001
 * @tc.desc: Test RemoveModifier
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_RemoveModifier_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    auto alphaModifier = std::make_shared<RSAlphaModifier>(0.5f);
    testNode->AddModifier(alphaModifier);

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
    testNode->RemoveModifier(alphaModifier);
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetIsCustomTextType_001
 * @tc.desc: Test SetIsCustomTextType
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetIsCustomTextType_001)
{
    std::vector<bool> customTypeValues = { true, false, true };

    for (size_t i = 0; i < customTypeValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetIsCustomTextType(customTypeValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_CloseImplicitCancelAnimation_001
 * @tc.desc: Test CloseImplicitCancelAnimation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_CloseImplicitCancelAnimation_001)
{
    std::vector<bool> closeValues = { true, false, true, false };

    for (size_t i = 0; i < closeValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->CloseImplicitCancelAnimation(closeValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_NotifyTransition_001
 * @tc.desc: Test NotifyTransition
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_NotifyTransition_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->NotifyTransition();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetFrameNodeInfo_001
 * @tc.desc: Test SetFrameNodeInfo
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetFrameNodeInfo_001)
{
    std::vector<uint64_t> infoValues = { 0, 100, 200, 300 };

    for (size_t i = 0; i < infoValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetFrameNodeInfo(infoValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetNeedUseCmdlistDrawRegion_001
 * @tc.desc: Test SetNeedUseCmdlistDrawRegion
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetNeedUseCmdlistDrawRegion_001)
{
    std::vector<bool> needUseList = { true, false, true };

    for (size_t i = 0; i < needUseList.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetNeedUseCmdlistDrawRegion(needUseList[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_MarkNodeSingleFrameComposer_001
 * @tc.desc: Test MarkNodeSingleFrameComposer
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_MarkNodeSingleFrameComposer_001)
{
    for (int i = 0; i < 4; i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000 - i * 0x00200000);
        testNode->MarkNodeSingleFrameComposer();
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_CalcExpectedFrameRate_001
 * @tc.desc: Test CalcExpectedFrameRate
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_CalcExpectedFrameRate_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->CalcExpectedFrameRate();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetDrawNode_001
 * @tc.desc: Test SetDrawNode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetDrawNode_001)
{
    std::vector<bool> drawNodeValues = { true, false, true, false };

    for (size_t i = 0; i < drawNodeValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetDrawNode(drawNodeValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_SyncDrawNodeType_001
 * @tc.desc: Test SyncDrawNodeType
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SyncDrawNodeType_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetDrawNodeType(1);
    testNode->SyncDrawNodeType();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

// ============================================================================
// Part 4: Node State and Utilities (状态和工具接口)
// ============================================================================

/*
 * @tc.name: RSNodeComprehensiveTest_DumpTree_001
 * @tc.desc: Test DumpTree
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_DumpTree_001)
{
    auto parent = RSCanvasNode::Create();
    parent->SetBounds({ 50, 50, 500, 500 });
    parent->SetBackgroundColor(0xffff0000);

    for (int i = 0; i < 3; i++) {
        auto child = RSCanvasNode::Create();
        child->SetBounds({ 50 + i * 150, 50, 100, 100 });
        child->SetBackgroundColor(0xff00ff00);
        parent->AddChild(child);
    }

    GetRootNode()->AddChild(parent);
    RegisterNode(parent);
    parent->DumpTree();
}

/*
 * @tc.name: RSNodeComprehensiveTest_Dump_001
 * @tc.desc: Test Dump
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_Dump_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
    testNode->Dump();
}

/*
 * @tc.name: RSNodeComprehensiveTest_DumpNode_001
 * @tc.desc: Test DumpNode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_DumpNode_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
    testNode->DumpNode();
}

/*
 * @tc.name: RSNodeComprehensiveTest_IsUniRenderEnabled_001
 * @tc.desc: Test IsUniRenderEnabled
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_IsUniRenderEnabled_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    bool isEnabled = testNode->IsUniRenderEnabled();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensiveTest_IsRenderServiceNode_001
 * @tc.desc: Test IsRenderServiceNode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_IsRenderServiceNode_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    bool isRSSNode = testNode->IsRenderServiceNode();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetDrawNodeChangeCallback_001
 * @tc.desc: Test SetDrawNodeChangeCallback
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetDrawNodeChangeCallback_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetDrawNodeChangeCallback([](NodeId id) {});
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensiveTest_SetDirty_001
 * @tc.desc: Test SetDirty with different types (3x3 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_SetDirty_001)
{
    std::vector<RSDirtyType> dirtyTypes = {
        RSDirtyType::GEOMETRY_DIRTY,
        RSDirtyType::APPEARANCE_DIRTY,
        RSDirtyType::CONTENT_DIRTY
    };

    for (size_t i = 0; i < dirtyTypes.size(); i++) {
        for (int j = 0; j < 3; j++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)j * 380 + 50, (int)i * 350 + 50, 300, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetDirty(dirtyTypes[i]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_MarkDirty_001
 * @tc.desc: Test MarkDirty
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_MarkDirty_001)
{
    std::vector<RSDirtyType> dirtyTypes = {
        RSDirtyType::GEOMETRY_DIRTY,
        RSDirtyType::APPEARANCE_DIRTY,
        RSDirtyType::ALL_DIRTY
    };

    for (size_t i = 0; i < dirtyTypes.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->MarkDirty(dirtyTypes[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensiveTest_IsGeometryDirty_001
 * @tc.desc: Test IsGeometryDirty
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_IsGeometryDirty_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->MarkDirty(RSDirtyType::GEOMETRY_DIRTY);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensiveTest_IsAppearanceDirty_001
 * @tc.desc: Test IsAppearanceDirty
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_IsAppearanceDirty_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->MarkDirty(RSDirtyType::APPEARANCE_DIRTY);
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensiveTest_UpdateLocalGeometry_001
 * @tc.desc: Test UpdateLocalGeometry
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_UpdateLocalGeometry_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->UpdateLocalGeometry();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensiveTest_UpdateGlobalGeometry_001
 * @tc.desc: Test UpdateGlobalGeometry
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensiveTest_UpdateGlobalGeometry_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->UpdateGlobalGeometry();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

// ============================================================================
// Part 5: SurfaceNode Specific Tests (SurfaceNode专用接口)
// ============================================================================

/*
 * @tc.name: RSNodeComprehensive_Surface_CreateNodeInRenderThread_001
 * @tc.desc: Test CreateNodeInRenderThread
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Surface_CreateNodeInRenderThread_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    // Create in render thread
    surfaceNode->CreateNodeInRenderThread();

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeComprehensive_Surface_MarkUIHidden_001
 * @tc.desc: Test MarkUIHidden
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Surface_MarkUIHidden_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    surfaceNode->MarkUIHidden();

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeComprehensive_Surface_IsBufferAvailable_001
 * @tc.desc: Test IsBufferAvailable
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Surface_IsBufferAvailable_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    bool isAvailable = surfaceNode->IsBufferAvailable();

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeComprehensive_Surface_SetAnimationFinished_001
 * @tc.desc: Test SetAnimationFinished
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Surface_SetAnimationFinished_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    surfaceNode->SetAnimationFinished(true);

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeComprehensive_Surface_MarkUiFrameAvailable_001
 * @tc.desc: Test MarkUiFrameAvailable
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Surface_MarkUiFrameAvailable_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    surfaceNode->MarkUiFrameAvailable();

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeComprehensive_Surface_IsSelfDrawingNode_001
 * @tc.desc: Test IsSelfDrawingNode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Surface_IsSelfDrawingNode_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    bool isSelfDrawing = surfaceNode->IsSelfDrawingNode();

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeComprehensive_Surface_SetAppRotationCorrection_001
 * @tc.desc: Test SetAppRotationCorrection
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Surface_SetAppRotationCorrection_001)
{
    std::vector<bool> correctionValues = { true, false, true };

    for (size_t i = 0; i < correctionValues.size(); i++) {
        auto surfaceNode = RSSurfaceNode::Create();
        surfaceNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        surfaceNode->SetBackgroundColor(0xffff0000);
        surfaceNode->SetAppRotationCorrection(correctionValues[i]);
        GetRootNode()->AddChild(surfaceNode);
        RegisterNode(surfaceNode);
    }
}

/*
 * @tc.name: RSNodeComprehensive_Surface_ResetContextAlpha_001
 * @tc.desc: Test ResetContextAlpha
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Surface_ResetContextAlpha_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);
    surfaceNode->SetAlpha(0.5f);

    surfaceNode->ResetContextAlpha();

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

/*
 * @tc.name: RSNodeComprehensive_Surface_CreateShadowSurfaceNode_001
 * @tc.desc: Test CreateShadowSurfaceNode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Surface_CreateShadowSurfaceNode_001)
{
    auto surfaceNode = RSSurfaceNode::Create();
    surfaceNode->SetBounds({ 50, 50, 400, 400 });
    surfaceNode->SetBackgroundColor(0xffff0000);

    surfaceNode->CreateShadowSurfaceNode();

    GetRootNode()->AddChild(surfaceNode);
    RegisterNode(surfaceNode);
}

// ============================================================================
// Part 6: DisplayNode Specific Tests (DisplayNode专用接口)
// ============================================================================

/*
 * @tc.name: RSNodeComprehensive_Display_AddDisplayNodeToTree_001
 * @tc.desc: Test AddDisplayNodeToTree
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Display_AddDisplayNodeToTree_001)
{
    auto displayNode = RSDisplayNode::Create();
    displayNode->SetBounds({ 50, 50, 1100, 1900 });
    displayNode->SetBackgroundColor(0xffff0000);

    displayNode->AddDisplayNodeToTree();

    GetRootNode()->AddChild(displayNode);
    RegisterNode(displayNode);
}

/*
 * @tc.name: RSNodeComprehensive_Display_IsMirrorDisplay_001
 * @tc.desc: Test IsMirrorDisplay
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_Display_IsMirrorDisplay_001)
{
    auto displayNode = RSDisplayNode::Create();
    displayNode->SetBounds({ 50, 50, 1100, 1900 });
    displayNode->SetBackgroundColor(0xffff0000);

    bool isMirror = displayNode->IsMirrorDisplay();

    GetRootNode()->AddChild(displayNode);
    RegisterNode(displayNode);
}

// ============================================================================
// Part 7: Particle and Emitter Tests (粒子和发射器接口)
// ============================================================================

/*
 * @tc.name: RSNodeComprehensive_SetEmitterUpdater_001
 * @tc.desc: Test SetEmitterUpdater
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetEmitterUpdater_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    testNode->SetEmitterUpdater([](std::shared_ptr<RSNode> node) {
        // Emitter updater function
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensive_SetParticleNoiseFields_001
 * @tc.desc: Test SetParticleNoiseFields with boundary values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetParticleNoiseFields_001)
{
    std::vector<std::tuple<float, float, float>> noiseParams = {
        { 0.0f, 0.0f, 0.0f },
        { 0.5f, 1.0f, 1.5f },
        { 1.0f, 2.0f, 3.0f },
        { 10.0f, 10.0f, 10.0f }
    };

    for (size_t i = 0; i < noiseParams.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);

        auto [param1, param2, param3] = noiseParams[i];
        testNode->SetParticleNoiseFields(param1, param2, param3);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensive_SetParticleRippleFields_001
 * @tc.desc: Test SetParticleRippleFields
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetParticleRippleFields_001)
{
    std::vector<std::tuple<float, float, float>> rippleParams = {
        { 0.5f, 1.0f, 1.5f },
        { 1.0f, 2.0f, 3.0f },
        { 2.0f, 4.0f, 6.0f }
    };

    for (size_t i = 0; i < rippleParams.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);

        auto [param1, param2, param3] = rippleParams[i];
        testNode->SetParticleRippleFields(param1, param2, param3);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensive_SetParticleVelocityFields_001
 * @tc.desc: Test SetParticleVelocityFields
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetParticleVelocityFields_001)
{
    std::vector<std::tuple<float, float, float>> velocityParams = {
        { 1.0f, 2.0f, 3.0f },
        { 5.0f, 10.0f, 15.0f },
        { 10.0f, 20.0f, 30.0f }
    };

    for (size_t i = 0; i < velocityParams.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);

        auto [param1, param2, param3] = velocityParams[i];
        testNode->SetParticleVelocityFields(param1, param2, param3);

        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

// ============================================================================
// Part 8: Callback and Event Tests (回调和事件接口)
// ============================================================================

/*
 * @tc.name: RSNodeComprehensive_SetPropertyNodeChangeCallback_001
 * @tc.desc: Test SetPropertyNodeChangeCallback
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetPropertyNodeChangeCallback_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    testNode->SetPropertyNodeChangeCallback([](NodeId id) {
        // Callback handler
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensive_SetNeedCallbackNodeChange_001
 * @tc.desc: Test SetNeedCallbackNodeChange
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetNeedCallbackNodeChange_001)
{
    std::vector<bool> needCallbackValues = { true, false, true };

    for (size_t i = 0; i < needCallbackValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetNeedCallbackNodeChange(needCallbackValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensive_RegisterTransitionPair_001
 * @tc.desc: Test RegisterTransitionPair
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_RegisterTransitionPair_001)
{
    auto node1 = RSCanvasNode::Create();
    node1->SetBounds({ 50, 50, 400, 400 });
    node1->SetBackgroundColor(0xffff0000);

    auto node2 = RSCanvasNode::Create();
    node2->SetBounds({ 500, 50, 400, 400 });
    node2->SetBackgroundColor(0xff00ff00);

    node1->RegisterTransitionPair(node2);

    GetRootNode()->AddChild(node1);
    GetRootNode()->AddChild(node2);
    RegisterNode(node1);
    RegisterNode(node2);
}

/*
 * @tc.name: RSNodeComprehensive_UnregisterTransitionPair_001
 * @tc.desc: Test UnregisterTransitionPair
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_UnregisterTransitionPair_001)
{
    auto node1 = RSCanvasNode::Create();
    node1->SetBounds({ 50, 50, 400, 400 });
    node1->SetBackgroundColor(0xffff0000);

    auto node2 = RSCanvasNode::Create();
    node2->SetBounds({ 500, 50, 400, 400 });
    node2->SetBackgroundColor(0xff00ff00);

    node1->RegisterTransitionPair(node2);

    GetRootNode()->AddChild(node1);
    GetRootNode()->AddChild(node2);
    RegisterNode(node1);
    RegisterNode(node2);

    node1->UnregisterTransitionPair(node2);
}

// ============================================================================
// Part 9: Texture and Material Tests (纹理和材质接口)
// ============================================================================

/*
 * @tc.name: RSNodeComprehensive_SyncTextureExport_001
 * @tc.desc: Test SyncTextureExport
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SyncTextureExport_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetTextureExport(true);
    testNode->SyncTextureExport();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensive_SetExportTypeChangedCallback_001
 * @tc.desc: Test SetExportTypeChangedCallback
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetExportTypeChangedCallback_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);

    testNode->SetExportTypeChangedCallback([](NodeId id, uint32_t type) {
        // Callback handler
    });

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensive_SetMaterialWithQualityLevel_001
 * @tc.desc: Test SetMaterialWithQualityLevel (3x4 matrix)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetMaterialWithQualityLevel_001)
{
    std::vector<int> qualityLevels = { 1, 2, 3 };
    std::vector<float> blurRadius = { 0, 10, 20, 30 };

    for (size_t row = 0; row < qualityLevels.size(); row++) {
        for (size_t col = 0; col < blurRadius.size(); col++) {
            auto testNode = RSCanvasNode::Create();
            testNode->SetBounds({ (int)col * 290 + 50, (int)row * 350 + 50, 250, 300 });
            testNode->SetBackgroundColor(0xffff0000);
            testNode->SetBackgroundFilterRadius(blurRadius[col]);
            testNode->SetMaterialWithQualityLevel(qualityLevels[row]);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);
        }
    }
}

// ============================================================================
// Part 10: Complex Scenario Tests (复杂场景测试)
// ============================================================================

/*
 * @tc.name: RSNodeComprehensive_ComplexHierarchy_001
 * @tc.desc: Test complex 4-level node hierarchy
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_ComplexHierarchy_001)
{
    auto root = RSCanvasNode::Create();
    root->SetBounds({ 50, 50, 1100, 1100 });
    root->SetBackgroundColor(0xffff0000);

    auto level1 = RSCanvasNode::Create();
    level1->SetBounds({ 50, 50, 1000, 1000 });
    level1->SetBackgroundColor(0xff00ff00);

    auto level2 = RSCanvasNode::Create();
    level2->SetBounds({ 50, 50, 900, 900 });
    level2->SetBackgroundColor(0xff0000ff);

    auto level3 = RSCanvasNode::Create();
    level3->SetBounds({ 50, 50, 800, 800 });
    level3->SetBackgroundColor(0xffffff00);

    level2->AddChild(level3);
    level1->AddChild(level2);
    root->AddChild(level1);

    GetRootNode()->AddChild(root);
    RegisterNode(root);
}

/*
 * @tc.name: RSNodeComprehensive_MultipleProperties_001
 * @tc.desc: Test multiple properties on single node
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_MultipleProperties_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 600, 600 });
    testNode->SetFrame({ 50, 50, 600, 600 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->SetAlpha(0.8f);
    testNode->SetRotation(45.0f);
    testNode->SetScale(1.2f, 1.2f);
    testNode->SetTranslate(10.0f, 10.0f);
    testNode->SetDrawNodeType(1);
    testNode->MarkRepaintBoundary(true);
    testNode->SetInstanceId(1000);
    testNode->MarkSuggestOpincNode();
    testNode->MarkUifirstNode();

    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

/*
 * @tc.name: RSNodeComprehensive_ExcludedFromNodeGroup_001
 * @tc.desc: Test ExcludedFromNodeGroup
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_ExcludedFromNodeGroup_001)
{
    std::vector<bool> excludedValues = { true, false, true };

    for (size_t i = 0; i < excludedValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->ExcludedFromNodeGroup(excludedValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensive_SetUIFirstSwitch_001
 * @tc.desc: Test SetUIFirstSwitch
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetUIFirstSwitch_001)
{
    std::vector<bool> switchValues = { true, false, true, false };

    for (size_t i = 0; i < switchValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetUIFirstSwitch(switchValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensive_SetUIContextToken_001
 * @tc.desc: Test SetUIContextToken
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetUIContextToken_001)
{
    std::vector<uint64_t> tokens = { 0, 1000, 2000, UINT64_MAX };

    for (size_t i = 0; i < tokens.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 280 + 50, 50, 250, 250 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetUIContextToken(tokens[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensive_SetSkipCheckInMultiInstance_001
 * @tc.desc: Test SetSkipCheckInMultiInstance
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_SetSkipCheckInMultiInstance_001)
{
    std::vector<bool> skipValues = { true, false, true };

    for (size_t i = 0; i < skipValues.size(); i++) {
        auto testNode = RSCanvasNode::Create();
        testNode->SetBounds({ (int)i * 380 + 50, 50, 350, 350 });
        testNode->SetBackgroundColor(0xffff0000);
        testNode->SetSkipCheckInMultiInstance(skipValues[i]);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
    }
}

/*
 * @tc.name: RSNodeComprehensive_UpdateOcclusionCullingStatus_001
 * @tc.desc: Test UpdateOcclusionCullingStatus
 * @tc.type: FUNC
 */
GRAPHIC_TEST(RSNodeComprehensiveTest, CONTENT_DISPLAY_TEST, RSNodeComprehensive_UpdateOcclusionCullingStatus_001)
{
    auto testNode = RSCanvasNode::Create();
    testNode->SetBounds({ 50, 50, 400, 400 });
    testNode->SetBackgroundColor(0xffff0000);
    testNode->UpdateOcclusionCullingStatus();
    GetRootNode()->AddChild(testNode);
    RegisterNode(testNode);
}

} // namespace OHOS::Rosen
