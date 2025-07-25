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

#include "common/rs_common_hook.h"
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/rs_test_util.h"
#include "system/rs_system_parameters.h"

#include "consumer_surface.h"
#include "draw/color.h"
#include "monitor/self_drawing_node_monitor.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/hwc/rs_uni_hwc_visitor.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "feature/round_corner_display/rs_round_corner_display.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;
namespace {
const OHOS::Rosen::RectI DEFAULT_RECT = {0, 80, 1000, 1000};
}
namespace OHOS::Rosen {
class RSUniHwcVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniHwcVisitorTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniHwcVisitorTest::TearDownTestCase() {}
void RSUniHwcVisitorTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
}
void RSUniHwcVisitorTest::TearDown() {}

/**
 * @tc.name: UpdateSrcRect001
 * @tc.desc: Test UpdateSrcRect with empty matrix
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateSrcRect001, TestSize.Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node1(id);
    node1.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node1.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node1.renderProperties_.SetBoundsWidth(2440);
    node1.renderProperties_.SetBoundsHeight(1080);
    node1.renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    node1.SetDstRect({0, 1000, 2440, 1080});
    node1.isFixRotationByUser_ = false;
    Drawing::Matrix totalMatrix;
    totalMatrix.SetMatrix(1, 0, 0, 0, 0, 0, 0, 0, 1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    ASSERT_NE(rsUniRenderVisitor->hwcVisitor_, nullptr);
    auto context = std::make_shared<RSContext>();
    constexpr NodeId DEFAULT_ID = 0xFFFF;
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID, 0, context);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    ScreenInfo screenInfo;
    screenInfo.width = 1440;
    screenInfo.height = 1080;
    rsUniRenderVisitor->curScreenNode_->SetScreenInfo(screenInfo);
    rsUniRenderVisitor->hwcVisitor_->UpdateSrcRect(node1, totalMatrix);
    RectI expectedSrcRect = {0, 0, 1440, 1080};
    EXPECT_TRUE(node1.GetSrcRect() == expectedSrcRect);
}

/**
 * @tc.name: UpdateSrcRect002
 * @tc.desc: Test UpdateSrcRect after applying valid clipRects to the boundary of a surface node
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateSrcRect002, TestSize.Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node1(id);
    node1.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node1.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node1.renderProperties_.SetBoundsWidth(2440);
    node1.renderProperties_.SetBoundsHeight(1080);
    node1.renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    node1.SetDstRect({0, 1000, 1440, 880});
    node1.isFixRotationByUser_ = false;
    Drawing::Matrix totalMatrix;
    totalMatrix.SetMatrix(1, 0, 0, 0, 1, 800, 0, 0, 1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    auto context = std::make_shared<RSContext>();
    constexpr NodeId DEFAULT_ID = 0xFFFF;
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID, 0, context);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    ScreenInfo screenInfo;
    screenInfo.width = 1440;
    screenInfo.height = 1080;
    rsUniRenderVisitor->curScreenNode_->SetScreenInfo(screenInfo);
    rsUniHwcVisitor->UpdateSrcRect(node1, totalMatrix);
    RectI expectedSrcRect = {0, 306, 638, 1347};
    EXPECT_TRUE(node1.GetSrcRect() == expectedSrcRect);
}

/**
 * @tc.name: UpdateSrcRect003
 * @tc.desc: Test UpdateSrcRect when we use the full boundary of a surface node
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateSrcRect003, TestSize.Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node1(id);
    node1.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node1.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node1.renderProperties_.SetBoundsWidth(2440);
    node1.renderProperties_.SetBoundsHeight(1080);
    node1.renderProperties_.frameGravity_ = Gravity::RESIZE;
    node1.SetDstRect({0, 1000, 2440, 1080});
    node1.isFixRotationByUser_ = false;
    Drawing::Matrix totalMatrix;
    totalMatrix.SetMatrix(1, 0, 0, 0, 1, 1000, 0, 0, 1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    auto context = std::make_shared<RSContext>();
    constexpr NodeId DEFAULT_ID = 0xFFFF;
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID, 0, context);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    ScreenInfo screenInfo;
    screenInfo.width = 1440;
    screenInfo.height = 1080;
    rsUniRenderVisitor->curScreenNode_->SetScreenInfo(screenInfo);
    rsUniHwcVisitor->UpdateSrcRect(node1, totalMatrix);
    RectI expectedSrcRect = {0, 0, 1080, 1653};
    EXPECT_TRUE(node1.GetSrcRect() == expectedSrcRect);
}

/**
 * @tc.name: UpdateDstRect001
 * @tc.desc: Test UpdateDstRect with empty rect
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateDstRect001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    config.id = 11;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode2, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    rsSurfaceRenderNode2->InitRenderParams();
    rsSurfaceRenderNode2->SetNodeName("testNode");
    rsSurfaceRenderNode2->SetLayerTop(true);
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(12, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    RectI absRect(0, 0, 0, 0);
    RectI clipRect(0, 0, 0, 0);
    rsUniRenderVisitor->curSurfaceNode_ = rsSurfaceRenderNode2;
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniHwcVisitor->UpdateDstRect(*rsSurfaceRenderNode, absRect, clipRect);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    ASSERT_EQ(rsSurfaceRenderNode->GetDstRect().left_, 0);
    rsUniHwcVisitor->UpdateDstRect(*rsSurfaceRenderNode2, absRect, clipRect);
    ASSERT_EQ(rsSurfaceRenderNode2->GetDstRect().left_, 0);
}

/**
 * @tc.name: UpdateDstRect002
 * @tc.desc: Test UpdateDstRect function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateDstRect002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    config.id = 11;
    rsSurfaceRenderNode->InitRenderParams();
    rsSurfaceRenderNode->SetHwcGlobalPositionEnabled(true);

    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(12, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    RectI absRect(0, 0, 0, 0);
    RectI clipRect(0, 0, 0, 0);
    rsUniHwcVisitor->UpdateDstRect(*rsSurfaceRenderNode, absRect, clipRect);
    ASSERT_EQ(rsSurfaceRenderNode->GetDstRect().left_, 0);
}

/**
 * @tc.name: UpdateHwcNodeByTransform_001
 * @tc.desc: UpdateHwcNodeByTransform Test, buffer of RSSurfaceHandler is not nullptr, and
 * consumer_ of of RSSurfaceHandler is nullptr, expect return directly
 * @tc.type:FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeByTransform_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(12, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(!node->GetRSSurfaceHandler() || !node->GetRSSurfaceHandler()->GetBuffer());

    node->surfaceHandler_->consumer_ = nullptr;
    ASSERT_EQ(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    rsUniHwcVisitor->UpdateHwcNodeByTransform(*node, matrix);
}

/**
 * @tc.name: UpdateHwcNodeByTransform_002
 * @tc.desc: UpdateHwcNodeByTransform Test, buffer of RSSurfaceHandler is not nullptr, and
 * consumer_ of of RSSurfaceHandler is not nullptr, and GetScalingMode is GSERROR_OK
 * scalingMode == ScalingMode::SCALING_MODE_SCALE_TO_WINDOW, expect neither LayerScaleDown nor LayerScaleFit
 * @tc.type:FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeByTransform_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(12, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(!node->GetRSSurfaceHandler() || !node->GetRSSurfaceHandler()->GetBuffer());

    const auto& surface = node->GetRSSurfaceHandler()->GetConsumer();
    ASSERT_NE(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    const auto& buffer = node->GetRSSurfaceHandler()->GetBuffer();
    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    rsUniHwcVisitor->UpdateHwcNodeByTransform(*node, matrix);
    ASSERT_EQ(node->GetRSSurfaceHandler()->GetBuffer()->GetSurfaceBufferScalingMode(), SCALING_MODE_SCALE_TO_WINDOW);
}

/**
 * @tc.name: UpdateHwcNodeByTransform_003
 * @tc.desc: UpdateHwcNodeByTransform Test, buffer of RSSurfaceHandler is not nullptr, and
 * consumer_ of of RSSurfaceHandler is not nullptr, and GetScalingMode is GSERROR_INVALID_ARGUMENTS
 * scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP, expect LayerScaleDown
 * @tc.type:FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeByTransform_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(12, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

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

    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    rsUniHwcVisitor->UpdateHwcNodeByTransform(*node, matrix);
}

/**
 * @tc.name: UpdateHwcNodeByTransform_004
 * @tc.desc: UpdateHwcNodeByTransform Test, buffer of RSSurfaceHandler is not nullptr, and
 * consumer_ of of RSSurfaceHandler is not nullptr, and GetScalingMode is GSERROR_INVALID_ARGUMENTS
 * scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT, expect LayerScaleFit
 * @tc.type:FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeByTransform_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(12, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(!node->GetRSSurfaceHandler() || !node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_NE(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    auto nodeParams = static_cast<RSSurfaceRenderParams*>(node->GetStagingRenderParams().get());
    ASSERT_NE(nodeParams, nullptr);

    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_FIT;
    auto& buffer = node->surfaceHandler_->buffer_.buffer;
    auto surface = static_cast<ConsumerSurface*>(node->surfaceHandler_->consumer_.refs_);
    ASSERT_NE(surface, nullptr);

    surface->consumer_ = nullptr;
    ASSERT_EQ(surface->GetScalingMode(buffer->GetSeqNum(), scalingMode), GSERROR_INVALID_ARGUMENTS);

    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    rsUniHwcVisitor->UpdateHwcNodeByTransform(*node, matrix);
}

/**
 * @tc.name: UpdateHwcNodeByTransform_005
 * @tc.desc: UpdateHwcNodeByTransform Test, buffer of RSSurfaceHandler is not nullptr, and
 * consumer_ of of RSSurfaceHandler is not nullptr, and GetScalingMode is GSERROR_INVALID_ARGUMENTS
 * scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT, expect LayerScaleFit
 * Because the solidLayer solution takes effect, API 18 is used for calculation.
 * @tc.type:FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeByTransform_005, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(rsUniRenderVisitor->hwcVisitor_, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(12, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(!node->GetRSSurfaceHandler() || !node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_NE(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_FIT;
    auto& buffer = node->surfaceHandler_->buffer_.buffer;
    auto surface = static_cast<ConsumerSurface*>(node->surfaceHandler_->consumer_.refs_);
    ASSERT_NE(surface, nullptr);

    surface->consumer_ = nullptr;
    ASSERT_EQ(surface->GetScalingMode(buffer->GetSeqNum(), scalingMode), GSERROR_INVALID_ARGUMENTS);

    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(node->GetStagingRenderParams().get());
    ASSERT_NE(stagingSurfaceParams, nullptr);
    stagingSurfaceParams->SetIsHwcEnabledBySolidLayer(true);
    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeByTransform(*node, matrix);
}

/**
 * @tc.name: UpdateHwcNodeEnableByBufferSize
 * @tc.desc: Test UpdateHwcNodeEnableByBufferSize with rosen-web node / non-rosen-web node.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByBufferSize, TestSize.Level1)
{
    // create input args.
    auto node1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto node2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node1->name_ = "RosenWeb_test";
    auto node3 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node3->GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node3->renderProperties_.frameGravity_ = Gravity::RESIZE;
    auto node4 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node4->GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node4->GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node4->GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node4->GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node4->renderProperties_.SetBoundsWidth(2440);
    node4->renderProperties_.SetBoundsHeight(1080);
    node4->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    rsUniHwcVisitor->UpdateHwcNodeEnableByBufferSize(*node1);
    rsUniHwcVisitor->UpdateHwcNodeEnableByBufferSize(*node2);
    rsUniHwcVisitor->UpdateHwcNodeEnableByBufferSize(*node3);
    rsUniHwcVisitor->UpdateHwcNodeEnableByBufferSize(*node4);
}

/**
 * @tc.name: UpdateHwcNodeEnable_001
 * @tc.desc: Test UpdateHwcNodeEnable when surfaceNode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnable_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;

    rsUniHwcVisitor->UpdateHwcNodeEnable();
}

/**
 * @tc.name: UpdateHwcNodeEnable_002
 * @tc.desc: Test UpdateHwcNodeEnable when hwcNodes is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnable_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->ResetChildHardwareEnabledNodes();
    ASSERT_EQ(surfaceNode->GetChildHardwareEnabledNodes().size(), 0);
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    rsUniRenderVisitor->curScreenNode_ = displayNode;

    rsUniHwcVisitor->UpdateHwcNodeEnable();
}

/**
 * @tc.name: UpdateHwcNodeEnable_003
 * @tc.desc: Test UpdateHwcNodeEnable when hwcNodePtr is not on the tree.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnable_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    NodeId childId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childId);
    childNode->SetIsOnTheTree(false);
    ASSERT_FALSE(childNode->IsOnTheTree());
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    rsUniRenderVisitor->curScreenNode_ = displayNode;

    rsUniHwcVisitor->UpdateHwcNodeEnable();
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_001
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode is hardware forced disabled.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());

    std::vector<RectI> hwcRects;
    rsUniRenderVisitor->curScreenNode_ = nullptr;
    rsUniHwcVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 0);
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext);
    rsUniHwcVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 0);
    rsUniRenderVisitor->curScreenNode_->InitRenderParams();
    rsUniRenderVisitor->curScreenNode_->SetForceCloseHdr(false);
    surfaceNode->SetVideoHdrStatus(HdrStatus::HDR_VIDEO);
    EXPECT_EQ(rsUniRenderVisitor->curScreenNode_->GetHasUniRenderHdrSurface(), false);
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_002
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode has corner radius and anco force do direct.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(true);
    surfaceNode->SetAncoFlags(static_cast<uint32_t>(0x0001));
    ASSERT_TRUE(surfaceNode->GetAncoForceDoDirect());

    std::vector<RectI> hwcRects;
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext);
    rsUniHwcVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, false);
    EXPECT_EQ(hwcRects.size(), 1);
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_003
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode intersects with hwcRects.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI(0, 0, 100, 100));
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(50, 50, 100, 100));
    ASSERT_TRUE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext);
    rsUniHwcVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 2);
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf_004
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode does not intersect with hwcRects.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI());
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(50, 50, 100, 100));
    ASSERT_FALSE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext);
    rsUniHwcVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 2);
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp001
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp with empty Rect
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(rsUniRenderVisitor->hwcVisitor_, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(0, 0, 0, 0);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(surfaceNode, hwcRects);
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);

    hwcRects.clear();
    hwcRects.emplace_back(0, 100, 100, 200);
    surfaceNode->SetDstRect(RectI(0, 199, 100, 300));
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(surfaceNode, hwcRects);

    hwcRects.clear();
    hwcRects.emplace_back(0, 199, 100, 200);
    surfaceNode->SetDstRect(RectI(0, 100, 100, 200));
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(surfaceNode, hwcRects);

    hwcRects.clear();
    surfaceNode->SetHardwareForcedDisabledState(true);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(surfaceNode, hwcRects);

    hwcRects.clear();
    hwcRects.emplace_back(0, 100, 100, 200);
    surfaceNode->SetDstRect(RectI(0, 150, 100, 200));
    surfaceNode->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(surfaceNode, hwcRects);
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp002
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp when hwcNode is hardware forced disabled.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(rsUniRenderVisitor->hwcVisitor_, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());

    std::vector<RectI> hwcRects;
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(surfaceNode, hwcRects);
    EXPECT_EQ(hwcRects.size(), 1);
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp003
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp when hwcNode is anco force do direct.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(rsUniRenderVisitor->hwcVisitor_, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(true);
    surfaceNode->SetAncoFlags(static_cast<uint32_t>(0x1));
    ASSERT_TRUE(surfaceNode->GetAncoForceDoDirect());
    
    std::vector<RectI> hwcRects;
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(surfaceNode, hwcRects);
    EXPECT_EQ(hwcRects.size(), 1);
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp004
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp when dst.Intersect(rect) equals true.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(rsUniRenderVisitor->hwcVisitor_, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI(0, 0, 100, 100));
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(50, 50, 100, 100);
    ASSERT_TRUE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(surfaceNode, hwcRects);
    EXPECT_EQ(hwcRects.size(), 1);
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp005
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp when dst.Intersect(rect) equals false.
 * @tc.type: FUNC
 * @tc.require: issueIBD45W
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp005, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(rsUniRenderVisitor->hwcVisitor_, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI());
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(50, 50, 100, 100);
    ASSERT_FALSE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(surfaceNode, hwcRects);
    EXPECT_EQ(hwcRects.size(), 2);
}

/**
 * @tc.name: UpdateHwcNodeEnableByNodeBelow
 * @tc.desc: Test RSUniRenderVistorTest.UpdateHwcNodeEnableByNodeBelow
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByNodeBelow, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());

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
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniHwcVisitor->UpdateHwcNodeEnableByNodeBelow();
}

/**
 * @tc.name: UpdateHwcNodeEnableByRotate001
 * @tc.desc: Test UpdateHwcNodeEnableByRotate
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByRotate_001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByRotate(node);
    ASSERT_FALSE(node->isHardwareForcedDisabled_);

    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(surfaceNode, nullptr);
    Drawing::Matrix matrix;
    surfaceNode->SetTotalMatrix(matrix);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByRotate(surfaceNode);
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByAlpha_001
 * @tc.desc: Test UpdateHwcNodeEnableByAlpha
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByAlpha_001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByAlpha(node);
    ASSERT_FALSE(node->isHardwareForcedDisabled_);

    NodeId id = 1;
    RSSurfaceNodeType type = RSSurfaceNodeType::DEFAULT;
    RSSurfaceRenderNodeConfig config = { .id = id, .nodeType = type };
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    EXPECT_NE(surfaceNode, nullptr);
    surfaceNode->SetGlobalAlpha(0.0f);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByAlpha(surfaceNode);
    ASSERT_TRUE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHardwareStateByBoundNEDstRectInApps001
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHardwareStateByBoundNEDstRectInApps001
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHardwareStateByBoundNEDstRectInApps001, TestSize.Level1)
{
    std::vector<RectI> abovedBounds;

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetHardwareForcedDisabledState(false);
    surfaceNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = {0, 0, 200, 200};

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    // test hwcNodes is empty
    rsUniRenderVisitor->hwcVisitor_->UpdateHardwareStateByBoundNEDstRectInApps(hwcNodes, abovedBounds);

    // test hwcNodes is not empty, and abovedBounds is empty
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    rsUniRenderVisitor->hwcVisitor_->UpdateHardwareStateByBoundNEDstRectInApps(hwcNodes, abovedBounds);

    surfaceNode->SetHardwareForcedDisabledState(true);
    rsUniRenderVisitor->hwcVisitor_->UpdateHardwareStateByBoundNEDstRectInApps(hwcNodes, abovedBounds);

    // test hwcNodes is not empty, and abovedBounds is not empty
    abovedBounds.emplace_back(RectI{0, 0, 200, 200});
    rsUniRenderVisitor->hwcVisitor_->UpdateHardwareStateByBoundNEDstRectInApps(hwcNodes, abovedBounds);
}

/**
 * @tc.name: UpdateHardwareStateByBoundNEDstRectInApps002
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHardwareStateByBoundNEDstRectInApps002
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHardwareStateByBoundNEDstRectInApps002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    std::vector<RectI> abovedBounds;
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = {0, 0, 200, 200};
    // abovedRect is inside of Bounds and equal to bound.
    surfaceNode->SetDstRect({0, 0, 200, 200});

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->GetRenderProperties().GetBoundsGeometry()->absRect_ = {0, 0, 200, 200};
    surfaceNode2->SetAncoFlags(false);
    surfaceNode2->SetAncoForceDoDirect(false);
    // reverse push
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode2));
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    surfaceNode2->SetDstRect({0, 0, 200, 200});
    rsUniRenderVisitor->hwcVisitor_->UpdateHardwareStateByBoundNEDstRectInApps(hwcNodes, abovedBounds);
    ASSERT_FALSE(surfaceNode2->IsHardwareForcedDisabled());

    // abovedRect is inside of Bounds and not equal to bound.
    surfaceNode->SetDstRect({0, 0, 200, 100});
    rsUniRenderVisitor->hwcVisitor_->UpdateHardwareStateByBoundNEDstRectInApps(hwcNodes, abovedBounds);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardwareStateByBoundNEDstRectInApps003
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHardwareStateByBoundNEDstRectInApps003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHardwareStateByBoundNEDstRectInApps003, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(rsUniRenderVisitor->hwcVisitor_, nullptr);

    std::vector<RectI> aboveBounds;
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetHardwareForcedDisabledState(false);
    surfaceNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectI{0, 0, 200, 200};

    // test Anco node
    surfaceNode->SetAncoFlags(true);
    surfaceNode->SetAncoForceDoDirect(true);
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    rsUniRenderVisitor->hwcVisitor_->UpdateHardwareStateByBoundNEDstRectInApps(hwcNodes, aboveBounds);
    ASSERT_TRUE(aboveBounds.empty());
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha001
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    RectI rect;
    bool isHardwareEnableByBackgroundAlpha = false;
    rsUniHwcVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes, rect, isHardwareEnableByBackgroundAlpha);
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha002
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetNodeHasBackgroundColorAlpha(true);
    surfaceNode->SetHardwareForcedDisabledState(true);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    RectI rect;
    bool isHardwareEnableByBackgroundAlpha = false;
    rsUniHwcVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes, rect, isHardwareEnableByBackgroundAlpha);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha003
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha003, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);

    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetNodeHasBackgroundColorAlpha(true);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode1));
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode2));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    RectI rect;
    bool isHardwareEnableByBackgroundAlpha = false;
    rsUniHwcVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes, rect, isHardwareEnableByBackgroundAlpha);
    ASSERT_FALSE(surfaceNode1->IsHardwareForcedDisabled());
    ASSERT_FALSE(surfaceNode2->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha004
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha004, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetNodeHasBackgroundColorAlpha(true);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    RectI rect;
    bool isHardwareEnableByBackgroundAlpha = false;
    rsUniHwcVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes, rect, isHardwareEnableByBackgroundAlpha);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha005
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha005, TestSize.Level1)
{
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    std::weak_ptr<RSSurfaceRenderNode> hwcNode;
    hwcNodes.push_back(hwcNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    RectI rect;
    bool isHardwareEnableByBackgroundAlpha = false;
    rsUniHwcVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes, rect, isHardwareEnableByBackgroundAlpha);
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha006
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha006, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetNodeHasBackgroundColorAlpha(true);
    ASSERT_NE(surfaceNode2->GetRenderProperties().GetBoundsGeometry(), nullptr);
    RectI absRect1 = RectI{0, 0, 200, 200};
    surfaceNode2->GetRenderProperties().GetBoundsGeometry()->absRect_ = absRect1;
    auto surfaceNode3 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode3, nullptr);
    ASSERT_NE(surfaceNode3->GetRenderProperties().GetBoundsGeometry(), nullptr);
    RectI absRect2 = RectI{100, 100, 50, 50};
    surfaceNode3->GetRenderProperties().GetBoundsGeometry()->absRect_ = absRect2;

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes1;
    hwcNodes1.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode1));
    hwcNodes1.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode2));
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes2;
    hwcNodes2.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode3));

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    RectI rect;
    bool isHardwareEnableByBackgroundAlpha = false;
    rsUniHwcVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes1, rect, isHardwareEnableByBackgroundAlpha);
    rsUniHwcVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes2, rect, isHardwareEnableByBackgroundAlpha);
    ASSERT_FALSE(surfaceNode1->IsHardwareForcedDisabled());
    ASSERT_FALSE(surfaceNode2->IsHardwareForcedDisabled());
    ASSERT_TRUE(surfaceNode3->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateTransparentHwcNodeEnable001
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateTransparentHwcNodeEnable
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateTransparentHwcNodeEnable001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetDstRect({0, 0, 100, 100});

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());

    surfaceNode->SetNodeHasBackgroundColorAlpha(true);
    surfaceNode->SetHardwareEnableHint(true);
    rsUniHwcVisitor->UpdateTransparentHwcNodeEnable(hwcNodes);
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateTransparentHwcNodeEnable002
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateTransparentHwcNodeEnable
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateTransparentHwcNodeEnable002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceConfig.id = 2;
    auto opacitySurfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(opacitySurfaceNode, nullptr);
    auto opacitySurfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(opacitySurfaceNode1, nullptr);
    // set transparent
    surfaceNode->SetNodeHasBackgroundColorAlpha(true);
    surfaceNode->SetHardwareEnableHint(true);
    surfaceNode->SetDstRect({0, 0, 100, 100});
    opacitySurfaceNode->SetDstRect({100, 0, 100, 100});
    opacitySurfaceNode->SetHardwareForcedDisabledState(true);
    opacitySurfaceNode1->SetDstRect({100, 0, 100, 100});
    opacitySurfaceNode1->SetHardwareForcedDisabledState(false);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(opacitySurfaceNode));
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    // A transparent HWC node is NOT intersected with another opacity disabled-HWC node below it.
    rsUniHwcVisitor->UpdateTransparentHwcNodeEnable(hwcNodes);
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());

    // A transparent HWC node is intersected with another opacity disabled-HWC node below it.
    opacitySurfaceNode->SetDstRect({50, 0, 100, 100});
    rsUniHwcVisitor->UpdateTransparentHwcNodeEnable(hwcNodes);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes1;
    hwcNodes1.push_back(std::weak_ptr<RSSurfaceRenderNode>(opacitySurfaceNode1));
    hwcNodes1.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    rsUniHwcVisitor->UpdateTransparentHwcNodeEnable(hwcNodes1);
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateTransparentHwcNodeEnable003
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateTransparentHwcNodeEnable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateTransparentHwcNodeEnable003, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetNodeHasBackgroundColorAlpha(false);
    surfaceNode->SetHardwareEnableHint(false);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    rsUniHwcVisitor->UpdateTransparentHwcNodeEnable(hwcNodes);

    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    rsUniHwcVisitor->UpdateTransparentHwcNodeEnable(hwcNodes);
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateChildHwcNodeEnabledByHwcNodeBelow
 * @tc.desc: Test RSUniRenderVistorTest.UpdateChildHwcNodeEnableByHwcNodeBelow
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateChildHwcNodeEnableByHwcNodeBelow, TestSize.Level2)
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
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    rsUniHwcVisitor->UpdateChildHwcNodeEnableByHwcNodeBelow(hwcRects, appNode);
}

/**
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByFilterRect with intersect rect
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByFilterRect001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

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
    surfaceNode2->renderProperties_.boundsGeo_->absRect_ = rect;
    surfaceNode1->AddChildHardwareEnabledNode(surfaceNode2);

    constexpr NodeId id = 1;
    auto filterNode = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(rect);

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 0);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByFilterRect with empty rect
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByFilterRect002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);

    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    RectI rect{left, top, width, height};

    constexpr NodeId id = 1;
    auto filterNode = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(rect);

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 0);
}

/**
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByFilterRect with no hwcNode
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByFilterRect003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);

    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};

    constexpr NodeId id = 1;
    auto filterNode = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(rect);

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode, *filterNode, 0);
}

/**
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByFilterRect with no hwcNode
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByFilterRect004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr;
    ASSERT_EQ(surfaceNode, nullptr);

    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};

    constexpr NodeId id = 1;
    auto filterNode = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(rect);

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode, *filterNode, 0);
}

/**
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByFilterRect with no hwcNode
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByFilterRect005, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr;
    ASSERT_EQ(surfaceNode, nullptr);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(node, nullptr);
    RSMainThread::Instance()->AddSelfDrawingNodes(node);

    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};

    constexpr NodeId id = 1;
    auto filterNode = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(rect);

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode, *filterNode, 0);
}

/**
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByFilterRect check white list
 * @tc.type: FUNC
 * @tc.require: issuesICKNNB
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByFilterRect006, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    const std::string collaborationBundleName = "com.example.devicecollaboration";
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.bundleName = collaborationBundleName;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    surfaceConfig.id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    RectI rect{0, 0, 300, 300};
    surfaceNode2->SetDstRect(rect);
    surfaceNode2->renderProperties_.boundsGeo_->absRect_ = rect;
    surfaceNode1->AddChildHardwareEnabledNode(surfaceNode2);
    constexpr NodeId id = 3;
    auto filterNode = std::make_shared<RSRenderNode>(id);
    filterNode->SetOldDirtyInSurface(rect);

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 0);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    surfaceNode2->instanceRootNodeId_ = 1;
    filterNode->instanceRootNodeId_ = 2;
    surfaceNode2->GetHwcRecorder().SetZOrderForHwcEnableByFilter(0);
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 1);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    surfaceNode2->GetHwcRecorder().SetZOrderForHwcEnableByFilter(2);
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 1);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    filterNode->instanceRootNodeId_ = 1;
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 1);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    filterNode->GetHwcRecorder().SetBlendWithBackground(true);
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 1);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode1;
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 1);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    RsCommonHook::Instance().SetFilterUnderHwcConfigByApp(collaborationBundleName, "1");
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 1);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByFilterRect IsBackgroundFilterUnderSurface
 * @tc.type: FUNC
 * @tc.require: issuesICKNNB
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByFilterRect007, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    const std::string collaborationBundleName = "com.example.devicecollaboration";
    RsCommonHook::Instance().SetFilterUnderHwcConfigByApp(collaborationBundleName, "1");
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.bundleName = collaborationBundleName;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode1;
    surfaceConfig.id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode2, nullptr);
    RectI rect{0, 0, 300, 300};
    surfaceNode2->SetDstRect(rect);
    surfaceNode2->renderProperties_.boundsGeo_->absRect_ = rect;
    surfaceNode1->AddChildHardwareEnabledNode(surfaceNode2);
    surfaceNode2->instanceRootNodeId_ = 1;
    surfaceNode2->GetHwcRecorder().SetZOrderForHwcEnableByFilter(101);
    constexpr NodeId id = 3;
    auto filterNode = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(rect);
    filterNode->instanceRootNodeId_ = 1;
    filterNode->GetHwcRecorder().SetZOrderForHwcEnableByFilter(100);
    filterNode->GetHwcRecorder().SetBlendWithBackground(true);
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 100);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(filterNode);
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 100);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    filterNode->parent_ = surfaceNode2;
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 100);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    surfaceNode2->parent_ = surfaceNode1;
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 100);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    surfaceNode2->parent_ = filterNode;
    filterNode->parent_ = surfaceNode1;
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 100);
    ASSERT_FALSE(surfaceNode2->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByFilterRect IsBackgroundFilterUnderSurface
 * @tc.type: FUNC
 * @tc.require: issuesICKNNB
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByFilterRect008, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    const std::string collaborationBundleName = "com.example.devicecollaboration";
    RsCommonHook::Instance().SetFilterUnderHwcConfigByApp(collaborationBundleName, "1");
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.bundleName = collaborationBundleName;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode1;
    surfaceConfig.id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode2, nullptr);
    RectI rect{0, 0, 300, 300};
    surfaceNode2->SetDstRect(rect);
    surfaceNode2->renderProperties_.boundsGeo_->absRect_ = rect;
    surfaceNode1->AddChildHardwareEnabledNode(surfaceNode2);
    surfaceNode2->instanceRootNodeId_ = 1;
    surfaceNode2->GetHwcRecorder().SetZOrderForHwcEnableByFilter(101);
    constexpr NodeId id = 4;
    auto filterNode = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(rect);
    filterNode->instanceRootNodeId_ = 1;
    filterNode->GetHwcRecorder().SetZOrderForHwcEnableByFilter(100);
    filterNode->GetHwcRecorder().SetBlendWithBackground(true);

    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(filterNode);
    surfaceNode2->parent_ = surfaceNode1;
    filterNode->parent_ = surfaceNode1;

    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 100);
    ASSERT_FALSE(surfaceNode2->IsHardwareForcedDisabled());

    surfaceNode2->GetHwcRecorder().SetZOrderForHwcEnableByFilter(99);
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 100);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());

    surfaceNode1->GetCurFrameInfoDetail().curFrameReverseChildren = true;
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 100);
    ASSERT_FALSE(surfaceNode2->IsHardwareForcedDisabled());

    surfaceNode2->GetHwcRecorder().SetZOrderForHwcEnableByFilter(101);
    surfaceNode2->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByFilterRect(surfaceNode1, *filterNode, 100);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalCleanFilter_001
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalCleanFilter when Intersect return false.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByGlobalCleanFilter_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
  
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::pair<NodeId, RectI>> cleanFilter;
    auto& properties = surfaceNode->GetMutableRenderProperties();
    auto offset = std::nullopt;
    Drawing::Matrix matrix;
    properties.UpdateGeometryByParent(&matrix, offset);
    cleanFilter.emplace_back(NodeId(0), RectI(50, 50, 100, 100));
    ASSERT_TRUE(surfaceNode->GetRenderProperties().GetBoundsGeometry()->GetAbsRect()
        .IntersectRect(cleanFilter[0].second).IsEmpty());

    rsUniHwcVisitor->UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter, *surfaceNode);
    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalCleanFilter_002
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalCleanFilter when rendernode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByGlobalCleanFilter_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::pair<NodeId, RectI>> cleanFilter;
    auto& properties = surfaceNode->GetMutableRenderProperties();
    auto offset = std::nullopt;
    Drawing::Matrix matrix;
    matrix.SetScale(100, 100);
    properties.UpdateGeometryByParent(&matrix, offset);
    cleanFilter.emplace_back(NodeId(0), RectI(50, 50, 100, 100));
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    constexpr NodeId id = 0;
    pid_t pid = ExtractPid(id);
    nodeMap.renderNodeMap_[pid][id] = nullptr;

    rsUniHwcVisitor->UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter, *surfaceNode);
    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalCleanFilter_003
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalCleanFilter when rendernode is not null and AIBarFilterCache is not valid.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByGlobalCleanFilter_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::pair<NodeId, RectI>> cleanFilter;
    auto& properties = surfaceNode->GetMutableRenderProperties();
    auto offset = std::nullopt;
    Drawing::Matrix matrix;
    matrix.SetScale(100, 100);
    properties.UpdateGeometryByParent(&matrix, offset);
    cleanFilter.emplace_back(NodeId(1), RectI(50, 50, 100, 100));
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    constexpr NodeId id = 1;
    pid_t pid = ExtractPid(id);
    auto node = std::make_shared<RSRenderNode>(id);
    nodeMap.renderNodeMap_[pid][id] = node;
    ASSERT_NE(node, nullptr);
    ASSERT_FALSE(node->CheckAndUpdateAIBarCacheStatus(false));

    rsUniHwcVisitor->UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter, *surfaceNode);
    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalCleanFilter_004
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalCleanFilter when rendernode is not null and Intersect with node.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByGlobalCleanFilter_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(rsUniRenderVisitor->hwcVisitor_, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::pair<NodeId, RectI>> cleanFilter;
    auto& properties = surfaceNode->GetMutableRenderProperties();
    auto offset = std::nullopt;
    Drawing::Matrix matrix;
    matrix.SetScale(100, 100);
    properties.UpdateGeometryByParent(&matrix, offset);
    constexpr NodeId id = 1;
    cleanFilter.emplace_back(NodeId(id), RectI(50, 50, 100, 100));
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    pid_t pid = ExtractPid(id);
    auto node = std::make_shared<RSRenderNode>(id);
    nodeMap.renderNodeMap_[pid][id] = node;

    ASSERT_NE(surfaceNode->renderProperties_.boundsGeo_, nullptr);
    surfaceNode->renderProperties_.boundsGeo_->absRect_ = RectI(60, 60, 80, 80);

    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter, *surfaceNode);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalDirtyFilter_001
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalDirtyFilter when Intersect return false.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByGlobalDirtyFilter_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::pair<NodeId, RectI>> dirtyFilter;
    auto& properties = surfaceNode->GetMutableRenderProperties();
    auto offset = std::nullopt;
    Drawing::Matrix matrix;
    properties.UpdateGeometryByParent(&matrix, offset);
    dirtyFilter.emplace_back(NodeId(0), RectI(50, 50, 100, 100));
    auto geo = surfaceNode->GetRenderProperties().GetBoundsGeometry();
    ASSERT_TRUE(geo->GetAbsRect().IntersectRect(dirtyFilter[0].second).IsEmpty());

    rsUniHwcVisitor->UpdateHwcNodeEnableByGlobalDirtyFilter(dirtyFilter, *surfaceNode);
    EXPECT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalDirtyFilter_002
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalDirtyFilter when rendernode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIAJY2P
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByGlobalDirtyFilter_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::pair<NodeId, RectI>> dirtyFilter;
    auto& properties = surfaceNode->GetMutableRenderProperties();
    auto offset = std::nullopt;
    Drawing::Matrix matrix;
    matrix.SetScale(100, 100);
    properties.UpdateGeometryByParent(&matrix, offset);
    dirtyFilter.emplace_back(NodeId(0), RectI(50, 50, 100, 100));
    auto geo = surfaceNode->GetRenderProperties().GetBoundsGeometry();
    ASSERT_FALSE(geo->GetAbsRect().IntersectRect(dirtyFilter[0].second).IsEmpty());

    rsUniHwcVisitor->UpdateHwcNodeEnableByGlobalDirtyFilter(dirtyFilter, *surfaceNode);
    EXPECT_TRUE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHwcNodeEnableByGlobalFilter with not nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIAE6YM
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByGlobalFilter, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    NodeId id = 0;
    std::weak_ptr<RSContext> context;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    NodeId screenNodeId = 3;
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, 0);
    rsUniRenderVisitor->curScreenNode_->InitRenderParams();

    rsUniHwcVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter nullptr / eqeual nodeid / hwcNodes empty.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByGlobalFilter001, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node.
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(std::make_shared<RSSurfaceRenderNode>(node->GetId()));

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniHwcVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter, child node force disabled hardware.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByGlobalFilter002, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node and surface node.
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    NodeId surfaceId = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceId);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(++surfaceId);
    childNode->isHardwareForcedDisabled_ = true;
    surfaceNode->AddChildHardwareEnabledNode(childNode);

    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniHwcVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
    ASSERT_TRUE(childNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter, dirty filter found.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByGlobalFilter003, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node and surface node.
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    NodeId surfaceId = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceId);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(++surfaceId);
    childNode->isHardwareForcedDisabled_ = false;
    childNode->dstRect_ = DEFAULT_RECT;
    surfaceNode->AddChildHardwareEnabledNode(childNode);

    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniHwcVisitor->transparentHwcDirtyFilter_[node->GetId()].push_back(std::pair(node->GetId(), DEFAULT_RECT));
    rsUniHwcVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
    ASSERT_FALSE(childNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdatePrepareClip_001
 * @tc.desc: Test UpdatePrepareClip001, clipToBounds_ & clipToframe_ = true;
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniHwcVisitorTest, UpdatePrepareClip001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->GetMutableRenderProperties().clipToBounds_ = true;
    node->GetMutableRenderProperties().clipToFrame_ = true;
    rsUniHwcVisitor->UpdatePrepareClip(*node);
}

/**
 * @tc.name: UpdatePrepareClip_002
 * @tc.desc: Test UpdatePrepareClip002, clipToBounds_ & clipToframe_ = false;
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniHwcVisitorTest, UpdatePrepareClip002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->GetMutableRenderProperties().clipToBounds_ = false;
    node->GetMutableRenderProperties().clipToFrame_ = false;
    rsUniHwcVisitor->UpdatePrepareClip(*node);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_001
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           curSurfaceNode_ is null or hwcNodes is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeRectInSkippedSubTree_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);

    {
        rsUniHwcVisitor->uniRenderVisitor_.curSurfaceNode_ = nullptr;
        rsUniHwcVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        ASSERT_NE(surfaceNode, nullptr);
        surfaceNode->ResetChildHardwareEnabledNodes();
        ASSERT_EQ(surfaceNode->GetChildHardwareEnabledNodes().size(), 0);
        rsUniHwcVisitor->uniRenderVisitor_.curSurfaceNode_ = surfaceNode;
        rsUniHwcVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_002
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           hwcNodePtr is nullptr or hwcNodePtr is not on the tree or GetCalcRectInPrepare is true.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeRectInSkippedSubTree_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);

    {
        std::shared_ptr<RSSurfaceRenderNode> childNode = nullptr;
        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniHwcVisitor->uniRenderVisitor_.curSurfaceNode_ = surfaceNode;
        rsUniHwcVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        NodeId childNodeId = 2;
        auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
        ASSERT_NE(childNode, nullptr);
        childNode->SetIsOnTheTree(false);
        ASSERT_FALSE(childNode->IsOnTheTree());
        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniHwcVisitor->uniRenderVisitor_.curSurfaceNode_ = surfaceNode;
        rsUniHwcVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
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
        rsUniHwcVisitor->uniRenderVisitor_.curSurfaceNode_ = surfaceNode;
        rsUniHwcVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_003
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false and parent is null.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeRectInSkippedSubTree_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
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
    rsUniHwcVisitor->uniRenderVisitor_.curSurfaceNode_ = surfaceNode;
    rsUniHwcVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_004
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           parent is not null and findInRoot is false.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeRectInSkippedSubTree_004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
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
    rsUniHwcVisitor->uniRenderVisitor_.curSurfaceNode_ = surfaceNode;
    rsUniHwcVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_005
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           parent is not null and findInRoot is true.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeRectInSkippedSubTree_005, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
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
    rsUniHwcVisitor->uniRenderVisitor_.curSurfaceNode_ = surfaceNode;
    NodeId screenNodeId = 3;
    auto rsContext = std::make_shared<RSContext>();
    rsUniHwcVisitor->uniRenderVisitor_.curScreenNode_ =
            std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    rsUniHwcVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_006
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false and
 *           parent's parent is not null.
 * @tc.type: FUNC
 * @tc.require: issueIAKJFE
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeRectInSkippedSubTree_006, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
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

    EXPECT_NE(parent->GetType(), RSRenderNodeType::SCREEN_NODE);
    auto grandparent = parent->GetParent().lock();
    ASSERT_NE(grandparent, nullptr);
    ASSERT_EQ(grandparent->GetId(), grandparentNodeId);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniHwcVisitor->uniRenderVisitor_.curSurfaceNode_ = surfaceNode;
    NodeId screenNodeId = 4;
    auto rsContext = std::make_shared<RSContext>();
    rsUniHwcVisitor->uniRenderVisitor_.curScreenNode_ =
            std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    rsUniHwcVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/**
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree_007
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHwcNodeRectInSkippedSubTree with not nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIAE6YM
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeRectInSkippedSubTree_007, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    int id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    rsUniHwcVisitor->UpdateHwcNodeRectInSkippedSubTree(*node);
}

/**
 * @tc.name: UpdateHwcNodeEnableByBackgroundAlpha001
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByBackgroundAlpha001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "surfaceNodeTest001";
    auto surfaceNode = RSTestUtil::CreateSurfaceNode(surfaceConfig);
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_NE(surfaceNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
}

/**
 * @tc.name: UpdateHwcNodeEnableByBackgroundAlpha002
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByBackgroundAlpha002, TestSize.Level1)
{
    constexpr uint8_t MAX_ALPHA = 255;
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "solidColorNodeTest002";
    auto surfaceNode = RSTestUtil::CreateSurfaceNode(surfaceConfig);
    surfaceNode->SetAncoForceDoDirect(false);
    auto& renderProperties = surfaceNode->GetMutableRenderProperties();
    Color solidColor(136, 136, 136, MAX_ALPHA);
    renderProperties.SetBackgroundColor(solidColor);
    renderProperties.SetAlpha(1);
    ASSERT_NE(surfaceNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(surfaceNode->GetStagingRenderParams().get());
    auto solidLayerColor = stagingSurfaceParams->GetSolidLayerColor();
    ASSERT_TRUE(solidLayerColor == RgbPalette::Transparent());
    surfaceNode->SetHardwareEnabled(true, SelfDrawingNodeType::XCOM);
    surfaceNode->UpdateRenderParams();
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    solidLayerColor = stagingSurfaceParams->GetSolidLayerColor();
    ASSERT_TRUE(solidLayerColor == RgbPalette::Transparent());
    renderProperties.SetBackgroundColor(RgbPalette::Black());
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    solidLayerColor = stagingSurfaceParams->GetSolidLayerColor();
    ASSERT_TRUE(solidLayerColor == RgbPalette::Transparent());
    Color alphaColor(136, 136, 136, 136);
    renderProperties.SetBackgroundColor(alphaColor);
    surfaceNode->SetHardwareEnabled(true, SelfDrawingNodeType::DEFAULT);
    surfaceNode->SetHardwareEnableHint(false);
    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(false);
    surfaceNode->UpdateRenderParams();
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    solidLayerColor = stagingSurfaceParams->GetSolidLayerColor();
    ASSERT_TRUE(solidLayerColor == RgbPalette::Transparent());
}

/**
 * @tc.name: UpdateHwcNodeEnableByBackgroundAlpha003
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByBackgroundAlpha003, TestSize.Level1)
{
    constexpr uint8_t MAX_ALPHA = 255;
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "solidColorNodeTest003";
    auto surfaceNode = RSTestUtil::CreateSurfaceNode(surfaceConfig);
    surfaceNode->SetAncoForceDoDirect(false);
    auto& renderProperties = surfaceNode->GetMutableRenderProperties();
    Color solidColor(136, 136, 136, MAX_ALPHA);
    renderProperties.SetBackgroundColor(solidColor);
    renderProperties.SetAlpha(1);
    ASSERT_NE(surfaceNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    surfaceNode->SetHardwareEnabled(true, SelfDrawingNodeType::XCOM);
    surfaceNode->SetHardwareEnableHint(false);
    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(false);
    surfaceNode->UpdateRenderParams();
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(surfaceNode->GetStagingRenderParams().get());
    auto solidLayerColor = stagingSurfaceParams->GetSolidLayerColor();
    ASSERT_TRUE(solidLayerColor == RgbPalette::Transparent());
    RsCommonHook::Instance().SetIsWhiteListForSolidColorLayerFlag(true);
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    solidLayerColor = stagingSurfaceParams->GetSolidLayerColor();
    ASSERT_TRUE(solidLayerColor == solidColor);
}

/**
 * @tc.name: UpdateHwcNodeEnableByBackgroundAlpha004
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByBackgroundAlpha004, TestSize.Level1)
{
    constexpr uint8_t MAX_ALPHA = 255;
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};
    uint32_t mWidth = 200;
    uint32_t mHeight = 200;
    RectI mRect{left, top, mWidth, mHeight};
    NodeId id = 0;

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = ++id;
    surfaceConfig.name = "alphaColorNodeTest004";
    auto surfaceNode = RSTestUtil::CreateSurfaceNode(surfaceConfig);
    surfaceNode->SetAncoForceDoDirect(false);
    auto& renderProperties = surfaceNode->GetMutableRenderProperties();
    Color alphaColor(136, 136, 136, 0);
    renderProperties.SetBackgroundColor(alphaColor);
    renderProperties.SetAlpha(1);
    surfaceNode->renderProperties_.boundsGeo_->absRect_ = rect;
    ASSERT_NE(surfaceNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    surfaceNode->SetHardwareEnabled(true, SelfDrawingNodeType::XCOM);
    surfaceNode->SetHardwareEnableHint(false);
    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(false);
    surfaceNode->UpdateRenderParams();
    RsCommonHook::Instance().SetIsWhiteListForSolidColorLayerFlag(true);
    auto rootNode = std::make_shared<RSRootRenderNode>(++id);
    rootNode->InitRenderParams();
    ASSERT_NE(rootNode, nullptr);

    auto rsCanvasRenderNode0 = std::make_shared<RSCanvasRenderNode>(++id);
    rsCanvasRenderNode0->InitRenderParams();
    ASSERT_NE(rsCanvasRenderNode0, nullptr);
    auto rsCanvasRenderNode1 = std::make_shared<RSCanvasRenderNode>(++id);
    rsCanvasRenderNode1->InitRenderParams();
    auto &canvasProperties = rsCanvasRenderNode1->GetMutableRenderProperties();
    Color solidColor(102, 102, 102, MAX_ALPHA);
    canvasProperties.SetBackgroundColor(solidColor);
    canvasProperties.SetAlpha(1);
    rsCanvasRenderNode1->renderProperties_.boundsGeo_->absRect_ = rect;
    ASSERT_NE(rsCanvasRenderNode1, nullptr);
    rsUniRenderVisitor->curSurfaceNode_->AddChild(rootNode, -1);
    rsUniRenderVisitor->curSurfaceNode_->GenerateFullChildrenList();
    rootNode->AddChild(rsCanvasRenderNode0, -1);
    rootNode->GenerateFullChildrenList();
    rsCanvasRenderNode0->AddChild(rsCanvasRenderNode1, -1);
    rsCanvasRenderNode0->GenerateFullChildrenList();
    rsCanvasRenderNode1->AddChild(surfaceNode, -1);
    rsCanvasRenderNode1->GenerateFullChildrenList();
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);

    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(surfaceNode->GetStagingRenderParams().get());
    auto solidLayerColor = stagingSurfaceParams->GetSolidLayerColor();
    ASSERT_TRUE(solidLayerColor == solidColor);
    RSSurfaceRenderNodeConfig childSurfaceConfig;
    childSurfaceConfig.id = ++id;
    childSurfaceConfig.name = "childSurfaceTest004";
    auto childSurface = RSTestUtil::CreateSurfaceNode(childSurfaceConfig);
    childSurface->renderProperties_.boundsGeo_->absRect_ = rect;
    ASSERT_NE(childSurface, nullptr);
    rsCanvasRenderNode1->ClearChildren();
    rsCanvasRenderNode1->AddChild(childSurface, 0);
    rsCanvasRenderNode1->AddChild(surfaceNode, 1);
    rsCanvasRenderNode1->GenerateFullChildrenList();
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());
    rsCanvasRenderNode1->ClearChildren();
    rsCanvasRenderNode1->AddChild(surfaceNode, -1);
    rsCanvasRenderNode1->GenerateFullChildrenList();
    rsCanvasRenderNode1->renderProperties_.boundsGeo_->absRect_ = mRect;
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());
    rsCanvasRenderNode1->ClearChildren();
    rsCanvasRenderNode1->AddChild(surfaceNode, -1);
    rsCanvasRenderNode1->GenerateFullChildrenList();
    canvasProperties.SetBackgroundColor(alphaColor);
    canvasProperties.SetAlpha(1);
    rsCanvasRenderNode1->renderProperties_.boundsGeo_->absRect_ = rect;
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHwcNodeEnableByBackgroundAlpha005
 * @tc.desc: Test RSUniHwcVisitorTest.UpdateHwcNodeEnableByBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeEnableByBackgroundAlpha005, TestSize.Level1)
{
    constexpr uint8_t MAX_ALPHA = 255;
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "solidColorNodeTest005";
    auto surfaceNode = RSTestUtil::CreateSurfaceNode(surfaceConfig);
    surfaceNode->SetAncoForceDoDirect(false);
    auto& renderProperties = surfaceNode->GetMutableRenderProperties();
    Color solidColor(136, 136, 136, MAX_ALPHA);
    renderProperties.SetBackgroundColor(solidColor);
    renderProperties.SetAlpha(1);
    ASSERT_NE(surfaceNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext->weak_from_this());
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    surfaceNode->SetHardwareEnabled(true, SelfDrawingNodeType::XCOM);
    surfaceNode->SetHardwareEnableHint(false);
    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(false);
    surfaceNode->UpdateRenderParams();
    RsCommonHook::Instance().SetIsWhiteListForSolidColorLayerFlag(false);
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(surfaceNode->GetStagingRenderParams().get());
    auto solidLayerColor = stagingSurfaceParams->GetSolidLayerColor();
    ASSERT_TRUE(solidLayerColor == RgbPalette::Transparent());
    RsCommonHook::Instance().SetIsWhiteListForSolidColorLayerFlag(true);
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    solidLayerColor = stagingSurfaceParams->GetSolidLayerColor();
    ASSERT_TRUE(solidLayerColor == solidColor);
    rsUniRenderVisitor->curScreenNode_->SetHasUniRenderHdrSurface(true);
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());
    rsUniRenderVisitor->curScreenNode_->SetHasUniRenderHdrSurface(false);
    renderProperties.SetBgBrightnessFract(0.5f);
    rsUniHwcVisitor->UpdateHwcNodeEnableByBackgroundAlpha(*surfaceNode);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode_001
 * @tc.desc: Test UpdateHwcNodeInfo with multi-params
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeInfo_001, TestSize.Level2)
{
    Drawing::Matrix absMatrix;
    RectI absRect;
    RectI rect = {0, 80, 1000, 1000};

    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->HwcSurfaceRecorder().SetIntersectWithPreviousFilter(true);
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;

    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->needCollectHwcNode_ = true;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    rsSurfaceRenderNode->isFixRotationByUser_ = false;
    rsUniRenderVisitor->hwcVisitor_->isHardwareForcedDisabled_ = true;
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeInfo(*rsSurfaceRenderNode, absMatrix, absRect);

    rsUniRenderVisitor->hwcVisitor_->isHardwareForcedDisabled_ = false;
    rsSurfaceRenderNode->dynamicHardwareEnable_ = false;
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeInfo(*rsSurfaceRenderNode, absMatrix, absRect);

    rsSurfaceRenderNode->dynamicHardwareEnable_ = true;
    surfaceNode->visibleRegion_.Reset();
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeInfo(*rsSurfaceRenderNode, absMatrix, absRect);

    surfaceNode->visibleRegion_.rects_.push_back(rect);
    surfaceNode->visibleRegion_.bound_ = rect;
    rsSurfaceRenderNode->isFixRotationByUser_ = true;
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeInfo(*rsSurfaceRenderNode, absMatrix, absRect);

    rsSurfaceRenderNode->isFixRotationByUser_ = false;
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeInfo(*rsSurfaceRenderNode, absMatrix, absRect, true);
}

/**
 * @tc.name: GetHwcVisibleEffectDirty001
 * @tc.desc: Test GetHwcVisibleEffectDirty Function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, GetHwcVisibleEffectDirty001, TestSize.Level2)
{
    NodeId parentNodeId = 1;
    RSRenderNode surfaceNode(parentNodeId);
    ASSERT_NE(surfaceNode.GetRenderProperties().GetBoundsGeometry(), nullptr);
    RectI absRect = RectI{0, 0, 200, 200};
    surfaceNode.GetRenderProperties().GetBoundsGeometry()->absRect_ = absRect;
    RectI globalRect = RectI{0, 0, 250, 250};

    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(12, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    auto result = rsUniHwcVisitor->GetHwcVisibleEffectDirty(surfaceNode, globalRect);
    RectI expect = RectI{0, 0, 250, 250};
    EXPECT_EQ(result, expect);
}

/**
 * @tc.name: CheckNodeOcclusion001
 * @tc.desc: Test CheckNodeOcclusion Function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, CheckNodeOcclusion001, TestSize.Level2)
{
    Color bgColor = RgbPalette::Transparent();
    RectI absRect = RectI(0, 0, 50, 50);
    auto surfaceNode = nullptr;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);
    auto result = rsUniHwcVisitor->CheckNodeOcclusion(surfaceNode, absRect, bgColor);
    ASSERT_FALSE(result);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);

    auto result1 = rsUniHwcVisitor->CheckNodeOcclusion(surfaceNode1, absRect, bgColor);
    ASSERT_FALSE(result1);

    surfaceNode1->GetRenderProperties().GetBoundsGeometry()->absRect_ = absRect;
    auto result2 = rsUniHwcVisitor->CheckNodeOcclusion(surfaceNode1, absRect, bgColor);
    ASSERT_TRUE(result2);

    surfaceNode1->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectI(60, 60, 100, 100);
    auto result3 = rsUniHwcVisitor->CheckNodeOcclusion(surfaceNode1, absRect, bgColor);
    ASSERT_FALSE(result3);

    auto result4 = rsUniHwcVisitor->CheckNodeOcclusion(surfaceNode1, RectI(), bgColor);
    ASSERT_FALSE(result4);

    surfaceNode1->GetRenderProperties().GetBoundsGeometry()->absRect_ = absRect;
    surfaceNode1->AddDirtyType(RSModifierType::CHILDREN);
    surfaceNode1->AddDirtyType(RSModifierType::BG_IMAGE);
    surfaceNode1->AddDirtyType(RSModifierType::HDR_BRIGHTNESS);
    surfaceNode1->UpdateDrawableVecV2();
    auto result5 = rsUniHwcVisitor->CheckNodeOcclusion(surfaceNode1, absRect, bgColor);
    ASSERT_TRUE(result5);
}

/**
 * @tc.name: FindAppBackgroundColor001
 * @tc.desc: Test FindAppBackgroundColor Function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, FindAppBackgroundColor001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    NodeId id1 = 1;
    RSSurfaceRenderNode childNode(id1);
    NodeId id2 = 2;
    RSSurfaceRenderNode childNode1(id2);
    RectI absRect = RectI{0, 0, 200, 200};
    childNode1.GetRenderProperties().GetBoundsGeometry()->absRect_ = absRect;

    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto rootNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rootNode, nullptr);

    rsUniHwcVisitor->FindAppBackgroundColor(childNode);
    rsUniHwcVisitor->FindAppBackgroundColor(childNode1);
}

/**
 * @tc.name: CheckSubTreeOcclusion
 * @tc.desc: Test CheckSubTreeOcclusion Function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, CheckSubTreeOcclusion, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    NodeId branchNodeId = 1;
    auto branchNode = std::make_shared<RSRenderNode>(branchNodeId);
    ASSERT_NE(branchNode, nullptr);
    RectI surfaceNodeAbsRect = RectI{0, 0, 100, 100};
    std::stack<Color> validBgColors;
    bool result1 = rsUniHwcVisitor->CheckSubTreeOcclusion(nullptr, surfaceNodeAbsRect, validBgColors);
    EXPECT_FALSE(result1);
    bool result2 = rsUniHwcVisitor->CheckSubTreeOcclusion(branchNode, surfaceNodeAbsRect, validBgColors);
    EXPECT_FALSE(result2);
}

/**
 * @tc.name: UpdateTopSurfaceSrcRect
 * @tc.desc: Test UpdateTopSurfaceSrcRect Function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateTopSurfaceSrcRect, TestSize.Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node1(id);
    node1.GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    node1.GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    node1.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node1.renderProperties_.SetBoundsWidth(2440);
    node1.renderProperties_.SetBoundsHeight(1080);
    node1.renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    node1.SetDstRect({0, 1000, 2440, 1080});
    node1.isFixRotationByUser_ = false;
    Drawing::Matrix absMatrix;
    absMatrix.SetMatrix(1, 0, 0, 0, 0, 0, 0, 0, 1);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(12, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    RectI absRect = RectI{0, 0, 1440, 1080};
    rsUniHwcVisitor->UpdateTopSurfaceSrcRect(node1, absMatrix, absRect);
    RectI expectedSrcRect = RectI{0, 0, 1440, 1080};
    EXPECT_FALSE(node1.GetSrcRect() == expectedSrcRect);
}

/**
 * @tc.name: UpdateIsOffscreen
 * @tc.desc: Test UpdateIsOffscreen Function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateIsOffscreen, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    NodeId id = 0;
    RSCanvasRenderNode node(id);
    bool result1 = rsUniHwcVisitor->UpdateIsOffscreen(node);
    EXPECT_FALSE(result1);

    rsUniHwcVisitor->RestoreIsOffscreen(true);
    bool result2 = rsUniHwcVisitor->UpdateIsOffscreen(node);
    EXPECT_TRUE(result2);
}

/**
 * @tc.name: IsDisableHwcOnExpandScreen
 * @tc.desc: Test IsDisableHwcOnExpandScreen Function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, IsDisableHwcOnExpandScreen, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniHwcVisitor = std::make_shared<RSUniHwcVisitor>(*rsUniRenderVisitor);
    ASSERT_NE(rsUniHwcVisitor, nullptr);

    bool result1 = rsUniHwcVisitor->IsDisableHwcOnExpandScreen();
    EXPECT_FALSE(result1);

    NodeId displayNodeId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(displayNodeId, 0, rsContext->weak_from_this());
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    bool result2 = rsUniHwcVisitor->IsDisableHwcOnExpandScreen();
    EXPECT_FALSE(result2);
}

/**
 * @tc.name: IsFindRootSuccess_001
 * @tc.desc: Test IsFindRootSuccess Function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, IsFindRootSuccess_001, TestSize.Level2)
{
    NodeId id = 0;
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(id);
    leashWindowNode->InitRenderParams();
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    auto rootNode = std::make_shared<RSRootRenderNode>(++id);
    rootNode->InitRenderParams();
    auto canvasNode1 = std::make_shared<RSCanvasRenderNode>(++id);
    canvasNode1->InitRenderParams();
    auto canvasNode2 = std::make_shared<RSCanvasRenderNode>(++id);
    canvasNode2->InitRenderParams();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(++id);
    surfaceNode->InitRenderParams();
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    surfaceNode->GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    surfaceNode->isHardwareEnabledNode_ = true;
    leashWindowNode->childHardwareEnabledNodes_.emplace_back(surfaceNode);
    leashWindowNode->AddChild(rootNode);
    rootNode->AddChild(canvasNode1);
    canvasNode1->AddChild(canvasNode2);
    canvasNode2->AddChild(surfaceNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curSurfaceNode_ = leashWindowNode;
    rsUniRenderVisitor->prepareClipRect_ = RectI(0, 0, 1000, 1000);
    auto rsRenderNode = std::static_pointer_cast<RSRenderNode>(surfaceNode);
    auto result = rsUniRenderVisitor->hwcVisitor_->IsFindRootSuccess(rsRenderNode, *rootNode);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: UpdateCroseInfoForProtectedHwcNode001
 * @tc.desc: Test UpdateCroseInfoForProtectedHwcNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateCroseInfoForProtectedHwcNode001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(rsUniRenderVisitor->hwcVisitor_, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    ASSERT_NE(surfaceNode, nullptr);

    ASSERT_FALSE(surfaceNode->GetFirstLevelNode());
    ASSERT_FALSE(surfaceNode->IsHwcCrossNode());
    ASSERT_FALSE(surfaceNode->GetHwcGlobalPositionEnabled());
    ASSERT_FALSE(surfaceNode->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED));

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->SetHwcGlobalPositionEnabled(true);
    surfaceNode->SetHwcCrossNode(true);
    rsUniRenderVisitor->hwcVisitor_->UpdateCrossInfoForProtectedHwcNode(*surfaceNode);

    ASSERT_TRUE(surfaceNode->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED));
    ASSERT_FALSE(surfaceNode->GetHwcGlobalPositionEnabled());
    ASSERT_FALSE(surfaceNode->IsHwcCrossNode());
}

/**
 * @tc.name: UpdateHwcNodeClipRect_001
 * @tc.desc: Test UpdateHwcNodeClipRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeClipRect_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    node->GetMutableRenderProperties().clipToBounds_ = true;
    node->GetMutableRenderProperties().clipToFrame_ = true;
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    NodeId configId = 1;
    RSRenderNode rootNode(configId);
    Drawing::Rect clipRect;
    Drawing::Matrix matrix;
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeClipRect(node, clipRect);
    Drawing::Rect expectRect = {0, 0, 0, 0};
    EXPECT_TRUE(clipRect == expectRect);
}

/**
 * @tc.name: UpdateHwcNodeClipRect_002
 * @tc.desc: Test UpdateHwcNodeClipRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeClipRect_002, TestSize.Level2)
{
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    surfaceNode->InitRenderParams();
    surfaceNode->renderProperties_.frameGravity_ = Gravity::RESIZE;
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    surfaceNode->GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    surfaceNode->isHardwareEnabledNode_ = true;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->prepareClipRect_ = RectI(0, 0, 1000, 1000);
    Drawing::Rect clipRect;
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeClipRect(surfaceNode, clipRect);
    Drawing::Rect expectedClipRect = {0, 0, 0, 0};
    EXPECT_EQ(clipRect, expectedClipRect);
}

/**
 * @tc.name: UpdateHwcNodeClipRect_003
 * @tc.desc: Test UpdateHwcNodeClipRect
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeClipRect_003, Function | SmallTest | Level2)
{
    NodeId id = 0;
    auto canvasNode = std::make_shared<RSSurfaceRenderNode>(id);
    canvasNode->InitRenderParams();
    canvasNode->renderProperties_.boundsGeo_->SetRect(0, 10, 20, 30);
    canvasNode->renderProperties_.clipToBounds_ = true;
    canvasNode->selfDrawRect_ = {0, 10, 20, 30};
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->prepareClipRect_ = RectI(0, 0, 1000, 1000);
    Drawing::Rect clipRect(0, 0, 100, 100);
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeClipRect(canvasNode, clipRect);
    Drawing::Rect expectedClipRect = {0, 10, 20, 40};
    EXPECT_EQ(clipRect, expectedClipRect);
}

/**
 * @tc.name: UpdateHwcNodeMatrix_001
 * @tc.desc: Test UpdateHwcNodeMatrix Function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeMatrix_001, TestSize.Level2)
{
    NodeId nodeId = 1;
    auto surfaceNode = std::make_shared<RSRenderNode>(nodeId);
    surfaceNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    Drawing::Matrix matrix;
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeMatrix(surfaceNode, matrix);
    Drawing::Matrix expectedMatrix;
    EXPECT_EQ(matrix, expectedMatrix);
}

/**
 * @tc.name: UpdateHwcNodeClipRectAndMatrix_001
 * @tc.desc: Test UpdateHwcNodeClipRectAndMatrix
 * @tc.type: FUNC
 * @tc.require: issueIBJ6BZ
 */
