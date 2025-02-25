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
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include "feature/round_corner_display/rs_round_corner_display.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"

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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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
 * @tc.require:issuesIB7OKO
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

    const auto& buffer = node->GetRSSurfaceHandler()->GetBuffer();
    rsUniRenderVisitor->UpdateHwcNodeByTransform(*node);
    ASSERT_EQ(node->GetRSSurfaceHandler()->GetBuffer()->GetSurfaceBufferScalingMode(), SCALING_MODE_SCALE_TO_WINDOW);
}

/**
 * @tc.name: UpdateHwcNodeByTransform_003
 * @tc.desc: UpdateHwcNodeByTransform Test, buffer of RSSurfaceHandler is not nullptr, and
 * consumer_ of of RSSurfaceHandler is not nullptr, and GetScalingMode is GSERROR_INVALID_ARGUMENTS
 * scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP, expect LayerScaleDown
 * @tc.type:FUNC
 * @tc.require:issuesIB7OKO
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

    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_CROP;
    auto& buffer = node->surfaceHandler_->buffer_.buffer;
    auto surface = static_cast<ConsumerSurface*>(node->surfaceHandler_->consumer_.refs_);
    ASSERT_NE(surface, nullptr);

    surface->consumer_ = nullptr;
    ASSERT_EQ(surface->GetScalingMode(buffer->GetSeqNum(), scalingMode), GSERROR_INVALID_ARGUMENTS);

    rsUniRenderVisitor->UpdateHwcNodeByTransform(*node);
}

/*
 * @tc.name: CheckLuminanceStatusChangeTest001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckLuminanceStatusChangeTest
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
 */
HWTEST_F(RSUniRenderVisitorTest, CheckLuminanceStatusChangeTest001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    for (int i = 0; i < 10; i++) {
        auto id = static_cast<ScreenId>(i);
        RSMainThread::Instance()->SetLuminanceChangingStatus(id, true);
        ASSERT_EQ(rsUniRenderVisitor->CheckLuminanceStatusChange(id), true);
        RSMainThread::Instance()->SetLuminanceChangingStatus(id, false);
        ASSERT_EQ(rsUniRenderVisitor->CheckLuminanceStatusChange(id), false);
    }
}

/*
 * @tc.name: UpdateSurfaceRenderNodeScaleTest
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateSurfaceRenderNodeScaleTest
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
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
    pid_t pid1 = ExtractPid(id1);
    nodeMap.renderNodeMap_[pid1][id1] = filterNode1;
    NodeId id2 = 3;
    pid_t pid2 = ExtractPid(id2);
    auto filterNode2 = std::make_shared<RSRenderNode>(id2);
    ASSERT_NE(filterNode2, nullptr);
    ASSERT_NE(filterNode2->renderContent_, nullptr);
    filterNode2->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    nodeMap.renderNodeMap_[pid2][id2] = filterNode2;
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
 * @tc.name: MarkBlurIntersectWithDRM
 * @tc.desc: Test RSUniRenderVisitorTest.MarkBlurIntersectWithDRM001 while surface node nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIAQZ4I
 */
HWTEST_F(RSUniRenderVisitorTest, MarkBlurIntersectWithDRM001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->MarkBlurIntersectWithDRM(surfaceNode);
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
    pid_t pid0 = ExtractPid(id0);
    nodeMap.renderNodeMap_[pid0][id0] = nullptr;
    NodeId id1 = 1;
    pid_t pid1 = ExtractPid(id1);
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[pid1][id1] = node1;
    NodeId id2 = 2;
    pid_t pid2 = ExtractPid(id2);
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[pid2][id2] = node2;

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
 * @tc.name: CheckMergeDisplayDirtyByTransparentFilter
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeDisplayDirtyByTransparentFilter
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparentFilter003, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id0 = 0;
    pid_t pid0 = ExtractPid(id0);
    nodeMap.renderNodeMap_[pid0][id0] = nullptr;
    NodeId id1 = 1;
    pid_t pid1 = ExtractPid(id1);
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[pid1][id1] = node1;
    NodeId id2 = 2;
    pid_t pid2 = ExtractPid(id2);
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[pid2][id2] = node2;

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
    ASSERT_NE(node2->renderContent_, nullptr);
    node2->renderContent_->renderProperties_.SetNeedDrawBehindWindow(true);
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
 * @tc.name: PrepareCanvasRenderNodeTest
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareCanvasRenderNodeTest
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
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
 * @tc.require: issueIB8P46
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareChildren001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    rsBaseRenderNode->InitRenderParams();
    ASSERT_NE(rsBaseRenderNode->stagingRenderParams_, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareChildren(*rsBaseRenderNode);
}

/*
 * @tc.name: DrawTargetSurfaceDirtyRegionForDFX001
 * @tc.desc: DrawTargetSurfaceDirtyRegionForDFX Test
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
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

/*
 * @tc.name: DrawAllSurfaceOpaqueRegionForDFX001
 * @tc.desc: DrawAllSurfaceOpaqueRegionForDFX Test
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
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
 * @tc.name: PrepareProxyRenderNode001
 * @tc.desc: PrepareProxyRenderNode001 Test
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
 */
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
    ASSERT_NE(rsProxyRenderNode, nullptr);
    rsUniRenderVisitor->PrepareProxyRenderNode(*rsProxyRenderNode);
    rsUniRenderVisitor->ProcessProxyRenderNode(*rsProxyRenderNode);

    config.id = 1;
    auto rsSurfaceRenderNodeS = std::make_shared<RSSurfaceRenderNode>(config);
    rsSurfaceRenderNodeS->AddChild(rsProxyRenderNode, 1);
    ASSERT_FALSE(rsSurfaceRenderNodeS->children_.empty());
    rsProxyRenderNode->Prepare(rsUniRenderVisitor);
}
/*
 * @tc.name: RSDisplayRenderNode001
 * @tc.desc: RSDisplayRenderNode001 Test
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
 */
