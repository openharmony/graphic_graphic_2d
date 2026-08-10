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

#include "gtest/gtest.h"

#include "common/rs_common_def.h"
#include "modifier_ng/appearance/rs_depth_space_render_modifier.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "params/rs_depth_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_depth_render_node.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties.h"
#include "property/rs_spatial_effect_def.h"
#include "property/rs_spatial_effect_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSpatialEffectManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    std::shared_ptr<RSRenderNode> CreateSpatialEffectNode(NodeId id);
    std::pair<std::shared_ptr<RSDepthRenderNode>, std::shared_ptr<RSRenderNode>> CreateDepthNodeHierarchy(
        NodeId depthId, NodeId effectId);
    static inline std::weak_ptr<RSContext> context_ = {};
};

void RSSpatialEffectManagerTest::SetUpTestCase() {}
void RSSpatialEffectManagerTest::TearDownTestCase() {}

void RSSpatialEffectManagerTest::SetUp()
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->spatialEffectDepthNodeMap_.clear();
    instance->depthSpatialEffectNodeMap_.clear();
    instance->masterGlobalDepthNodeMap_.clear();
    instance->depthResourceNodeMap_.clear();
    instance->depthBackgroundNodeMap_.clear();
}

void RSSpatialEffectManagerTest::TearDown()
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->spatialEffectDepthNodeMap_.clear();
    instance->depthSpatialEffectNodeMap_.clear();
    instance->masterGlobalDepthNodeMap_.clear();
    instance->depthResourceNodeMap_.clear();
    instance->depthBackgroundNodeMap_.clear();
}

std::shared_ptr<RSRenderNode> RSSpatialEffectManagerTest::CreateSpatialEffectNode(NodeId id)
{
    auto node = std::make_shared<RSRenderNode>(id);
    auto& props = node->GetMutableRenderProperties();
    SpatialEffectPara spatialEffectPara;
    spatialEffectPara.leftTop = Vector3f(0.0f, 0.0f, 1.0f);
    spatialEffectPara.rightTop = Vector3f(1.0f, 0.0f, 1.0f);
    spatialEffectPara.leftBottom = Vector3f(0.0f, 1.0f, 1.0f);
    spatialEffectPara.rightBottom = Vector3f(1.0f, 1.0f, 1.0f);
    props.SetSpatialEffectPara(spatialEffectPara);
    return node;
}

std::pair<std::shared_ptr<RSDepthRenderNode>, std::shared_ptr<RSRenderNode>>
RSSpatialEffectManagerTest::CreateDepthNodeHierarchy(NodeId depthId, NodeId effectId)
{
    auto depthNode = std::make_shared<RSDepthRenderNode>(depthId, context_);
    auto effectNode = CreateSpatialEffectNode(effectId);
    depthNode->AddChild(effectNode);
    return { depthNode, effectNode };
}