HWTEST_F(RSUniHwcVisitorTest, UpdateHwcNodeClipRectAndMatrix_001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(id);
    leashWindowNode->InitRenderParams();
    auto rootNode = std::make_shared<RSRootRenderNode>(++id);
    rootNode->InitRenderParams();
    auto canvasNode1 = std::make_shared<RSCanvasRenderNode>(++id);
    canvasNode1->InitRenderParams();
    canvasNode1->renderProperties_.boundsGeo_->SetRect(0, 10, 20, 30);
    canvasNode1->renderProperties_.clipToBounds_ = true;
    canvasNode1->selfDrawRect_ = {0, 10, 20, 30};
    Drawing::Matrix canvasNodeMatrix;
    canvasNodeMatrix.SetMatrix(1.f, 0.f, 50.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
    canvasNode1->renderProperties_.boundsGeo_->ConcatMatrix(canvasNodeMatrix);
    auto canvasNode2 = std::make_shared<RSCanvasRenderNode>(++id);
    canvasNode2->InitRenderParams();
    canvasNode2->renderProperties_.boundsGeo_->SetRect(-10, 10, 20, 30);
    canvasNode2->renderProperties_.clipToBounds_ = true;
    canvasNode2->selfDrawRect_ = {0, 10, 20, 30};
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(++id);
    surfaceNode->InitRenderParams();
    Drawing::Matrix surfaceMatrix;
    surfaceMatrix.SetMatrix(1.f, 0.f, 0.f, 0.f, 1.f, 100.f, 0.f, 0.f, 1.f);
    surfaceNode->renderProperties_.boundsGeo_->ConcatMatrix(surfaceMatrix);
    surfaceNode->renderProperties_.boundsGeo_->SetRect(-10, 10, 20, 10);
    surfaceNode->renderProperties_.clipToBounds_ = true;
    surfaceNode->renderProperties_.clipToFrame_ = true;
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = SurfaceBuffer::Create();
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferWidth(1080);
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer->SetSurfaceBufferHeight(1653);
    surfaceNode->GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    surfaceNode->isHardwareEnabledNode_ = true;
    leashWindowNode->childHardwareEnabledNodes_.emplace_back(surfaceNode);
    leashWindowNode->AddChild(rootNode);
    rootNode->AddChild(canvasNode1);
    canvasNode1->AddChild(canvasNode2);
    canvasNode2->AddChild(surfaceNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curSurfaceNode_ = leashWindowNode;
    rsUniRenderVisitor->prepareClipRect_ = RectI(0, 0, 1000, 1000);
    RectI clipRect;
    Drawing::Matrix matrix;
    rsUniRenderVisitor->hwcVisitor_->UpdateHwcNodeClipRectAndMatrix(surfaceNode, *rootNode, clipRect, matrix);
    RectI expectedClipRect = {50, 10, 20, 30};
    Drawing::Matrix expectedMatrix;
    expectedMatrix.SetMatrix(1.f, 0.f, 50.f, 0.f, 1.f, 100.f, 0.f, 0.f, 1.f);
    EXPECT_EQ(clipRect, expectedClipRect);
    EXPECT_EQ(matrix, expectedMatrix);
}

} // namespace OHOS::Rosen