HWTEST_F(RSUniRenderVisitorTest, RSDisplayRenderNode001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(0, config, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsDisplayRenderNode->Prepare(rsUniRenderVisitor);
    rsDisplayRenderNode->Process(rsUniRenderVisitor);
}

/*
 * @tc.name: CheckQuickSkipPrepareParamSetAndGetValid001
 * @tc.desc: Check if param set and get apis are valid.
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
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
    const static int paddingDigit = 1;
    NodeId testPid = testId << paddingDigit;
    pid = ExtractPid(testPid);
    ASSERT_EQ(pid, testId);
    (void)system::SetParameter("rosen.quickskipprepare.enabled", std::to_string(defaultParam));
}

/*
 * @tc.name: CheckSurfaceRenderNodeStatic001
 * @tc.desc: Generate static surface render node(app window node) and execute preparation step.
 *           Get trace and check corresponding node's preparation and 'Skip' info exist.
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
*/
HWTEST_F(RSUniRenderVisitorTest, CheckSurfaceRenderNodeStatic001, TestSize.Level1)
{
    int defaultParam = (int)RSSystemParameters::GetQuickSkipPrepareType();
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "1");
    ASSERT_EQ((int)RSSystemParameters::GetQuickSkipPrepareType(), 1);
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

/*
 * @tc.name: CheckSurfaceRenderNodeNotStatic001
 * @tc.desc: Generate not static surface render node(self drawing, leash window) and execute preparation step.
 *           Get trace and check corresponding node's preparation exists and no 'Skip' info.
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
*/
HWTEST_F(RSUniRenderVisitorTest, CheckSurfaceRenderNodeNotStatic001, TestSize.Level1)
{
    int defaultParam = (int)RSSystemParameters::GetQuickSkipPrepareType();
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "1");
    ASSERT_EQ((int)RSSystemParameters::GetQuickSkipPrepareType(), 1);
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
 * @tc.name: CalcDirtyDisplayRegion01
 * @tc.desc: Set surface to transparent, add a canvas node to create a transparent dirty region
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
*/
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyDisplayRegion01, TestSize.Level1)
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

    Occlusion::Rect rect{0, 0, 1, 1};
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
 * @tc.name: PrepareRootRenderNode001
 * @tc.desc: Set surface to transparent, add a canvas node to create a transparent dirty region
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
*/
HWTEST_F(RSUniRenderVisitorTest, PrepareRootRenderNode001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSRootRenderNode>(0, rsContext->weak_from_this());
    rsRootRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);

    auto& property = rsRootRenderNode->GetMutableRenderProperties();
    property.SetVisible(false);
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
    rsUniRenderVisitor->ProcessRootRenderNode(*rsRootRenderNode);
}

/*
 * @tc.name: CalcDirtyRegionForFilterNode01
 * @tc.desc: Create a filter effect to test filter node
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
*/
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyRegionForFilterNode01, TestSize.Level1)
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
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    // create a filter effect
    const float blurRadiusX = 30.0f;
    const float blurRadiusY = 30.0f;
    auto filter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    rsCanvasRenderNode->GetMutableRenderProperties().SetFilter(filter);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
}

/*
 * @tc.name: SetSurfafaceGlobalDirtyRegion01
 * @tc.desc: Add two surfacenode child to test global dirty region
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
*/
HWTEST_F(RSUniRenderVisitorTest, SetSurfafaceGlobalDirtyRegion01, TestSize.Level1)
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
    ASSERT_FALSE(rsDisplayRenderNode->children_.empty());

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
}

/*
 * @tc.name: IsWatermarkFlagChanged001
 * @tc.desc: Test RSUniRenderVisitorTest.IsWatermarkFlagChanged01 test
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
 */
HWTEST_F(RSUniRenderVisitorTest, IsWatermarkFlagChanged001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->IsWatermarkFlagChanged();
}

/**
 * @tc.name: ProcessSurfaceRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode when displaynode is null
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    std::shared_ptr<RSDisplayRenderNode> node = nullptr;

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetParent(node);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
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
 * @tc.name: UpdateSurfaceRenderNodeRotate002
 * @tc.desc: Test UpdateSurfaceRenderNodeRotate when IsMainWindowType is false
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceRenderNodeRotate002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->UpdateSurfaceRenderNodeRotate(*node);
    ASSERT_FALSE(node->isRotating_);
}

/**
 * @tc.name: IsSubTreeOccluded002
 * @tc.desc: Test IsSubTreeOccluded with empty node
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, IsSubTreeOccluded002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isOcclusionEnabled_ = true;

    ASSERT_EQ(rsUniRenderVisitor->IsSubTreeOccluded(*node), true);
    ASSERT_EQ(node->dirtyStatus_, RSRenderNode::NodeDirty::CLEAN);
}

/**
 * @tc.name: IsLeashAndHasMainSubNode002
 * @tc.desc: Test IsLeashAndHasMainSubNode without nullptr
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, IsLeashAndHasMainSubNode002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_FALSE(rsUniRenderVisitor->IsLeashAndHasMainSubNode(*node));
}

/**
 * @tc.name: IsLeashAndHasMainSubNode003
 * @tc.desc: Test IsLeashAndHasMainSubNode with leashAndHasMainSubNode
 * @tc.type: FUNC
 * @tc.require: issueIB20EZ
 */
HWTEST_F(RSUniRenderVisitorTest, IsLeashAndHasMainSubNode003, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    NodeId id = 1;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(canvasNode, nullptr);
    node->AddChild(canvasNode);
    node->GenerateFullChildrenList();
    ASSERT_EQ(rsUniRenderVisitor->IsLeashAndHasMainSubNode(*node), false);
    node->AddChild(surfaceNode);
    node->GenerateFullChildrenList();
    ASSERT_EQ(rsUniRenderVisitor->IsLeashAndHasMainSubNode(*node), true);
}

