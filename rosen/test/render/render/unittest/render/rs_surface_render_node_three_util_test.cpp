/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <system_ability_definition.h>
#include <unistd.h>

#include "display_engine/rs_luminance_control.h"
#include "ipc_callbacks/buffer_clear_callback_proxy.h"
#include "gmock/gmock.h"
#include "pipeline/rs_context.h"
#include "params/rs_surface_render_params.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNodeThreeUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline Drawing::Canvas drawingCanvas_;
    uint8_t maxAlpha = 255;
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
    RectI defaultLargeRect = {0, 0, 100, 100};
    RectI defaultSmallRect = {0, 0, 20, 20};
};

void RSSurfaceRenderNodeThreeUtilTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSSurfaceRenderNodeThreeUtilTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSSurfaceRenderNodeThreeUtilTest::SetUp() {}
void RSSurfaceRenderNodeThreeUtilTest::TearDown() {}

class TestDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit TestDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

class MockRSSurfaceRenderNode : public RSSurfaceRenderNode {
public:
    explicit MockRSSurfaceRenderNode(NodeId id,
        const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false)
        : RSSurfaceRenderNode(id, context, isTextureExportNode) {}
    ~MockRSSurfaceRenderNode() override {}
    MOCK_CONST_METHOD0(NeedDrawBehindWindow, bool());
    MOCK_CONST_METHOD0(GetFilterRect, RectI());
};

