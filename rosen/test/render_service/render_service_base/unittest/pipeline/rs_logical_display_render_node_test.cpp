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

#include "animation/rs_render_property_animation.h"
#include "params/rs_logical_display_render_params.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_draw_cmd.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "visitor/rs_node_visitor.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSRenderPropertyAnimationMock : public RSRenderPropertyAnimation {
public:
    RSRenderPropertyAnimationMock(
        AnimationId id, const PropertyId& propertyId,
        const std::shared_ptr<RSRenderPropertyBase>& originValue)
        : RSRenderPropertyAnimation(id, propertyId, originValue)
    {}
    void RebuildPropertyValue(float fraction) override {}
};

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
 * @tc.name: IsOnlyHDRAnimationTest
 * @tc.desc: test results of IsOnlyHDRAnimation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, IsOnlyHDRAnimationTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_EQ(displayNode->GetModifiersNG(ModifierNG::RSModifierType::HDR_BRIGHTNESS).empty(), true);
    EXPECT_EQ(displayNode->IsOnlyHDRAnimation(), false);

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    property->id_ = 1;
    ModifierId id = 1;
    std::shared_ptr<ModifierNG::RSRenderModifier> modifier = nullptr;
    RSRootRenderNode::ModifierNGContainer modifiers { modifier };
    EXPECT_EQ(displayNode->IsOnlyHDRAnimation(), false);

    auto modifier2 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::HDR_BRIGHTNESS, property, id, ModifierNG::RSPropertyType::HDR_BRIGHTNESS_FACTOR);
    modifiers.emplace_back(modifier2);
    displayNode->modifiersNG_.emplace(ModifierNG::RSModifierType::HDR_BRIGHTNESS, modifiers);
    auto animationProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimationMock>(0, 1, animationProperty);
    displayNode->animationManager_ = std::make_shared<RSAnimationManager>();
    displayNode->animationManager_->animations_[0] = renderPropertyAnimation;
    displayNode->IsOnlyHDRAnimation();

    auto modifier3 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::HDR_BRIGHTNESS, property, id, ModifierNG::RSPropertyType::HDR_BRIGHTNESS);
    modifiers.emplace_back(modifier3);
    renderPropertyAnimation->propertyId_ = 2;
    std::shared_ptr<RSRenderAnimation> animation2;
    displayNode->animationManager_->animations_[1] = animation2;
    displayNode->IsOnlyHDRAnimation();
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

