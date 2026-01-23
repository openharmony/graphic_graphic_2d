/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
    static void TestProcessor(NodeId, AnimationId, uint64_t, AnimationCallbackEvent);
protected:
    static inline NodeId nodeId_;
    static inline AnimationId animationId_;
    static inline uint64_t token_;
};

void RSAnimationCommandTest::SetUpTestCase() {}
void RSAnimationCommandTest::TearDownTestCase() {}
void RSAnimationCommandTest::SetUp() {}
void RSAnimationCommandTest::TearDown() {}
void RSAnimationCommandTest::TestProcessor(
    NodeId nodeId, AnimationId animId, uint64_t token, AnimationCallbackEvent event)
{
    nodeId_ = nodeId;
    animationId_ = animId;
    token_ = token;
}

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
    uint64_t token = 1;
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimationCommandHelper::SetAnimationCallbackProcessor(TestProcessor);
    AnimationCommandHelper::AnimationCallback(context, targetId, animId, token, event);
    ASSERT_EQ(nodeId_, targetId);
    ASSERT_EQ(animationId_, animId);
    ASSERT_EQ(token_, token);
}

/**
 * @tc.name: AnimationCallback001
 * @tc.desc: test results of AnimationCallback
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSAnimationCommandTest, AnimationCallback001, TestSize.Level1)
{
    AnimationCommandHelper::AnimationCallbackProcessor processor = [](
        NodeId nodeId, AnimationId animId, uint64_t token, AnimationCallbackEvent event) {};
    AnimationCommandHelper::SetAnimationCallbackProcessor(processor);
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    uint64_t token = 1;
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimationCommandHelper::AnimationCallback(context, targetId, animId, token, event);
    EXPECT_TRUE(targetId == -1);
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
    EXPECT_FALSE(targetId);
}

/**
 * @tc.name: InteractiveAnimator001
 * @tc.desc: InteractiveAnimator test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAnimationCommandTest, InteractiveAnimator001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 0;
    InteractiveImplictAnimatorId targetIdI = 0;
    float fraction = 1.f;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) == nullptr);
    AnimationCommandHelper::DestoryInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::InteractiveAnimatorAddAnimations(context, targetId, animations);
    AnimationCommandHelper::PauseInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::ContinueInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::FinishInteractiveAnimator(context, targetIdI, RSInteractiveAnimationPosition::CURRENT);
    AnimationCommandHelper::ReverseInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::SetFractionInteractiveAnimator(context, targetIdI, fraction);
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, false);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) != nullptr);

    AnimationCommandHelper::InteractiveAnimatorAddAnimations(context, targetId, animations);
    AnimationCommandHelper::PauseInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::ContinueInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::FinishInteractiveAnimator(context, targetIdI, RSInteractiveAnimationPosition::CURRENT);
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, true);
    AnimationCommandHelper::ReverseInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::SetFractionInteractiveAnimator(context, targetIdI, fraction);
    AnimationCommandHelper::DestoryInteractiveAnimator(context, targetIdI);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) == nullptr);
}
} // namespace OHOS::Rosen
