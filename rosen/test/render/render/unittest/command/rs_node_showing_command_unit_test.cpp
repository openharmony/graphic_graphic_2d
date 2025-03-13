/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "command/rs_node_showing_command.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSNodeGetShowingPropertyAndCancelAnimationUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeGetShowingPropertyAndCancelAnimationUnitTest::SetUpTestCase() {}
void RSNodeGetShowingPropertyAndCancelAnimationUnitTest::SetUpTestCase() {}
void RSNodeGetShowingPropertyAndCancelAnimationUnitTest::TearDownTestCase() {}
void RSNodeGetShowingPropertyAndCancelAnimationUnitTest::SetUp() {}
void RSNodeGetShowingPropertyAndCancelAnimationUnitTest::TearDown() {}

class RSNodeGetShowingPropertiesAndCancelAnimationUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeGetShowingPropertiesAndCancelAnimationUnitTest::SetUpTestCase() {}
void RSNodeGetShowingPropertiesAndCancelAnimationUnitTest::TearDownTestCase() {}
void RSNodeGetShowingPropertiesAndCancelAnimationUnitTest::SetUp() {}
void RSNodeGetShowingPropertiesAndCancelAnimationUnitTest::TearDown() {}

class RSNodeGetAnimationsValueFractionUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeGetAnimationsValueFractionUnitTest::SetUpTestCase() {}
void RSNodeGetAnimationsValueFractionUnitTest::TearDownTestCase() {}
void RSNodeGetAnimationsValueFractionUnitTest::SetUp() {}
void RSNodeGetAnimationsValueFractionUnitTest::TearDown() {}

