/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <memory>

#include "common/rs_common_hook.h"
#include "gtest/gtest.h"
#include "limit_number.h"
#include "mock/mock_matrix.h"
#include "rs_test_util.h"
#include "system/rs_system_parameters.h"

#include "consumer_surface.h"
#include "draw/color.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_visitor.h"

using namespace testing;
using namespace testing::ext;

namespace {
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
    const OHOS::Rosen::RectI DEFAULT_RECT = {0, 80, 1000, 1000};
    const OHOS::Rosen::RectI DEFAULT_FILTER_RECT = {0, 0, 500, 500};
    const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
    constexpr int MAX_ALPHA = 255;
    constexpr OHOS::Rosen::NodeId DEFAULT_NODE_ID = 100;
}

namespace OHOS::Rosen {
class RSUniRenderVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Mock::MatrixMock* matrixMock_;
};

void RSUniRenderVisitorTest::SetUpTestCase()
{
    matrixMock_ = Mock::MatrixMock::GetInstance();
    EXPECT_CALL(*matrixMock_, GetMinMaxScales(_)).WillOnce(testing::Return(false));
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderVisitorTest::TearDownTestCase() {}
void RSUniRenderVisitorTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
}
void RSUniRenderVisitorTest::TearDown()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
    system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

/**
 * @tc.name: AfterUpdateSurfaceDirtyCalc_001
 * @tc.desc: AfterUpdateSurfaceDirtyCalc Test, property.GetBoundsGeometry() is null, expect false
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, AfterUpdateSurfaceDirtyCalc_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    auto& property = node->renderContent_->renderProperties_;
    property.boundsGeo_ = nullptr;
    ASSERT_EQ(property.GetBoundsGeometry(), nullptr);

    ASSERT_FALSE(rsUniRenderVisitor->AfterUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: AfterUpdateSurfaceDirtyCalc_002
 * @tc.desc: AfterUpdateSurfaceDirtyCalc Test，property.GetBoundsGeometry() not null
 * node.IsHardwareEnabledType() && node.GetZorderChanged() && curSurfaceNode_ is true
 * node is not LeashOrMainWindow and not MainWindowType
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, AfterUpdateSurfaceDirtyCalc_002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    ASSERT_NE(node->renderContent_, nullptr);
    auto& property = node->renderContent_->renderProperties_;
    property.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    ASSERT_NE(node->GetRenderProperties().GetBoundsGeometry(), nullptr);

    node->isHardwareEnabledNode_ = true;
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    ASSERT_TRUE(node->IsHardwareEnabledType());
    node->zOrderChanged_ = true;
    ASSERT_TRUE(node->GetZorderChanged());
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    ASSERT_TRUE((node->IsHardwareEnabledType() && node->GetZorderChanged() && rsUniRenderVisitor->curSurfaceNode_));

    node->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    ASSERT_FALSE(node->IsLeashOrMainWindow());
    ASSERT_FALSE(node->IsMainWindowType());

    ASSERT_TRUE(rsUniRenderVisitor->AfterUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: AfterUpdateSurfaceDirtyCalc_003
 * @tc.desc: AfterUpdateSurfaceDirtyCalc Test，property.GetBoundsGeometry() not null
 * node.IsHardwareEnabledType() && node.GetZorderChanged() && curSurfaceNode_ is true
 * node is LeashOrMainWindow and is not MainWindowType
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, AfterUpdateSurfaceDirtyCalc_003, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    ASSERT_NE(node->renderContent_, nullptr);
    auto& property = node->renderContent_->renderProperties_;
    property.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    ASSERT_NE(node->GetRenderProperties().GetBoundsGeometry(), nullptr);

    node->isHardwareEnabledNode_ = true;
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    ASSERT_TRUE(node->IsHardwareEnabledType());
    node->zOrderChanged_ = true;
    ASSERT_TRUE(node->GetZorderChanged());
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    ASSERT_TRUE((node->IsHardwareEnabledType() && node->GetZorderChanged() && rsUniRenderVisitor->curSurfaceNode_));

    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ASSERT_TRUE(node->IsLeashOrMainWindow());
    ASSERT_FALSE(node->IsMainWindowType());

    ASSERT_TRUE(rsUniRenderVisitor->AfterUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: UpdateLeashWindowVisibleRegionEmpty_001
 * @tc.desc: UpdateLeashWindowVisibleRegionEmpty Test, node IsLeashWindow,
 * childSurfaceNode not null and  childSurfaceNode->IsAppWindow() is true
 * childSurfaceNode->GetVisibleRegion().IsEmpty() is false
 * expect isLeashWindowVisibleRegionEmpty_ = false
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateLeashWindowVisibleRegionEmpty_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ASSERT_TRUE(node->IsLeashWindow());

    auto child = std::make_shared<RSSurfaceRenderNode>(id + 1);
    node->AddChild(child);
    node->GenerateFullChildrenList();
    ASSERT_NE((*(node->GetSortedChildren())).size(), 0);

    auto childSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
    ASSERT_NE(childSurfaceNode, nullptr);
    childSurfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_TRUE(childSurfaceNode->IsAppWindow());
    childSurfaceNode->visibleRegion_.rects_.resize(1);
    childSurfaceNode->visibleRegion_.bound_ = OHOS::Rosen::Occlusion::Rect(1, 1, 2, 2);
    ASSERT_FALSE(childSurfaceNode->GetVisibleRegion().IsEmpty());

    node->isLeashWindowVisibleRegionEmpty_ = true;
    rsUniRenderVisitor->UpdateLeashWindowVisibleRegionEmpty(*node);
    ASSERT_FALSE(node->isLeashWindowVisibleRegionEmpty_);
}

/**
 * @tc.name: UpdateLeashWindowVisibleRegionEmpty_002
 * @tc.desc: UpdateLeashWindowVisibleRegionEmpty Test, node IsLeashWindow,
 * childSurfaceNode not null and  childSurfaceNode->IsAppWindow() is true
 * childSurfaceNode->GetVisibleRegion().IsEmpty() is true
 * expect isLeashWindowVisibleRegionEmpty_ = true
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateLeashWindowVisibleRegionEmpty_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ASSERT_TRUE(node->IsLeashWindow());

    auto child = std::make_shared<RSSurfaceRenderNode>(id + 1);
    node->AddChild(child);
    node->GenerateFullChildrenList();
    ASSERT_NE((*(node->GetSortedChildren())).size(), 0);

    auto childSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
    ASSERT_NE(childSurfaceNode, nullptr);
    childSurfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_TRUE(childSurfaceNode->IsAppWindow());
    childSurfaceNode->visibleRegion_.rects_.resize(0);
    childSurfaceNode->visibleRegion_.bound_ = OHOS::Rosen::Occlusion::Rect();
    ASSERT_TRUE(childSurfaceNode->GetVisibleRegion().IsEmpty());

    node->isLeashWindowVisibleRegionEmpty_ = false;
    rsUniRenderVisitor->UpdateLeashWindowVisibleRegionEmpty(*node);
    ASSERT_TRUE(node->isLeashWindowVisibleRegionEmpty_);
}

/**
 * @tc.name: UpdateLeashWindowVisibleRegionEmpty_003
 * @tc.desc: UpdateLeashWindowVisibleRegionEmpty Test, node IsLeashWindow,
 * childSurfaceNode not null and  childSurfaceNode->IsAppWindow() is false
 * expect isLeashWindowVisibleRegionEmpty_ = false
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateLeashWindowVisibleRegionEmpty_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ASSERT_TRUE(node->IsLeashWindow());

    auto child = std::make_shared<RSSurfaceRenderNode>(id + 1);
    node->AddChild(child);
    node->GenerateFullChildrenList();
    ASSERT_NE((*(node->GetSortedChildren())).size(), 0);

    auto childSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
    ASSERT_NE(childSurfaceNode, nullptr);
    childSurfaceNode->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    ASSERT_FALSE(childSurfaceNode->IsAppWindow());

    node->isLeashWindowVisibleRegionEmpty_ = true;
    rsUniRenderVisitor->UpdateLeashWindowVisibleRegionEmpty(*node);
    ASSERT_FALSE(node->isLeashWindowVisibleRegionEmpty_);
}

/**
 * @tc.name: UpdateLeashWindowVisibleRegionEmpty_004
 * @tc.desc: UpdateLeashWindowVisibleRegionEmpty Test, node IsLeashWindow,
 * childSurfaceNode is null
 * expect isLeashWindowVisibleRegionEmpty_ = false
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateLeashWindowVisibleRegionEmpty_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ASSERT_TRUE(node->IsLeashWindow());

    const std::vector<std::shared_ptr<RSRenderNode>> constChildrenList { nullptr, nullptr };
    node->fullChildrenList_ = std::make_shared<const std::vector<std::shared_ptr<RSRenderNode>>>(constChildrenList);

    auto& childrenList = *(node->GetSortedChildren());
    ASSERT_NE(childrenList.size(), 0);
    for (const auto& child : childrenList) {
        auto childSurfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
        ASSERT_EQ(childSurfaceNode, nullptr);
    }

    node->isLeashWindowVisibleRegionEmpty_ = true;
    rsUniRenderVisitor->UpdateLeashWindowVisibleRegionEmpty(*node);
    ASSERT_FALSE(node->isLeashWindowVisibleRegionEmpty_);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode_001
 * @tc.desc: UpdateHwcNodeInfoForAppNode Test, node.GetNeedCollectHwcNode() is true
 * node.IsHardwareEnabledType() is false
 * curSurfaceNode_ is nullptr
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->needCollectHwcNode_ = true;
    ASSERT_TRUE(node->GetNeedCollectHwcNode());
    ASSERT_FALSE(node->IsHardwareEnabledType());
    rsUniRenderVisitor->curSurfaceNode_ = nullptr;

    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*node);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode_002
 * @tc.desc: UpdateHwcNodeInfoForAppNode Test, node.GetNeedCollectHwcNode() is true
 * node.IsHardwareEnabledType() is true
 * curSurfaceNode_ is nullptr
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->needCollectHwcNode_ = true;
    ASSERT_TRUE(node->GetNeedCollectHwcNode());
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->isHardwareEnabledNode_ = true;
    ASSERT_TRUE(node->IsHardwareEnabledType());
    rsUniRenderVisitor->curSurfaceNode_ = nullptr;

    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*node);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode_003
 * @tc.desc: UpdateHwcNodeInfoForAppNode Test, node.GetNeedCollectHwcNode() is true
 * node.IsHardwareEnabledType() is false
 * curSurfaceNode_ is not nullptr
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->needCollectHwcNode_ = true;
    ASSERT_TRUE(node->GetNeedCollectHwcNode());
    ASSERT_FALSE(node->IsHardwareEnabledType());
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);

    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*node);
}

/**
 * @tc.name: UpdateHwcNodeByTransform_001
 * @tc.desc: UpdateHwcNodeByTransform Test, buffer of RSSurfaceHandler is not nullptr, and
 * consumer_ of of RSSurfaceHandler is nullptr, expect return directly
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeByTransform_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(!node->GetRSSurfaceHandler() || !node->GetRSSurfaceHandler()->GetBuffer());

    node->surfaceHandler_->consumer_ = nullptr;
    ASSERT_EQ(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    rsUniRenderVisitor->UpdateHwcNodeByTransform(*node);
}

/**
 * @tc.name: UpdateHwcNodeByTransform_002
 * @tc.desc: UpdateHwcNodeByTransform Test, buffer of RSSurfaceHandler is not nullptr, and
 * consumer_ of of RSSurfaceHandler is not nullptr, and GetScalingMode is GSERROR_OK
 * scalingMode == ScalingMode::SCALING_MODE_SCALE_TO_WINDOW, expect neither LayerScaleDown nor LayerScaleFit
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeByTransform_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(!node->GetRSSurfaceHandler() || !node->GetRSSurfaceHandler()->GetBuffer());

    const auto& surface = node->GetRSSurfaceHandler()->GetConsumer();
    ASSERT_NE(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    auto nodeParams = static_cast<RSSurfaceRenderParams*>(node->GetStagingRenderParams().get());
    ASSERT_NE(nodeParams, nullptr);
    ScalingMode preScalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    nodeParams->SetPreScalingMode(preScalingMode);
    ASSERT_EQ(nodeParams->GetPreScalingMode(), SCALING_MODE_SCALE_TO_WINDOW); // SetPreScalingMode succeed
    const auto& buffer = node->GetRSSurfaceHandler()->GetBuffer();

    rsUniRenderVisitor->UpdateHwcNodeByTransform(*node);
    ASSERT_EQ(preScalingMode, SCALING_MODE_SCALE_TO_WINDOW);
}

/**
 * @tc.name: UpdateHwcNodeByTransform_003
 * @tc.desc: UpdateHwcNodeByTransform Test, buffer of RSSurfaceHandler is not nullptr, and
 * consumer_ of of RSSurfaceHandler is not nullptr, and GetScalingMode is GSERROR_INVALID_ARGUMENTS
 * scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP, expect LayerScaleDown
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeByTransform_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(!node->GetRSSurfaceHandler() || !node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_NE(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    auto nodeParams = static_cast<RSSurfaceRenderParams*>(node->GetStagingRenderParams().get());
    ASSERT_NE(nodeParams, nullptr);

    ScalingMode preScalingMode = ScalingMode::SCALING_MODE_SCALE_CROP;
    nodeParams->SetPreScalingMode(preScalingMode);
    auto& buffer = node->surfaceHandler_->buffer_.buffer;
    auto surface = static_cast<ConsumerSurface*>(node->surfaceHandler_->consumer_.refs_);
    ASSERT_NE(surface, nullptr);

    surface->consumer_ = nullptr;
    ASSERT_EQ(surface->GetScalingMode(buffer->GetSeqNum(), preScalingMode), GSERROR_INVALID_ARGUMENTS);
    ASSERT_EQ(nodeParams->GetPreScalingMode(), ScalingMode::SCALING_MODE_SCALE_CROP);

    rsUniRenderVisitor->UpdateHwcNodeByTransform(*node);
    ASSERT_EQ(nodeParams->GetPreScalingMode(), ScalingMode::SCALING_MODE_SCALE_CROP);
}

/**
 * @tc.name: UpdateHwcNodeByTransform_004
 * @tc.desc: UpdateHwcNodeByTransform Test, buffer of RSSurfaceHandler is not nullptr, and
 * consumer_ of of RSSurfaceHandler is not nullptr, and GetScalingMode is GSERROR_INVALID_ARGUMENTS
 * scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT, expect LayerScaleFit
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeByTransform_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(!node->GetRSSurfaceHandler() || !node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_NE(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    auto nodeParams = static_cast<RSSurfaceRenderParams*>(node->GetStagingRenderParams().get());
    ASSERT_NE(nodeParams, nullptr);

    ScalingMode preScalingMode = ScalingMode::SCALING_MODE_SCALE_FIT;
    nodeParams->SetPreScalingMode(preScalingMode);
    auto& buffer = node->surfaceHandler_->buffer_.buffer;
    auto surface = static_cast<ConsumerSurface*>(node->surfaceHandler_->consumer_.refs_);
    ASSERT_NE(surface, nullptr);

    surface->consumer_ = nullptr;
    ASSERT_EQ(surface->GetScalingMode(buffer->GetSeqNum(), preScalingMode), GSERROR_INVALID_ARGUMENTS);
    ASSERT_EQ(nodeParams->GetPreScalingMode(), ScalingMode::SCALING_MODE_SCALE_FIT);

    rsUniRenderVisitor->UpdateHwcNodeByTransform(*node);
    ASSERT_EQ(nodeParams->GetPreScalingMode(), ScalingMode::SCALING_MODE_SCALE_FIT);
}

/*
 * @tc.name: UpdateSurfaceRenderNodeScaleTest
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateSurfaceRenderNodeScaleTest
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceRenderNodeScaleTest, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 11;     // leash window config id
    config.name = "leashWindowNode";
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(leashWindowNode, nullptr);
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    auto deviceTypeStr = system::GetParameter("const.product.devicetype", "pc");
    system::SetParameter("const.product.devicetype", "pc");
    RSMainThread::Instance()->SetDeviceType();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateSurfaceRenderNodeScale(*leashWindowNode);
    system::SetParameter("const.product.devicetype", deviceTypeStr);
    RSMainThread::Instance()->SetDeviceType();
}

/*
 * @tc.name: CheckLuminanceStatusChangeTest
 * @tc.desc: Test RSUniRenderVisitorTest.CheckLuminanceStatusChangeTest
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, CheckLuminanceStatusChangeTest, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSMainThread::Instance()->SetLuminanceChangingStatus(true);
    ASSERT_EQ(rsUniRenderVisitor->CheckLuminanceStatusChange(), true);
    RSMainThread::Instance()->SetLuminanceChangingStatus(false);
    ASSERT_EQ(rsUniRenderVisitor->CheckLuminanceStatusChange(), false);
}

/*
 * @tc.name: PrepareCanvasRenderNodeTest
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareCanvasRenderNodeTest
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareCanvasRenderNodeTest, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsCanvasRenderNode->GetId());

    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "scbScreenNode";
    auto scbScreenNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(scbScreenNode, nullptr);
    scbScreenNode->SetSurfaceNodeType(RSSurfaceNodeType::SCB_SCREEN_NODE);
    scbScreenNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(scbScreenNode->GetId());

    config.id = 11;
    config.name = "leashWindowNode";
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(leashWindowNode, nullptr);
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    leashWindowNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(leashWindowNode->GetId());

    rsCanvasRenderNode->SetParent(std::weak_ptr<RSSurfaceRenderNode>(leashWindowNode));
    rsUniRenderVisitor->PrepareCanvasRenderNode(*rsCanvasRenderNode);

    rsCanvasRenderNode->SetParent(std::weak_ptr<RSSurfaceRenderNode>(scbScreenNode));
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    rsUniRenderVisitor->curDisplayDirtyManager_ = rsDisplayRenderNode->GetDirtyManager();
    rsUniRenderVisitor->currentVisitDisplay_ = 0;
    rsUniRenderVisitor->PrepareCanvasRenderNode(*rsCanvasRenderNode);
}

/*
 * @tc.name: PrepareChildren001
 * @tc.desc: PrepareChildren Test
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareChildren001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    rsBaseRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareChildren(*rsBaseRenderNode);
}

/*
 * @tc.name: DrawAllSurfaceOpaqueRegionForDFX001
 * @tc.desc: DrawAllSurfaceOpaqueRegionForDFX Test
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, DrawAllSurfaceOpaqueRegionForDFX001, TestSize.Level1)
{
    system::SetParameter("rosen.uni.opaqueregiondebug", "1");
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 0, 10, 10));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    rsDisplayRenderNode->SetScreenId(0);
    auto rsScreen = std::make_unique<impl::RSScreen>(0, false, HdiOutput::CreateHdiOutput(0), nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    system::SetParameter("rosen.uni.opaqueregiondebug", "0");
}

/*
 * @tc.name: DrawTargetSurfaceDirtyRegionForDFX001
 * @tc.desc: DrawTargetSurfaceDirtyRegionForDFX Test
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, DrawTargetSurfaceDirtyRegionForDFX001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.name = "SurfaceDirtyDFX";
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 0, 10, 10));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    rsDisplayRenderNode->SetScreenId(0);
    auto rsScreen = std::make_unique<impl::RSScreen>(0, false, HdiOutput::CreateHdiOutput(0), nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    system::SetParameter("rosen.dirtyregiondebug.surfacenames", "SurfaceDirtyDFX");
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    system::SetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

HWTEST_F(RSUniRenderVisitorTest, PrepareProxyRenderNode001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    std::weak_ptr<RSSurfaceRenderNode> rsSurfaceRenderNodeW(rsSurfaceRenderNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto rsContext = std::make_shared<RSContext>();

    NodeId id = 0;
    NodeId targetID = 0;
    std::shared_ptr<RSProxyRenderNode> rsProxyRenderNode(
        new RSProxyRenderNode(id, rsSurfaceRenderNodeW, targetID, rsContext->weak_from_this()));
    rsUniRenderVisitor->PrepareProxyRenderNode(*rsProxyRenderNode);
    rsUniRenderVisitor->ProcessProxyRenderNode(*rsProxyRenderNode);

    config.id = 1;
    auto rsSurfaceRenderNodeS = std::make_shared<RSSurfaceRenderNode>(config);
    rsSurfaceRenderNodeS->AddChild(rsProxyRenderNode, 1);
    rsProxyRenderNode->Prepare(rsUniRenderVisitor);
}

HWTEST_F(RSUniRenderVisitorTest, RSDisplayRenderNode001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(0, config, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsDisplayRenderNode->Prepare(rsUniRenderVisitor);
    rsDisplayRenderNode->Process(rsUniRenderVisitor);
}

/*
 * @tc.name: CheckQuickSkipPrepareParamSetAndGetValid001
 * @tc.desc: Check if param set and get apis are valid.
 * @tc.type: FUNC
 * @tc.require: I67FKA
*/
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipPrepareParamSetAndGetValid001, TestSize.Level1)
{
    int defaultParam = (int)RSSystemParameters::GetQuickSkipPrepareType();
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "0");
    int param = (int)RSSystemParameters::GetQuickSkipPrepareType();
    ASSERT_EQ(param, 0);
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "1");
    param = (int)RSSystemParameters::GetQuickSkipPrepareType();
    ASSERT_EQ(param, 1);

    NodeId testId = 10;
    pid_t pid = ExtractPid(testId);
    ASSERT_EQ(pid, 0);
    const int paddingDigit = 32;
    NodeId testPid = testId << paddingDigit;
    pid = ExtractPid(testPid);
    ASSERT_EQ(pid, testId);
    (void)system::SetParameter("rosen.quickskipprepare.enabled", std::to_string(defaultParam));
}