/**
 * @tc.name: UpdateSurfaceCacheContentStaticFlag
 * @tc.desc: test results of UpdateSurfaceCacheContentStaticFlag
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, UpdateSurfaceCacheContentStaticFlag, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->InitRenderParams();
    node->addedToPendingSyncList_ = true;
    auto params = static_cast<RSSurfaceRenderParams*>(node->stagingRenderParams_.get());
    EXPECT_NE(params, nullptr);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node->UpdateSurfaceCacheContentStaticFlag(true);
    EXPECT_FALSE(params->GetSurfaceCacheContentStatic());
    node->UpdateSurfaceCacheContentStaticFlag(false);
    EXPECT_TRUE(params->GetSurfaceCacheContentStatic());

    node->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node->UpdateSurfaceCacheContentStaticFlag(true);
    EXPECT_FALSE(params->GetSurfaceCacheContentStatic());

    node->isSubTreeDirty_ = true;
    node->isContentDirty_ = true;
    node->UpdateSurfaceCacheContentStaticFlag(false);
    EXPECT_FALSE(params->GetSurfaceCacheContentStatic());

    node->isSubTreeDirty_ = false;
    node->isContentDirty_ = false;
    node->UpdateSurfaceCacheContentStaticFlag(false);
    EXPECT_TRUE(params->GetSurfaceCacheContentStatic());
}

/**
 * @tc.name: UpdateSurfaceSubTreeDirtyFlag
 * @tc.desc: test results of UpdateSurfaceSubTreeDirtyFlag
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, UpdateSurfaceSubTreeDirtyFlag, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    node->addedToPendingSyncList_ = true;
    node->UpdateSurfaceSubTreeDirtyFlag();
    ASSERT_NE(node->stagingRenderParams_, nullptr);
}

/**
 * @tc.name: UpdateDrawingCacheNodes
 * @tc.desc: test results of UpdateDrawingCacheNodes
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, UpdateDrawingCacheNodes, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    auto renderNode = std::make_shared<RSRenderNode>(id + 1);
    node->UpdateDrawingCacheNodes(nullptr);
    EXPECT_EQ(node->drawingCacheNodes_.size(), 0);
    node->UpdateDrawingCacheNodes(renderNode);
    EXPECT_EQ(node->drawingCacheNodes_.size(), 1);
}

/**
 * @tc.name: UpdateFilterCacheStatusWithVisible
 * @tc.desc: test results of UpdateFilterCacheStatusWithVisible
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, UpdateFilterCacheStatusWithVisible, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    bool visible = true;
    renderNode->UpdateFilterCacheStatusWithVisible(visible);
    renderNode->UpdateFilterCacheStatusWithVisible(visible);
    ASSERT_TRUE(renderNode->prevVisible_);
}

/**
 * @tc.name: UpdateFilterCacheStatusIfNodeStatic
 * @tc.desc: test results of UpdateFilterCacheStatusIfNodeStatic
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, UpdateFilterCacheStatusIfNodeStatic, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->UpdateFilterCacheStatusIfNodeStatic(RectI(0, 0, 100, 100), true);
    auto mockNode1 = std::make_shared<RSEffectRenderNode>(id + 1);
    node->filterNodes_.emplace_back(mockNode1);
    auto mockNode2 = std::make_shared<RSRenderNode>(id + 2);
    node->filterNodes_.emplace_back(mockNode2);
    auto mockNode3 = std::make_shared<RSRenderNode>(id + 3);
    mockNode3->isOnTheTree_ = true;
    mockNode3->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    node->filterNodes_.emplace_back(mockNode3);
    auto mockNode4 = std::make_shared<RSRenderNode>(id + 4);
    mockNode4->isOnTheTree_ = true;
    mockNode4->GetMutableRenderProperties().needFilter_ = true;
    mockNode4->GetMutableRenderProperties().filter_ = std::make_shared<RSFilter>();
    node->filterNodes_.emplace_back(mockNode4);
    std::shared_ptr<RSRenderNode> mockNode5 = nullptr;
    node->filterNodes_.emplace_back(mockNode5);
    node->dirtyManager_->UpdateCacheableFilterRect({0, 0, 100, 100});
    node->oldDirtyInSurface_ = {0, 0, 10, 10};
    std::shared_ptr<RSRenderNode> mockNode6 = std::make_shared<RSEffectRenderNode>(id + 6);
    mockNode6->isOnTheTree_ = true;
    mockNode6->GetMutableRenderProperties().needFilter_ = true;
    mockNode6->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    node->filterNodes_.emplace_back(mockNode6);
    node->UpdateFilterCacheStatusIfNodeStatic(RectI(0, 0, 100, 100), false);
    ASSERT_NE(node->filterNodes_.size(), 0);
}

/**
 * @tc.name: ResetOpaqueRegion
 * @tc.desc: test results of ResetOpaqueRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, ResetOpaqueRegion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id);
    RectI absRect { 0, 0, 100, 100 };
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    bool isFocusWindow = true;
    Occlusion::Region res = testNode->ResetOpaqueRegion(absRect, screenRotation, isFocusWindow);
    EXPECT_EQ(res.rects_.size(), 0);
    isFocusWindow = false;
    res = testNode->ResetOpaqueRegion(absRect, screenRotation, isFocusWindow);
    EXPECT_NE(res.rects_.size(), 0);
}

/**
 * @tc.name: SetUnfocusedWindowOpaqueRegion
 * @tc.desc: test results of SetUnfocusedWindowOpaqueRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetUnfocusedWindowOpaqueRegion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    RectI absRect { 0, 0, 100, 100 };
    ScreenRotation rotationCases[] = { ScreenRotation::ROTATION_0, ScreenRotation::ROTATION_90,
        ScreenRotation::ROTATION_180, ScreenRotation::ROTATION_270, ScreenRotation::INVALID_SCREEN_ROTATION };
    for (ScreenRotation rotation : rotationCases) {
        Occlusion::Region opaqueRegion = surfaceNode->SetUnfocusedWindowOpaqueRegion(absRect, rotation);
        EXPECT_NE(opaqueRegion.rects_.size(), 0);
    }
}

/**
 * @tc.name: SetFocusedWindowOpaqueRegion
 * @tc.desc: test results of SetFocusedWindowOpaqueRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetFocusedWindowOpaqueRegion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    RectI absRect { 0, 0, 100, 100 };
    ScreenRotation rotationCases[] = { ScreenRotation::ROTATION_0, ScreenRotation::ROTATION_90,
    ScreenRotation::ROTATION_180, ScreenRotation::ROTATION_270, ScreenRotation::INVALID_SCREEN_ROTATION };
    for (ScreenRotation rotation : rotationCases) {
        Occlusion::Region opaqueRegion = renderNode->SetFocusedWindowOpaqueRegion(absRect, rotation);
        EXPECT_EQ(opaqueRegion.rects_.size(), 0);
    }
}

/**
 * @tc.name: ResetSurfaceContainerRegion
 * @tc.desc: test results of ResetSurfaceContainerRegion
 * @tc.type: FUNC
 * @tc.require: issueI9JAFQ
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, ResetSurfaceContainerRegion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    RectI screeninfo { 0, 0, 100, 100 };
    RectI absRect { 0, 0, 50, 50 };
    ScreenRotation rotationCases[] = { ScreenRotation::ROTATION_0, ScreenRotation::ROTATION_90,
    ScreenRotation::ROTATION_180, ScreenRotation::ROTATION_270, ScreenRotation::INVALID_SCREEN_ROTATION };
    renderNode->ResetSurfaceContainerRegion(screeninfo, absRect, rotationCases[0]);
    renderNode->containerConfig_.hasContainerWindow_ = true;
    for (ScreenRotation rotation : rotationCases) {
        renderNode->ResetSurfaceContainerRegion(screeninfo, absRect, rotation);
        EXPECT_FALSE(renderNode->containerRegion_.IsEmpty());
    }
}

/**
 * @tc.name: OnSync
 * @tc.desc: test results of OnSync
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, OnSync, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(id + 1);
    surfaceNode->OnSync();
    ASSERT_EQ(surfaceNode->renderDrawable_, nullptr);
    surfaceNode->renderDrawable_ = std::make_shared<TestDrawableAdapter>(surfaceNode2);
    surfaceNode->OnSync();
    ASSERT_NE(surfaceNode->renderDrawable_, nullptr);
    surfaceNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceNode->OnSync();
    ASSERT_TRUE(surfaceNode->IsMainWindowType());
    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    surfaceNode->OnSync();
    ASSERT_TRUE(surfaceNode->IsLeashWindow());
    surfaceNode->nodeType_ = RSSurfaceNodeType::DEFAULT;
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
    surfaceNode->OnSync();
    ASSERT_TRUE(surfaceNode->GetLastFrameUifirstFlag() == MultiThreadCacheType::NONE);
    surfaceNode->nodeType_ = RSSurfaceNodeType::DEFAULT;
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::LEASH_WINDOW);
    surfaceNode->stagingRenderParams_ = nullptr;
    surfaceNode->OnSync();
    ASSERT_TRUE(surfaceNode->GetLastFrameUifirstFlag() != MultiThreadCacheType::NONE);
    surfaceNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    surfaceNode->OnSync();
    ASSERT_NE(surfaceNode->stagingRenderParams_, nullptr);
    surfaceNode->nodeType_ = RSSurfaceNodeType::SCB_SCREEN_NODE;
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
    surfaceNode->OnSync();
    ASSERT_NE(surfaceNode->stagingRenderParams_, nullptr);
}

/**
 * @tc.name: HDRPresentTest001
 * @tc.desc: test SetHDRPresent and GetHDRPresent
 * @tc.type:FUNC
 * @tc.require: issueIB6Y6O
 *
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, HDRPresentTest001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(id + 2, rsContext);
    ASSERT_NE(childNode, nullptr);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(leashWindowNode, nullptr);

    childNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    NodeId childNodeId = childNode->GetId();
    pid_t childNodePid = ExtractPid(childNodeId);
    parentNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(id + 1);
    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    NodeId leashWindowNodeId = leashWindowNode->GetId();
    pid_t leashWindowNodePid = ExtractPid(leashWindowNodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[childNodePid][childNodeId] = childNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[leashWindowNodePid][leashWindowNodeId] = leashWindowNode;

    parentNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    childNode->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    leashWindowNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;

    leashWindowNode->AddChild(parentNode);
    parentNode->AddChild(childNode);
    leashWindowNode->SetIsOnTheTree(true);
    parentNode->SetIsOnTheTree(true);
    childNode->SetIsOnTheTree(true);

    childNode->SetHDRPresent(true);
    ASSERT_FALSE(childNode->GetHDRPresent());
    leashWindowNode->SetHDRPresent(false);
    ASSERT_FALSE(leashWindowNode->GetHDRPresent());
}

/**
 * @tc.name: HDRPresentTest002
 * @tc.desc: test IncreaseHDRNum and ReduceHDRNum
 * @tc.type:FUNC
 * @tc.require: issueIB6Y6O
 *
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, HDRPresentTest002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(id + 2, rsContext);
    ASSERT_NE(childNode, nullptr);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(leashWindowNode, nullptr);

    childNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    NodeId childNodeId = childNode->GetId();
    pid_t childNodePid = ExtractPid(childNodeId);
    parentNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(id + 1);
    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    NodeId leashWindowNodeId = leashWindowNode->GetId();
    pid_t leashWindowNodePid = ExtractPid(leashWindowNodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[childNodePid][childNodeId] = childNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[leashWindowNodePid][leashWindowNodeId] = leashWindowNode;

    parentNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    childNode->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    leashWindowNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;

    leashWindowNode->AddChild(parentNode);
    parentNode->AddChild(childNode);
    leashWindowNode->SetIsOnTheTree(true);
    parentNode->SetIsOnTheTree(true);
    childNode->SetIsOnTheTree(true);

    childNode->IncreaseHDRNum(HDRComponentType::IMAGE);
    EXPECT_TRUE(childNode->GetHDRPresent());
    childNode->ReduceHDRNum(HDRComponentType::IMAGE);
    EXPECT_FALSE(childNode->GetHDRPresent());

    childNode->IncreaseHDRNum(HDRComponentType::UICOMPONENT);
    EXPECT_TRUE(childNode->GetHDRPresent());
    childNode->ReduceHDRNum(HDRComponentType::UICOMPONENT);
    EXPECT_FALSE(childNode->GetHDRPresent());

    childNode->IncreaseHDRNum(HDRComponentType::EFFECT);
    EXPECT_TRUE(childNode->IsHdrEffectColorGamut());
    childNode->ReduceHDRNum(HDRComponentType::EFFECT);
    EXPECT_FALSE(childNode->IsHdrEffectColorGamut());
    childNode->ReduceHDRNum(HDRComponentType::EFFECT); // different branch if call again
    EXPECT_FALSE(childNode->IsHdrEffectColorGamut());
}

/**
 * @tc.name: UpdateNodeColorSpace001
 * @tc.desc: test UpdateNodeColorSpace with appWindow node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, UpdateNodeColorSpace001, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->InitRenderParams();
    node->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    // subTree colorspace: None/sRGB, colorSpace_: None/sRGB
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    // subTree colorspace: P3, colorSpace_: None/sRGB
    node->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    // subTree colorspace: None/sRGB, colorSpace_: P3
    node->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    node->colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    // subTree colorspace: P3, colorSpace_: P3
    node->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    // subTree colorspace: BT2020, colorSpace_: P3
    node->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);

    // subTree colorspace: sRGB, colorSpace_: BT2020
    node->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    node->colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020;
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
}

/**
 * @tc.name: UpdateNodeColorSpace002
 * @tc.desc: test UpdateNodeColorSpace with selfDrawing node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, UpdateNodeColorSpace002, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->InitRenderParams();
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;

    // subTree colorspace: None/sRGB, colorSpace_: None/sRGB
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    // subTree colorspace: P3, colorSpace_: None/sRGB
    node->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    // subTree colorspace: None/sRGB, colorSpace_: P3
    node->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    node->colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    // subTree colorspace: P3, colorSpace_: P3
    node->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    // subTree colorspace: BT2020, colorSpace_: P3
    node->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);

    // subTree colorspace: sRGB, colorSpace_: BT2020
    node->SetNodeColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    node->colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020;
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);

    node->nodeType_ = RSSurfaceNodeType::DEFAULT;
    auto nodeColorSpace = node->GetNodeColorSpace();
    node->UpdateNodeColorSpace();
    EXPECT_EQ(node->GetNodeColorSpace(), nodeColorSpace);
}

/**
 * @tc.name: CheckIfOcclusionReusable
 * @tc.desc: test results of CheckIfOcclusionReusable
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, CheckIfOcclusionReusable, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    std::queue<NodeId> surfaceNodesIds;
    ASSERT_TRUE(surfaceNode->CheckIfOcclusionReusable(surfaceNodesIds));
    surfaceNodesIds.push(id + 1);
    ASSERT_TRUE(surfaceNode->CheckIfOcclusionReusable(surfaceNodesIds));
    surfaceNodesIds.push(id);
    ASSERT_FALSE(surfaceNode->CheckIfOcclusionReusable(surfaceNodesIds));
}

/**
 * @tc.name: CheckParticipateInOcclusion
 * @tc.desc: test results of CheckParticipateInOcclusion
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, CheckParticipateInOcclusion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    EXPECT_FALSE(node->CheckParticipateInOcclusion(false));
}

/**
 * @tc.name: CheckAndUpdateOpaqueRegion
 * @tc.desc: test results of CheckAndUpdateOpaqueRegion
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, CheckAndUpdateOpaqueRegion, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    RectI screeninfo { 0, 0, 100, 100 };
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    node->CheckAndUpdateOpaqueRegion(screeninfo, screenRotation, true);
    EXPECT_FALSE(node->IsOpaqueRegionChanged());
}

/**
 * @tc.name: UpdateChildrenFilterRects
 * @tc.desc: test results of UpdateChildrenFilterRects
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, UpdateChildrenFilterRects, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id);
    std::shared_ptr<RSRenderNode> filterNode = std::make_shared<RSRenderNode>(id + 1);
    RectI rect { 0, 0, 20, 20 };
    bool cacheValid = true;
    testNode->ResetChildrenFilterRects();
    testNode->UpdateChildrenFilterRects(filterNode, rect, cacheValid);
    testNode->UpdateChildrenFilterRects(filterNode, RectI(), cacheValid);
    const std::vector<RectI>& filterRects = testNode->GetChildrenNeedFilterRects();
    ASSERT_EQ(filterRects.size(), 1);
}

/**
 * @tc.name: GetOriAncoForceDoDirect
 * @tc.desc: test results of GetOriAncoForceDoDirect
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, GetOriAncoForceDoDirect, TestSize.Level1)
{
    RSSurfaceRenderNode::SetAncoForceDoDirect(false);
    EXPECT_FALSE(RSSurfaceRenderNode::GetOriAncoForceDoDirect());
}

/**
 * @tc.name: SetAncoFlags
 * @tc.desc: test results of SetAncoFlags
 * @tc.type: FUNC
 * @tc.require: issueIARZ3Q
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetAncoFlags, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id);
    NodeId idTop = 1;
    testNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(testNode->stagingRenderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
 
    testNode->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));
    EXPECT_EQ(testNode->isForceRefresh_, false);
    EXPECT_EQ(surfaceParams->isForceRefresh_, false);
    testNode->SetAncoFlags(static_cast<uint32_t>(AncoFlags::ANCO_NATIVE_NODE));
    EXPECT_EQ(testNode->isForceRefresh_, true);
    EXPECT_EQ(surfaceParams->isForceRefresh_, true);
}

/**
 * @tc.name: SetTunnelLayerId
 * @tc.desc: test results of SetTunnelLayerId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetTunnelLayerId, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    ASSERT_EQ(testNode->GetTunnelLayerId(), 0);
    testNode->SetTunnelLayerId(1);
    ASSERT_EQ(testNode->GetTunnelLayerId(), 1);
}
 
/**
 * @tc.name: IsHardwareForcedDisabled001
 * @tc.desc: test results of IsHardwareForcedDisabled001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, IsHardwareForcedDisabled001, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    ASSERT_EQ(testNode->GetTunnelLayerId(), 0);
 
    testNode->SetTunnelLayerId(1);
    ASSERT_EQ(testNode->GetTunnelLayerId(), 1);
    ASSERT_EQ(testNode->IsHardwareForcedDisabled(), false);
}

/**
 * @tc.name: SetStencilVal
 * @tc.desc: test SetStencilVal
 * @tc.type: FUNC
 * @tc.require: issueIBO35Y
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetStencilVal, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->SetStencilVal(-1);
}

/**
 * @tc.name: CheckUpdateHwcNodeLayerInfo
 * @tc.desc: test results of CheckUpdateHwcNodeLayerInfo
 * @tc.type: FUNC
 * @tc.require: issueIAKCYI
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, CheckUpdateHwcNodeLayerInfo, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    RectI screeninfo;
    RectI absRect;
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    bool isFocusWindow = true;
    Vector4<int> cornerRadius;
    ASSERT_FALSE(node->CheckOpaqueRegionBaseInfo(screeninfo, absRect, screenRotation, isFocusWindow, cornerRadius));
    bool hasContainer = true;
    node->containerConfig_.Update(hasContainer, rrect);

    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    node->addedToPendingSyncList_ = true;
    node->isHardwareForcedDisabled_ = false;
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_BUTT;

    RSLayerInfo layerInfo;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(node->stagingRenderParams_.get());
    surfaceParams->SetLayerInfo(layerInfo);
    node->UpdateHwcNodeLayerInfo(transform);
    auto layer = node->stagingRenderParams_->GetLayerInfo();
    ASSERT_TRUE(layer.layerType == GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
}

/**
 * @tc.name: BufferClearCallbackProxy
 * @tc.desc: test results of BufferClearCallbackProxy
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, BufferClearCallbackProxy, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    ASSERT_NE(remoteObject, nullptr);
    sptr<RSBufferClearCallbackProxy> callback = new RSBufferClearCallbackProxy(remoteObject);
    ASSERT_NE(callback, nullptr);
    testNode->RegisterBufferClearListener(callback);
    testNode->SetNotifyRTBufferAvailable(true);
    ASSERT_TRUE(testNode->isNotifyRTBufferAvailable_);
}

/**
 * @tc.name: MarkBlurIntersectDRMTest
 * @tc.desc: test if node could be marked BlurIntersectWithDRM correctly
 * @tc.type: FUNC
 * @tc.require: issuesIAQZ4I
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, MarkBlurIntersectDRMTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->MarkBlurIntersectWithDRM(true, true);
}

/**
 * @tc.name: SetNeedCacheSurface
 * @tc.desc: test if node could be marked NeedCacheSurface correctly
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetNeedCacheSurface, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    testNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    ASSERT_NE(testNode->stagingRenderParams_, nullptr);

    testNode->SetNeedCacheSurface(true);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(testNode->stagingRenderParams_.get());
    ASSERT_TRUE(surfaceParams->GetNeedCacheSurface());

    testNode->SetNeedCacheSurface(false);
    surfaceParams = static_cast<RSSurfaceRenderParams*>(testNode->stagingRenderParams_.get());
    ASSERT_FALSE(surfaceParams->GetNeedCacheSurface());
}

/**
 * @tc.name: SetForceDisableClipHoleForDRM
 * @tc.desc: test if node could be forced to disable cliphole for DRM correctly
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetForceDisableClipHoleForDRM, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    testNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    ASSERT_NE(testNode->stagingRenderParams_, nullptr);

    testNode->SetForceDisableClipHoleForDRM(true);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(testNode->stagingRenderParams_.get());
    ASSERT_TRUE(surfaceParams->GetForceDisableClipHoleForDRM());

    testNode->SetForceDisableClipHoleForDRM(false);
    surfaceParams = static_cast<RSSurfaceRenderParams*>(testNode->stagingRenderParams_.get());
    ASSERT_FALSE(surfaceParams->GetForceDisableClipHoleForDRM());
}

/**
 * @tc.name: ResetIsBufferFlushed
 * @tc.desc: test if node could be marked IsBufferFlushed correctly
 * @tc.type: FUNC
 * @tc.require: issueIBEBTA
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, ResetIsBufferFlushed, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    testNode->stagingRenderParams_ = nullptr;
    ASSERT_EQ(testNode->stagingRenderParams_, nullptr);
    testNode->ResetIsBufferFlushed();

    testNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    ASSERT_NE(testNode->stagingRenderParams_, nullptr);

    testNode->ResetIsBufferFlushed();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(testNode->stagingRenderParams_.get());
    ASSERT_FALSE(surfaceParams->GetIsBufferFlushed());
}

/**
 * @tc.name: ResetSurfaceNodeStates
 * @tc.desc: test if node could Reset Surface Node States correctly
 * @tc.type: FUNC
 * @tc.require: #IBZ3UR
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, ResetSurfaceNodeStates, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    testNode->stagingRenderParams_ = nullptr;
    ASSERT_EQ(testNode->stagingRenderParams_, nullptr);
    testNode->ResetSurfaceNodeStates();

    testNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    ASSERT_NE(testNode->stagingRenderParams_, nullptr);

    testNode->ResetSurfaceNodeStates();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(testNode->stagingRenderParams_.get());
    ASSERT_FALSE(surfaceParams->GetIsBufferFlushed());
    ASSERT_FALSE(testNode->GetAnimateState());
    ASSERT_FALSE(testNode->IsRotating());
    ASSERT_FALSE(testNode->IsSpecialLayerChanged());
}

/**
 * @tc.name: DealWithDrawBehindWindowTransparentRegion
 * @tc.desc: DealWithDrawBehindWindowTransparentRegion, without such effect, noting updated.
 * @tc.type: FUNC
 * @tc.require: issueIBJJRI
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, DealWithDrawBehindWindowTransparentRegion001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    auto regionBeforeProcess = Occlusion::Region{Occlusion::Rect{defaultLargeRect}};
    testNode->opaqueRegion_ = regionBeforeProcess;
    testNode->DealWithDrawBehindWindowTransparentRegion();
    ASSERT_TRUE(regionBeforeProcess.Sub(testNode->opaqueRegion_).IsEmpty());
}

/**
 * @tc.name: DealWithDrawBehindWindowTransparentRegion
 * @tc.desc: DealWithDrawBehindWindowTransparentRegion, filter rect should be subtract from opaque region.
 * @tc.type: FUNC
 * @tc.require: issueIBJJRI
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, DealWithDrawBehindWindowTransparentRegion002, TestSize.Level1)
{
    auto testNode = std::make_shared<MockRSSurfaceRenderNode>(id);
    ASSERT_NE(testNode, nullptr);

    auto regionBeforeProcess = Occlusion::Region{Occlusion::Rect{defaultLargeRect}};
    testNode->opaqueRegion_ = regionBeforeProcess;
    testNode->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    testNode->drawBehindWindowRegion_ = defaultSmallRect;
    testNode->childrenBlurBehindWindow_ = { INVALID_NODEID };

    EXPECT_CALL(*testNode, NeedDrawBehindWindow()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*testNode, GetFilterRect()).WillRepeatedly(testing::Return(defaultSmallRect));

    testNode->DealWithDrawBehindWindowTransparentRegion();
    ASSERT_FALSE(regionBeforeProcess.Sub(testNode->opaqueRegion_).IsEmpty());
}

/**
 * @tc.name: UpdateVirtualScreenWhiteListInfo
 * @tc.desc: test UpdateVirtualScreenWhiteListInfo.
 * @tc.type: FUNC
 * @tc.require: issueICF7P6
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, UpdateVirtualScreenWhiteListInfo, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    std::shared_ptr<RSSurfaceRenderNode> parent = nullptr;
    node->SetParent(parent);
    node->SetLeashPersistentId(id + 1);
    ASSERT_EQ(node->parent_.lock(), nullptr);
    std::unordered_map<ScreenId, std::unordered_set<uint64_t>> allWhiteListInfo;
    ScreenId screenId = 1;
    allWhiteListInfo[screenId] = {node->GetId()};
    node->UpdateVirtualScreenWhiteListInfo(allWhiteListInfo);
    parent = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    node->SetParent(parent);
    ASSERT_NE(node->parent_.lock(), nullptr);
    allWhiteListInfo[screenId] = {node->GetLeashPersistentId()};
    node->UpdateVirtualScreenWhiteListInfo(allWhiteListInfo);

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->RSRenderNode::UpdateVirtualScreenWhiteListInfo();

    parent->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    parent->UpdateVirtualScreenWhiteListInfo(allWhiteListInfo);
}

/**
 * @tc.name: GetSourceDisplayRenderNodeId
 * @tc.desc: test Set/GetSourceDisplayRenderNodeId.
 * @tc.type: FUNC
 * @tc.require: issueIBJJRI
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, GetSourceDisplayRenderNodeId, TestSize.Level1)
{
    auto testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(testNode, nullptr);
    NodeId sourceDisplayRenderNodeId = 1;
    testNode->SetSourceScreenRenderNodeId(sourceDisplayRenderNodeId);
    ASSERT_EQ(testNode->GetSourceDisplayRenderNodeId(), sourceDisplayRenderNodeId);
}

/**
 * @tc.name: SetTopLayerZOrderTest
 * @tc.desc: Test SetTopLayerZOrder
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetTopLayerZOrderTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->isLayerTop_ = false;
    node->SetTopLayerZOrder(1);
    EXPECT_NE(node->GetTopLayerZOrder(), 1);

    node->isLayerTop_ = true;
    node->SetTopLayerZOrder(1);
    EXPECT_EQ(node->GetTopLayerZOrder(), 1);
}

/**
 * @tc.name: SetSurfaceBufferOpaqueTest
 * @tc.desc: SetSurfaceBufferOpaque and GetSurfaceBufferOpaque
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetSurfaceBufferOpaqueTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetSurfaceBufferOpaque(true);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    node->SetSurfaceBufferOpaque(true);
    node->SetSurfaceBufferOpaque(true);
    ASSERT_EQ(node->GetSurfaceBufferOpaque(), true);
    ASSERT_EQ(node->GetBlendType(), GraphicBlendType::GRAPHIC_BLEND_NONE);
    node->SetSurfaceBufferOpaque(false);
    ASSERT_FALSE(node->GetSurfaceBufferOpaque());
    ASSERT_EQ(node->GetBlendType(), GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
}

/**
 * @tc.name: SetUIFirstVisibleFilterRectTest
 * @tc.desc: Test set visible filter region to surface node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetUIFirstVisibleFilterRectTest, TestSize.Level1)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    RectI rect { 0, 0, 50, 50 };
    node->addedToPendingSyncList_ = false;
    node->SetUIFirstVisibleFilterRect(rect);
    ASSERT_FALSE(node->addedToPendingSyncList_);

    auto context = std::make_shared<RSContext>();
    node->context_ = context;
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(node->stagingRenderParams_.get());
    ASSERT_TRUE(surfaceParams->GetUifirstVisibleFilterRect().IsEmpty());
    node->SetUIFirstVisibleFilterRect(rect);
    ASSERT_TRUE(node->addedToPendingSyncList_);
    ASSERT_FALSE(surfaceParams->GetUifirstVisibleFilterRect().IsEmpty());
}

/**
 * @tc.name: IsAncestorScreenFrozenTest
 * @tc.desc: IsAncestorScreenFrozen
 * @tc.type:FUNC
 * @tc.require: issue21227
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, IsAncestorScreenFrozenTest, TestSize.Level1)
{
    NodeId id = 10000;
    auto context = std::make_shared<RSContext>();
    ASSERT_NE(context, nullptr);
    auto firstLevelNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(firstLevelNode, nullptr);
    EXPECT_TRUE(context->GetMutableNodeMap().RegisterRenderNode(firstLevelNode));
    EXPECT_FALSE(firstLevelNode->IsAncestorScreenFrozen());
    auto screenNode = std::make_shared<RSScreenRenderNode>(id + 1, 0);
    ASSERT_NE(screenNode, nullptr);
    screenNode->forceFreeze_ = true;
    firstLevelNode->SetAncestorScreenNode(screenNode);
    ASSERT_TRUE(firstLevelNode->IsAncestorScreenFrozen());

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->firstLevelNodeId_ = firstLevelNode->GetId();
    EXPECT_TRUE(surfaceNode->IsAncestorScreenFrozen());
    context->GetMutableNodeMap().UnregisterRenderNode(firstLevelNode->GetId());
}

/**
 * @tc.name: CheckCloneCircleTest
 * @tc.desc: CheckCloneCircle
 * @tc.type:FUNC
 * @tc.require: issue21227
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, CheckCloneCircleTest, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto nodeId = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(nodeId, context);
    EXPECT_TRUE(node1->CheckCloneCircle(node1, node1, *context));

    auto node2 = std::make_shared<RSSurfaceRenderNode>(nodeId + 1, context);
    EXPECT_FALSE(node1->CheckCloneCircle(node1, node2, *context));

    auto node3 = std::make_shared<RSSurfaceRenderNode>(nodeId + 2, context);
    node1->SetParent(node3);
    context->nodeMap.RegisterRenderNode(std::static_pointer_cast<RSBaseRenderNode>(node3));
    EXPECT_FALSE(node1->CheckCloneCircle(node1, node2, *context));

    auto node4 = std::make_shared<RSSurfaceRenderNode>(nodeId + 3, context);
    node2->clonedSourceNodeId_ = nodeId + 3;
    context->nodeMap.RegisterRenderNode(std::static_pointer_cast<RSBaseRenderNode>(node4));
    EXPECT_FALSE(node1->CheckCloneCircle(node1, node2, *context));

    node4->clonedSourceNodeId_ = nodeId + 2;
    EXPECT_TRUE(node1->CheckCloneCircle(node1, node2, *context));
}

/**
+ * @tc.name: SetRelatedTest
+ * @tc.desc: Test set Related filter region to surface node
+ * @tc.type:FUNC
+ * @tc.require: issue21227
+ */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetRelatedTest, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->addedToPendingSyncList_ = false;
    node->SetRelated(true);
    ASSERT_FALSE(node->addedToPendingSyncList_);
    ASSERT_FALSE(node->IsRelated());

    auto context = std::make_shared<RSContext>();
    node->context_ = context;
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    node->SetRelated(true);
    ASSERT_TRUE(node->addedToPendingSyncList_);
    ASSERT_TRUE(node->IsRelated());
}