/**
 * @tc.name: Instance001
 * @tc.desc: test singleton instance
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, Instance001, TestSize.Level1)
{
    auto instance1 = RSSpatialEffectManager::Instance();
    auto instance2 = RSSpatialEffectManager::Instance();
    ASSERT_EQ(instance1, instance2);
}

/**
 * @tc.name: RegisterSpatialEffect001
 * @tc.desc: test RegisterSpatialEffect with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, RegisterSpatialEffect001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->RegisterSpatialEffect(nullptr);
    // Should not crash
    EXPECT_EQ(instance->spatialEffectDepthNodeMap_.size(), 0);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 0);
}

/**
 * @tc.name: RegisterSpatialEffect002
 * @tc.desc: test RegisterSpatialEffect with node but no depth ancestor
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, RegisterSpatialEffect002, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto regularNode = std::make_shared<RSRenderNode>(100);
    instance->RegisterSpatialEffect(regularNode);

    EXPECT_EQ(instance->spatialEffectDepthNodeMap_.size(), 0);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 0);
}

/**
 * @tc.name: UnregisterSpatialEffect001
 * @tc.desc: test UnregisterSpatialEffect with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, UnregisterSpatialEffect001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->UnregisterSpatialEffect(nullptr);
    EXPECT_EQ(instance->spatialEffectDepthNodeMap_.size(), 0);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 0);
}

/**
 * @tc.name: UnregisterSpatialEffect002
 * @tc.desc: test UnregisterSpatialEffect with unregistered node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, UnregisterSpatialEffect002, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto regularNode = std::make_shared<RSRenderNode>(300);
    size_t initialSize = instance->spatialEffectDepthNodeMap_.size();
    instance->UnregisterSpatialEffect(regularNode);
    EXPECT_EQ(instance->spatialEffectDepthNodeMap_.size(), initialSize);
}

/**
 * @tc.name: RegisterDepthSpace001
 * @tc.desc: test RegisterDepthSpace with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, RegisterDepthSpace001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->RegisterDepthSpace(nullptr);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 0);
}

/**
 * @tc.name: RegisterDepthSpace002
 * @tc.desc: test RegisterDepthSpace with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, RegisterDepthSpace002, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto regularNode = std::make_shared<RSRenderNode>(500);
    instance->RegisterDepthSpace(regularNode);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);
}

/**
 * @tc.name: RegisterDepthResource001
 * @tc.desc: test RegisterDepthResource with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, RegisterDepthResource001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->RegisterDepthResource(nullptr);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 0);
}

/**
 * @tc.name: RegisterDepthResource002
 * @tc.desc: test RegisterDepthResource with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, RegisterDepthResource002, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto resourceNode = std::make_shared<RSRenderNode>(600);
    instance->RegisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 1);
}

/**
 * @tc.name: UnregisterDepthResource001
 * @tc.desc: test UnregisterDepthResource with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, UnregisterDepthResource001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->UnregisterDepthResource(nullptr);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 0);
}

/**
 * @tc.name: UnregisterDepthResource002
 * @tc.desc: test UnregisterDepthResource with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, UnregisterDepthResource002, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto resourceNode = std::make_shared<RSRenderNode>(700);

    instance->RegisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 1);

    instance->UnregisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 0);
}

/**
 * @tc.name: RegisterDepthBackground001
 * @tc.desc: test RegisterDepthBackground with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, RegisterDepthBackground001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->RegisterDepthBackground(nullptr);
    EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), 0);
}

/**
 * @tc.name: RegisterDepthBackground002
 * @tc.desc: test RegisterDepthBackground with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, RegisterDepthBackground002, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto bgNode = std::make_shared<RSRenderNode>(800);
    instance->RegisterDepthBackground(bgNode);
    EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), 1);
}

/**
 * @tc.name: UnregisterDepthBackground001
 * @tc.desc: test UnregisterDepthBackground with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, UnregisterDepthBackground001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->UnregisterDepthBackground(nullptr);
    EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), 0);
}

/**
 * @tc.name: UnregisterDepthBackground002
 * @tc.desc: test UnregisterDepthBackground with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, UnregisterDepthBackground002, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto bgNode = std::make_shared<RSRenderNode>(900);

    instance->RegisterDepthBackground(bgNode);
    EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), 1);

    instance->UnregisterDepthBackground(bgNode);
    EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), 0);
}

/**
 * @tc.name: GetMasterGlobalDepthNode001
 * @tc.desc: test GetMasterGlobalDepthNode when no global depth node exists
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, GetMasterGlobalDepthNode001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto globalNode = instance->GetMasterGlobalDepthNode(114);
    ASSERT_TRUE(globalNode.expired());
}

/**
 * @tc.name: GetAncestorDepthNode001
 * @tc.desc: test GetAncestorDepthNode for unregistered node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, GetAncestorDepthNode001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto regularNode = std::make_shared<RSRenderNode>(1100);
    auto ancestor = instance->GetAncestorDepthNode(*regularNode);
    ASSERT_TRUE(ancestor.expired());
}

/**
 * @tc.name: GetDepthResourceNode001
 * @tc.desc: test GetDepthResourceNode
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, GetDepthResourceNode001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSRenderNode>(1);
    EXPECT_TRUE(instance->GetDepthResourceNode(*depthNode).expired());

    auto depthResourceNode = std::make_shared<RSRenderNode>(2);
    instance->depthDepthResourceNodeMap_.emplace(depthNode->GetId(), depthResourceNode->weak_from_this());
    EXPECT_FALSE(instance->GetDepthResourceNode(*depthNode).expired());
}

/**
 * @tc.name: ProcessDepthNodeAndSpatialEffectNodeDirty001
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty with no registered nodes
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessDepthNodeAndSpatialEffectNodeDirty001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_TRUE(instance->spatialEffectDepthNodeMap_.empty());
    EXPECT_TRUE(instance->depthSpatialEffectNodeMap_.empty());
}

/**
 * @tc.name: ProcessDepthNodeAndSpatialEffectNodeDirty002
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty with registered nodes
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessDepthNodeAndSpatialEffectNodeDirty002, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSRenderNode>(1200);
    auto resourceNode = std::make_shared<RSRenderNode>(1201);
    auto bgNode = std::make_shared<RSRenderNode>(1202);

    instance->RegisterDepthSpace(depthNode);
    instance->RegisterDepthResource(resourceNode);
    instance->RegisterDepthBackground(bgNode);

    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 1);
    EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), 1);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);
}

/**
 * @tc.name: ProcessDepthNodeAndSpatialEffectNodeDirty003
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty with expired nodes
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessDepthNodeAndSpatialEffectNodeDirty003, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSRenderNode>(1300);
    auto resourceNode = std::make_shared<RSRenderNode>(1301);

    instance->RegisterDepthSpace(depthNode);
    instance->RegisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 1);

    depthNode.reset();
    resourceNode.reset();

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_TRUE(instance->depthSpatialEffectNodeMap_.empty());
    EXPECT_TRUE(instance->depthResourceNodeMap_.empty());
}

/**
 * @tc.name: ProcessDepthNodeAndSpatialEffectNodeDirty004
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty without master global depth node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessDepthNodeAndSpatialEffectNodeDirty004, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSRenderNode>(1300);
    constexpr auto depthSpaceModifierTypeNG = static_cast<uint16_t>(ModifierNG::RSModifierType::DEPTH_SPACE);
    depthNode->dirtyTypesNG_.set(depthSpaceModifierTypeNG);
    RSRenderNode::ModifierNGContainer modifierContainer =
        { std::make_shared<ModifierNG::RSDepthSpaceRenderModifier>() };
    depthNode->modifiersNG_.emplace(ModifierNG::RSModifierType::DEPTH_SPACE, modifierContainer);

    instance->RegisterDepthSpace(depthNode);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_TRUE(depthNode->dirtyTypesNG_.test(depthSpaceModifierTypeNG));

    depthNode->SetDirty(true);
    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_TRUE(depthNode->dirtyTypesNG_.test(depthSpaceModifierTypeNG));
}

/**
 * @tc.name: ProcessDepthNodeAndSpatialEffectNodeDirty005
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty with master global depth node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessDepthNodeAndSpatialEffectNodeDirty005, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    NodeId logicalDisplayNodeId = 114;

    auto masterDepthNode = std::make_shared<RSDepthRenderNode>(1300, context->weak_from_this());
    masterDepthNode->SetDepthSpaceType(DepthSpaceType::INSTANCE);
    masterDepthNode->logicalDisplayNodeId_ = logicalDisplayNodeId;
    constexpr auto depthSpaceModifierTypeNG = static_cast<uint16_t>(ModifierNG::RSModifierType::DEPTH_SPACE);
    masterDepthNode->dirtyTypesNG_.set(depthSpaceModifierTypeNG);
    RSRenderNode::ModifierNGContainer modifierContainer =
        { std::make_shared<ModifierNG::RSDepthSpaceRenderModifier>() };
    masterDepthNode->modifiersNG_.emplace(ModifierNG::RSModifierType::DEPTH_SPACE, modifierContainer);

    auto surfaceNodeId = 514;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId, context->weak_from_this());
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SCB_WALLPAPER;
    context->GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    masterDepthNode->instanceRootNodeId_ = surfaceNodeId;

    EXPECT_TRUE(instance->IsMasterGlobalDepthNodeAndUpdate(masterDepthNode));

    instance->RegisterDepthSpace(masterDepthNode);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_TRUE(masterDepthNode->dirtyTypesNG_.test(depthSpaceModifierTypeNG));

    masterDepthNode->SetDirty(true);
    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_FALSE(masterDepthNode->dirtyTypesNG_.test(depthSpaceModifierTypeNG));

    auto slaveDepthNode = std::make_shared<RSDepthRenderNode>(1301, context->weak_from_this());
    slaveDepthNode->SetDepthSpaceType(DepthSpaceType::GLOBAL);

    instance->RegisterDepthSpace(masterDepthNode);

    masterDepthNode->ResetAccumulateDirtyStatus();
    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_FALSE(slaveDepthNode->IsDirty());

    masterDepthNode->SetDirty();
    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_FALSE(slaveDepthNode->IsDirty());

    slaveDepthNode->logicalDisplayNodeId_ = logicalDisplayNodeId;
    masterDepthNode->ResetAccumulateDirtyStatus();
    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_FALSE(slaveDepthNode->IsDirty());

    masterDepthNode->SetDirty();
    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_TRUE(slaveDepthNode->IsDirty());
}

/**
 * @tc.name: StressTest_MultipleNodes001
 * @tc.desc: test with many nodes registered
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, StressTest_MultipleNodes001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    constexpr int nodeCount = 10;

    for (int i = 0; i < nodeCount; ++i) {
        auto depthNode = std::make_shared<RSRenderNode>(1500 + i);
        instance->RegisterDepthSpace(depthNode);
    }
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), nodeCount);

    for (int i = 0; i < nodeCount; ++i) {
        auto resourceNode = std::make_shared<RSRenderNode>(1600 + i);
        instance->RegisterDepthResource(resourceNode);
    }
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), nodeCount);

    for (int i = 0; i < nodeCount; ++i) {
        auto bgNode = std::make_shared<RSRenderNode>(1700 + i);
        instance->RegisterDepthBackground(bgNode);
    }
    EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), nodeCount);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 0);
}

/**
 * @tc.name: RegisterUnregisterCycle001
 * @tc.desc: test register/unregister cycle
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, RegisterUnregisterCycle001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto resourceNode = std::make_shared<RSRenderNode>(1800);

    instance->RegisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 1);

    instance->UnregisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 0);

    instance->RegisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 1);

    instance->UnregisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 0);
}

/**
 * @tc.name: EdgeCase_EmptyNodeMap001
 * @tc.desc: test with all empty node maps
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, EdgeCase_EmptyNodeMap001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();

    // Call all methods with empty state
    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();

    // Verify no global depth node
    auto globalNode = instance->GetMasterGlobalDepthNode(114);
    ASSERT_TRUE(globalNode.expired());
}

/**
 * @tc.name: MultipleRegistrations001
 * @tc.desc: test registering same node multiple times
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, MultipleRegistrations001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto resourceNode = std::make_shared<RSRenderNode>(1900);

    instance->RegisterDepthResource(resourceNode);
    instance->RegisterDepthResource(resourceNode);
    instance->RegisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 1);

    instance->UnregisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 0);
}

/**
 * @tc.name: ProcessLargeNumberOfNodes001
 * @tc.desc: test processing large number of nodes
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessLargeNumberOfNodes001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();

    const int nodeCount = 50;
    for (int i = 0; i < nodeCount; ++i) {
        auto node = std::make_shared<RSRenderNode>(2000 + i);
        instance->RegisterDepthSpace(node);
    }
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), nodeCount);

    for (int i = 0; i < 5; ++i) {
        instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    }
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 0);
}

/**
 * @tc.name: LifecycleTest001
 * @tc.desc: test full lifecycle of node registration and unregistration
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, LifecycleTest001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();

    auto depthNode = std::make_shared<RSRenderNode>(2100);
    auto resourceNode = std::make_shared<RSRenderNode>(2101);
    auto bgNode = std::make_shared<RSRenderNode>(2102);

    instance->RegisterDepthSpace(depthNode);
    instance->RegisterDepthResource(resourceNode);
    instance->RegisterDepthBackground(bgNode);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 1);
    EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), 1);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();

    instance->UnregisterDepthResource(resourceNode);
    instance->UnregisterDepthBackground(bgNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 0);
    EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), 0);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);
}

/**
 * @tc.name: ConcurrentAccess001
 * @tc.desc: test multiple sequential operations
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ConcurrentAccess001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();

    for (int cycle = 0; cycle < 3; ++cycle) {
        auto node = std::make_shared<RSRenderNode>(2200 + cycle);
        instance->RegisterDepthResource(node);
        EXPECT_EQ(instance->depthResourceNodeMap_.size(), 1);

        instance->ProcessDepthNodeAndSpatialEffectNodeDirty();

        instance->UnregisterDepthResource(node);
        EXPECT_EQ(instance->depthResourceNodeMap_.size(), 0);
    }
}

/**
 * @tc.name: RegisterWithExpiredNodes001
 * @tc.desc: test registering nodes and then letting them expire
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, RegisterWithExpiredNodes001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();

    {
        auto node = std::make_shared<RSRenderNode>(2300);
        instance->RegisterDepthResource(node);
        instance->RegisterDepthBackground(node);
    }

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_TRUE(instance->depthResourceNodeMap_.empty());
    EXPECT_TRUE(instance->depthBackgroundNodeMap_.empty());
}

/**
 * @tc.name: GetAncestorDepthNodeWithNullNode001
 * @tc.desc: test GetAncestorDepthNode behavior
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, GetAncestorDepthNodeWithNullNode001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto regularNode = std::make_shared<RSRenderNode>(2400);

    // Should return expired pointer for unregistered node
    auto ancestor = instance->GetAncestorDepthNode(*regularNode);
    ASSERT_TRUE(ancestor.expired());
}

/**
 * @tc.name: ProcessAndPrepareCycle001
 * @tc.desc: test multiple process and prepare cycles
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessAndPrepareCycle001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto node = std::make_shared<RSRenderNode>(2500);

    instance->RegisterDepthSpace(node);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);

    for (int i = 0; i < 10; ++i) {
        instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
        EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);
    }
}

/**
 * @tc.name: MixedOperations001
 * @tc.desc: test mixed registration, unregistration, and processing
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, MixedOperations001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();

    std::array<std::shared_ptr<RSRenderNode>, 10> nodes;
    for (int i = 0; i < 10; ++i) {
        nodes[i] = std::make_shared<RSRenderNode>(2600 + i);

        switch (i % 3) {
            case 0:
                instance->RegisterDepthSpace(nodes[i]);
                break;
            case 1:
                instance->RegisterDepthResource(nodes[i]);
                break;
            case 2:
                instance->RegisterDepthBackground(nodes[i]);
                break;
            default:
                break;
        }
    }

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();

    instance->UnregisterDepthResource(nodes[1]);
    instance->UnregisterDepthBackground(nodes[2]);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();

    const int depthNodes = 4;
    const int resourceNodes = 2;
    const int bgNodes = 2;
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), depthNodes);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), resourceNodes);
    EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), bgNodes);
}

/**
 * @tc.name: ProcessWithExpiredWeakPtr
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty with expired weak pointers
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessWithExpiredWeakPtr, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    {
        auto [depthNode, effectNode] = CreateDepthNodeHierarchy(3000, 3001);
        instance->RegisterSpatialEffect(effectNode);
        EXPECT_EQ(instance->spatialEffectDepthNodeMap_.size(), 1);
        EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);
    }

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_TRUE(instance->spatialEffectDepthNodeMap_.empty());
}

/**
 * @tc.name: ProcessWithDepthNodeCastFail
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty handles cast failure
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessWithDepthNodeCastFail, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto regularNode = std::make_shared<RSRenderNode>(3100);
    instance->RegisterDepthSpace(regularNode);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);
}

/**
 * @tc.name: ProcessWithMasterGlobalDirty
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty with master global dirty node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessWithMasterGlobalDirty, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(3200, context_);
    depthNode->SetDepthSpaceType(DepthSpaceType::INSTANCE);
    instance->RegisterDepthSpace(depthNode);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);

    auto effectNode = CreateSpatialEffectNode(3201);
    depthNode->AddChild(effectNode);
    instance->RegisterSpatialEffect(effectNode);
    EXPECT_EQ(instance->spatialEffectDepthNodeMap_.size(), 1);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);
}

/**
 * @tc.name: ProcessWithGlobalDepthSpaceType
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty marks GLOBAL depth nodes dirty
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessWithGlobalDepthSpaceType, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto masterDepthNode = std::make_shared<RSDepthRenderNode>(3300, context_);
    masterDepthNode->SetDepthSpaceType(DepthSpaceType::INSTANCE);
    instance->RegisterDepthSpace(masterDepthNode);

    auto globalDepthNode = std::make_shared<RSDepthRenderNode>(3301, context_);
    globalDepthNode->SetDepthSpaceType(DepthSpaceType::GLOBAL);
    instance->RegisterDepthSpace(globalDepthNode);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 2);

    auto masterEffectNode = CreateSpatialEffectNode(3302);
    masterDepthNode->AddChild(masterEffectNode);
    instance->RegisterSpatialEffect(masterEffectNode);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 2);
}

/**
 * @tc.name: MixedNodes_CleanExpiredNodes
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty cleans expired mixed nodes
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, MixedNodes_CleanExpiredNodes, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(3400, context_);
    instance->RegisterDepthSpace(depthNode);

    auto resourceNode1 = std::make_shared<RSRenderNode>(3401);
    auto resourceNode2 = std::make_shared<RSRenderNode>(3402);
    instance->RegisterDepthResource(resourceNode1);
    instance->RegisterDepthResource(resourceNode2);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 2);

    {
        auto bgNode = std::make_shared<RSRenderNode>(3403);
        instance->RegisterDepthBackground(bgNode);
        EXPECT_EQ(instance->depthBackgroundNodeMap_.size(), 1);
    }

    auto effectNode1 = CreateSpatialEffectNode(3404);
    auto effectNode2 = CreateSpatialEffectNode(3405);
    depthNode->AddChild(effectNode1);
    depthNode->AddChild(effectNode2);
    instance->RegisterSpatialEffect(effectNode1);
    instance->RegisterSpatialEffect(effectNode2);
    EXPECT_EQ(instance->spatialEffectDepthNodeMap_.size(), 2);

    effectNode2.reset();
    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_EQ(instance->spatialEffectDepthNodeMap_.size(), 1);
    EXPECT_TRUE(instance->depthBackgroundNodeMap_.empty());
}

/**
 * @tc.name: CleanExpiredNodes_WithExpiredDepthNode
 * @tc.desc: test CleanExpiredNodes remove entries with expired depth node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, CleanExpiredNodes_WithExpiredDepthNode, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(3500, context_);
    auto effectNode = CreateSpatialEffectNode(3501);
    depthNode->AddChild(effectNode);
    instance->RegisterSpatialEffect(effectNode);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);

    depthNode.reset();
    instance->CleanExpiredNodes();
    EXPECT_TRUE(instance->depthSpatialEffectNodeMap_.empty());
}

/**
 * @tc.name: CleanExpiredNodes_WithExpiredSpatialEffectNode
 * @tc.desc: test CleanExpiredNodes remove spatial effect node with expired lock
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, CleanExpiredNodes_WithExpiredSpatialEffectNode, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(3600, context_);
    {
        auto effectNode = CreateSpatialEffectNode(3601);
        depthNode->AddChild(effectNode);
        instance->RegisterSpatialEffect(effectNode);
    }
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);

    instance->CleanExpiredNodes();
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_[3600].second.size(), 0);
}

/**
 * @tc.name: CleanExpiredNodes_WithLostSpatialEffectProperty
 * @tc.desc: test CleanExpiredNodes clean nodes without spatial effect property
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, CleanExpiredNodes_WithLostSpatialEffectProperty, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(3700, context_);
    auto effectNode = std::make_shared<RSRenderNode>(3701);
    depthNode->AddChild(effectNode);
    effectNode->renderProperties_.GetEffect().spatialEffectVariantPara_.reset();
    instance->RegisterSpatialEffect(effectNode);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);

    instance->CleanExpiredNodes();
    EXPECT_TRUE(instance->depthSpatialEffectNodeMap_[3700].second.empty());
}

/**
 * @tc.name: ResourceNodeInfectsDepthNodeDirty_WithDirtyResource
 * @tc.desc: test ResourceNodeInfectsDepthNodeDirty marks parent dirty when resource is dirty
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ResourceNodeInfectsDepthNodeDirty_WithDirtyResource, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(3800, context_);
    auto resourceNode = std::make_shared<RSRenderNode>(3801);
    depthNode->AddChild(resourceNode);
    resourceNode->SetDirty();
    instance->RegisterDepthResource(resourceNode);
    EXPECT_EQ(instance->depthResourceNodeMap_.size(), 1);

    instance->ResourceNodeInfectsDepthNodeDirty(instance->depthResourceNodeMap_);
    EXPECT_TRUE(depthNode->IsDirty());
}

/**
 * @tc.name: ResourceNodeInfectsDepthNodeDirty_WithCleanResource
 * @tc.desc: test ResourceNodeInfectsDepthNodeDirty does nothing with clean resource
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ResourceNodeInfectsDepthNodeDirty_WithCleanResource, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(3900, context_);
    auto resourceNode = std::make_shared<RSRenderNode>(3901);
    depthNode->AddChild(resourceNode);
    instance->RegisterDepthResource(resourceNode);

    bool wasDirty = depthNode->IsDirty();
    instance->ResourceNodeInfectsDepthNodeDirty(instance->depthResourceNodeMap_);
    EXPECT_EQ(depthNode->IsDirty(), wasDirty);
}

/**
 * @tc.name: BuildDepthDepthResourceNodeMap001
 * @tc.desc: test BuildDepthDepthResourceNodeMap
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, BuildDepthDepthResourceNodeMap001, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    instance->depthResourceNodeMap_.emplace(1, std::weak_ptr<RSRenderNode>());
    instance->BuildDepthDepthResourceNodeMap();
    EXPECT_TRUE(instance->depthDepthResourceNodeMap_.empty());

    instance->depthResourceNodeMap_.clear();
    NodeId depthNodeId = 1;
    auto depthNode = std::make_shared<RSRenderNode>(depthNodeId);
    NodeId depthResourceNodeId = 2;
    auto depthResourceNode = std::make_shared<RSRenderNode>(depthResourceNodeId);
    instance->depthResourceNodeMap_.emplace(depthResourceNodeId, depthResourceNode->weak_from_this());
    instance->BuildDepthDepthResourceNodeMap();
    EXPECT_TRUE(instance->depthDepthResourceNodeMap_.empty());

    depthNode->AddChild(depthResourceNode);
    instance->BuildDepthDepthResourceNodeMap();
    EXPECT_EQ(instance->depthDepthResourceNodeMap_.size(), 1);
}

/**
 * @tc.name: IsMasterGlobalDepthNodeAndUpdate_WithNullNode
 * @tc.desc: test IsMasterGlobalDepthNodeAndUpdate with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, IsMasterGlobalDepthNodeAndUpdate_WithNullNode, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    EXPECT_FALSE(instance->IsMasterGlobalDepthNodeAndUpdate(nullptr));
}

/**
 * @tc.name: IsMasterGlobalDepthNodeAndUpdate_WithNonInstanceType
 * @tc.desc: test IsMasterGlobalDepthNodeAndUpdate returns false for non-INSTANCE type
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, IsMasterGlobalDepthNodeAndUpdate_WithNonInstanceType, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(4000, context_);
    depthNode->SetDepthSpaceType(DepthSpaceType::GLOBAL);

    EXPECT_FALSE(instance->IsMasterGlobalDepthNodeAndUpdate(depthNode));
}

/**
 * @tc.name: IsMasterGlobalDepthNodeAndUpdate_WithoutInstanceRoot
 * @tc.desc: test IsMasterGlobalDepthNodeAndUpdate with null instance root
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, IsMasterGlobalDepthNodeAndUpdate_WithoutInstanceRoot, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(4100, context_);
    depthNode->SetDepthSpaceType(DepthSpaceType::INSTANCE);

    EXPECT_FALSE(instance->IsMasterGlobalDepthNodeAndUpdate(depthNode));
}

/**
 * @tc.name: IsMasterGlobalDepthNodeAndUpdate_WithInstanceRootNotSurface
 * @tc.desc: test IsMasterGlobalDepthNodeAndUpdate with instance root not surface
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, IsMasterGlobalDepthNodeAndUpdate_WithInstanceRootNotSurface, TestSize.Level1)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();

    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(4300, context->weak_from_this());
    depthNode->SetDepthSpaceType(DepthSpaceType::INSTANCE);

    auto nodeId = 114;
    auto renderNode = std::make_shared<RSRenderNode>(nodeId, context->weak_from_this());
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);
    depthNode->instanceRootNodeId_ = nodeId;

    EXPECT_FALSE(instance->IsMasterGlobalDepthNodeAndUpdate(depthNode));
}

/**
 * @tc.name: IsMasterGlobalDepthNodeAndUpdate_WithNonWallpaperType
 * @tc.desc: test IsMasterGlobalDepthNodeAndUpdate with non-wallpaper surface
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, IsMasterGlobalDepthNodeAndUpdate_WithNonWallpaperType, TestSize.Level1)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();

    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(4300, context->weak_from_this());
    depthNode->SetDepthSpaceType(DepthSpaceType::INSTANCE);

    auto surfaceNodeId = 114;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId, context->weak_from_this());
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::DEFAULT_WINDOW;
    context->GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    depthNode->instanceRootNodeId_ = surfaceNodeId;

    EXPECT_FALSE(instance->IsMasterGlobalDepthNodeAndUpdate(depthNode));
}

/**
 * @tc.name: IsMasterGlobalDepthNodeAndUpdate_Success
 * @tc.desc: test IsMasterGlobalDepthNodeAndUpdate success
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, IsMasterGlobalDepthNodeAndUpdate_Success, TestSize.Level1)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();

    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(4300, context->weak_from_this());
    depthNode->SetDepthSpaceType(DepthSpaceType::INSTANCE);

    auto surfaceNodeId = 114;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId, context->weak_from_this());
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SCB_WALLPAPER;
    context->GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    depthNode->instanceRootNodeId_ = surfaceNodeId;

    EXPECT_TRUE(instance->IsMasterGlobalDepthNodeAndUpdate(depthNode));
}

/**
 * @tc.name: SetSpatialEffectNodeDirty_WithValidNodes
 * @tc.desc: test SetSpatialEffectNodeDirty marks all spatial effect nodes dirty
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, SetSpatialEffectNodeDirty_WithValidNodes, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(5300, context_);
    auto effectNode1 = CreateSpatialEffectNode(5301);
    auto effectNode2 = CreateSpatialEffectNode(5302);
    depthNode->AddChild(effectNode1);
    depthNode->AddChild(effectNode2);

    instance->spatialEffectDepthNodeMap_.clear();
    instance->depthSpatialEffectNodeMap_.clear();

    RSSpatialEffectManager::NodeMap spatialEffectNodeMap;
    spatialEffectNodeMap[5301] = effectNode1->weak_from_this();
    spatialEffectNodeMap[5302] = effectNode2->weak_from_this();

    instance->SetSpatialEffectNodeDirty(spatialEffectNodeMap);
    EXPECT_TRUE(effectNode1->IsDirty());
    EXPECT_TRUE(effectNode2->IsDirty());
}

/**
 * @tc.name: ProcessDepthNodeAndSpatialEffectNodeDirty_WithMasterGlobalDirty
 * @tc.desc: test ProcessDepthNodeAndSpatialEffectNodeDirty with master global dirty node
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectManagerTest, ProcessDepthNodeAndSpatialEffectNodeDirty_WithMasterGlobalDirty, TestSize.Level1)
{
    auto instance = RSSpatialEffectManager::Instance();
    auto depthNode = std::make_shared<RSDepthRenderNode>(5400, context_);
    depthNode->SetDepthSpaceType(DepthSpaceType::INSTANCE);
    depthNode->SetDirty();
    instance->RegisterDepthSpace(depthNode);
    EXPECT_EQ(instance->depthSpatialEffectNodeMap_.size(), 1);

    auto effectNode = CreateSpatialEffectNode(5401);
    depthNode->AddChild(effectNode);
    instance->RegisterSpatialEffect(effectNode);
    EXPECT_EQ(instance->spatialEffectDepthNodeMap_.size(), 1);

    instance->ProcessDepthNodeAndSpatialEffectNodeDirty();
    EXPECT_TRUE(effectNode->IsDirty());
}

} // namespace OHOS::Rosen