/*
 * @tc.name: CheckSurfaceRenderNodeNotStatic001
 * @tc.desc: Generate not static surface render node(self drawing, leash window) and execute preparation step.
 *           Get trace and check corresponding node's preparation exists and no 'Skip' info.
 * @tc.type: FUNC
 * @tc.require: I67FKA
*/
HWTEST_F(RSUniRenderVisitorTest, CheckSurfaceRenderNodeNotStatic001, TestSize.Level1)
{
    int defaultParam = (int)RSSystemParameters::GetQuickSkipPrepareType();
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "1");

    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "selfDrawTestNode";
    auto selfDrawSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    selfDrawSurfaceRenderNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    selfDrawSurfaceRenderNode->InitRenderParams();
    config.id = 11;
    config.name = "leashWindowTestNode";
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    leashWindowNode->InitRenderParams();

    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(12, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsDisplayRenderNode->AddChild(selfDrawSurfaceRenderNode, -1);
    rsDisplayRenderNode->AddChild(leashWindowNode, -1);
    // execute add child
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    // test if skip testNode
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    (void)system::SetParameter("rosen.quickskipprepare.enabled", std::to_string(defaultParam));
}

/*
 * @tc.name: CheckSurfaceRenderNodeStatic001
 * @tc.desc: Generate static surface render node(app window node) and execute preparation step.
 *           Get trace and check corresponding node's preparation and 'Skip' info exist.
 * @tc.type: FUNC
 * @tc.require: I67FKA
*/
HWTEST_F(RSUniRenderVisitorTest, CheckSurfaceRenderNodeStatic001, TestSize.Level1)
{
    int defaultParam = (int)RSSystemParameters::GetQuickSkipPrepareType();
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "1");

    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "appWindowTestNode";
    auto defaultSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    defaultSurfaceRenderNode->InitRenderParams();
    defaultSurfaceRenderNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsDisplayRenderNode->AddChild(defaultSurfaceRenderNode, -1);
    // execute add child
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    // test if skip testNode
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    (void)system::SetParameter("rosen.quickskipprepare.enabled", std::to_string(defaultParam));
}

