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
    auto property = std::make_shared<RSRenderPropertyBase>();
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    EXPECT_TRUE(animation.Marshalling(parcel) == false);
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
    auto property = std::make_shared<RSRenderPropertyBase>();
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
    auto property = std::make_shared<RSRenderPropertyBase>();
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
    auto property = std::make_shared<RSRenderPropertyBase>();
    RSNodeGetShowingPropertyAndCancelAnimation animation(targetId, property);
    EXPECT_TRUE(animation.ReadFromParcel(parcel) == false);
}

/**
 * @tc.name: Process001
 * @tc.desc: test results of Process
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeGetShowingPropertyAndCancelAnimationTest, Process001, TestSize.Level1)
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
} // namespace OHOS::Rosen