/**
 * @tc.name: QuickPrepareDisplayRenderNode001
 * @tc.desc: Test QuickPrepareDisplayRenderNode with display node
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareDisplayRenderNode001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(rsDisplayRenderNode->dirtyManager_, nullptr);
    rsDisplayRenderNode->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->QuickPrepareDisplayRenderNode(*rsDisplayRenderNode);
}

/**
 * @tc.name: CheckFilterCacheNeedForceClearOrSave001
 * @tc.desc: Test CheckFilterCacheNeedForceClearOrSave with multi-RSRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterCacheNeedForceClearOrSave001, TestSize.Level2)
{
    auto node = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CheckFilterCacheNeedForceClearOrSave(*node);

    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    node->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    ASSERT_NE(node->renderContent_, nullptr);
    rsUniRenderVisitor->CheckFilterCacheNeedForceClearOrSave(*node);
}

/**
 * @tc.name: PrepareForUIFirstNode001
 * @tc.desc: Test PrepareForUIFirstNode with multi-rsSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForUIFirstNode001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->PrepareForUIFirstNode(*rsSurfaceRenderNode);

    rsSurfaceRenderNode->lastFrameUifirstFlag_ = MultiThreadCacheType::LEASH_WINDOW;
    rsSurfaceRenderNode->isProtectedLayer_ = true;
    rsUniRenderVisitor->PrepareForUIFirstNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: UpdateNodeVisibleRegion001
 * @tc.desc: Test UpdateNodeVisibleRegion when needRecalculateOcclusion_ is true
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateNodeVisibleRegion001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->needRecalculateOcclusion_ = true;
    rsUniRenderVisitor->PrepareForUIFirstNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: CalculateOcclusion004
 * @tc.desc: Test CalculateOcclusion with multi-rsSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOcclusion004, TestSize.Level2)
{
    std::shared_ptr<RSSurfaceRenderNode> rsSurfaceRenderNode = nullptr;
    ASSERT_EQ(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CalculateOcclusion(*rsSurfaceRenderNode);

    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsUniRenderVisitor->needRecalculateOcclusion_ = true;
    rsUniRenderVisitor->isAllSurfaceVisibleDebugEnabled_ = true;
    rsUniRenderVisitor->CalculateOcclusion(*rsSurfaceRenderNode);
}

/**
 * @tc.name: CalculateOcclusion005
 * @tc.desc: Test CalculateOcclusion if need skip in expand screen
 * @tc.type: FUNC
 * @tc.require: issueIB35IS
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOcclusion005, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 0;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();

    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(1, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    rsUniRenderVisitor->curDisplayNode_->SetIsFirstVisitCrossNodeDisplay(true);
    rsUniRenderVisitor->CalculateOcclusion(*rsSurfaceRenderNode);

    rsUniRenderVisitor->curDisplayNode_->SetIsFirstVisitCrossNodeDisplay(false);
    rsUniRenderVisitor->CalculateOcclusion(*rsSurfaceRenderNode);

    rsSurfaceRenderNode->isCrossNode_ = true;
    rsSurfaceRenderNode->firstLevelNodeId_ = config.id;
    rsUniRenderVisitor->CalculateOcclusion(*rsSurfaceRenderNode);
}

/**
 * @tc.name: QuickPrepareCanvasRenderNode001
 * @tc.desc: Test QuickPrepareCanvasRenderNode with multi-params
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareCanvasRenderNode001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);

    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);
}

/**
 * @tc.name: UpdateSubSurfaceNodeRectInSkippedSubTree001
 * @tc.desc: Test UpdateSubSurfaceNodeRectInSkippedSubTree with multi-params
 * @tc.type: FUNC
 * @tc.require: issueIB20EZ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSubSurfaceNodeRectInSkippedSubTree001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = nullptr;
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    rsUniRenderVisitor->UpdateSubSurfaceNodeRectInSkippedSubTree(*node);

    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node->renderContent_, nullptr);
    auto& property = node->renderContent_->renderProperties_;
    property.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    ASSERT_NE(node->GetRenderProperties().GetBoundsGeometry(), nullptr);
    rsUniRenderVisitor->UpdateSubSurfaceNodeRectInSkippedSubTree(*node);
}

/**
 * @tc.name: ProcessAncoNode001
 * @tc.desc: Test ProcessAncoNode without buffer
 * @tc.type: FUNC
 * @tc.require: issueIB7GLO
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessAncoNode001, TestSize.Level2)
{
    auto hwcNodePtr = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(hwcNodePtr, nullptr);
    hwcNodePtr->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));
    hwcNodePtr->SetGlobalAlpha(0.0f);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessAncoNode(hwcNodePtr);

    hwcNodePtr->SetGlobalAlpha(1.0f);
    rsUniRenderVisitor->ProcessAncoNode(hwcNodePtr);
}

/**
 * @tc.name: ProcessAncoNode002
 * @tc.desc: Test ProcessAncoNode with buffer
 * @tc.type: FUNC
 * @tc.require: issueIB7GLO
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessAncoNode002, TestSize.Level2)
{
    auto hwcNodePtr = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNodePtr, nullptr);
    hwcNodePtr->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessAncoNode(hwcNodePtr);
}

/**
 * @tc.name: UpdateDisplayZoomState001
 * @tc.desc: Test UpdateDisplayZoomState
 * @tc.type: FUNC
 * @tc.require: issueIB20EZ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateDisplayZoomState001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = nullptr;
    rsUniRenderVisitor->UpdateDisplayZoomState();

    NodeId id = 1;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    rsUniRenderVisitor->UpdateDisplayZoomState();
    ASSERT_EQ(rsUniRenderVisitor->curDisplayNode_->IsZoomStateChange(), false);
}

/**
 * @tc.name: UpdateLeashWindowVisibleRegionEmpty001
 * @tc.desc: Test UpdateLeashWindowVisibleRegionEmpty without nullptr
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateLeashWindowVisibleRegionEmpty001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto rsSurfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode1, nullptr);
    config.id++;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode2, nullptr);
    config.id++;
    auto rsSurfaceRenderNode3 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode3, nullptr);
    config.id++;
    auto rsSurfaceRenderNode4 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode4, nullptr);
    rsSurfaceRenderNode1->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    rsSurfaceRenderNode2->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    rsSurfaceRenderNode3->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    rsSurfaceRenderNode4->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;

    Occlusion::Region region{ Occlusion::Rect{ 0, 0, 100, 100 } };
    rsSurfaceRenderNode2->SetVisibleRegion(region);
    rsSurfaceRenderNode1->AddChild(rsSurfaceRenderNode2);
    rsSurfaceRenderNode1->GenerateFullChildrenList();
    rsUniRenderVisitor->UpdateLeashWindowVisibleRegionEmpty(*rsSurfaceRenderNode1);

    rsSurfaceRenderNode1->AddChild(rsSurfaceRenderNode3);
    rsSurfaceRenderNode1->RemoveChild(rsSurfaceRenderNode2);
    rsSurfaceRenderNode1->GenerateFullChildrenList();
    rsUniRenderVisitor->UpdateLeashWindowVisibleRegionEmpty(*rsSurfaceRenderNode1);

    rsSurfaceRenderNode1->AddChild(rsSurfaceRenderNode4);
    rsSurfaceRenderNode1->RemoveChild(rsSurfaceRenderNode3);
    rsSurfaceRenderNode1->GenerateFullChildrenList();
    rsUniRenderVisitor->UpdateLeashWindowVisibleRegionEmpty(*rsSurfaceRenderNode1);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode002
 * @tc.desc: Test UpdateHwcNodeInfoForAppNode with multi-params
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode002, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->needCollectHwcNode_ = true;
    rsSurfaceRenderNode->isHardwareEnabledNode_ = true;
    rsSurfaceRenderNode->isHardWareDisabledByReverse_ = false;
    rsSurfaceRenderNode->isFixRotationByUser_ = false;
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->needCollectHwcNode_ = true;
    surfaceNode->visibleRegion_.Reset();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->isHardwareForcedDisabled_ = true;
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);

    rsUniRenderVisitor->isHardwareForcedDisabled_ = false;
    rsSurfaceRenderNode->dynamicHardwareEnable_ = false;
    rsSurfaceRenderNode->isFixRotationByUser_ = true;
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);

    rsSurfaceRenderNode->dynamicHardwareEnable_ = true;
    ASSERT_NE(rsSurfaceRenderNode->renderContent_, nullptr);
    rsSurfaceRenderNode->renderContent_->renderProperties_.boundsGeo_ = nullptr;
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: QuickPrepareChildren001
 * @tc.desc: Test QuickPrepareChildren
 * @tc.type: FUNC
 * @tc.require: issueIB7GLO
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareChildren001, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->lastFrameSubTreeSkipped_ = true;
    auto dirtyManager = rsSurfaceRenderNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = rsSurfaceRenderNode;
    rsUniRenderVisitor->QuickPrepareChildren(*rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with isUIFirst_
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode003, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    surfaceNode->SetParent(node);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with skipLayer
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
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
 * @tc.name: ProcessSurfaceRenderNode005
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with securityLayer
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode005, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
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
 * @tc.name: ProcessSurfaceRenderNode004
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with capture window in directly render
 * @tc.type: FUNC
 * @tc.require: issueIB8P46
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode004, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
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
 * @tc.name: PrepareEffectRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareEffectRenderNode api
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareEffectRenderNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id1 = 0;
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(id1,
        displayConfig, rsContext->weak_from_this());
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    NodeId id2 = 1;
    auto node = std::make_shared<RSEffectRenderNode>(id2, rsContext->weak_from_this());
    node->InitRenderParams();
    rsUniRenderVisitor->PrepareEffectRenderNode(*node);
}

/**
 * @tc.name: ProcessSurfaceRenderNode005
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode while surface node has protected layer
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode005, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

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
 * @tc.name: ProcessSurfaceRenderNode006
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode while surface node does not have protected layer
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode006, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

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
 * @tc.name: ProcessEffectRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessEffectRenderNode api
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessEffectRenderNode001, TestSize.Level1)
{
    NodeId id = 0;
    RSEffectRenderNode node(id);
    node.stagingRenderParams_ = std::make_unique<RSRenderParams>(node.GetId());
    ASSERT_NE(node.stagingRenderParams_, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessEffectRenderNode(node);
}

/**
 * @tc.name: HandleColorGamuts001
 * @tc.desc: HandleColorGamuts for virtual screen
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
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
    displayNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    rsUniRenderVisitor->HandleColorGamuts(*displayNode, screenManager);

    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(displayNode->GetScreenId(), screenColorGamut);
    ASSERT_EQ(displayNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));

    screenManager->RemoveVirtualScreen(virtualScreenId);
}

/**
 * @tc.name: CheckColorSpaceWithSelfDrawingNode001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckColorSpaceWithSelfDrawingNode while
 *           selfDrawingNode's color space is not equal to GRAPHIC_COLOR_GAMUT_SRGB,
 *           and this node will be drawn with gpu
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, CheckColorSpaceWithSelfDrawingNode001, TestSize.Level2)
{
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(selfDrawingNode, nullptr);
    selfDrawingNode->SetHardwareForcedDisabledState(true);
    selfDrawingNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    selfDrawingNode->SetIsOnTheTree(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    GraphicColorGamut colorSpace = GRAPHIC_COLOR_GAMUT_ADOBE_RGB;
    rsUniRenderVisitor->CheckColorSpaceWithSelfDrawingNode(*selfDrawingNode, colorSpace);
    ASSERT_EQ(colorSpace, selfDrawingNode->GetColorSpace());
}

/**
 * @tc.name: CheckColorSpace001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckColorSpace while
 *           app Window node's color space is not equal to GRAPHIC_COLOR_GAMUT_SRGB
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, CheckColorSpace001, TestSize.Level2)
{
    auto appWindowNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(appWindowNode, nullptr);
    appWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    appWindowNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    rsUniRenderVisitor->CheckColorSpace(*appWindowNode);
    ASSERT_EQ(rsUniRenderVisitor->curDisplayNode_->GetColorSpace(), appWindowNode->GetColorSpace());
}

/**
 * @tc.name: PrepareForCrossNodeTest001
 * @tc.desc: Test PrepareForCrossNode
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForCrossNodeTest001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    node->isCrossNode_ = true;
    rsUniRenderVisitor->curDisplayNode_->SetIsFirstVisitCrossNodeDisplay(true);
    rsUniRenderVisitor->PrepareForCrossNode(*node);
}

/**
 * @tc.name: CheckSkipCrossNodeTest001
 * @tc.desc: Test CheckSkipCrossNode
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipCrossNodeTest001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceRenderNode = RSSurfaceRenderNode(1);
    RSDisplayNodeConfig config = {};
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(2, config);
    rsUniRenderVisitor->CheckSkipCrossNode(surfaceRenderNode);
    ASSERT_FALSE(rsUniRenderVisitor->CheckSkipCrossNode(surfaceRenderNode));
    surfaceRenderNode.isCrossNode_ = true;
    ASSERT_FALSE(rsUniRenderVisitor->CheckSkipCrossNode(surfaceRenderNode));
    ASSERT_TRUE(rsUniRenderVisitor->CheckSkipCrossNode(surfaceRenderNode));
}

/**
 * @tc.name: CheckPixelFormatWithSelfDrawingNode001
 * @tc.desc: Test CheckPixelFormatWithSelfDrawingNode
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, CheckPixelFormatWithSelfDrawingNode001, TestSize.Level2)
{
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(selfDrawingNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    GraphicPixelFormat pixelFormat = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    selfDrawingNode->SetIsOnTheTree(false);
    rsUniRenderVisitor->CheckPixelFormatWithSelfDrawingNode(*selfDrawingNode, pixelFormat);
    selfDrawingNode->SetIsOnTheTree(true);
    rsUniRenderVisitor->CheckPixelFormatWithSelfDrawingNode(*selfDrawingNode, pixelFormat);

    selfDrawingNode->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->CheckPixelFormatWithSelfDrawingNode(*selfDrawingNode, pixelFormat);
    selfDrawingNode->SetHardwareForcedDisabledState(true);
    rsUniRenderVisitor->CheckPixelFormatWithSelfDrawingNode(*selfDrawingNode, pixelFormat);

    ASSERT_NE(selfDrawingNode->GetRSSurfaceHandler(), nullptr);
    auto bufferHandle = selfDrawingNode->GetRSSurfaceHandler()->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_1010102;
    rsUniRenderVisitor->CheckPixelFormatWithSelfDrawingNode(*selfDrawingNode, pixelFormat);
}

/*
 * @tc.name: UpdatePixelFormatAfterHwcCalc001
 * @tc.desc: Test UpdatePixelFormatAfterHwcCalc
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdatePixelFormatAfterHwcCalc001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(selfDrawingNode, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    selfDrawingNode->SetAncestorDisplayNode(0, displayNode);
    selfDrawingNode->SetHardwareForcedDisabledState(true);

    ASSERT_NE(selfDrawingNode->GetRSSurfaceHandler(), nullptr);
    auto bufferHandle = selfDrawingNode->GetRSSurfaceHandler()->buffer_.buffer->GetBufferHandle();
    ASSERT_NE(bufferHandle, nullptr);
    bufferHandle->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_1010102;
    rsUniRenderVisitor->UpdatePixelFormatAfterHwcCalc(*displayNode);
}

/**
 * @tc.name: CheckColorSpaceWithSelfDrawingNode002
 * @tc.desc: Test RSUniRenderVisitorTest.CheckColorSpaceWithSelfDrawingNode while
 *           selfDrawingNode's color space is not equal to GRAPHIC_COLOR_GAMUT_SRGB,
 *           and this node will not be drawn with gpu
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, CheckColorSpaceWithSelfDrawingNode002, TestSize.Level2)
{
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(selfDrawingNode, nullptr);
    selfDrawingNode->SetProtectedLayer(true);
    selfDrawingNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    selfDrawingNode->SetIsOnTheTree(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    GraphicColorGamut colorSpace = GRAPHIC_COLOR_GAMUT_ADOBE_RGB;
    rsUniRenderVisitor->CheckColorSpaceWithSelfDrawingNode(*selfDrawingNode, colorSpace);
    ASSERT_NE(colorSpace, selfDrawingNode->GetColorSpace());
}

#ifndef ROSEN_CROSS_PLATFORM
/**
 * @tc.name: UpdateColorSpaceWithMetadata
 * @tc.desc: test results of UpdateColorSpaceWithMetadata, if node has no buffer
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateColorSpaceWithMetadata001, TestSize.Level1)
{
    // register instance root node
    auto instanceRootNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(instanceRootNode, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = instanceRootNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = instanceRootNode;
    // create subsurface node
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = instanceRootNode->GetId();

    ASSERT_NE(surfaceNode->GetInstanceRootNode(), nullptr);
    surfaceNode->UpdateColorSpaceWithMetadata();
    ASSERT_EQ(surfaceNode->GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: UpdateColorSpaceWithMetadata
 * @tc.desc: test results of UpdateColorSpaceWithMetadata, if node has buffer
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateColorSpaceWithMetadata002, TestSize.Level1)
{
    // register instance root node
    auto instanceRootNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(instanceRootNode, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = instanceRootNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = instanceRootNode;
    // create subsurface node
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    const sptr<SurfaceBuffer>& buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    ASSERT_NE(buffer, nullptr);
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = instanceRootNode->GetId();

    ASSERT_NE(surfaceNode->GetInstanceRootNode(), nullptr);
    surfaceNode->UpdateColorSpaceWithMetadata();
    ASSERT_EQ(surfaceNode->GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}
#endif

/*
 * @tc.name: UpdateColorSpaceAfterHwcCalc_001
 * @tc.desc: Test UpdateColorSpaceAfterHwcCalc when there is a P3 selfDrawingNode.
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateColorSpaceAfterHwcCalc_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(selfDrawingNode, nullptr);
    RSMainThread::Instance()->AddSelfDrawingNodes(selfDrawingNode);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    rsUniRenderVisitor->UpdateColorSpaceAfterHwcCalc(*displayNode);
    ASSERT_EQ(displayNode->GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    selfDrawingNode->SetAncestorDisplayNode(displayNode->GetScreenId(), displayNode);
    selfDrawingNode->SetHardwareForcedDisabledState(true);
    selfDrawingNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    rsUniRenderVisitor->UpdateColorSpaceAfterHwcCalc(*displayNode);
    ASSERT_EQ(displayNode->GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
}

/*
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent001
 * @tc.desc: Reset but keep single node's surfaceInfo
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
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
 * @tc.require: issueB94PQ
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
 * @tc.require: issueB94PQ
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
 * @tc.name: UpdateHwcNodeEnableByNodeBelow
 * @tc.desc: Test RSUniRenderVistorTest.UpdateHwcNodeEnableByNodeBelow
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByNodeBelow, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto hwcNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(hwcNode1, nullptr);
    surfaceConfig.id = 2;
    auto hwcNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(hwcNode2, nullptr);
    hwcNode1->SetIsOnTheTree(true);
    hwcNode2->SetIsOnTheTree(false);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode1);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode2);

    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateHwcNodeEnableByNodeBelow();
}

/*
 * @tc.name: UpdateChildHwcNodeEnabledByHwcNodeBelow
 * @tc.desc: Test RSUniRenderVistorTest.UpdateChildHwcNodeEnableByHwcNodeBelow
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
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
 * @tc.require: issueB94PQ
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
 * @tc.require: issueB94PQ
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
 * @tc.require: issueB94PQ
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
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node is capture window
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode002, TestSize.Level2)
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
 * @tc.name: QuickPrepareSurfaceRenderNode0043
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has protected layer
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode003, TestSize.Level2)
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
 * @tc.name: QuickPrepareSurfaceRenderNode0024
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has skip layer
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode004, TestSize.Level2)
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
 * @tc.name: PrepareSurfaceRenderNode005
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSurfaceRenderNode while surface node does not have protected layer
 * @tc.type: FUNC
 * @tc.require: issueB94PQ
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
 * @tc.require: issueIB9EZC
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
 * @tc.require: issueIB9EZC
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
 * @tc.require: issueIB9EZC
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
 * @tc.require: issueIB9EZC
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
 * @tc.require: issueIB9EZC
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
 * @tc.require: issueIB9EZC
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
 * @tc.require: issueIB9EZC
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
 * @tc.require: issueIB9EZC
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeFilterDirtyByIntersectWithDirty001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(DEFAULT_NODE_ID,
        displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();C
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
 * @tc.require: issueIB9EZC
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
 * @tc.type: issueIB9EZC
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

    Occlusion::Region region{ Occlusion::Rect{ 0, 0, 1, 1 } };
    surfaceNode->SetVisibleRegion(region);
    canvasNode->SetOldDirtyInSurface({ 0, 0, 1, 1 });
    rsUniRenderVisitor->UpdateDisplayDirtyAndExtendVisibleRegion();
}

/*
 * @tc.name: CheckMergeDisplayDirtyByPosChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByPosChanged with position change
 * @tc.type: FUNC
 * @tc.require: issueIB9EZC
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
 * @tc.require: issueIB9EZC
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
 * @tc.name: CheckMergeDisplayDirtyByTransparent001
 * @tc.desc: Test CheckMergeDisplayDirtyByTransparent with transparent node
 * @tc.type: FUNC
 * @tc.require: issueIB9EZC
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
    Occlusion::Region visibleRegion{DEFAULT_RECT};
    rsSurfaceRenderNode->SetVisibleRegion(visibleRegion);
    auto dirtyRect = rsSurfaceRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion();
    ASSERT_TRUE(rsSurfaceRenderNode->GetVisibleRegion().IsIntersectWith(dirtyRect));
    rsSurfaceRenderNode->oldDirtyInSurface_ = DEFAULT_RECT;

    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparent(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByZorderChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByZorderChanged with Z order change node
 * @tc.type: FUNC
 * @tc.require: issueIB9EZC
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
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty001
 * @tc.desc: Test UpdateSurfaceDirtyAndGlobalDirty
 * @tc.type: FUNC
 * @tc.require: issueIB9EZC
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
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 1, 1, 1));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_EQ(rsUniRenderVisitor->curDisplayDirtyManager_->dirtyRegion_.left_, 0);
}

/**
 * @tc.name: UpdateDstRect001
 * @tc.desc: Test UpdateDstRect with empty rect
 * @tc.type: FUNC
 * @tc.require: issueIB9EZC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateDstRect001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 1;
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
 * * @tc.name: UpdateHwcNodeEnableByRotateAndAlpha001
 * @tc.desc: Test UpdateHwcNodeEnableByRotateAndAlpha for empty node
 * @tc.type: FUNC
 * @tc.require: issueIB9EZC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByRotateAndAlpha001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->UpdateHwcNodeEnableByRotateAndAlpha(node);
    ASSERT_FALSE(node->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableBySrcRect001
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect with empty node
 * @tc.type: FUNC
 * @tc.require: issueIB9EZC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableBySrcRect001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->UpdateHwcNodeEnableBySrcRect(*node);
    ASSERT_FALSE(node->isHardwareForcedDisabledBySrcRect_);
}

/*
 * @tc.name: UpdateHwcNodeEnableBySrcRect002
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect when consumer is not nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableBySrcRect002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    ASSERT_NE(surfaceNode->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    rsUniRenderVisitor->UpdateHwcNodeEnableBySrcRect(*surfaceNode);
}

/*
 * @tc.name: UpdateHwcNodeEnableBySrcRect003
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect when node is hardware disabled by src rect.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableBySrcRect003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->isHardwareForcedDisabledBySrcRect_ = true;
    ASSERT_TRUE(surfaceNode->IsHardwareDisabledBySrcRect());

    rsUniRenderVisitor->UpdateHwcNodeEnableBySrcRect(*surfaceNode);
}

/*
 * @tc.name: UpdateHwcNodeEnable001
 * @tc.desc: Test UpdateHwcNodeEnable when surfaceNode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable001, TestSize.Level2)
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
 * @tc.name: UpdateHwcNodeEnable002
 * @tc.desc: Test UpdateHwcNodeEnable when hwcNodes is empty.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable002, TestSize.Level2)
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
 * @tc.name: UpdateHwcNodeEnable003
 * @tc.desc: Test UpdateHwcNodeEnable when hwcNodePtr is not on the tree.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable003, TestSize.Level2)
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
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf001
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode is hardware forced disabled.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf001, TestSize.Level2)
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
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf002
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode has corner radius and anco force do direct.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(true);
    surfaceNode->SetAncoFlags(static_cast<uint32_t>(0x0001));
    ASSERT_TRUE(surfaceNode->GetAncoForceDoDirect());

    std::vector<RectI> hwcRects;
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, false);
    EXPECT_EQ(hwcRects.size(), 1);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf003
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode intersects with hwcRects.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI(0, 0, 1, 1));
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(0, 0, 1, 1));
    ASSERT_TRUE(surfaceNode->GetDstRect().Intersect(RectI(0, 0, 1, 1)));

    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 2);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf004
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode does not intersect with hwcRects.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->isProtectedLayer_ = true;
    surfaceNode->isOnTheTree_ = true;
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
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree001
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           curSurfaceNode_ is null or hwcNodes is empty.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree001, TestSize.Level2)
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
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree002
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           hwcNodePtr is nullptr or hwcNodePtr is not on the tree or GetCalcRectInPrepare is true.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree002, TestSize.Level2)
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
        ASSERT_EQ(childNode, nullptr);
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
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree003
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false and parent is null.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree003, TestSize.Level2)
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
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree004
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           parent is not null and findInRoot is false.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree004, TestSize.Level2)
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
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree005
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           parent is not null and findInRoot is true.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree005, TestSize.Level2)
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
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/**
 * @tc.name: UpdateSrcRect001
 * @tc.desc: Test UpdateSrcRect with empty matrix
 * @tc.type: FUNC
 * @tc.require: issueIB9EZC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSrcRect001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 1;
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
 * @tc.name: UpdateSrcRect001
 * @tc.desc: Test UpdateSrcRect with empty matrix
 * @tc.type: FUNC
 * @tc.require: issueIB9EZC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSrcRect001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 1;
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
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, BeforeUpdateSurfaceDirtyCalc001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(1, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: BeforeUpdateSurfaceDirtyCalc002
 * @tc.desc: Test BeforeUpdateSurfaceDirtyCalc with nonEmpty node
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, BeforeUpdateSurfaceDirtyCalc002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(1, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    node->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    node->SetNodeName("CapsuleWindow");
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: NeedPrepareChindrenInReverseOrder001
 * @tc.desc: Test NeedPrepareChindrenInReverseOrder with default constructed visitor
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, NeedPrepareChindrenInReverseOrder001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(1, rsContext->weak_from_this());
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
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, NeedPrepareChindrenInReverseOrder002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(1, rsContext->weak_from_this());
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
 * @tc.name: SurfaceOcclusionCallbackToWMS001
 * @tc.desc: Test SurfaceOcclusionCallbackToWMS with default constructed visitor
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
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
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, GetCurrentBlackList001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    EXPECT_TRUE(rsUniRenderVisitor->GetCurrentBlackList().empty());
}

/**
 * @tc.name: GetCurrentBlackList002
 * @tc.desc: Test GetCurrentBlackList002, screenManager_ && curDisplayNode_ != nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, GetCurrentBlackList002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 1;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(id, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_EQ(rsUniRenderVisitor->GetCurrentBlackList().size(), 0);
}

/**
 * @tc.name: IsLeashAndHasMainSubNode001
 * @tc.desc: Test IsLeashAndHasMainSubNode with empty node
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, IsLeashAndHasMainSubNode001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    ASSERT_FALSE(rsUniRenderVisitor->IsLeashAndHasMainSubNode(*node));
}

/**
 * @tc.name: QuickPrepareDisplayRenderNode003
 * @tc.desc: Test QuickPrepareDisplayRenderNode with display node
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareDisplayRenderNode003, TestSize.Level2)
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
 * @tc.name: UpdateVirtualScreenInfo001
 * @tc.desc: Test UpdateVirtualScreenInfo for no-mirror display
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateVirtualScreenInfo001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_EQ(rsDisplayRenderNode->IsMirrorDisplay(), false);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->UpdateVirtualScreenInfo(*rsDisplayRenderNode);
}

/**
 * @tc.name: CalculateOcclusion001
 * @tc.desc: Test CalculateOcclusion with empty node
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOcclusion001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 1;
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
 * @tc.name: UpdateVirtualScreenInfo002
 * @tc.desc: Test UpdateVirtualScreenInfo for mirror display with normal params
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateVirtualScreenInfo002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto screenManager = CreateOrGetScreenManager();
    // 480, 320 width and height for test
    auto id = screenManager->CreateVirtualScreen("virtualScreen01", 1, 1, nullptr);
    // 480, 320 width and height for test
    auto mirrorId = screenManager->CreateVirtualScreen("virtualScreen02", 1, 1, nullptr);
    screenManager->SetScreenMirror(id, mirrorId);

    displayConfig.screenId = id;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->SetIsMirrorDisplay(true);

    displayConfig.screenId = mirrorId;
    auto mirrorNode = std::make_shared<RSDisplayRenderNode>(mirrorId, displayConfig, rsContext->weak_from_this());
    mirrorNode->AddSecurityLayer(1);  // layerId for test
    mirrorNode->AddSecurityLayer(2);  // layerId for test
    mirrorNode->AddSecurityVisibleLayer(1);  // layerId for test
    mirrorNode->AddSecurityVisibleLayer(2);  // layerId for test
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->screenManager_ = screenManager;
    ASSERT_NE(rsUniRenderVisitor->screenManager_, nullptr);
    rsUniRenderVisitor->UpdateVirtualScreenInfo(*rsDisplayRenderNode);

    screenManager->SetVirtualScreenSecurityExemptionList(id, {1});
    rsUniRenderVisitor->UpdateVirtualScreenInfo(*rsDisplayRenderNode);

    screenManager->SetVirtualScreenSecurityExemptionList(id, {1, 2});  // layerId for test
    rsUniRenderVisitor->UpdateVirtualScreenInfo(*rsDisplayRenderNode);

    screenManager->SetVirtualScreenSecurityExemptionList(id, {1, 2, 3});  // layerId for test
    rsUniRenderVisitor->UpdateVirtualScreenInfo(*rsDisplayRenderNode);

    screenManager->SetVirtualScreenSecurityExemptionList(id, {1, 3});  // layerId for test
    rsUniRenderVisitor->UpdateVirtualScreenInfo(*rsDisplayRenderNode);

    screenManager->SetMirrorScreenVisibleRect(id, {0, 0, 720, 1280});  // rect for test
    rsUniRenderVisitor->UpdateVirtualScreenInfo(*rsDisplayRenderNode);
}

/**
 * @tc.name: IsSubTreeOccluded01
 * @tc.desc: Test IsSubTreeOccluded with empty node
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, IsSubTreeOccluded001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->IsSubTreeOccluded(*node);
    ASSERT_EQ(node->dirtyStatus_, RSRenderNode::NodeDirty::CLEAN);
}

/**
 * @tc.name: UpdateSurfaceRenderNodeRotate01
 * @tc.desc: Test UpdateSurfaceRenderNodeRotate with empty node
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceRenderNodeRotate001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->UpdateSurfaceRenderNodeRotate(*node);
    ASSERT_FALSE(node->isRotating_);
}

/*
 * @tc.name: UpdateHwcNodeEnableByFilterRect001
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeEnableByFilterRect with intersect rect
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
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

    constexpr uint32_t left = 0;
    constexpr uint32_t top = 0;
    constexpr uint32_t width = 300;
    constexpr uint32_t height = 300;
    RectI rect{left, top, width, height};
    surfaceNode2->SetDstRect(rect);
    surfaceNode1->AddChildHardwareEnabledNode(surfaceNode2);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeEnableByFilterRect(surfaceNode1, rect, 1);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateVirtualScreenInfo003
 * @tc.desc: Test UpdateVirtualScreenInfo for mirror display with abnormal params
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateVirtualScreenInfo003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->SetIsMirrorDisplay(true);
    ASSERT_EQ(rsDisplayRenderNode->GetMirrorSource().lock(), nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_EQ(rsUniRenderVisitor->screenManager_, nullptr);
    rsUniRenderVisitor->UpdateVirtualScreenInfo(*rsDisplayRenderNode);

    // 12 non-zero node id
    auto mirrorNode = std::make_shared<RSDisplayRenderNode>(12, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;
    rsUniRenderVisitor->UpdateVirtualScreenInfo(*rsDisplayRenderNode);
}

/*
 * @tc.name: UpdateHwcNodeEnableByFilterRect001
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeEnableByFilterRect with empty rect
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
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
    rsUniRenderVisitor->UpdateHwcNodeEnableByFilterRect(surfaceNode1, rect, 1);
}

/*
 * @tc.name: UpdateHwcNodeEnableByFilterRect001
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeEnableByFilterRect with no hwcNode
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByFilterRect003, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);

    constexpr uint32_t left = 0;
    constexpr uint32_t top = 0;
    constexpr uint32_t width = 300;
    constexpr uint32_t height = 300;
    RectI rect{left, top, width, height};
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeEnableByFilterRect(surfaceNode, rect, 1);
}

/*
 * @tc.name: UpdateHwcNodeEnableByGlobalCleanFilter001
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalCleanFilter when Intersect return false.
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
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
 * @tc.name: UpdateHwcNodeEnableByGlobalCleanFilter002
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalCleanFilter when rendernode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
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
    pid_t pid = ExtractPid(id);
    nodeMap.renderNodeMap_[pid][id] = nullptr;

    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter, *surfaceNode);
    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateHwcNodeEnableByGlobalCleanFilter003
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalCleanFilter when rendernode is not null and AIBarFilterCache is not valid.
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
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
    pid_t pid = ExtractPid(id);
    auto node = std::make_shared<RSRenderNode>(id);
    nodeMap.renderNodeMap_[pid][id] = node;
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(node->IsAIBarFilterCacheValid());

    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter, *surfaceNode);
    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateHwcNodeEnableBySrcRect001
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect when node is hardware forced disabled.
 * @tc.type: FUNC
 * @tc.require: issueIBD11A
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

} // OHOS::Rosen