HWTEST_F(RSUniRenderVisitorTest, PrepareRootRenderNode001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSRootRenderNode>(0, rsContext->weak_from_this());
    rsRootRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);

    auto& property = rsRootRenderNode->GetMutableRenderProperties();
    property.SetVisible(false);
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
    rsUniRenderVisitor->ProcessRootRenderNode(*rsRootRenderNode);
}

/*
 * @tc.name: CalcDirtyDisplayRegion
 * @tc.desc: Set surface to transparent, add a canvas node to create a transparent dirty region
 * @tc.type: FUNC
 * @tc.require: I68IPR
*/
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyDisplayRegion, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    Occlusion::Rect rect{0, 80, 2560, 1600};
    Occlusion::Region region{rect};
    VisibleData vData;
    std::map<NodeId, RSVisibleLevel> pidVisMap;

    auto partialRenderType = RSSystemProperties::GetUniPartialRenderEnabled();
    auto isPartialRenderEnabled = (partialRenderType != PartialRenderType::DISABLED);
    ASSERT_EQ(isPartialRenderEnabled, true);

    // set surface to transparent, add a canvas node to create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsSurfaceRenderNode->SetVisibleRegionRecursive(region, vData, pidVisMap);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
}

/*
 * @tc.name: CalcDirtyRegionForFilterNode
 * @tc.desc: Create a filter effect to test filter node
 * @tc.type: FUNC
 * @tc.require: I68IPR
*/
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyRegionForFilterNode, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    // create a filter effect
    float blurRadiusX = 30.0f;
    float blurRadiusY = 30.0f;
    auto filter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    rsCanvasRenderNode->GetMutableRenderProperties().SetFilter(filter);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
}

/*
 * @tc.name: SetSurfafaceGlobalDirtyRegion
 * @tc.desc: Add two surfacenode child to test global dirty region
 * @tc.type: FUNC
 * @tc.require: I68IPR
*/
HWTEST_F(RSUniRenderVisitorTest, SetSurfafaceGlobalDirtyRegion, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode1->InitRenderParams();
    Occlusion::Rect rect{0, 80, 2560, 1600};
    Occlusion::Region region{rect};
    VisibleData vData;
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    rsSurfaceRenderNode1->SetVisibleRegionRecursive(region, vData, pidVisMap);

    config.id = 11;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode2->InitRenderParams();
    Occlusion::Rect rect2{100, 100, 500, 1500};
    Occlusion::Region region2{rect2};
    rsSurfaceRenderNode2->SetVisibleRegionRecursive(region2, vData, pidVisMap);

    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(9, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode1, -1);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode2, -1);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
}

/*
 * @tc.name: IsWatermarkFlagChanged
 * @tc.desc: Test RSUniRenderVisitorTest.IsWatermarkFlagChanged test
 * @tc.type: FUNC
 * @tc.require: issuesIA8LNR
 */
HWTEST_F(RSUniRenderVisitorTest, IsWatermarkFlagChanged, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->IsWatermarkFlagChanged();
}

