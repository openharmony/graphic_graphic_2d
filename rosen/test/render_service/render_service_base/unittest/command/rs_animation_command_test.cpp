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

#include "gtest/gtest.h"
#include "include/command/rs_animation_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void TestProcessor(NodeId, AnimationId, AnimationCallbackEvent);
};

void RSAnimationCommandTest::SetUpTestCase() {}
void RSAnimationCommandTest::TearDownTestCase() {}
void RSAnimationCommandTest::SetUp() {}
void RSAnimationCommandTest::TearDown() {}
void RSAnimationCommandTest::TestProcessor(NodeId nodeId, AnimationId animId, AnimationCallbackEvent event) {}

/**
 * @tc.name: TestRSAnimationCommand001
 * @tc.desc: AnimationFinishCallback test.
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, TestRSAnimationCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimationCommandHelper::SetAnimationCallbackProcessor(TestProcessor);
    AnimationCommandHelper::AnimationCallback(context, targetId, animId, event);
}

/**
 * @tc.name: CreateParticleAnimation001
 * @tc.desc: CreateParticleAnimation test.
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateParticleAnimation001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    std::shared_ptr<RSRenderParticleAnimation> animation = nullptr;
    AnimationCommandHelper::CreateParticleAnimation(context, targetId, animation);

    NodeId id = static_cast<NodeId>(1);
    RSContext context2;

    std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSBaseRenderNode>(id);
    context2.GetMutableNodeMap().RegisterRenderNode(node);
    AnimationCommandHelper::CreateParticleAnimation(context2, id, nullptr);

    std::shared_ptr<RSRenderParticleAnimation> animation2 = std::make_shared<RSRenderParticleAnimation>();
    AnimationCommandHelper::CreateParticleAnimation(context2, id, animation2);

    AnimationCommandHelper::CancelAnimation(context2, id, 0);
}

/**
 * @tc.name: AnimationCallback001
 * @tc.desc: test results of AnimationCallback
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSAnimationCommandTest, AnimationCallback001, TestSize.Level1)
{
    AnimationCommandHelper::AnimationCallbackProcessor processor = [](NodeId nodeId, AnimationId animId,
                                                                       AnimationCallbackEvent event) {};
    AnimationCommandHelper::SetAnimationCallbackProcessor(processor);
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimationCommandHelper::AnimationCallback(context, targetId, animId, event);
    EXPECT_TRUE(targetId == -1);
}

/**
 * @tc.name: CreateAnimation001
 * @tc.desc: test results of CreateAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSAnimationCommandTest, CreateAnimation001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 1;
    std::shared_ptr<RSRenderAnimation> animation = nullptr;
    AnimationCommandHelper::CreateAnimation(context, targetId, animation);
    EXPECT_TRUE(targetId);

    targetId = 0;
    AnimationCommandHelper::CreateAnimation(context, targetId, animation);
    EXPECT_TRUE(targetId == 0);

    animation = std::make_shared<RSRenderAnimation>();
    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    PropertyId id = 0;
    auto property = std::shared_ptr<RSRenderProperty<Drawing::Matrix>>();
    auto modifier = std::make_shared<RSGeometryTransRenderModifier>(property);
    node->modifiers_[id] = modifier;
    AnimationCommandHelper::CreateAnimation(context, targetId, animation);
    EXPECT_TRUE(modifier != nullptr);
}

/**
 * @tc.name: CreateParticleAnimation002
 * @tc.desc: test results of CreateParticleAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSAnimationCommandTest, CreateParticleAnimation002, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 1;
    std::shared_ptr<RSRenderParticleAnimation> animation = nullptr;
    AnimationCommandHelper::CreateParticleAnimation(context, targetId, animation);
    EXPECT_TRUE(targetId);

    targetId = 0;
    AnimationCommandHelper::CreateParticleAnimation(context, targetId, animation);
    EXPECT_TRUE(targetId == 0);

    animation = std::make_shared<RSRenderParticleAnimation>();
    AnimationCommandHelper::CreateParticleAnimation(context, targetId, animation);
    EXPECT_TRUE(animation != nullptr);
}

/**
 * @tc.name: CancelAnimation001
 * @tc.desc: test results of CancelAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSAnimationCommandTest, CancelAnimation001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 1;
    PropertyId propertyId = 1;
    AnimationCommandHelper::CancelAnimation(context, targetId, propertyId);
    EXPECT_TRUE(targetId);

    targetId = 0;
    AnimationCommandHelper::CancelAnimation(context, targetId, propertyId);
    EXPECT_TRUE(targetId == 0);
}
} // namespace OHOS::Rosen