/**
 * @tc.name: ClearModifiersByPidTest
 * @tc.desc: test results of ClearModifiersByPid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, ClearModifiersByPidTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    EXPECT_NE(renderNode, nullptr);
    int32_t pid = getpid();
    renderNode->ClearModifiersByPid(pid);
}

/**
 * @tc.name: UpdateDimensionsTest
 * @tc.desc: test results of UpdateDimensions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, UpdateDimensionsTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto renderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    renderNode->InitRenderParams();
    auto& geo = renderNode->GetRenderProperties().GetBoundsGeometry();
    geo->SetRotation(90);
    renderNode->UpdateRotation();
    renderNode->UpdateFixedSize();

    renderNode->UpdateRotation();
    renderNode->UpdateFixedSize();

    geo->SetRotation(270);
    renderNode->UpdateRotation();
    renderNode->UpdateRotation();
    renderNode->UpdateFixedSize();
    EXPECT_EQ(renderNode->GetCompositeType(), CompositeType::HARDWARE_COMPOSITE);
}

/**
 * @tc.name: IsOnlyHDRAnimationWithNullManager001
 * @tc.desc: Verify IsOnlyHDRAnimation returns false with null animationManager and HDR modifiers
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, IsOnlyHDRAnimationWithNullManager001, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);

    // Add HDR modifiers but NO animationManager (don't add any animation)
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    property->id_ = 1;
    ModifierId id = 1;

    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::HDR_BRIGHTNESS, property, id, ModifierNG::RSPropertyType::HDR_BRIGHTNESS_FACTOR);
    RSRootRenderNode::ModifierNGContainer modifiers { modifier };
    displayNode->modifiersNG_.emplace(ModifierNG::RSModifierType::HDR_BRIGHTNESS, modifiers);

    // animationManager_ is null since no animations were added
    EXPECT_EQ(displayNode->GetAnimationManager(), nullptr);
    EXPECT_FALSE(displayNode->IsOnlyHDRAnimation());
}

/**
 * @tc.name: IsInBlendModeGroupNonlinearTest
 * @tc.desc: Verify IsInBlendModeGroup with NonlinearBlendModes group
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, IsInBlendModeGroupNonlinearTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    // Nonlinear modes should return true
    EXPECT_TRUE(displayNode->IsInBlendModeGroup(
        static_cast<int>(RSColorBlendMode::SCREEN), RSLogicalDisplayRenderNode::NonlinearBlendModes));

    // Non-nonlinear modes should return false
    EXPECT_FALSE(displayNode->IsInBlendModeGroup(
        static_cast<int>(RSColorBlendMode::SRC), RSLogicalDisplayRenderNode::NonlinearBlendModes));

    // Boundary: negative and large values
    EXPECT_FALSE(displayNode->IsInBlendModeGroup(-1, RSLogicalDisplayRenderNode::NonlinearBlendModes));
}

/**
 * @tc.name: IsInBlendModeGroupMultipleGroupsTest
 * @tc.desc: Verify IsInBlendModeGroup across Child, Empty, Parent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, IsInBlendModeGroupMultipleGroupsTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    EXPECT_TRUE(displayNode->IsInBlendModeGroup(static_cast<int>(RSColorBlendMode::SRC_OVER),
        RSLogicalDisplayRenderNode::ChildBlendModes));
    EXPECT_FALSE(displayNode->IsInBlendModeGroup(static_cast<int>(RSColorBlendMode::NONE),
        RSLogicalDisplayRenderNode::EmptyBlendModes));
    EXPECT_TRUE(displayNode->IsInBlendModeGroup(static_cast<int>(RSColorBlendMode::SRC_IN),
        RSLogicalDisplayRenderNode::ParentBlendModes));
}

/**
 * @tc.name: HasNonlinearBlendModeTest
 * @tc.desc: Verify HasNonlinearBlendMode with all linear and nonlinear modes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, HasNonlinearBlendModeTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    // Nonlinear modes
    EXPECT_TRUE(displayNode->HasNonlinearBlendMode(static_cast<int>(RSColorBlendMode::SCREEN)));

    // Linear modes
    EXPECT_FALSE(displayNode->HasNonlinearBlendMode(static_cast<int>(RSColorBlendMode::SRC)));

    // Boundary
    EXPECT_FALSE(displayNode->HasNonlinearBlendMode(100));
}

/**
 * @tc.name: HasChildBlendModeTest
 * @tc.desc: Verify HasChildBlendMode for all ChildBlendModes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, HasChildBlendModeTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    EXPECT_TRUE(displayNode->HasChildBlendMode(static_cast<int>(RSColorBlendMode::SRC_OVER)));
    EXPECT_FALSE(displayNode->HasChildBlendMode(static_cast<int>(RSColorBlendMode::PLUS)));
}

/**
 * @tc.name: HasEmptyBlendModeTest
 * @tc.desc: Verify HasEmptyBlendMode for all EmptyBlendModes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, HasEmptyBlendModeTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    EXPECT_TRUE(displayNode->HasEmptyBlendMode(static_cast<int>(RSColorBlendMode::XOR)));
    EXPECT_FALSE(displayNode->HasEmptyBlendMode(static_cast<int>(RSColorBlendMode::DST)));
    EXPECT_FALSE(displayNode->HasEmptyBlendMode(0));
}

/**
 * @tc.name: HasParentBlendModeTest
 * @tc.desc: Verify HasParentBlendMode for all ParentBlendModes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, HasParentBlendModeTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    EXPECT_TRUE(displayNode->HasParentBlendMode(static_cast<int>(RSColorBlendMode::SRC)));
    EXPECT_FALSE(displayNode->HasParentBlendMode(static_cast<int>(RSColorBlendMode::SRC_OVER)));
}

/**
 * @tc.name: IncreaseBlendModeNodeTest
 * @tc.desc: Verify IncreaseBlendModeNode increments counter correctly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, IncreaseBlendModeNodeTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 0);

    displayNode->IncreaseBlendModeNode(nodeId);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 1);

    displayNode->IncreaseBlendModeNode(nodeId);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 2);

    // Different node id
    NodeId nodeId2 = nodeId + 1;
    displayNode->IncreaseBlendModeNode(nodeId2);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 3);

    // Multiple increments for same node
    displayNode->IncreaseBlendModeNode(nodeId);
    displayNode->IncreaseBlendModeNode(nodeId);
    displayNode->IncreaseBlendModeNode(nodeId);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 6);
}

/**
 * @tc.name: RemoveBlendModeNodeTest
 * @tc.desc: Verify RemoveBlendModeNode removes node from map correctly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, RemoveBlendModeNodeTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    // Decrease non-existing node should not crash
    displayNode->RemoveBlendModeNode(nodeId);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 0);

    // Increase then decrease
    displayNode->IncreaseBlendModeNode(nodeId);
    displayNode->IncreaseBlendModeNode(nodeId);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 2);

    displayNode->RemoveBlendModeNode(nodeId);
    // After decrease, node is removed entirely
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 0);

    // Multiple node ids
    NodeId nodeId2 = nodeId + 1;
    NodeId nodeId3 = nodeId + 2;
    displayNode->IncreaseBlendModeNode(nodeId);
    displayNode->IncreaseBlendModeNode(nodeId2);
    displayNode->IncreaseBlendModeNode(nodeId3);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 3);

    displayNode->RemoveBlendModeNode(nodeId2);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 2);

    displayNode->RemoveBlendModeNode(nodeId);
    displayNode->RemoveBlendModeNode(nodeId3);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 0);
}

/**
 * @tc.name: GetDstAlphaBlendModeNodeCountTest
 * @tc.desc: Verify GetDstAlphaBlendModeNodeCount returns correct sum
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, GetDstAlphaBlendModeNodeCountTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    // Initially 0
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 0);

    // Single node multiple increments
    displayNode->IncreaseBlendModeNode(nodeId);
    displayNode->IncreaseBlendModeNode(nodeId);
    displayNode->IncreaseBlendModeNode(nodeId);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 3);

    // Multiple nodes
    NodeId nodeId2 = nodeId + 1;
    displayNode->IncreaseBlendModeNode(nodeId2);
    displayNode->IncreaseBlendModeNode(nodeId2);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 5);

    // Remove one node
    displayNode->RemoveBlendModeNode(nodeId);
    EXPECT_EQ(displayNode->GetDstAlphaBlendModeNodeCount(), 2);
}

/**
 * @tc.name: CheckAncestorChildBlendModeBlendZeroEmptyCurrentTest
 * @tc.desc: Verify CheckAncestorChildBlendMode when blendMode==0 and current is Empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, CheckAncestorChildBlendModeBlendZeroEmptyCurrentTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        0, static_cast<int>(RSColorBlendMode::SRC), false, false));

    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        0, static_cast<int>(RSColorBlendMode::DST_ATOP), false, false));

    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        0, static_cast<int>(RSColorBlendMode::XOR), false, false));
}

/**
 * @tc.name: CheckAncestorChildBlendModeBlendZeroNonEmptyParentTest
 * @tc.desc: Verify CheckAncestorChildBlendMode when blendMode==0, current not Empty, but IsParentBlendMode=true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, CheckAncestorChildBlendModeBlendZeroNonEmptyParentTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    // blendMode=0, current=SRC_IN (not Empty), IsParentBlendMode=true
    // -> isParent=true, then check HasChildBlendMode(0)
    // 0 is NONE which is not in ChildBlendModes => false
    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        0, static_cast<int>(RSColorBlendMode::SRC_IN), false, true));

    // blendMode=0, current=DST_OUT (not Empty), IsParentBlendMode=true
    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        0, static_cast<int>(RSColorBlendMode::DST_OUT), false, true));
}

/**
 * @tc.name: CheckAncestorChildBlendModeNonZeroParentBlendModeTest
 * @tc.desc: Verify CheckAncestorChildBlendMode for non-zero blendMode with ParentBlendMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, CheckAncestorChildBlendModeNonZeroParentBlendModeTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::DST_OVER),
        static_cast<int>(RSColorBlendMode::SRC), false, false));

    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::SRC_OUT),
        static_cast<int>(RSColorBlendMode::SRC_IN), false, false));

    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::SRC),
        static_cast<int>(RSColorBlendMode::DST_OUT), false, false));
}

/**
 * @tc.name: CheckAncestorChildBlendModeNonParentBlendModeTest
 * @tc.desc: Verify CheckAncestorChildBlendMode returns false when current is not ParentBlendMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, CheckAncestorChildBlendModeNonParentBlendModeTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::SRC_OVER),
        static_cast<int>(RSColorBlendMode::DST), false, false));

    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::DST_OVER),
        static_cast<int>(RSColorBlendMode::SRC_OVER), false, false));

    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::DST_OVER),
        static_cast<int>(RSColorBlendMode::CLEAR), false, false));

    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::SRC_OVER),
        static_cast<int>(RSColorBlendMode::PLUS), false, false));
}

/**
 * @tc.name: CheckAncestorChildBlendModeNonChildBlendModeTest
 * @tc.desc: Verify CheckAncestorChildBlendMode when ParentBlendMode but not Child
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, CheckAncestorChildBlendModeNonChildBlendModeTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::DST),
        static_cast<int>(RSColorBlendMode::SRC), false, false));

    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::CLEAR),
        static_cast<int>(RSColorBlendMode::SRC_IN), false, false));

    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::PLUS),
        static_cast<int>(RSColorBlendMode::XOR), false, false));
}

/**
 * @tc.name: CheckAncestorChildBlendModeOverrideFlagsTest
 * @tc.desc: Verify CheckAncestorChildBlendMode with IsEmptyBlendMode and IsParentBlendMode flags
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, CheckAncestorChildBlendModeOverrideFlagsTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    // blendMode=0, current=DST (not Empty), IsEmptyBlendMode=true => should return true
    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        0, static_cast<int>(RSColorBlendMode::DST), true, false));

    // blendMode=0, current=None (not Empty), IsEmptyBlendMode=true => should return true
    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        0, static_cast<int>(RSColorBlendMode::NONE), true, false));

    // blendMode=SRC (is Child), current=DST (not Parent), IsParentBlendMode=true => should return true
    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::SRC),
        static_cast<int>(RSColorBlendMode::DST), false, true));

    // blendMode=DST_OVER (is Child), current=CLEAR (not Parent), IsParentBlendMode=true => should return true
    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::DST_OVER),
        static_cast<int>(RSColorBlendMode::CLEAR), false, true));

    // blendMode=SRC (is Child), IsParentBlendMode=false, current=DST_IN (is Parent) => should return true
    EXPECT_TRUE(displayNode->CheckAncestorChildBlendMode(
        static_cast<int>(RSColorBlendMode::SRC),
        static_cast<int>(RSColorBlendMode::DST_IN), false, false));
}

/**
 * @tc.name: CheckAncestorChildBlendModeBoundaryTest
 * @tc.desc: Verify CheckAncestorChildBlendMode with boundary values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLogicalDisplayRenderNodeTest, CheckAncestorChildBlendModeBoundaryTest, TestSize.Level1)
{
    constexpr NodeId nodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayNode, nullptr);

    // blendMode=0, current not Empty, not Parent, with empty flag false
    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        0, static_cast<int>(RSColorBlendMode::SRC_OVER), false, false));

    // blendMode=0, current=DARKEN (not Empty), not Parent
    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        0, static_cast<int>(RSColorBlendMode::DARKEN), false, false));

    // Non-zero blendMode not in ChildBlendModes
    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        -1, static_cast<int>(RSColorBlendMode::SRC), false, false));

    EXPECT_FALSE(displayNode->CheckAncestorChildBlendMode(
        100, static_cast<int>(RSColorBlendMode::SRC_IN), false, false));
}

} // namespace OHOS::Rosen