/**
 * @tc.name: ProcessSurfaceRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode when displaynode is null
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSubThread_ = true;
    std::shared_ptr<RSDisplayRenderNode> node = nullptr;

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetParent(node);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with isSubThread_ and isUIFirst_
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    surfaceNode->SetParent(node);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
    rsUniRenderVisitor->isSubThread_ = false;
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with skipLayer
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode003, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    node->SetSecurityDisplay(true);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    surfaceNode->SetParent(node);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode004
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with securityLayer
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode004, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    node->SetSecurityDisplay(true);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    surfaceNode->SetParent(node);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode005
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with capture window in directly render
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode005, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    surfaceNode->SetParent(node);
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode006
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with capture window in offscreen render
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode006, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    surfaceNode->SetParent(node);
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode007
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode while surface node has protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode007, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    surfaceNode->SetParent(node);
    surfaceNode->SetProtectedLayer(true);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode008
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode while surface node does not have protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode008, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    surfaceNode->SetParent(node);
    surfaceNode->SetProtectedLayer(false);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: PrepareEffectRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareEffectRenderNode api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareEffectRenderNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id1 = 1;
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(id1,
        displayConfig, rsContext->weak_from_this());
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    NodeId id2 = 2;
    auto node = std::make_shared<RSEffectRenderNode>(id2, rsContext->weak_from_this());
    node->InitRenderParams();
    rsUniRenderVisitor->PrepareEffectRenderNode(*node);
}

/**
 * @tc.name: ProcessEffectRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessEffectRenderNode api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessEffectRenderNode001, TestSize.Level1)
{
    NodeId id = 0;
    RSEffectRenderNode node(id);
    node.stagingRenderParams_ = std::make_unique<RSRenderParams>(node.GetId());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessEffectRenderNode(node);
}

/**
 * @tc.name: CheckColorSpace001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckColorSpace while
 *           app Window node's color space is not equal to GRAPHIC_COLOR_GAMUT_SRGB
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderVisitorTest, CheckColorSpace001, TestSize.Level2)
{
    auto appWindowNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(appWindowNode, nullptr);
    appWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    appWindowNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CheckColorSpace(*appWindowNode);
    ASSERT_EQ(rsUniRenderVisitor->newColorSpace_, appWindowNode->GetColorSpace());
}

/**
 * @tc.name: HandleColorGamuts001
 * @tc.desc: HandleColorGamuts for virtual screen
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleColorGamuts001, TestSize.Level2)
{
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtual screen 001", 0, 0, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    RSDisplayNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSDisplayRenderNode>(0, config, rsContext->weak_from_this());
    displayNode->SetScreenId(virtualScreenId);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->newColorSpace_ = GRAPHIC_COLOR_GAMUT_ADOBE_RGB;
    rsUniRenderVisitor->HandleColorGamuts(*displayNode, screenManager);

    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(displayNode->GetScreenId(), screenColorGamut);
    ASSERT_EQ(rsUniRenderVisitor->GetColorGamut(), static_cast<GraphicColorGamut>(screenColorGamut));

    screenManager->RemoveVirtualScreen(virtualScreenId);
}

/**
 * @tc.name: UpdateColorSpaceToIntanceRootNode
 * @tc.desc: test results of UpdateColorSpaceToIntanceRootNode, if node has no buffer
 * @tc.type: FUNC
 * @tc.require: issueIAOTNY
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateColorSpaceToIntanceRootNode001, TestSize.Level1)
{
    // register instance root node
    auto instanceRootNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(instanceRootNode, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.renderNodeMap_[instanceRootNode->GetId()] = instanceRootNode;
    // create subsurface node
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = instanceRootNode->GetId();

    ASSERT_NE(surfaceNode->GetInstanceRootNode(), nullptr);
    surfaceNode->UpdateColorSpaceToIntanceRootNode();
    ASSERT_EQ(surfaceNode->GetSubSurfaceColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: UpdateColorSpaceToIntanceRootNode
 * @tc.desc: test results of UpdateColorSpaceToIntanceRootNode, if node has buffer
 * @tc.type: FUNC
 * @tc.require: issueIAOTNY
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateColorSpaceToIntanceRootNode002, TestSize.Level1)
{
    // register instance root node
    auto instanceRootNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(instanceRootNode, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.renderNodeMap_[instanceRootNode->GetId()] = instanceRootNode;
    // create subsurface node
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = instanceRootNode->GetId();

    ASSERT_NE(surfaceNode->GetInstanceRootNode(), nullptr);
    surfaceNode->UpdateColorSpaceToIntanceRootNode();
    ASSERT_EQ(surfaceNode->GetSubSurfaceColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/*
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent001
 * @tc.desc: Reset but keep single node's surfaceInfo
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, ResetCurSurfaceInfoAsUpperSurfaceParent001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto upperSurfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(upperSurfaceNode, nullptr);
    surfaceNode->SetParent(upperSurfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);

    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->ResetCurSurfaceInfoAsUpperSurfaceParent(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->curSurfaceNode_, surfaceNode);
}

/*
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent002
 * @tc.desc: Reset but keep node's surfaceInfo since upper surface's InstanceRootNode is not registered
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, ResetCurSurfaceInfoAsUpperSurfaceParent002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto upperSurfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(upperSurfaceNode, nullptr);
    upperSurfaceNode->instanceRootNodeId_ = upperSurfaceNode->GetId();
    ASSERT_EQ(upperSurfaceNode->GetInstanceRootNode(), nullptr);
    surfaceNode->SetParent(std::weak_ptr<RSSurfaceRenderNode>(upperSurfaceNode));

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);

    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->ResetCurSurfaceInfoAsUpperSurfaceParent(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->curSurfaceNode_, surfaceNode);
}

/*
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent003
 * @tc.desc: Reset curSurfaceInfo when upper surface is leash/main window
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, ResetCurSurfaceInfoAsUpperSurfaceParent003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto upperSurfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(upperSurfaceNode, nullptr);
    upperSurfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    surfaceNode->SetParent(upperSurfaceNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->ResetCurSurfaceInfoAsUpperSurfaceParent(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->curSurfaceNode_, surfaceNode);
}

/*
 * @tc.name: UpdateChildHwcNodeEnabledByHwcNodeBelow
 * @tc.desc: Test RSUniRenderVistorTest.UpdateChildHwcNodeEnableByHwcNodeBelow
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateChildHwcNodeEnableByHwcNodeBelow, TestSize.Level2)
{
    auto appNode = RSTestUtil::CreateSurfaceNode();
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto hwcNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(hwcNode1, nullptr);
    surfaceConfig.id = 2;
    auto hwcNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(hwcNode2, nullptr);
    std::weak_ptr<RSSurfaceRenderNode> hwcNode3;
    hwcNode1->SetIsOnTheTree(true);
    hwcNode2->SetIsOnTheTree(false);
    appNode->AddChildHardwareEnabledNode(hwcNode1);
    appNode->AddChildHardwareEnabledNode(hwcNode2);
    appNode->AddChildHardwareEnabledNode(hwcNode3);
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(0, 0, 0, 0);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->UpdateChildHwcNodeEnableByHwcNodeBelow(hwcRects, appNode);
}

/*
 * @tc.name: PrepareForUIFirstNode001
 * @tc.desc: Test PrePareForUIFirstNode with last frame uifirst flag is not leash window and hardware enabled
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForCapsuleWindowMode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::NONE);
    surfaceNode->SetHardwareForcedDisabledState(false);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareForUIFirstNode(*surfaceNode);
}

/*
 * @tc.name: PrepareForUIFirstNode002
 * @tc.desc: Test PrePareForUIFirstNode with last frame uifirst flag is leash window and hardware disabled
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForCapsuleWindowMode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetLastFrameUifirstFlag(MultiThreadCacheType::LEASH_WINDOW);
    surfaceNode->SetHardwareForcedDisabledState(true);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareForUIFirstNode(*surfaceNode);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSurfaceRenderNode while surface node has security layer
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasSecSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->displayHasSecSurface_[rsUniRenderVisitor->currentVisitDisplay_], true);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has skip layer
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    displayNode->AddChild(surfaceNode, 0);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasSkipSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->displayHasSkipSurface_[rsUniRenderVisitor->currentVisitDisplay_], true);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node is capture window
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->AddChild(surfaceNode, 0);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->hasCaptureWindow_[rsUniRenderVisitor->currentVisitDisplay_] = false;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->hasCaptureWindow_[rsUniRenderVisitor->currentVisitDisplay_], true);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode004
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->SetProtectedLayer(true);

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    displayNode->AddChild(surfaceNode, 0);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasProtectedSurface_[rsUniRenderVisitor->currentVisitDisplay_] = true;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
}

/*
 * @tc.name: PrepareSurfaceRenderNode005
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSurfaceRenderNode while surface node does not have protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSurfaceRenderNode005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->SetProtectedLayer(false);

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasProtectedSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;
    rsUniRenderVisitor->PrepareSurfaceRenderNode(*surfaceNode);
}

/*
 * @tc.name: PrepareDisplayRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareDisplayRenderNode while dsiplay node has security surface
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareDisplayRenderNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    ASSERT_NE(displayNode->GetDirtyManager(), nullptr);
    displayNode->AddChild(surfaceNode, 0);
    displayNode->GenerateFullChildrenList();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(CreateOrGetScreenManager(), nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasSecSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;

    rsUniRenderVisitor->PrepareDisplayRenderNode(*displayNode);
}

/*
 * @tc.name: PrepareDisplayRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareDisplayRenderNode while dsiplay node has skip surface
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareDisplayRenderNode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    ASSERT_NE(displayNode->GetDirtyManager(), nullptr);
    displayNode->AddChild(surfaceNode, 0);
    displayNode->GenerateFullChildrenList();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(CreateOrGetScreenManager(), nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasSkipSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;

    rsUniRenderVisitor->PrepareDisplayRenderNode(*displayNode);
}

/*
 * @tc.name: PrepareDisplayRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareDisplayRenderNode while dsiplay node has capture window surface
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareDisplayRenderNode003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    ASSERT_NE(displayNode->GetDirtyManager(), nullptr);
    displayNode->AddChild(surfaceNode, 0);
    displayNode->GenerateFullChildrenList();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(CreateOrGetScreenManager(), nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->hasCaptureWindow_[rsUniRenderVisitor->currentVisitDisplay_] = false;

    rsUniRenderVisitor->PrepareDisplayRenderNode(*displayNode);
}

/**
 * @tc.name: FindInstanceChildOfDisplay001
 * @tc.desc: Test FindInstanceChildOfDisplay while node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderVisitorTest, FindInstanceChildOfDisplay001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->FindInstanceChildOfDisplay(nullptr), INVALID_NODEID);
}

/**
 * @tc.name: FindInstanceChildOfDisplay002
 * @tc.desc: Test FindInstanceChildOfDisplay while node's parent is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderVisitorTest, FindInstanceChildOfDisplay002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->GetParent().reset();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->FindInstanceChildOfDisplay(surfaceNode), INVALID_NODEID);
}

/**
 * @tc.name: FindInstanceChildOfDisplay003
 * @tc.desc: Test FindInstanceChildOfDisplay while node's parent is display node
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderVisitorTest, FindInstanceChildOfDisplay003, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->FindInstanceChildOfDisplay(surfaceNode), surfaceNode->GetId());
}

/**
 * @tc.name: FindInstanceChildOfDisplay004
 * @tc.desc: Test FindInstanceChildOfDisplay while node's parent isn't display node
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderVisitorTest, FindInstanceChildOfDisplay004, TestSize.Level2)
{
    RSDisplayNodeConfig config;
    NodeId id = 0;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id++);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(canvasNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);

    displayNode->AddChild(canvasNode);
    canvasNode->AddChild(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->FindInstanceChildOfDisplay(surfaceNode), canvasNode->GetId());
}

/*
 * @tc.name: CheckMergeFilterDirtyByIntersectWithDirty001
 * @tc.desc: Test CheckMergeFilterDirtyByIntersectWithDirty
 * @tc.type: FUNC
 * @tc.require: issueIAO5GW
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeFilterDirtyByIntersectWithDirty001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(DEFAULT_NODE_ID,
        displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    OcclusionRectISet filterSet;
    NodeId filterNodeId = DEFAULT_NODE_ID;
    // 1.filterSet not empty, currentFrameDirty empty
    filterSet.insert({filterNodeId, DEFAULT_RECT});
    rsUniRenderVisitor->CheckMergeFilterDirtyByIntersectWithDirty(filterSet, true);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), true);

    // 2.filterSet not empty and intersect with currentFrameDirty dirty not changed after merge
    filterSet.insert({filterNodeId, DEFAULT_RECT});
    rsDisplayRenderNode->GetDirtyManager()->MergeDirtyRect(DEFAULT_RECT);
    rsUniRenderVisitor->CheckMergeFilterDirtyByIntersectWithDirty(filterSet, true);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);

    // 3.filterSet not empty and intersect with currentFrameDirty, dirty changed after merge
    filterSet.insert({filterNodeId, DEFAULT_FILTER_RECT});
    rsUniRenderVisitor->CheckMergeFilterDirtyByIntersectWithDirty(filterSet, true);
    bool isRectEqual = (rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion() == DEFAULT_RECT);
    ASSERT_EQ(isRectEqual, false);
}

/*
 * @tc.name: CheckMergeSurfaceDirtysForDisplay001
 * @tc.desc: Test CheckMergeSurfaceDirtysForDisplay with transparent node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeSurfaceDirtysForDisplay001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    // set surface to transparent, add a canvas node to create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    // 80, 2560, 1600. dummy value  used to create rectangle with non-zero dimension
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    rsUniRenderVisitor->CheckMergeSurfaceDirtysForDisplay(rsSurfaceRenderNode);
    ASSERT_EQ(rsUniRenderVisitor->curDisplayDirtyManager_->dirtyRegion_.left_, 0);
}


/*
 * @tc.name: UpdateDisplayDirtyAndExtendVisibleRegion
 * @tc.desc: Test UpdateDisplayDirtyAndExtendVisibleRegion
 * @tc.type: FUNC
 * @tc.require: issueIAN75I
*/
HWTEST_F(RSUniRenderVisitorTest, UpdateDisplayDirtyAndExtendVisibleRegion, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->UpdateDisplayDirtyAndExtendVisibleRegion();

    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(5, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsUniRenderVisitor->QuickPrepareDisplayRenderNode(*rsDisplayRenderNode);
    RSSurfaceRenderNodeConfig surfaceConfig;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    rsDisplayRenderNode->RecordMainAndLeashSurfaces(surfaceNode);

    auto canvasNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(canvasNode, nullptr);
    auto& property = canvasNode->GetMutableRenderProperties();
    property.SetLightUpEffect(0.2f);
    property.UpdateFilter();
    surfaceNode->UpdateVisibleFilterChild(*canvasNode);
    auto visibleFilterChildren = surfaceNode->GetVisibleFilterChild();
    ASSERT_NE(visibleFilterChildren.size(), 0);
    rsUniRenderVisitor->UpdateDisplayDirtyAndExtendVisibleRegion();

    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(canvasNode);
    auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(canvasNode->GetId());
    ASSERT_NE(filterNode, nullptr);
    rsUniRenderVisitor->UpdateDisplayDirtyAndExtendVisibleRegion();

    Occlusion::Region region{ Occlusion::Rect{ 0, 0, 100, 100 } };
    surfaceNode->SetVisibleRegion(region);
    canvasNode->SetOldDirtyInSurface({ 50, 50, 70, 70 });
    rsUniRenderVisitor->UpdateDisplayDirtyAndExtendVisibleRegion();
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparent001
 * @tc.desc: Test CheckMergeDisplayDirtyByTransparent with transparent node
 * @tc.type: FUNC
 * @tc.require: issueIAD6OW
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparent001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode to transparent, create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    rsSurfaceRenderNode->SetSrcRect(DEFAULT_RECT);
    rsSurfaceRenderNode->GetDirtyManager()->MergeDirtyRect(DEFAULT_RECT);
    rsSurfaceRenderNode->oldDirtyInSurface_ = DEFAULT_RECT;

    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparent(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByZorderChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByZorderChanged with Z order change node
 * @tc.type: FUNC
 * @tc.require: issueIAD6OW
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByZorderChanged001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode Zorder change
    rsSurfaceRenderNode->zOrderChanged_ = true;
    rsSurfaceRenderNode->oldDirtyInSurface_ = DEFAULT_RECT;

    rsUniRenderVisitor->CheckMergeDisplayDirtyByZorderChanged(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByPosChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByPosChanged with position change
 * @tc.type: FUNC
 * @tc.require: issueIAD6OW
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByPosChanged001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode pos change
    rsDisplayRenderNode->UpdateSurfaceNodePos(config.id, RectI());
    rsDisplayRenderNode->ClearCurrentSurfacePos();
    rsDisplayRenderNode->UpdateSurfaceNodePos(config.id, DEFAULT_RECT);

    rsUniRenderVisitor->CheckMergeDisplayDirtyByPosChanged(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByShadowChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByShadowChanged with shadow change
 * @tc.type: FUNC
 * @tc.require: issueIAD6OW
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByShadowChanged001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode shadow change
    rsSurfaceRenderNode->isShadowValidLastFrame_ = true;
    rsSurfaceRenderNode->GetDirtyManager()->MergeDirtyRect(DEFAULT_RECT);
    rsSurfaceRenderNode->oldDirtyInSurface_ = DEFAULT_RECT;

    rsUniRenderVisitor->CheckMergeDisplayDirtyByShadowChanged(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty001
 * @tc.desc: Test UpdateSurfaceDirtyAndGlobalDirty
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
*/
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    // set surface to transparent, add a canvas node to create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    // 80, 2560, 1600. dummy value used to create rectangle with non-zero dimension
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_EQ(rsUniRenderVisitor->curDisplayDirtyManager_->dirtyRegion_.left_, 0);
}

/**
 * @tc.name: UpdateHwcNodeEnableByRotateAndAlpha001
 * @tc.desc: Test UpdateHwcNodeEnableByRotateAndAlpha for empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByRotateAndAlpha001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->UpdateHwcNodeEnableByRotateAndAlpha(node);
    ASSERT_FALSE(node->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp001
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp with empty Rect
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(0, 0, 0, 0);
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(hwcRects, node);
    ASSERT_FALSE(node->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableBySrcRect001
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect with empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableBySrcRect001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->UpdateHwcNodeEnableBySrcRect(*node);
    ASSERT_FALSE(node->isHardwareForcedDisabledBySrcRect_);
}

/**
 * @tc.name: UpdateDstRect001
 * @tc.desc: Test UpdateDstRect with empty rect
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateDstRect001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    RectI absRect(0, 0, 0, 0);
    RectI clipRect(0, 0, 0, 0);
    rsUniRenderVisitor->UpdateDstRect(*rsSurfaceRenderNode, absRect, clipRect);
    ASSERT_EQ(rsSurfaceRenderNode->GetDstRect().left_, 0);
}

/**
 * @tc.name: UpdateSrcRect001
 * @tc.desc: Test UpdateSrcRect with empty matrix
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSrcRect001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    Drawing::Matrix absMatrix;
    RectI absRect(0, 0, 0, 0);
    rsSurfaceRenderNode->GetMutableRenderProperties().SetBounds({0, 0, 0, 0});
    rsUniRenderVisitor->UpdateSrcRect(*rsSurfaceRenderNode, absMatrix, absRect);
    ASSERT_EQ(rsSurfaceRenderNode->GetSrcRect().left_, 0);
}

/**
 * @tc.name: BeforeUpdateSurfaceDirtyCalc001
 * @tc.desc: Test BeforeUpdateSurfaceDirtyCalc with empty node
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, BeforeUpdateSurfaceDirtyCalc001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: BeforeUpdateSurfaceDirtyCalc002
 * @tc.desc: Test BeforeUpdateSurfaceDirtyCalc with nonEmpty node
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, BeforeUpdateSurfaceDirtyCalc002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    node->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    node->SetNodeName("CapsuleWindow");
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: SurfaceOcclusionCallbackToWMS001
 * @tc.desc: Test SurfaceOcclusionCallbackToWMS with default constructed visitor
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, SurfaceOcclusionCallbackToWMS001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SurfaceOcclusionCallbackToWMS();
}

/**
 * @tc.name: GetCurrentBlackList001
 * @tc.desc: Test GetCurrentBlackList with default constructed visitor
 * @tc.type: FUNC
 * @tc.require: issuesIAMODH
 */
HWTEST_F(RSUniRenderVisitorTest, GetCurrentBlackList001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    EXPECT_TRUE(rsUniRenderVisitor->GetCurrentBlackList().empty());
}

/**
 * @tc.name: GetCurrentBlackList
 * @tc.desc: Test GetCurrentBlackList002, screenManager_ && curDisplayNode_ != nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAMODH
 */
HWTEST_F(RSUniRenderVisitorTest, GetCurrentBlackList002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_EQ(rsUniRenderVisitor->GetCurrentBlackList().size(), 0);
}

/**
 * @tc.name: NeedPrepareChindrenInReverseOrder001
 * @tc.desc: Test NeedPrepareChindrenInReverseOrder with default constructed visitor
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, NeedPrepareChindrenInReverseOrder001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    ASSERT_NE(rsBaseRenderNode, nullptr);
    rsBaseRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_FALSE(rsUniRenderVisitor->NeedPrepareChindrenInReverseOrder(*rsBaseRenderNode));
}

/**
 * @tc.name: NeedPrepareChindrenInReverseOrder002
 * @tc.desc: Test NeedPrepareChindrenInReverseOrder with different type nodes
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, NeedPrepareChindrenInReverseOrder002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    ASSERT_NE(rsBaseRenderNode, nullptr);
    rsBaseRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto node1 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node1, nullptr);
    node1->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    node1->AddChild(rsBaseRenderNode);
    ASSERT_TRUE(rsUniRenderVisitor->NeedPrepareChindrenInReverseOrder(*node1));

    auto node2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node2, nullptr);
    node2->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    node1->RemoveChild(rsBaseRenderNode);
    node1->AddChild(node2);
    ASSERT_TRUE(rsUniRenderVisitor->NeedPrepareChindrenInReverseOrder(*node1));

    node1->AddChild(rsBaseRenderNode);
    ASSERT_TRUE(rsUniRenderVisitor->NeedPrepareChindrenInReverseOrder(*node1));
}

/**
 * @tc.name: IsLeashAndHasMainSubNode001
 * @tc.desc: Test IsLeashAndHasMainSubNode with empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, IsLeashAndHasMainSubNode001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    ASSERT_FALSE(rsUniRenderVisitor->IsLeashAndHasMainSubNode(*node));
}

/**
 * @tc.name: CalculateOcclusion001
 * @tc.desc: Test CalculateOcclusion with empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOcclusion001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    rsUniRenderVisitor->CalculateOcclusion(*rsSurfaceRenderNode);
    ASSERT_FALSE(rsUniRenderVisitor->needRecalculateOcclusion_);
}

/**
 * @tc.name: QuickPrepareDisplayRenderNode004
 * @tc.desc: Test QuickPrepareDisplayRenderNode with display node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareDisplayRenderNode004, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->QuickPrepareDisplayRenderNode(*rsDisplayRenderNode);
    ASSERT_FALSE(rsUniRenderVisitor->ancestorNodeHasAnimation_);
}

/**
 * @tc.name: IsSubTreeOccluded001
 * @tc.desc: Test IsSubTreeOccluded with empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, IsSubTreeOccluded001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->IsSubTreeOccluded(*node);
    ASSERT_EQ(node->dirtyStatus_, RSRenderNode::NodeDirty::CLEAN);
}

/**
 * @tc.name: UpdateSurfaceRenderNodeRotate001
 * @tc.desc: Test UpdateSurfaceRenderNodeRotate with empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceRenderNodeRotate001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->UpdateSurfaceRenderNodeRotate(*node);
    ASSERT_FALSE(node->isRotating_);
}

/*
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeEnableByFilterRect with intersect rect
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByFilterRect001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);
    surfaceConfig.id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode2, nullptr);

    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};
    surfaceNode2->SetDstRect(rect);
    surfaceNode1->AddChildHardwareEnabledNode(surfaceNode2);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeEnableByFilterRect(surfaceNode1, rect);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());
}

/*
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeEnableByFilterRect with empty rect
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByFilterRect002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);

    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    RectI rect{left, top, width, height};
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeEnableByFilterRect(surfaceNode1, rect);
}

/*
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeEnableByFilterRect with no hwcNode
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByFilterRect003, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);

    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeEnableByFilterRect(surfaceNode, rect);
}

/*
 * @tc.name: UpdateHwcNodeEnableByGlobalCleanFilter_001
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalCleanFilter when Intersect return false.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalCleanFilter_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::pair<NodeId, RectI>> cleanFilter;
    surfaceNode->SetDstRect(RectI());
    cleanFilter.emplace_back(NodeId(0), RectI(50, 50, 100, 100));
    ASSERT_FALSE(surfaceNode->GetDstRect().Intersect(cleanFilter[0].second));

    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter, *surfaceNode);
    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateHwcNodeEnableByGlobalCleanFilter_002
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalCleanFilter when rendernode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalCleanFilter_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::pair<NodeId, RectI>> cleanFilter;
    surfaceNode->SetDstRect(RectI(0, 0, 100, 100));
    cleanFilter.emplace_back(NodeId(0), RectI(50, 50, 100, 100));
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    constexpr NodeId id = 0;
    nodeMap.renderNodeMap_[id] = nullptr;

    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter, *surfaceNode);
    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateHwcNodeEnableByGlobalCleanFilter_003
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalCleanFilter when rendernode is not null and AIBarFilterCache is not valid.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalCleanFilter_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::pair<NodeId, RectI>> cleanFilter;
    surfaceNode->SetDstRect(RectI(0, 0, 100, 100));
    cleanFilter.emplace_back(NodeId(1), RectI(50, 50, 100, 100));
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    constexpr NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    nodeMap.renderNodeMap_[id] = node;
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(node->IsAIBarFilterCacheValid());

    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter, *surfaceNode);
    EXPECT_TRUE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateHwcNodeEnableBySrcRect_001
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect when node is hardware forced disabled.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableBySrcRect_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());
    rsUniRenderVisitor->UpdateHwcNodeEnableBySrcRect(*surfaceNode);
}

/*
 * @tc.name: UpdateHwcNodeEnableBySrcRect_002
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect when consumer is not nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableBySrcRect_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    ASSERT_NE(surfaceNode->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    rsUniRenderVisitor->UpdateHwcNodeEnableBySrcRect(*surfaceNode);
}

/*
 * @tc.name: UpdateHwcNodeEnableBySrcRect_003
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect when node is hardware disabled by src rect.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableBySrcRect_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->isHardwareForcedDisabledBySrcRect_ = true;
    ASSERT_TRUE(surfaceNode->IsHardwareDisabledBySrcRect());

    rsUniRenderVisitor->UpdateHwcNodeEnableBySrcRect(*surfaceNode);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp_001
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp when hwcNode is hardware forced disabled.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());

    std::vector<RectI> hwcRects;
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(hwcRects, surfaceNode);
    EXPECT_EQ(hwcRects.size(), 0);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp_002
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp when hwcNode is anco force do direct.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(true);
    surfaceNode->SetAncoFlags(static_cast<uint32_t>(0x0001));
    ASSERT_TRUE(surfaceNode->GetAncoForceDoDirect());
    
    std::vector<RectI> hwcRects;
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(hwcRects, surfaceNode);
    EXPECT_EQ(hwcRects.size(), 1);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp_003
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp when dst.Intersect(rect) equals true.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI(0, 0, 100, 100));
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(50, 50, 100, 100));
    ASSERT_TRUE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(hwcRects, surfaceNode);
    EXPECT_EQ(hwcRects.size(), 1);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp_004
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp when dst.Intersect(rect) equals false.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI());
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(50, 50, 100, 100));
    ASSERT_FALSE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(hwcRects, surfaceNode);
    EXPECT_EQ(hwcRects.size(), 2);
}

/*
 * @tc.name: UpdateHwcNodeEnable_001
 * @tc.desc: Test UpdateHwcNodeEnable when surfaceNode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    rsUniRenderVisitor->UpdateHwcNodeEnable();
}

/*
 * @tc.name: UpdateHwcNodeEnable_002
 * @tc.desc: Test UpdateHwcNodeEnable when hwcNodes is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->ResetChildHardwareEnabledNodes();
    ASSERT_EQ(surfaceNode->GetChildHardwareEnabledNodes().size(), 0);
    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    rsUniRenderVisitor->UpdateHwcNodeEnable();
}

/*
 * @tc.name: UpdateHwcNodeEnable_003
 * @tc.desc: Test UpdateHwcNodeEnable when hwcNodePtr is not on the tree.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    NodeId childId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childId);
    childNode->SetIsOnTheTree(false);
    ASSERT_FALSE(childNode->IsOnTheTree());
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    rsUniRenderVisitor->UpdateHwcNodeEnable();
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_001
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode is hardware forced disabled.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());

    std::vector<RectI> hwcRects;
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 0);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_002
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode has corner radius and anco force do direct.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(true);
    surfaceNode->SetAncoFlags(static_cast<uint32_t>(0x0001));
    ASSERT_TRUE(surfaceNode->GetAncoForceDoDirect());

    std::vector<RectI> hwcRects;
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, false);
    EXPECT_EQ(hwcRects.size(), 1);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_003
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode intersects with hwcRects.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI(0, 0, 100, 100));
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(50, 50, 100, 100));
    ASSERT_TRUE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 1);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_004
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode does not intersect with hwcRects.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI());
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(50, 50, 100, 100));
    ASSERT_FALSE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 2);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_001
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           curSurfaceNode_ is null or hwcNodes is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);

    {
        rsUniRenderVisitor->curSurfaceNode_ = nullptr;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        ASSERT_NE(surfaceNode, nullptr);
        surfaceNode->ResetChildHardwareEnabledNodes();
        ASSERT_EQ(surfaceNode->GetChildHardwareEnabledNodes().size(), 0);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_002
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           hwcNodePtr is nullptr or hwcNodePtr is not on the tree or GetCalcRectInPrepare is true.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);

    {
        NodeId childNodeId = 2;
        auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
        childNode = nullptr;

        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        NodeId childNodeId = 2;
        auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
        ASSERT_NE(childNode, nullptr);
        childNode->SetIsOnTheTree(false);
        ASSERT_FALSE(childNode->IsOnTheTree());

        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        NodeId childNodeId = 2;
        auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
        ASSERT_NE(childNode, nullptr);
        childNode->SetIsOnTheTree(true);
        ASSERT_TRUE(childNode->IsOnTheTree());
        childNode->SetCalcRectInPrepare(true);
        ASSERT_TRUE(childNode->GetCalcRectInPrepare());

        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_003
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false and parent is null.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());

    ASSERT_EQ(childNode->GetParent().lock(), nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_004
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           parent is not null and findInRoot is false.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());

    NodeId fakeParentNodeId = 3;
    auto fakeParentNode = std::make_shared<RSSurfaceRenderNode>(fakeParentNodeId);
    ASSERT_NE(fakeParentNode, nullptr);
    fakeParentNode->AddChild(childNode);

    auto parent = childNode->GetParent().lock();
    ASSERT_NE(parent, nullptr);
    ASSERT_NE(parent->GetId(), parentNodeId);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_005
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           parent is not null and findInRoot is true.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_005, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());

    parentNode->AddChild(childNode);
    auto parent = childNode->GetParent().lock();
    ASSERT_NE(parent, nullptr);
    ASSERT_EQ(parent->GetId(), parentNodeId);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_006
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false and
 *           parent's parent is not null.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree_006, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());

    parentNode->AddChild(childNode);
    auto parent = childNode->GetParent().lock();
    ASSERT_NE(parent, nullptr);
    ASSERT_EQ(parent->GetId(), parentNodeId);

    NodeId grandparentNodeId = 3;
    auto grandparentNode = std::make_shared<RSSurfaceRenderNode>(grandparentNodeId);
    ASSERT_NE(grandparentNode, nullptr);
    grandparentNode->InitRenderParams();
    grandparentNode->AddChild(parentNode);

    EXPECT_NE(parent->GetType(), RSRenderNodeType::DISPLAY_NODE);
    auto grandparent = parent->GetParent().lock();
    ASSERT_NE(grandparent, nullptr);
    ASSERT_EQ(grandparent->GetId(), grandparentNodeId);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: CheckMergeGlobalFilterForDisplay
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeGlobalFilterForDisplay
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeGlobalFilterForDisplay001, TestSize.Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id0 = 0;
    nodeMap.renderNodeMap_[id0] = nullptr;
    NodeId id1 = 1;
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[id1] = node1;
    NodeId id2 = 2;
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[id2] = node2;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};
    rsUniRenderVisitor->containerFilter_.insert({node2->GetId(), rect});
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    auto dirtyRegion = Occlusion::Region{ Occlusion::Rect{ rect } };
    rsUniRenderVisitor->CheckMergeGlobalFilterForDisplay(dirtyRegion);
}

/*
 * @tc.name: UpdateOccludedStatusWithFilterNode
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateOccludedStatusWithFilterNode while surface node nullptr
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateOccludedStatusWithFilterNode001, TestSize.Level2)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateOccludedStatusWithFilterNode(surfaceNode);
}

/*
 * @tc.name: UpdateOccludedStatusWithFilterNode
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateOccludedStatusWithFilterNode with surfaceNode
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateOccludedStatusWithFilterNode002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    ASSERT_NE(surfaceNode1->renderContent_, nullptr);
    surfaceNode1->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    surfaceNode1->isOccludedByFilterCache_ = true;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id1 = 2;
    auto filterNode1 = std::make_shared<RSRenderNode>(id1);
    ASSERT_NE(filterNode1, nullptr);
    nodeMap.renderNodeMap_[id1] = filterNode1;
    NodeId id2 = 3;
    auto filterNode2 = std::make_shared<RSRenderNode>(id2);
    ASSERT_NE(filterNode2, nullptr);
    ASSERT_NE(filterNode2->renderContent_, nullptr);
    filterNode2->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    nodeMap.renderNodeMap_[id2] = filterNode2;
    surfaceNode1->visibleFilterChild_.emplace_back(filterNode1->GetId());
    surfaceNode1->visibleFilterChild_.emplace_back(filterNode2->GetId());

    ASSERT_FALSE(filterNode1->isOccluded_);
    ASSERT_FALSE(filterNode2->isOccluded_);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateOccludedStatusWithFilterNode(surfaceNode1);
    ASSERT_TRUE(filterNode2->isOccluded_);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentFilter
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeDisplayDirtyByTransparentFilter with mainWindow
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparentFilter001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    ASSERT_TRUE(surfaceNode->GetVisibleRegion().IsEmpty());

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 260;
    uint32_t height = 600;
    RectI rect{left, top, width, height};
    Occlusion::Region region{rect};
    NodeId displayNodeId = 2;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentFilter(surfaceNode, region);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentFilter
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeDisplayDirtyByTransparentFilter
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparentFilter002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id0 = 0;
    nodeMap.renderNodeMap_[id0] = nullptr;
    NodeId id1 = 1;
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[id1] = node1;
    NodeId id2 = 2;
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[id2] = node2;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left1 = 0;
    uint32_t top1 = 0;
    uint32_t width1 = 260;
    uint32_t height1 = 600;
    RectI rect1{left1, top1, width1, height1};
    Occlusion::Region region1{rect1};
    uint32_t left2 = 0;
    uint32_t top2 = 0;
    uint32_t width2 = 200;
    uint32_t height2 = 300;
    RectI rect2{left2, top2, width2, height2};
    rsUniRenderVisitor->transparentCleanFilter_[surfaceNode->GetId()].push_back({id0, rect2});
    rsUniRenderVisitor->transparentCleanFilter_[surfaceNode->GetId()].push_back({id1, rect2});
    rsUniRenderVisitor->transparentCleanFilter_[surfaceNode->GetId()].push_back({id2, rect2});
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    ASSERT_NE(node2->renderContent_, nullptr);
    node2->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    node2->renderContent_->renderProperties_.SetFilter(filter);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);

    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentFilter(surfaceNode, region1);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentRegions
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeDisplayDirtyByTransparentRegions with container window
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparentRegions001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    uint32_t left1 = 0;
    uint32_t top1 = 0;
    uint32_t width1 = 200;
    uint32_t height1 = 300;
    RectI rect{left1, top1, width1, height1};
    surfaceNode->GetDirtyManager()->SetCurrentFrameDirtyRect(rect);
    surfaceNode->containerConfig_.hasContainerWindow_ = true;
    uint32_t left2 = 0;
    uint32_t top2 = 0;
    uint32_t width2 = 260;
    uint32_t height2 = 600;
    RectI rect1{left2, top2, width2, height2};
    Occlusion::Region region1{rect1};
    surfaceNode->containerRegion_ = region1;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentRegions(*surfaceNode);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentRegions
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeDisplayDirtyByTransparentRegions with no container window
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparentRegions002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->isNodeDirty_ = true;
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 260;
    uint32_t height = 600;
    Occlusion::Rect rect1{left, top, width, height};
    Occlusion::Region region1{rect1};
    surfaceNode->transparentRegion_ = region1;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentRegions(*surfaceNode);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeInfoForAppNode with not nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIAE6YM
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHwcNodeRectInSkippedSubTree with not nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIAE6YM
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    int id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*node);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHwcNodeEnableByGlobalFilter with not nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIAE6YM
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalFilter, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 0;
    std::weak_ptr<RSContext> context;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();

    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
}

/**
 * @tc.name: CollectEffectInfo001
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with not parent node.
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    constexpr NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    rsUniRenderVisitor->CollectEffectInfo(*node);
}

/**
 * @tc.name: CollectEffectInfo002
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, need filter
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    constexpr NodeId nodeId = 1;
    constexpr NodeId parentNodeId = 2;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    auto parent = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parent, nullptr);
    node->InitRenderParams();
    parent->InitRenderParams();
    parent->AddChild(node);
    node->GetMutableRenderProperties().needFilter_ = true;
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_TRUE(parent->ChildHasVisibleFilter());
}

/**
 * @tc.name: CollectEffectInfo003
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, useEffect
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    constexpr NodeId nodeId = 1;
    constexpr NodeId parentNodeId = 2;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    auto parent = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parent, nullptr);
    node->InitRenderParams();
    parent->InitRenderParams();
    parent->AddChild(node);
    node->GetMutableRenderProperties().useEffect_ = true;
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_TRUE(parent->ChildHasVisibleEffect());
}

/*
 * @tc.name: CheckIsGpuOverDrawBufferOptimizeNode001
 * @tc.desc: Verify function CheckIsGpuOverDrawBufferOptimizeNode while node has no child
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIsGpuOverDrawBufferOptimizeNode001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(surfaceNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CheckIsGpuOverDrawBufferOptimizeNode(*surfaceNode);
    ASSERT_FALSE(surfaceNode->IsGpuOverDrawBufferOptimizeNode());
}

/*
 * @tc.name: CheckIsGpuOverDrawBufferOptimizeNode002
 * @tc.desc: Verify function CheckIsGpuOverDrawBufferOptimizeNode while node has child
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIsGpuOverDrawBufferOptimizeNode002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    surfaceNode->SetIsScale(true);

    NodeId childNodeId = config.id + 1;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    surfaceNode->AddChild(childNode, -1);
    surfaceNode->GenerateFullChildrenList();

    NodeId rootNodeId = childNodeId + 1;
    auto rootNode = std::make_shared<RSRenderNode>(rootNodeId);
    childNode->AddChild(rootNode, -1);

    NodeId canvasNodeId = rootNodeId + 1;
    auto canvasNode = std::make_shared<RSRenderNode>(canvasNodeId);
    auto& properties = canvasNode->GetMutableRenderProperties();
    Color color(0, 0, 0, MAX_ALPHA);
    properties.SetBackgroundColor(color);
    rootNode->AddChild(canvasNode, -1);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curCornerRadius_ = {1.0f, 1.0f, 1.0f, 1.0f};
    rsUniRenderVisitor->CheckIsGpuOverDrawBufferOptimizeNode(*surfaceNode);
    ASSERT_TRUE(surfaceNode->IsGpuOverDrawBufferOptimizeNode());
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion001
 * @tc.desc: Test MergeRemovedChildDirtyRegion while node has removed child which dirty rect is empty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode);
    ASSERT_FALSE(surfaceNode->HasRemovedChild());
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion002
 * @tc.desc: Test MergeRemovedChildDirtyRegion while has removed child which dirty rect isn't empty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), RectI(0, 0, 0, 0));
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion003
 * @tc.desc: Test MergeRemovedChildDirtyRegion while curSurfaceDirtyManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;

    RSDisplayNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSDisplayRenderNode>(0, config, rsContext->weak_from_this());
    auto dirtyManager = displayNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = nullptr;
    rsUniRenderVisitor->curDisplayDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), RectI(0, 0, 0, 0));
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion004
 * @tc.desc: Test MergeRemovedChildDirtyRegion while need map
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->oldClipRect_ = DEFAULT_RECT;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode, true);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), DEFAULT_RECT);
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion005
 * @tc.desc: Test MergeRemovedChildDirtyRegion while node's bounds geometry is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->oldClipRect_ = DEFAULT_RECT;
    surfaceNode->GetMutableRenderProperties().boundsGeo_ = nullptr;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode, true);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), DEFAULT_RECT);
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion006
 * @tc.desc: Test MergeRemovedChildDirtyRegion while dirtyManager is target for FDX
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion006, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->oldClipRect_ = DEFAULT_RECT;
    surfaceNode->GetDirtyManager()->MarkAsTargetForDfx();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode, true);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), DEFAULT_RECT);
}

/*
 * @tc.name: PrevalidateHwcNode001
 * @tc.desc: Test while prevalidate hwcNode disable
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, PrevalidateHwcNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    NodeId id = 1;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isPrevalidateHwcNodeEnable_ = false;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->PrevalidateHwcNode();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty002
 * @tc.desc: Test while surface node has hardware enabled node as child
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty002, TestSize.Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);

    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(true);
    RsCommonHook::Instance().SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty003
 * @tc.desc: Test while current frame dirty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty003, TestSize.Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);

    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(true);
    RsCommonHook::Instance().SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);

    auto dirtyManager = surfaceNode->GetDirtyManager();
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_RECT);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty004
 * @tc.desc: Test while app window node skip in calculate global dirty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty004, TestSize.Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);

    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(true);
    RsCommonHook::Instance().SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);

    auto dirtyManager = surfaceNode->GetDirtyManager();
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_RECT);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty001
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty without curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->containerFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->containerFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty002
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with non-transparent curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->SetAbilityBGAlpha(MAX_ALPHA);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = surfaceNodeId + 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->globalFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->globalFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty003
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with transparent curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->SetAbilityBGAlpha(0);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = surfaceNodeId + 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->transparentCleanFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->transparentCleanFilter_.empty());
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha, TestSize.Level1)
{
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    std::weak_ptr<RSSurfaceRenderNode> hwcNode;
    hwcNodes.push_back(hwcNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/**
 * @tc.name: IsNodeAboveInsideOfNodeBelow
 * @tc.desc: Test RSUnitRenderVisitorTest.IsNodeAboveInsideOfNodeBelow
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, IsNodeAboveInsideOfNodeBelow, TestSize.Level1)
{
    const RectI rectAbove;
    std::list<RectI> hwcNodeRectList;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->IsNodeAboveInsideOfNodeBelow(rectAbove, hwcNodeRectList);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter nullptr / eqeual nodeid / hwcNodes empty.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalFilter001, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node.
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(std::make_shared<RSSurfaceRenderNode>(node->GetId()));

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter, child node force disabled hardware.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalFilter002, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node and surface node.
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    NodeId surfaceId = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceId);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(++surfaceId);
    childNode->isHardwareForcedDisabled_ = true;
    surfaceNode->AddChildHardwareEnabledNode(childNode);

    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
    ASSERT_TRUE(childNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter, dirty filter found.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalFilter003, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node and surface node.
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    NodeId surfaceId = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceId);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(++surfaceId);
    childNode->isHardwareForcedDisabled_ = false;
    childNode->dstRect_ = DEFAULT_RECT;
    surfaceNode->AddChildHardwareEnabledNode(childNode);

    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->transparentDirtyFilter_[node->GetId()].push_back(std::pair(node->GetId(), DEFAULT_RECT));
    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
    ASSERT_TRUE(childNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByBufferSize
 * @tc.desc: Test UpdateHwcNodeEnableByBufferSize with rosen-web node / non-rosen-web node.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByBufferSize, TestSize.Level1)
{
    // create input args.
    auto node1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto node2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node1->name_ = "RosenWeb_test";

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeEnableByBufferSize(*node1);
    rsUniRenderVisitor->UpdateHwcNodeEnableByBufferSize(*node2);
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionAndCreateLayer
 * @tc.desc: Test UpdateHwcNodeDirtyRegionAndCreateLayer with default surface node (empty child).
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionAndCreateLayer001, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node);
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionAndCreateLayer
 * @tc.desc: Test UpdateHwcNodeDirtyRegionAndCreateLayer with off-tree/on-tree child node.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionAndCreateLayer002, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    NodeId childId = 1;
    auto childNode1 = std::make_shared<RSSurfaceRenderNode>(childId);
    childNode1->SetIsOnTheTree(false);
    auto childNode2 = std::make_shared<RSSurfaceRenderNode>(++childId);
    childNode2->SetIsOnTheTree(true);
    node->AddChildHardwareEnabledNode(childNode1);
    node->AddChildHardwareEnabledNode(childNode2);

    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node);
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionForApp
 * @tc.desc: Test UpdateHwcNodeDirtyRegionForApp, current frame enable status different from last frame.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionForApp001, TestSize.Level1)
{
    // create input args.
    auto appNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    appNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();

    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    hwcNode->dstRect_ = DEFAULT_RECT;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionForApp(appNode, hwcNode);
    ASSERT_FALSE(appNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionForApp
 * @tc.desc: Test UpdateHwcNodeDirtyRegionForApp, current frame enable status different from last frame.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionForApp002, TestSize.Level1)
{
    // create input args.
    auto appNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    appNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();

    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    hwcNode->dstRect_ = DEFAULT_RECT;
    hwcNode->isLastFrameHardwareEnabled_ = true;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->hasMirrorDisplay_ = true;
    hwcNode->GetRSSurfaceHandler()->SetCurrentFrameBufferConsumed();
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionForApp(appNode, hwcNode);
    ASSERT_FALSE(appNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: CalculateOcclusion
 * @tc.desc: Test CalculateOcclusion002, curDisplayNode_ = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOcclusion002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = nullptr;
    rsUniRenderVisitor->CalculateOcclusion(*node);
}

/**
 * @tc.name: CalculateOcclusion
 * @tc.desc: Test CalculateOcclusion003, isAllSurfaceVisibleDebugEnabled_ = true
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOcclusion003, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->isAllSurfaceVisibleDebugEnabled_ = true;
    rsUniRenderVisitor->CalculateOcclusion(*node);
}

/**
 * @tc.name: CollectOcclusionInfoForWMS
 * @tc.desc: Test CollectOcclusionInfoForWMS001, node.IsMainWindowType() return false
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, CollectOcclusionInfoForWMS001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::SCB_SCREEN_NODE);
    rsUniRenderVisitor->CollectOcclusionInfoForWMS(*node);
}

/**
 * @tc.name: CollectOcclusionInfoForWMS
 * @tc.desc: Test CollectOcclusionInfoForWMS002, instanceNode == nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, CollectOcclusionInfoForWMS002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    std::shared_ptr<RSContext> context = nullptr;
    node->context_ = context;
    rsUniRenderVisitor->CollectOcclusionInfoForWMS(*node);
}

/**
 * @tc.name: QuickPrepareEffectRenderNode
 * @tc.desc: Test QuickPrepareEffectRenderNode001, dirtyManager = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareEffectRenderNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNodeWithBuffer();
    rsUniRenderVisitor->curSurfaceDirtyManager_ = nullptr;
    NodeId id = 2;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSEffectRenderNode>(id, rsContext->weak_from_this());
    node->InitRenderParams();
    rsUniRenderVisitor->QuickPrepareEffectRenderNode(*node);
}

/**
 * @tc.name: QuickPrepareEffectRenderNode
 * @tc.desc: Test QuickPrepareEffectRenderNode002, dirtyManager != nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareEffectRenderNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    NodeId id1 = 2;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSEffectRenderNode>(id1, rsContext->weak_from_this());
    node->InitRenderParams();
    NodeId id2 = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id2, config);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->QuickPrepareEffectRenderNode(*node);
}

/**
 * @tc.name: UpdatePrepareClip
 * @tc.desc: Test UpdatePrepareClip001, geoPtr = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, UpdatePrepareClip001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->GetMutableRenderProperties().boundsGeo_ = nullptr;
    rsUniRenderVisitor->UpdatePrepareClip(*node);
}

/**
 * @tc.name: UpdatePrepareClip
 * @tc.desc: Test UpdatePrepareClip002, clipToBounds_ & clipToframe_ = true;
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, UpdatePrepareClip002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->GetMutableRenderProperties().clipToBounds_ = true;
    node->GetMutableRenderProperties().clipToFrame_ = true;
    rsUniRenderVisitor->UpdatePrepareClip(*node);
}

/**
 * @tc.name: UpdatePrepareClip
 * @tc.desc: Test UpdatePrepareClip003, clipToBounds_ & clipToframe_ = false;
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, UpdatePrepareClip003, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->GetMutableRenderProperties().clipToBounds_ = false;
    node->GetMutableRenderProperties().clipToFrame_ = false;
    rsUniRenderVisitor->UpdatePrepareClip(*node);
}

/**
 * @tc.name: IsFirstFrameOfOverdrawSwitch
 * @tc.desc: Test IsFirstFrameOfOverdrawSwitch
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, IsFirstFrameOfOverdrawSwitch, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->IsFirstFrameOfOverdrawSwitch(), false);
}
} // OHOS::Rosen