/**
 * @tc.name: SetRelatedSourceNodeTest
 * @tc.desc: Test set RelatedSourceNode filter region to surface node
 * @tc.type:FUNC
 * @tc.require: issue21227
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetRelatedSourceNodeTest, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->addedToPendingSyncList_ = false;
    node->SetRelatedSourceNode(true);
    ASSERT_FALSE(node->addedToPendingSyncList_);
    ASSERT_FALSE(node->IsRelatedSourceNode());

    auto context = std::make_shared<RSContext>();
    node->context_ = context;
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    node->SetRelatedSourceNode(true);
    ASSERT_TRUE(node->addedToPendingSyncList_);
    ASSERT_TRUE(node->IsRelatedSourceNode());
}

/**
 * @tc.name: SetUifirstStartingWindowIdTest
 * @tc.desc: Test set uifirst starting window id
 * @tc.type:FUNC
 * @tc.require: issue21674
 */
HWTEST_F(RSSurfaceRenderNodeThreeUtilTest, SetUifirstStartingWindowIdTest, TestSize.Level1)
{
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    NodeId startingWindowId = 100;
    node->stagingRenderParams_ = nullptr;
    node->SetUifirstStartingWindowId(startingWindowId);
    EXPECT_EQ(node->GetUifirstStartingWindowId(), INVALID_NODEID);

    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    node->SetUifirstStartingWindowId(startingWindowId);
    EXPECT_EQ(node->GetUifirstStartingWindowId(), startingWindowId);
}
} // namespace Rosen
} // namespace OHOS