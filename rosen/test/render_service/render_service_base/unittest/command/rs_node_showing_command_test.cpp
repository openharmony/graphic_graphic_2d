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
class RSNodeGetShowingPropertyAndCancelAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeGetShowingPropertyAndCancelAnimationTest::SetUpTestCase() {}
void RSNodeGetShowingPropertyAndCancelAnimationTest::TearDownTestCase() {}
void RSNodeGetShowingPropertyAndCancelAnimationTest::SetUp() {}
void RSNodeGetShowingPropertyAndCancelAnimationTest::TearDown() {}

class RSNodeGetShowingPropertiesAndCancelAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeGetShowingPropertiesAndCancelAnimationTest::SetUpTestCase() {}
void RSNodeGetShowingPropertiesAndCancelAnimationTest::TearDownTestCase() {}
void RSNodeGetShowingPropertiesAndCancelAnimationTest::SetUp() {}
void RSNodeGetShowingPropertiesAndCancelAnimationTest::TearDown() {}

class RSNodeGetAnimationsValueFractionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeGetAnimationsValueFractionTest::SetUpTestCase() {}
void RSNodeGetAnimationsValueFractionTest::TearDownTestCase() {}
void RSNodeGetAnimationsValueFractionTest::SetUp() {}
void RSNodeGetAnimationsValueFractionTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationTest, Marshalling001, TestSize.Level1)
{
    NodeId targetId = 0;
    Parcel parcel;
    auto property = std::make_shared<RSRenderProperty<bool>>();
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_TRUE(animation.Marshalling(parcel) == false);
#endif
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationTest, Unmarshalling001, TestSize.Level1)
{
    NodeId targetId = 0;
    Parcel parcel;
    auto property = std::make_shared<RSRenderProperty<bool>>();
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    EXPECT_TRUE(animation.Unmarshalling(parcel) == nullptr);
}

/**
 * @tc.name: CheckHeader001
 * @tc.desc: test results of CheckHeader
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationTest, CheckHeader001, TestSize.Level1)
{
    NodeId targetId = 0;
    Parcel parcel;
    auto property = std::make_shared<RSRenderProperty<bool>>();
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    EXPECT_TRUE(animation.CheckHeader(parcel) == false);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc: test results of ReadFromParcel
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationTest, ReadFromParcel001, TestSize.Level1)
{
    NodeId targetId = 0;
    Parcel parcel;
    auto property = std::make_shared<RSRenderProperty<bool>>();
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    EXPECT_TRUE(animation.ReadFromParcel(parcel) == false);
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationTest, Marshalling001, TestSize.Level1)
{
    uint64_t timeoutNS = 0;
    Parcel parcel;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    EXPECT_TRUE(animation.Marshalling(parcel));
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationTest, Unmarshalling001, TestSize.Level1)
{
    uint64_t timeoutNS = 0;
    Parcel parcel;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    EXPECT_TRUE(animation.Unmarshalling(parcel) == nullptr);
}

/**
 * @tc.name: CheckHeader001
 * @tc.desc: test results of CheckHeader
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationTest, CheckHeader001, TestSize.Level1)
{
    uint64_t timeoutNS = 0;
    Parcel parcel;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    EXPECT_TRUE(animation.CheckHeader(parcel) == false);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc: test results of ReadFromParcel
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationTest, ReadFromParcel001, TestSize.Level1)
{
    uint64_t timeoutNS = 0;
    Parcel parcel;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    EXPECT_TRUE(animation.ReadFromParcel(parcel) == false);
}

/**
 * @tc.name: Process001
 * @tc.desc: test results of Process
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationTest, Process001, TestSize.Level1)
{
    RSContext context;
    uint64_t timeoutNS = 0;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    NodeId nodeId = 0;
    PropertyId propertyId = 0;
    auto renderProperty = std::make_shared<RSRenderProperty<bool>>();
    std::vector<AnimationId> animationIds = { 0 };
    std::pair<std::pair<NodeId, PropertyId>, std::pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>>
        newEntry(std::make_pair(nodeId, propertyId), std::make_pair(renderProperty, animationIds));
    animation.propertiesMap_.insert(newEntry);
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
 * @tc.name: Process003
 * @tc.desc: test results of Process
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationTest, Process003, TestSize.Level1)
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
 * @tc.name: IsCallingPidValid001
 * @tc.desc: test results of IsCallingPidValid
 * @tc.type: FUNC
 * @tc.require: issueIBAUG4
 */
HWTEST_F(RSNodeGetShowingPropertiesAndCancelAnimationTest, IsCallingPidValid001, TestSize.Level1)
{
    RSContext context;
    uint64_t timeoutNS = 0;
    RSNodeGetShowingPropertiesAndCancelAnimation animation(timeoutNS);
    NodeId nodeId = 0;
    PropertyId propertyId = 0;
    auto renderProperty = std::make_shared<RSRenderProperty<bool>>();
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
 * @tc.name: IsCallingPidValid001
 * @tc.desc: test results of IsCallingPidValid
 * @tc.type: FUNC
 * @tc.require: issueIBAUG4
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationTest, IsCallingPidValid001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 0;
    auto property = std::make_shared<RSRenderProperty<bool>>();
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
 * @tc.name: IsCallingPidValid001
 * @tc.desc: test results of IsCallingPidValid
 * @tc.type: FUNC
 * @tc.require: issueIBAUG4
 */
HWTEST_F(RSNodeGetAnimationsValueFractionTest, IsCallingPidValid001, TestSize.Level1)
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
