/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
class RSSurfaceRenderNodeTest2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline Drawing::Canvas drawingCanvas_;
};

void RSSurfaceRenderNodeTest2::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSSurfaceRenderNodeTest2::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSSurfaceRenderNodeTest2::SetUp() {}
void RSSurfaceRenderNodeTest2::TearDown() {}


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
 * @tc.name: ResetSurfaceContainerRegionTest1
 * @tc.desc: test results of ResetSurfaceContainerRegion
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeTest2, ResetSurfaceContainerRegionTest1, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    RectI screeninfo { 0, 0, 200, 200 };
    RectI absRect { 0, 0, 100, 100 };
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
 * @tc.name: CalcFilterCacheValidForOcclusionTest003
 * @tc.desc: test results of CalcFilterCacheValidForOcclusion
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeTest2, CalcFilterCacheValidForOcclusionTest003, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->isFilterCacheFullyCovered_ = true;
    node->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    node->CalcFilterCacheValidForOcclusion();
    EXPECT_TRUE(node->isFilterCacheStatusChanged_);

    node->isFilterCacheFullyCovered_ = false;
    node->isFilterCacheValidForOcclusion_ = false;
    node->CalcFilterCacheValidForOcclusion();
    EXPECT_FALSE(node->isFilterCacheStatusChanged_);
}

/**
 * @tc.name: UpdateFilterCacheStatusIfNodeStaticTest002
 * @tc.desc: test results of UpdateFilterCacheStatusIfNodeStatic
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeTest2, UpdateFilterCacheStatusIfNodeStaticTest002, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->UpdateFilterCacheStatusIfNodeStatic(RectI(0, 0, 600, 600), true);
    auto node1 = std::make_shared<RSEffectRenderNode>(id + 1);
    node->filterNodes_.emplace_back(node1);
    auto node2 = std::make_shared<RSRenderNode>(id + 2);
    node->filterNodes_.emplace_back(node2);
    auto node3 = std::make_shared<RSRenderNode>(id + 3);
    node3->isOnTheTree_ = true;
    node3->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    node->filterNodes_.emplace_back(node3);
    auto node4 = std::make_shared<RSRenderNode>(id + 4);
    node4->isOnTheTree_ = true;
    node4->GetMutableRenderProperties().needFilter_ = true;
    node4->GetMutableRenderProperties().filter_ = std::make_shared<RSFilter>();
    node->filterNodes_.emplace_back(node4);
    std::shared_ptr<RSRenderNode> node5 = nullptr;
    node->filterNodes_.emplace_back(node5);
    node->dirtyManager_->UpdateCacheableFilterRect({0, 0, 600, 600});
    node->oldDirtyInSurface_ = {0, 0, 10, 10};
    std::shared_ptr<RSRenderNode> node6 = std::make_shared<RSEffectRenderNode>(id + 6);
    node6->isOnTheTree_ = true;
    node6->GetMutableRenderProperties().needFilter_ = true;
    node6->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    node->filterNodes_.emplace_back(node6);
    node->UpdateFilterCacheStatusIfNodeStatic(RectI(0, 0, 600, 600), false);
    ASSERT_NE(node->filterNodes_.size(), 0);
}

/**
 * @tc.name: ProcessRenderAfterChildrenTest002
 * @tc.desc: test results of ProcessRenderAfterChildren
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeTest2, ProcessRenderAfterChildrenTest002, TestSize.Level1)
{
    Drawing::Canvas canvasArgs;
    auto canvas = std::make_shared<RSPaintFilterCanvas>(&canvasArgs);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->ProcessRenderAfterChildren(*canvas);
    EXPECT_FALSE(node->needDrawAnimateProperty_);
}

/**
 * @tc.name: SetContextAlphaTest002
 * @tc.desc: test results of SetContextAlpha
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeTest2, SetContextAlphaTest002, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> testNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    testNode->SetContextAlpha(1.0f, true);
    testNode->SetContextAlpha(0.8f, true);
    testNode->SetContextAlpha(0.6f, false);
    EXPECT_EQ(testNode->contextAlpha_, 0.6f);
}

/**
 * @tc.name: GetFirstLevelNodeId003
 * @tc.desc: Test GetFirstLevelNode for app window node which parent is leash window node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeTest2, GetFirstLevelNodeId003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto id = 0;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(childNode, nullptr);
    auto id2 = id + 1;
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id2, rsContext);
    ASSERT_NE(parentNode, nullptr);

    childNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    NodeId childNodeId = childNode->GetId();
    pid_t childNodePid = ExtractPid(childNodeId);
    NodeId parentNodeId = parentNode->GetId();
    pid_t parentNodePid = ExtractPid(parentNodeId);
    rsContext->GetMutableNodeMap().renderNodeMap_[childNodePid][childNodeId] = childNode;
    rsContext->GetMutableNodeMap().renderNodeMap_[parentNodePid][parentNodeId] = parentNode;

    parentNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    childNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    parentNode->AddChild(childNode);
    parentNode->SetIsOnTheTree(true);
    ASSERT_EQ(childNode->GetFirstLevelNodeId(), parentNode->GetId());
}

/**
 * @tc.name: SetClonedNodeInfoTest002
 * @tc.desc: function test SetClonedNodeInfo
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceRenderNodeTest2, SetClonedNodeInfoTest002, TestSize.Level2)
{
    NodeId id = 5;
    auto context = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    node->SetClonedNodeInfo(id, true, false);
    bool result = node->clonedSourceNodeId_ == id;
    ASSERT_FALSE(result);
    
    NodeId id2 = id + 1;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id2, context);
    context->nodeMap.RegisterRenderNode(std::static_pointer_cast<RSBaseRenderNode>(node2));
    node->SetClonedNodeInfo(id2, true, false);
    result = node->clonedSourceNodeId_ == id2;
    ASSERT_TRUE(result);

    node->clonedSourceNodeId_ = INVALID_NODEID;
    node->SetParent(node2);
    node->SetClonedNodeInfo(id2, true, false);
    result = node->clonedSourceNodeId_ == id2;
    ASSERT_FALSE(result);
}
} // namespace Rosen
} // namespace OHOS