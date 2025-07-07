/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "params/rs_logical_display_render_params.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_draw_cmd.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "visitor/rs_node_visitor.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSLogicalDisplayRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline Drawing::Canvas canvas;
};

void RSLogicalDisplayRenderNodeTest::SetUpTestCase() {}
void RSLogicalDisplayRenderNodeTest::TearDownTestCase() {}
void RSLogicalDisplayRenderNodeTest::SetUp() {}
void RSLogicalDisplayRenderNodeTest::TearDown() {}

/**
 * @tc.name: QuickPrepareTest
 * @tc.desc: test results of QuickPrepare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, QuickPrepareTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    std::shared_ptr<RSNodeVisitor> visitor;
    EXPECT_EQ(visitor, nullptr);
    renderNode->QuickPrepare(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    renderNode->QuickPrepare(visitor);
}

/**
 * @tc.name: PrepareTest
 * @tc.desc: test results of Prepare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, PrepareTest, TestSize.Level0)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    std::shared_ptr<RSNodeVisitor> visitor;
    EXPECT_EQ(visitor, nullptr);
    renderNode->Prepare(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    renderNode->Prepare(visitor);
}

/**
 * @tc.name: ProcessTest
 * @tc.desc: test results of Process
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, ProcessTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    std::shared_ptr<RSNodeVisitor> visitor;
    EXPECT_EQ(visitor, nullptr);
    renderNode->Process(visitor);
    
    visitor = std::make_shared<RSRenderThreadVisitor>();
    renderNode->Process(visitor);
}

/**
 * @tc.name: UpdateRenderParamsTest
 * @tc.desc: test results of UpdateRenderParams
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, UpdateRenderParamsTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_NE(renderNode, nullptr);
    renderNode->stagingRenderParams_ = nullptr;
    renderNode->UpdateRenderParams();

    std::shared_ptr<RSLogicalDisplayRenderNode> tempNode;
    renderNode->mirrorSource_ = tempNode;
    renderNode->UpdateRenderParams();

    constexpr NodeId newNodeId = 2;
    auto rsDisplayRenderNode = std::make_shared<RSLogicalDisplayRenderNode>(newNodeId, config);
    renderNode->mirrorSource_ = rsDisplayRenderNode;
}

/**
 * @tc.name: SetWindowContainerTest
 * @tc.desc: test results of SetWindowContainer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, SetWindowContainerTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    renderNode->SetWindowContainer(nullptr);
    EXPECT_EQ(renderNode->windowContainer_, nullptr);

    NodeId rootId = 1;
    auto rootNode = std::make_shared<RSBaseRenderNode>(rootId);
    renderNode->SetWindowContainer(rootNode);
    EXPECT_NE(renderNode->windowContainer_, nullptr);

    NodeId newRootId = 2;
    auto newRootNode = std::make_shared<RSBaseRenderNode>(newRootId);
    renderNode->SetWindowContainer(newRootNode);
    EXPECT_NE(renderNode->windowContainer_, nullptr);

    renderNode->SetWindowContainer(nullptr);
}

/**
 * @tc.name: GetWindowContainerTest
 * @tc.desc: test results of GetWindowContainer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, GetWindowContainerTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_EQ(renderNode->GetWindowContainer(), nullptr);
}

/**
 * @tc.name: NotifyScreenNotSwitchingTest
 * @tc.desc: test results of NotifyScreenNotSwitching
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, NotifyScreenNotSwitchingTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_NE(renderNode, nullptr);
    renderNode->NotifyScreenNotSwitching();
    renderNode->SetScreenStatusNotifyTask([](bool status) {
    });
    renderNode->NotifyScreenNotSwitching();
}

/**
 * @tc.name: GetRotationTest
 * @tc.desc: test results of GetRotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, GetRotationTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    renderNode->renderProperties_.boundsGeo_ = nullptr;
    EXPECT_EQ(renderNode->GetRotation(), ScreenRotation::ROTATION_0);
    renderNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    EXPECT_EQ(renderNode->GetRotation(), ScreenRotation::ROTATION_0);
}

/**
 * @tc.name: IsRotationChangedTest
 * @tc.desc: test results of IsRotationChanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, IsRotationChangedTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    renderNode->renderProperties_.boundsGeo_ = nullptr;
    EXPECT_FALSE(renderNode->IsRotationChanged());
    renderNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    EXPECT_FALSE(renderNode->IsRotationChanged());
}

/**
 * @tc.name: UpdateRotationTest
 * @tc.desc: test results of UpdateRotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, UpdateRotationTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_NE(renderNode, nullptr);
    renderNode->stagingRenderParams_ = nullptr;
    renderNode->UpdateRotation();
    renderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(nodeId);
    renderNode->UpdateRotation();
    renderNode->renderProperties_.boundsGeo_ = nullptr;
    renderNode->UpdateRotation();
    renderNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    renderNode->UpdateRotation();
}

/**
 * @tc.name: IsLastRotationChangedTest
 * @tc.desc: test results of IsLastRotationChanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, IsLastRotationChangedTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_FALSE(renderNode->IsLastRotationChanged());
}

/**
 * @tc.name: GetPreRotationStatusTest
 * @tc.desc: test results of GetPreRotationStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, GetPreRotationStatusTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_FALSE(renderNode->GetPreRotationStatus());
}

/**
 * @tc.name: GetCurRotationStatusTest
 * @tc.desc: test results of GetCurRotationStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, GetCurRotationStatusTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_FALSE(renderNode->GetCurRotationStatus());
}

/**
 * @tc.name: UpdateOffscreenRenderParamsTest
 * @tc.desc: test results of UpdateOffscreenRenderParams
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, UpdateOffscreenRenderParamsTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    renderNode->UpdateOffscreenRenderParams(false);
    EXPECT_NE(renderNode, nullptr);
}

/**
 * @tc.name: SetMirrorSourceTest
 * @tc.desc: test results of SetMirrorSource
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, SetMirrorSourceTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    std::weak_ptr<RSContext> context = {};

    constexpr NodeId newNodeId = 2;
    auto rsDisplayRenderNode = std::make_shared<RSLogicalDisplayRenderNode>(newNodeId, config);
    renderNode->isMirrorDisplay_ = true;
    renderNode->SetMirrorSource(nullptr);
    renderNode->SetMirrorSource(rsDisplayRenderNode);

    renderNode->isMirrorDisplay_ = false;
    renderNode->SetMirrorSource(nullptr);
    renderNode->SetMirrorSource(rsDisplayRenderNode);
    EXPECT_NE(renderNode->mirrorSource_.lock(), nullptr);
}

/**
 * @tc.name: IsMirrorDisplayChangedTest
 * @tc.desc: test results of IsMirrorDisplayChanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, IsMirrorDisplayChangedTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_FALSE(renderNode->IsMirrorDisplayChanged());
}

/**
 * @tc.name: ResetMirrorDisplayChangedFlagTest
 * @tc.desc: test results of ResetMirrorDisplayChangedFlag
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, ResetMirrorDisplayChangedFlagTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    renderNode->ResetMirrorDisplayChangedFlag();
    EXPECT_FALSE(renderNode->isMirrorDisplayChanged_);
}

/**
 * @tc.name: SetVirtualScreenMuteStatusTest
 * @tc.desc: test results of SetVirtualScreenMuteStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, SetVirtualScreenMuteStatusTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    renderNode->SetVirtualScreenMuteStatus(true);
    EXPECT_TRUE(renderNode->virtualScreenMuteStatus_);
}

/**
 * @tc.name: GetVirtualScreenMuteStatusTest
 * @tc.desc: test results of GetVirtualScreenMuteStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, GetVirtualScreenMuteStatusTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_FALSE(renderNode->GetVirtualScreenMuteStatus());
}

/**
 * @tc.name: GetSpecialLayerMgrTest
 * @tc.desc: test results of GetSpecialLayerMgr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, GetSpecialLayerMgrTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_NE(renderNode, nullptr);
    renderNode->GetSpecialLayerMgr();
}

/**
 * @tc.name: AddSecurityLayerTest
 * @tc.desc: test results of AddSecurityLayer and GetSecurityLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, AddSecurityLayerTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    constexpr NodeId newNodeId = 2;
    renderNode->AddSecurityLayer(newNodeId);
    EXPECT_FALSE(renderNode->GetSecurityLayerList().empty());
}

/**
 * @tc.name: AddSecurityVisibleLayerTest
 * @tc.desc: test results of AddSecurityVisibleLayer and GetSecurityVisibleLayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, AddSecurityVisibleLayerTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    constexpr NodeId newNodeId = 2;
    renderNode->AddSecurityVisibleLayer(newNodeId);
    EXPECT_FALSE(renderNode->GetSecurityVisibleLayerList().empty());
}

/**
 * @tc.name: SetSecurityExemptionTest
 * @tc.desc: test results of SetSecurityExemption and GetSecurityExemption
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, SetSecurityExemption, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    renderNode->SetSecurityExemption(true);
    EXPECT_TRUE(renderNode->GetSecurityExemption());
}

/**
 * @tc.name: SetHasSecLayerInVisibleRectTest
 * @tc.desc: test results of SetHasSecLayerInVisibleRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, SetHasSecLayerInVisibleRectTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    renderNode->SetHasSecLayerInVisibleRect(true);
    EXPECT_TRUE(renderNode->hasSecLayerInVisibleRect_);
}

/**
 * @tc.name: GetCompositeTypeTest
 * @tc.desc: test results of GetCompositeType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, GetCompositeTypeTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_EQ(renderNode->GetCompositeType(), CompositeType::HARDWARE_COMPOSITE);
}
} // namespace OHOS::Rosen