/**
 * @tc.name: TestMarshalling01
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationUnitTest, TestMarshalling01, TestSize.Level1)
{
    NodeId targetId = 0;
    Parcel parcel;
    auto property = std::make_shared<RSRenderPropertyBase>();
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    EXPECT_TRUE(animation.Marshalling(parcel) == false);
}

/**
 * @tc.name: TestUnmarshalling01
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationUnitTest, TestUnmarshalling01, TestSize.Level1)
{
    NodeId targetId = 0;
    Parcel parcel;
    auto property = std::make_shared<RSRenderPropertyBase>();
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    EXPECT_TRUE(animation.Unmarshalling(parcel) == nullptr);
}

/**
 * @tc.name: TestCheckHeader01
 * @tc.desc: test results of CheckHeader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationUnitTest, TestCheckHeader01, TestSize.Level1)
{
    NodeId targetId = 0;
    Parcel parcel;
    auto property = std::make_shared<RSRenderPropertyBase>();
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    EXPECT_TRUE(animation.CheckHeader(parcel) == false);
}

/**
 * @tc.name: TestReadFromParcel01
 * @tc.desc: test results of ReadFromParcel
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationUnitTest, TestReadFromParcel01, TestSize.Level1)
{
    NodeId targetId = 0;
    Parcel parcel;
    auto property = std::make_shared<RSRenderPropertyBase>();
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    EXPECT_TRUE(animation.ReadFromParcel(parcel) == false);
}

/**
 * @tc.name: TestProcess01
 * @tc.desc: test results of Process
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationUnitTest, TestProcess01, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 0;
    PropertyId id = 0;
    auto property = std::make_shared<RSRenderPropertyBase>(id);
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    animation.Process(context);
    EXPECT_TRUE(property != nullptr);

    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    auto renderProperty = std::shared_ptr<RSRenderProperty<Drawing::Matrix>>();
    auto modifier = std::make_shared<RSGeometryTransRenderModifier>(renderProperty);
    node->modifiers_[id] = modifier;
    animation.Process(context);
    EXPECT_TRUE(node->modifiers_.empty() != true);

    animation.targetId_ = 1;
    animation.Process(context);
    EXPECT_TRUE(property != nullptr);
}

/**
 * @tc.name: TestMarshalling01
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationUnitTest, TestMarshalling01, TestSize.Level1)
{
    uint64_t timeoutNS = 0;
    Parcel parcel;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    EXPECT_TRUE(animation.Marshalling(parcel));
}

/**
 * @tc.name: TestUnmarshalling01
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationUnitTest, TestUnmarshalling01, TestSize.Level1)
{
    uint64_t timeoutNS = 0;
    Parcel parcel;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    EXPECT_TRUE(animation.Unmarshalling(parcel) == nullptr);
}

/**
 * @tc.name: TestCheckHeader01
 * @tc.desc: test results of CheckHeader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationUnitTest, TestCheckHeader01, TestSize.Level1)
{
    uint64_t timeoutNS = 0;
    Parcel parcel;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    EXPECT_TRUE(animation.CheckHeader(parcel) == false);
}

/**
 * @tc.name: TestReadFromParcel01
 * @tc.desc: test results of ReadFromParcel
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationUnitTest, TestReadFromParcel01, TestSize.Level1)
{
    uint64_t timeoutNS = 0;
    Parcel parcel;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    EXPECT_TRUE(animation.ReadFromParcel(parcel) == false);
}

/**
 * @tc.name: TestProcess01
 * @tc.desc: test results of Process
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationUnitTest, TestProcess01, TestSize.Level1)
{
    RSContext context;
    uint64_t timeoutNS = 0;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    NodeId nodeId = 0;
    PropertyId propertyId = 0;
    auto renderProperty = std::make_shared<RSRenderPropertyBase>();
    std::vector<AnimationId> animationIds = { 0 };
    std::pair<std::pair<NodeId, PropertyId>, std::pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>>
        newEntry(std::make_pair(nodeId, propertyId), std::make_pair(renderProperty, animationIds));
    animation.propertiesMap_.insert(newEntry);
    animation.Process(context);
    EXPECT_TRUE(renderProperty != nullptr);

    NodeId targetId = 0;
    PropertyId id = 0;
    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    auto property = std::shared_ptr<RSRenderProperty<Drawing::Matrix>>();
    auto modifier = std::make_shared<RSGeometryTransRenderModifier>(property);
    node->modifiers_[id] = modifier;
    animation.Process(context);
    EXPECT_TRUE(renderProperty != nullptr);

    nodeId = 1;
    propertyId = 1;
    std::pair<std::pair<NodeId, PropertyId>, std::pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>>
        entry(std::make_pair(nodeId, propertyId), std::make_pair(renderProperty, animationIds));
    animation.propertiesMap_.insert(entry);
    animation.Process(context);
    EXPECT_TRUE(renderProperty != nullptr);
}

/**
 * @tc.name: TestProcess02
 * @tc.desc: test results of Process
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationUnitTest, TestProcess02, TestSize.Level1)
{
    uint64_t timeoutNS = 0;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    RSContext context;
    context.nodeMap.renderNodeMap_.clear();

    std::shared_ptr<RSBaseRenderNode> renderNodeTest1 = std::make_shared<RSBaseRenderNode>(0);
    EXPECT_NE(renderNodeTest1, nullptr);
    renderNodeTest1->modifiers_[0] = nullptr;
    context.nodeMap.renderNodeMap_[ExtractPid(0)][0] = renderNodeTest1;

    std::shared_ptr<RSBaseRenderNode> renderNodeTest2 = std::make_shared<RSBaseRenderNode>(1);
    EXPECT_NE(renderNodeTest2, nullptr);
    auto propertyTest2 = std::make_shared<RSRenderProperty<Drawing::Matrix>>();
    EXPECT_NE(propertyTest2, nullptr);
    auto modifierTest2 = std::make_shared<RSGeometryTransRenderModifier>(propertyTest2);
    EXPECT_NE(modifierTest2, nullptr);
    modifierTest2->property_ = nullptr;
    renderNodeTest2->modifiers_[1] = modifierTest2;
    context.nodeMap.renderNodeMap_[ExtractPid(1)][1] = renderNodeTest2;

    std::shared_ptr<RSBaseRenderNode> renderNodeTest3 = std::make_shared<RSBaseRenderNode>(2);
    EXPECT_NE(renderNodeTest3, nullptr);
    auto propertyTest3 = std::make_shared<RSRenderProperty<Drawing::Matrix>>();
    EXPECT_NE(propertyTest3, nullptr);
    auto modifierTest3 = std::make_shared<RSGeometryTransRenderModifier>(propertyTest3);
    EXPECT_NE(modifierTest3, nullptr);
    renderNodeTest3->modifiers_[2] = modifierTest3;
    context.nodeMap.renderNodeMap_[ExtractPid(2)][2] = renderNodeTest3;

    auto renderPropertyTest1 = std::make_shared<RSRenderPropertyBase>();
    EXPECT_NE(renderPropertyTest1, nullptr);
    auto renderPropertyTest2 = std::make_shared<RSRenderPropertyBase>();
    EXPECT_NE(renderPropertyTest2, nullptr);
    auto renderPropertyTest3 = std::make_shared<RSRenderPropertyBase>();
    EXPECT_NE(renderPropertyTest3, nullptr);
    auto renderPropertyTest4 = std::make_shared<RSRenderPropertyBase>();
    EXPECT_NE(renderPropertyTest4, nullptr);
    std::vector<AnimationId> animationIdsTest1 = { 0 };
    std::vector<AnimationId> animationIdsTest2 = { 0 };
    std::vector<AnimationId> animationIdsTest3 = { 0 };
    std::vector<AnimationId> animationIdsTest4 = { 0 };
    animation.propertiesMap_.emplace(std::make_pair(0, 0), std::make_pair(renderPropertyTest1, animationIdsTest1));
    animation.propertiesMap_.emplace(std::make_pair(1, 1), std::make_pair(renderPropertyTest1, animationIdsTest1));
    animation.propertiesMap_.emplace(std::make_pair(2, 2), std::make_pair(renderPropertyTest1, animationIdsTest1));
    animation.propertiesMap_.emplace(std::make_pair(3, 3), std::make_pair(renderPropertyTest1, animationIdsTest1));
    animation.Process(context);
}

/**
 * @tc.name: TestProcess03
 * @tc.desc: test results of Process
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationUnitTest, TestProcess03, TestSize.Level1)
{
    RSContext context;
    RSNodeGetAnimationsValueFraction animation(0);
    animation.nodeId_ = 0;
    animation.animationId_ = 0;
    animation.Process(context);

    std::shared_ptr<RSBaseRenderNode> renderNode = std::make_shared<RSBaseRenderNode>(0);
    EXPECT_NE(renderNode, nullptr);
    renderNode->animationManager_.animations_.clear();
    std::shared_ptr<RSRenderAnimation> animationTest = std::make_shared<RSRenderAnimation>(0);
    EXPECT_NE(animationTest, nullptr);
    renderNode->animationManager_.animations_.emplace(0, animationTest);
    context.nodeMap.renderNodeMap_[0][0] = renderNode;
    animation.Process(context);

    animation.nodeId_ = 1;
    animation.Process(context);
}

/**
 * @tc.name: TestIsCallingPidValid01
 * @tc.desc: test results of IsCallingPidValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationUnitTest, TestIsCallingPidValid01, TestSize.Level1)
{
    RSContext context;
    uint64_t timeoutNS = 0;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    NodeId nodeId = 0;
    PropertyId propertyId = 0;
    auto renderProperty = std::make_shared<RSRenderPropertyBase>();
    std::vector<AnimationId> animationIds = { 0 };
    std::pair<std::pair<NodeId, PropertyId>, std::pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>>
        newEntry(std::make_pair(nodeId, propertyId), std::make_pair(renderProperty, animationIds));
    animation.propertiesMap_.insert(newEntry);
    EXPECT_TRUE(animation.IsCallingPidValid(0, context.GetNodeMap()));
    EXPECT_FALSE(animation.IsCallingPidValid(1, context.GetNodeMap()));

    pid_t pid = 1;
    nodeId = (static_cast<NodeId>(pid) << 32) | 1;
    propertyId = 1;
    std::pair<std::pair<NodeId, PropertyId>, std::pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>>
        entry(std::make_pair(nodeId, propertyId), std::make_pair(renderProperty, animationIds));
    animation.propertiesMap_.insert(entry);
    animation.Process(context);
    EXPECT_FALSE(animation.IsCallingPidValid(0, context.GetNodeMap()));
    EXPECT_FALSE(animation.IsCallingPidValid(1, context.GetNodeMap()));

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceNode->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
    context.nodeMap.AddUIExtensionSurfaceNode(surfaceNode);
    EXPECT_FALSE(animation.IsCallingPidValid(0, context.GetNodeMap()));
    EXPECT_TRUE(animation.IsCallingPidValid(1, context.GetNodeMap()));
}

/**
 * @tc.name: TestIsCallingPidValid01
 * @tc.desc: test results of IsCallingPidValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationUnitTest, TestIsCallingPidValid01, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 0;
    PropertyId id = 0;
    auto property = std::make_shared<RSRenderPropertyBase>(id);
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    animation.Process(context);
    EXPECT_TRUE(animation.IsCallingPidValid(0, context.GetNodeMap()));
    EXPECT_FALSE(animation.IsCallingPidValid(1, context.GetNodeMap()));
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceNode->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
    context.nodeMap.AddUIExtensionSurfaceNode(surfaceNode);
    EXPECT_TRUE(animation.IsCallingPidValid(1, context.GetNodeMap()));
}

/**
 * @tc.name: TestIsCallingPidValid01
 * @tc.desc: test results of IsCallingPidValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeGetAnimationsValueFractionUnitTest, TestIsCallingPidValid01, TestSize.Level1)
{
    RSContext context;
    RSNodeGetAnimationsValueFraction animation(0);
    animation.nodeId_ = 0;
    animation.animationId_ = 0;
    EXPECT_TRUE(animation.IsCallingPidValid(0, context.GetNodeMap()));
    EXPECT_FALSE(animation.IsCallingPidValid(1, context.GetNodeMap()));
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceNode->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
    context.nodeMap.AddUIExtensionSurfaceNode(surfaceNode);
    EXPECT_TRUE(animation.IsCallingPidValid(1, context.GetNodeMap()));
}
} // namespace OHOS::Rosen
