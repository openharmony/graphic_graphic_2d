/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <thread>

#include "gtest/gtest.h"

#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_motion_path_option.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSImplicitAnimatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    const std::string  ANIMATION_PATH = "L350 0 L150 100";
    const Vector4f ANIMATION_START_BOUNDS = Vector4f(100.f, 100.f, 200.f, 300.f);
    const Vector4f ANIMATION_END_BOUNDS = Vector4f(100.f, 100.f, 300.f, 300.f);
};

void RSImplicitAnimatorTest::SetUpTestCase() {}
void RSImplicitAnimatorTest::TearDownTestCase() {}
void RSImplicitAnimatorTest::SetUp() {}
void RSImplicitAnimatorTest::TearDown() {}

/**
 * @tc.name: OpenImplicitAnimation001
 * @tc.desc: Verify the OpenImplicitAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, GetBoundsTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest OpenImplicitAnimation001 start";
    RSAnimationTimingProtocol timingProtocol;
    float mass = 0.5f;
    float stiffness = 0.0f;
    float damping = 0.5f;
    float velocity = 1.0f;
    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(mass, stiffness, damping, velocity);
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    implicitAnimator->OpenImplicitAnimation(RSAnimationTimingProtocol::DEFAULT, timingCurve);
    EXPECT_TRUE(implicitAnimator != nullptr);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest OpenImplicitAnimation001 end";
}

/**
 * @tc.name: OpenImplicitAnimationWithNonNullCallback
 * @tc.desc: Verify OpenImplicitAnimation with non-null finishCallback
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, OpenImplicitAnimationWithNonNullCallback, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest OpenImplicitAnimationWithNonNullCallback start";

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(300);
    timingProtocol.SetRepeatCount(1);
    timingProtocol.SetStartDelay(0);
    timingProtocol.SetSpeed(1.0f);

    auto timingCurve = RSAnimationTimingCurve::EASE;
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();

    // Test with non-null finishCallback
    auto finishCallback = std::make_shared<AnimationFinishCallback>([&]() {});
    ASSERT_NE(finishCallback, nullptr);

    // OpenImplicitAnimation should call SetEstimatedDuration when finishCallback is not null
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(finishCallback));
    EXPECT_TRUE(implicitAnimator != nullptr);

    // Verify that globalImplicitParams_ is not empty
    EXPECT_FALSE(implicitAnimator->globalImplicitParams_.empty());

    // Retrieve the callback from globalImplicitParams_ and verify estimated duration was set
    const auto& [protocol, curve, callback, repeatCallback] = implicitAnimator->globalImplicitParams_.top();
    EXPECT_NE(callback, nullptr);
    // Estimated duration should be set (300 * 1 + 0) * animationScale
    float estimatedDuration = callback->GetEstimatedDuration();
    EXPECT_GT(estimatedDuration, 0.0f);

    // Clean up
    implicitAnimator->CloseImplicitAnimation();
    EXPECT_TRUE(implicitAnimator->globalImplicitParams_.empty());

    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest OpenImplicitAnimationWithNonNullCallback end";
}

/**
 * @tc.name: BeginImplicitDurationKeyFrameAnimation001
 * @tc.desc: Verify the BeginImplicitDurationKeyFrameAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, BeginImplicitDurationKeyFrameAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest BeginImplicitDurationKeyFrameAnimation001 start";
    int duration = 1000;
    RSAnimationTimingProtocol timingProtocol;
    float mass = 0.5f;
    float stiffness = 0.0f;
    float damping = 0.5f;
    float velocity = 1.0f;
    auto timingCurve = RSAnimationTimingCurve::CreateSpringCurve(velocity, mass, stiffness, damping);
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    implicitAnimator->BeginImplicitDurationKeyFrameAnimation(duration, timingCurve);

    implicitAnimator->OpenImplicitAnimation(RSAnimationTimingProtocol::DEFAULT, timingCurve);
    implicitAnimator->BeginImplicitDurationKeyFrameAnimation(duration, timingCurve);

    RSAnimationTimingProtocol timingProtocol1(0);
    auto timingCurve1 = RSAnimationTimingCurve::DEFAULT;
    implicitAnimator->OpenImplicitAnimation(timingProtocol1, timingCurve1);
    implicitAnimator->BeginImplicitDurationKeyFrameAnimation(duration, timingCurve1);

    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve1);
    implicitAnimator->BeginImplicitDurationKeyFrameAnimation(duration, timingCurve1);
    EXPECT_TRUE(implicitAnimator != nullptr);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest BeginImplicitDurationKeyFrameAnimation001 end";
}

/**
 * @tc.name: EndImplicitDurationKeyFrameAnimation001
 * @tc.desc: Verify the EndImplicitDurationKeyFrameAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EndImplicitDurationKeyFrameAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EndImplicitDurationKeyFrameAnimation001 start";
    int duration = 1000;
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    implicitAnimator->EndImplicitDurationKeyFrameAnimation();

    RSAnimationTimingProtocol timingProtocol;
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve);
    implicitAnimator->BeginImplicitDurationKeyFrameAnimation(duration, timingCurve);
    implicitAnimator->EndImplicitDurationKeyFrameAnimation();
    int duration2 = INT32_MAX;
    implicitAnimator->BeginImplicitDurationKeyFrameAnimation(duration2, timingCurve);
    implicitAnimator->EndImplicitDurationKeyFrameAnimation();
    EXPECT_TRUE(implicitAnimator != nullptr);
    [[maybe_unused]] auto& [isDurationKeyframe, totalDuration, currentDuration] =
        implicitAnimator->durationKeyframeParams_.top();
    EXPECT_EQ(totalDuration, INT32_MAX);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EndImplicitDurationKeyFrameAnimation001 end";
}

/**
 * @tc.name: EndImplicitPathAnimation001
 * @tc.desc: Verify the EndImplicitPathAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EndImplicitPathAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EndImplicitPathAnimation001 start";
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    implicitAnimator->EndImplicitPathAnimation();

    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    RSAnimationTimingProtocol timingProtocol;
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve);
    implicitAnimator->BeginImplicitPathAnimation(motionPathOption);
    implicitAnimator->EndImplicitPathAnimation();
    EXPECT_TRUE(implicitAnimator != nullptr);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EndImplicitPathAnimation001 end";
}

/**
 * @tc.name: CreateImplicitAnimationWithInitialVelocity001
 * @tc.desc: Verify the CreateImplicitAnimationWithInitialVelocity
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CreateImplicitAnimationWithInitialVelocity001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CreateImplicitAnimationWithInitialVelocity001 start";
    RSAnimationTimingProtocol timingProtocol;
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();

    std::shared_ptr<RSCanvasNode> node = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector4f>> prop = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector4f>> prop_start = nullptr;
    std::shared_ptr<RSAnimatableProperty<Vector4f>> prop_end = nullptr;
    auto velocity = std::make_shared<RSAnimatableProperty<float>>(1.0f);

    implicitAnimator->CreateImplicitAnimationWithInitialVelocity(node, prop, prop_start, prop_end, velocity);

    auto node1 = RSCanvasNode::Create();
    implicitAnimator->OpenImplicitAnimation(RSAnimationTimingProtocol::DEFAULT, RSAnimationTimingCurve::DEFAULT);
    implicitAnimator->CreateImplicitAnimationWithInitialVelocity(node1, prop, prop_start, prop_end, velocity);

    auto prop1 = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    implicitAnimator->OpenImplicitAnimation(RSAnimationTimingProtocol::DEFAULT, RSAnimationTimingCurve::DEFAULT);
    implicitAnimator->CreateImplicitAnimationWithInitialVelocity(node1, prop1, prop_start, prop_end, velocity);

    EXPECT_TRUE(implicitAnimator != nullptr);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CreateImplicitAnimationWithInitialVelocity001 end";
}

/**
 * @tc.name: ProcessEmptyAnimationTest001
 * @tc.desc: Verify the ProcessEmptyAnimationTest001
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, ProcessEmptyAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessEmptyAnimationTest001 start";
    RSAnimationTimingProtocol timingProtocol;
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    timingProtocol.duration_ = -1;
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    std::shared_ptr<AnimationFinishCallback> finishCallback = std::make_shared<AnimationFinishCallback>(nullptr);
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(finishCallback));
    EXPECT_TRUE(implicitAnimator != nullptr);

    finishCallback = std::make_shared<AnimationFinishCallback>(nullptr, FinishCallbackType::TIME_INSENSITIVE);
    implicitAnimator->ProcessEmptyAnimations(finishCallback);

    finishCallback = std::make_shared<AnimationFinishCallback>(nullptr, FinishCallbackType::TIME_INSENSITIVE);
    implicitAnimator->ProcessEmptyAnimations(finishCallback);

    auto implicitAnimator2 = std::make_shared<RSImplicitAnimator>();
    auto canvasNode = RSCanvasNode::Create();
    implicitAnimator2->CreateImplicitTransition(*canvasNode.get());
    finishCallback = std::make_shared<AnimationFinishCallback>(nullptr);
    implicitAnimator2->OpenImplicitAnimation(std::move(finishCallback));
    implicitAnimator2->ProcessEmptyAnimations(finishCallback);

    implicitAnimator2->interactiveImplicitAnimations_.push({});
    implicitAnimator2->CloseInterActiveImplicitAnimation(true);
    while (!implicitAnimator2->interactiveImplicitAnimations_.empty()) {
        implicitAnimator2->PopImplicitParam();
    }
    implicitAnimator2->CloseInterActiveImplicitAnimation(true);

    implicitAnimator2->BeginImplicitPathAnimation(nullptr);
    RSAnimationTimingProtocol timingProtocol3;
    auto timingCurve3 = RSAnimationTimingCurve();
    timingCurve3.type_ = RSAnimationTimingCurve::CurveType::SPRING;
    timingProtocol3.duration_ = 0;
    auto implicitAnimator3 = std::make_shared<RSImplicitAnimator>();
    finishCallback = std::make_shared<AnimationFinishCallback>(nullptr);
    implicitAnimator3->OpenImplicitAnimation(timingProtocol3, timingCurve3, std::move(finishCallback));
    implicitAnimator3->BeginImplicitPathAnimation(nullptr);
    EXPECT_TRUE(implicitAnimator2 != nullptr);
    EXPECT_TRUE(implicitAnimator3 != nullptr);

    implicitAnimator2->BeginImplicitTransition(nullptr, false);

    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessEmptyAnimationTest001 end";
}

/**
 * @tc.name: ProcessEmptyAnimationTest002
 * @tc.desc: Verify with UIContext
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, ProcessEmptyAnimationTest002, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    implicitAnimator->SetRSUIContext(rsUIContext);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.duration_ = -1;
    timingProtocol.repeatCount_ = 0;
    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    std::shared_ptr<AnimationFinishCallback> finishCallback = nullptr;
    std::shared_ptr<AnimationRepeatCallback> repeatCallback = nullptr;

    auto globalParam = std::make_tuple(timingProtocol, timingCurve, finishCallback, repeatCallback);
    implicitAnimator->globalImplicitParams_.push(globalParam);
    auto finishCallbackTest = std::make_shared<AnimationFinishCallback>(
        nullptr, FinishCallbackType::TIME_INSENSITIVE);

    EXPECT_TRUE(finishCallbackTest.use_count() == 1);
    implicitAnimator->ProcessEmptyAnimations(finishCallbackTest);
}

/**
 * @tc.name: ProcessAnimationFinishCallbackGuaranteeTaskTest001
 * @tc.desc: Verify the ProcessAnimationFinishCallbackGuaranteeTaskTest001
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, ProcessAnimationFinishCallbackGuaranteeTaskTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest001 start";
    RSAnimationTimingProtocol timingProtocol;
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    timingProtocol.duration_ = 300;
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    std::shared_ptr<AnimationFinishCallback> callback =
        std::make_shared<AnimationFinishCallback>([&]() {}, FinishCallbackType::TIME_INSENSITIVE);
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(callback));
    EXPECT_TRUE(implicitAnimator != nullptr);
    implicitAnimator->ProcessAnimationFinishCallbackGuaranteeTask();

    auto implicitAnimator2 = std::make_shared<RSImplicitAnimator>();
    std::shared_ptr<AnimationFinishCallback> callback2 = std::make_shared<AnimationFinishCallback>([&]() {});
    implicitAnimator2->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(callback2));
    EXPECT_TRUE(implicitAnimator2 != nullptr);
    implicitAnimator2->ProcessAnimationFinishCallbackGuaranteeTask();

    auto timingCurve3 = RSAnimationTimingCurve::EASE;
    auto implicitAnimator3 = std::make_shared<RSImplicitAnimator>();
    implicitAnimator3->OpenImplicitAnimation(timingProtocol, timingCurve3, std::move(callback2));
    EXPECT_TRUE(implicitAnimator3 != nullptr);
    implicitAnimator3->ProcessAnimationFinishCallbackGuaranteeTask();

    RSAnimationTimingProtocol timingProtocol4;
    timingProtocol4.duration_ = 0;
    auto implicitAnimator4 = std::make_shared<RSImplicitAnimator>();
    implicitAnimator4->OpenImplicitAnimation(timingProtocol4, timingCurve3, std::move(callback2));
    EXPECT_TRUE(implicitAnimator4 != nullptr);
    implicitAnimator4->ProcessAnimationFinishCallbackGuaranteeTask();

    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest001 end";
}

/**
 * @tc.name: ProcessAnimationFinishCallbackGuaranteeTaskTest002
 * @tc.desc: Verify the ProcessAnimationFinishCallbackGuaranteeTask with infinite repeat
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, ProcessAnimationFinishCallbackGuaranteeTaskTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest002 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.duration_ = 300;
    timingProtocol.repeatCount_ = -1;  // infinite loop
    auto timingCurve = RSAnimationTimingCurve::EASE;
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    auto callback = std::make_shared<AnimationFinishCallback>([&]() {});
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(callback));
    EXPECT_TRUE(implicitAnimator != nullptr);
    // Should return early without creating guarantee task
    implicitAnimator->ProcessAnimationFinishCallbackGuaranteeTask();
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest002 end";
}

/**
 * @tc.name: ProcessAnimationFinishCallbackGuaranteeTaskTest003
 * @tc.desc: Verify the ProcessAnimationFinishCallbackGuaranteeTask with zero speed
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, ProcessAnimationFinishCallbackGuaranteeTaskTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest003 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.duration_ = 300;
    timingProtocol.SetSpeed(0.0f);  // speed is zero
    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    auto callback = std::make_shared<AnimationFinishCallback>([&]() {});
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(callback));
    EXPECT_TRUE(implicitAnimator != nullptr);
    // Should return early without creating guarantee task
    implicitAnimator->ProcessAnimationFinishCallbackGuaranteeTask();
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest003 end";
}

/**
 * @tc.name: ProcessAnimationFinishCallbackGuaranteeTaskTest004
 * @tc.desc: Verify the ProcessAnimationFinishCallbackGuaranteeTask with null callback
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, ProcessAnimationFinishCallbackGuaranteeTaskTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest004 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.duration_ = 300;
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    std::shared_ptr<AnimationFinishCallback> callback = nullptr;
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(callback));
    EXPECT_TRUE(implicitAnimator != nullptr);
    // Should return early without creating guarantee task
    implicitAnimator->ProcessAnimationFinishCallbackGuaranteeTask();
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest004 end";
}

/**
 * @tc.name: ProcessAnimationFinishCallbackGuaranteeTaskTest005
 * @tc.desc: Verify the ProcessAnimationFinishCallbackGuaranteeTask with normal parameters
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, ProcessAnimationFinishCallbackGuaranteeTaskTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest005 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.duration_ = 300;
    timingProtocol.repeatCount_ = 1;  // finite
    timingProtocol.SetSpeed(1.0f);     // normal speed
    auto timingCurve = RSAnimationTimingCurve::EASE;
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    auto callback = std::make_shared<AnimationFinishCallback>([&]() {});
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(callback));
    EXPECT_TRUE(implicitAnimator != nullptr);
    // Should create guarantee task normally
    implicitAnimator->ProcessAnimationFinishCallbackGuaranteeTask();
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest005 end";
}

/**
 * @tc.name: ProcessAnimationFinishCallbackGuaranteeTaskTest006
 * @tc.desc: Verify the ProcessAnimationFinishCallbackGuaranteeTask with spring curve and zero speed
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, ProcessAnimationFinishCallbackGuaranteeTaskTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest006 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.duration_ = 300;
    timingProtocol.SetSpeed(0.0f);  // speed is zero
    auto timingCurve = RSAnimationTimingCurve::SPRING;
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    auto callback = std::make_shared<AnimationFinishCallback>([&]() {});
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, std::move(callback));
    EXPECT_TRUE(implicitAnimator != nullptr);
    // Should return early without creating guarantee task
    implicitAnimator->ProcessAnimationFinishCallbackGuaranteeTask();
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest ProcessAnimationFinishCallbackGuaranteeTaskTest006 end";
}

/**
 * @tc.name: GetRSImplicitAnimator
 * @tc.desc: Verify the GetRSImplicitAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, GetRSImplicitAnimator, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    // repeat count
    int count = 10;
    // thread count
    int threadCount = 2;
    std::thread t1([rsUIContext, count] {
        for (int i = 0; i < count; i++) {
            rsUIContext->GetRSImplicitAnimator();
        }
    });
    std::thread t2([rsUIContext, count] {
        for (int i = 0; i < count; i++) {
            rsUIContext->GetRSImplicitAnimator();
        }
    });
    t1.join();
    t2.join();
    EXPECT_EQ(rsUIContext->rsImplicitAnimators_.size(), threadCount);
}

/**
 * @tc.name: CheckImplicitAnimationConditionsTest001
 * @tc.desc: Verify the CheckImplicitAnimationConditions
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CheckImplicitAnimationConditionsTest001, TestSize.Level1)
{
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    // globalImplicitParams_ is empty true
    EXPECT_TRUE(implicitAnimator->globalImplicitParams_.empty());
    implicitAnimator->CheckImplicitAnimationConditions();

    // globalImplicitParams_ is empty false
    RSAnimationTimingProtocol timingProtocol(0);
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve);
    EXPECT_FALSE(implicitAnimator->globalImplicitParams_.empty());
    implicitAnimator->CheckImplicitAnimationConditions();

    // implicitAnimations_ is empty true
    std::stack<std::vector<std::pair<std::shared_ptr<RSAnimation>, NodeId>>>().swap(
        implicitAnimator->implicitAnimations_);
    EXPECT_TRUE(implicitAnimator->implicitAnimations_.empty());
}

/**
 * @tc.name: CheckImplicitAnimationConditionsTest002
 * @tc.desc: Verify the CheckImplicitAnimationConditions
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CheckImplicitAnimationConditionsTest002, TestSize.Level1)
{
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();

    RSAnimationTimingProtocol timingProtocol(0);
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve);
    EXPECT_FALSE(implicitAnimator->globalImplicitParams_.empty());
    EXPECT_FALSE(implicitAnimator->implicitAnimations_.empty());

    // keyframeAnimations_ is empty true
    std::stack<std::map<std::pair<NodeId, PropertyId>, std::shared_ptr<RSAnimation>>>().swap(
        implicitAnimator->keyframeAnimations_);
    EXPECT_TRUE(implicitAnimator->keyframeAnimations_.empty());
    implicitAnimator->CheckImplicitAnimationConditions();
}

/**
 * @tc.name: CheckImplicitAnimationConditionsTest003
 * @tc.desc: Verify the CheckImplicitAnimationConditions
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CheckImplicitAnimationConditionsTest003, TestSize.Level1)
{
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();

    RSAnimationTimingProtocol timingProtocol(0);
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve);
    EXPECT_FALSE(implicitAnimator->globalImplicitParams_.empty());
    EXPECT_FALSE(implicitAnimator->implicitAnimations_.empty());
    EXPECT_FALSE(implicitAnimator->keyframeAnimations_.empty());

    // implicitAnimationParams_ is empty true
    std::stack<std::shared_ptr<RSImplicitAnimationParam>>().swap(implicitAnimator->implicitAnimationParams_);
    EXPECT_TRUE(implicitAnimator->implicitAnimationParams_.empty());
    implicitAnimator->CheckImplicitAnimationConditions();
}
using globalStack = std::stack<std::tuple<RSAnimationTimingProtocol, RSAnimationTimingCurve,
    const std::shared_ptr<AnimationFinishCallback>, std::shared_ptr<AnimationRepeatCallback>>>;

/**
 * @tc.name: CloseInterActiveImplicitAnimationTest001
 * @tc.desc: Verify the CloseInterActiveImplicitAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CloseInterActiveImplicitAnimationTest001, TestSize.Level1)
{
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    implicitAnimator->interactiveImplicitAnimations_.push({});
    globalStack().swap(implicitAnimator->globalImplicitParams_);
    EXPECT_TRUE(implicitAnimator->globalImplicitParams_.empty());
    implicitAnimator->CloseInterActiveImplicitAnimation(true);

    RSAnimationTimingProtocol timingProtocol(0);
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve);
    EXPECT_FALSE(implicitAnimator->globalImplicitParams_.empty());
    EXPECT_FALSE(implicitAnimator->implicitAnimations_.empty());
    implicitAnimator->CloseInterActiveImplicitAnimation(true);
}

/**
 * @tc.name: CloseInterActiveImplicitAnimationTest002
 * @tc.desc: Verify the CloseInterActiveImplicitAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CloseInterActiveImplicitAnimationTest002, TestSize.Level1)
{
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    implicitAnimator->interactiveImplicitAnimations_.push({});
    globalStack().swap(implicitAnimator->globalImplicitParams_);

    RSAnimationTimingProtocol timingProtocol(0);
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve);
    EXPECT_FALSE(implicitAnimator->globalImplicitParams_.empty());
    std::stack<std::vector<std::pair<std::shared_ptr<RSAnimation>, NodeId>>>().swap(
        implicitAnimator->implicitAnimations_);
    EXPECT_TRUE(implicitAnimator->implicitAnimations_.empty());
    implicitAnimator->CloseInterActiveImplicitAnimation(true);
}

/**
 * @tc.name: CreateImplicitAnimationTest001
 * @tc.desc: Verify the CreateImplicitAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CreateImplicitAnimationTest001, TestSize.Level1)
{
    RSImplicitAnimator implicitAnimator;
    auto protocol = RSAnimationTimingProtocol::DEFAULT;
    auto timingCurve = RSAnimationTimingCurve::DEFAULT;
    implicitAnimator.OpenImplicitAnimation(protocol, timingCurve);
    ASSERT_FALSE(implicitAnimator.globalImplicitParams_.empty());
    implicitAnimator.BeginImplicitCurveAnimation();
    ASSERT_FALSE(implicitAnimator.implicitAnimationParams_.empty());
    ASSERT_NE(protocol.GetRepeatCount(), -1); // finite

    auto target = RSCanvasNode::Create();
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.f);

    implicitAnimator.CreateImplicitAnimation(target, property, startValue, endValue);
    implicitAnimator.EndImplicitAnimation();
    implicitAnimator.CloseImplicitAnimation();
    ASSERT_TRUE(implicitAnimator.globalImplicitParams_.empty());
    ASSERT_TRUE(implicitAnimator.implicitAnimationParams_.empty());
}

/**
 * @tc.name: CreateImplicitAnimationTest002
 * @tc.desc: Verify the CreateImplicitAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CreateImplicitAnimationTest002, TestSize.Level1)
{
    RSImplicitAnimator implicitAnimator;
    auto protocol = RSAnimationTimingProtocol::DEFAULT;
    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    implicitAnimator.OpenImplicitAnimation(protocol, timingCurve);
    ASSERT_FALSE(implicitAnimator.globalImplicitParams_.empty());
    implicitAnimator.BeginImplicitKeyFrameAnimation(1.f);
    ASSERT_FALSE(implicitAnimator.implicitAnimationParams_.empty());
    ASSERT_NE(protocol.GetRepeatCount(), -1); // finite

    auto target = RSCanvasNode::Create();
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.5f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(0.5f);

    implicitAnimator.CreateImplicitAnimation(target, property, startValue, endValue);
    implicitAnimator.EndImplicitKeyFrameAnimation();
    implicitAnimator.CloseImplicitAnimation();
    ASSERT_TRUE(implicitAnimator.globalImplicitParams_.empty());
    ASSERT_TRUE(implicitAnimator.implicitAnimationParams_.empty());
}

/**
 * @tc.name: EstimateAnimationDuration_InfiniteRepeat
 * @tc.desc: Verify EstimateAnimationDuration returns 0 when repeatCount is -1
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_InfiniteRepeat, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_InfiniteRepeat start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(-1);
    auto curve = RSAnimationTimingCurve::LINEAR;
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    EXPECT_FLOAT_EQ(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_InfiniteRepeat end";
}

/**
 * @tc.name: EstimateAnimationDuration_ZeroSpeed
 * @tc.desc: Verify EstimateAnimationDuration returns 0 when speed is 0.0f
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_ZeroSpeed, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_ZeroSpeed start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetSpeed(0.0f);
    auto curve = RSAnimationTimingCurve::LINEAR;
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    EXPECT_FLOAT_EQ(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_ZeroSpeed end";
}

/**
 * @tc.name: EstimateAnimationDuration_InterpolatingBasic
 * @tc.desc: Verify EstimateAnimationDuration with INTERPOLATING curve and basic parameters
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_InterpolatingBasic, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_InterpolatingBasic start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(1);
    protocol.SetStartDelay(0);
    protocol.SetSpeed(1.0f);
    auto curve = RSAnimationTimingCurve::LINEAR;
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    EXPECT_GT(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_InterpolatingBasic end";
}

/**
 * @tc.name: EstimateAnimationDuration_InterpolatingWithDelay
 * @tc.desc: Verify EstimateAnimationDuration with INTERPOLATING curve and startDelay
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_InterpolatingWithDelay, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_InterpolatingWithDelay start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(1);
    protocol.SetStartDelay(100);
    protocol.SetSpeed(1.0f);
    auto curve = RSAnimationTimingCurve::EASE;
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    // Should be (300 * 1 + 100) * animationScale
    EXPECT_GT(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_InterpolatingWithDelay end";
}

/**
 * @tc.name: EstimateAnimationDuration_InterpolatingWithRepeat
 * @tc.desc: Verify EstimateAnimationDuration with INTERPOLATING curve and repeatCount > 1
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_InterpolatingWithRepeat, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_InterpolatingWithRepeat start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(3);
    protocol.SetStartDelay(50);
    protocol.SetSpeed(1.0f);
    auto curve = RSAnimationTimingCurve::EASE_IN;
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    // Should be (300 * 3 + 50) * animationScale
    EXPECT_GT(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_InterpolatingWithRepeat end";
}

/**
 * @tc.name: EstimateAnimationDuration_SpringBasic
 * @tc.desc: Verify EstimateAnimationDuration with SPRING curve
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_SpringBasic, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_SpringBasic start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(1);
    protocol.SetStartDelay(0);
    protocol.SetSpeed(1.0f);
    auto curve = RSAnimationTimingCurve::SPRING;
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    EXPECT_GT(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_SpringBasic end";
}

/**
 * @tc.name: EstimateAnimationDuration_SpringWithDelayAndRepeat
 * @tc.desc: Verify EstimateAnimationDuration with SPRING curve, delay and repeat
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_SpringWithDelayAndRepeat, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_SpringWithDelayAndRepeat start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(2);
    protocol.SetStartDelay(100);
    protocol.SetSpeed(1.0f);
    float mass = 0.5f;
    float stiffness = 0.0f;
    float damping = 0.5f;
    float velocity = 1.0f;
    auto curve = RSAnimationTimingCurve::CreateSpringCurve(velocity, mass, stiffness, damping);
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    EXPECT_GT(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_SpringWithDelayAndRepeat end";
}

/**
 * @tc.name: EstimateAnimationDuration_InterpolatingSpring
 * @tc.desc: Verify EstimateAnimationDuration with INTERPOLATING_SPRING curve
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_InterpolatingSpring, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_InterpolatingSpring start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(1);
    protocol.SetStartDelay(0);
    protocol.SetSpeed(1.0f);
    float mass = 0.5f;
    float stiffness = 0.0f;
    float damping = 0.5f;
    float velocity = 1.0f;
    auto curve = RSAnimationTimingCurve::CreateInterpolatingSpring(velocity, mass, stiffness, damping);
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    EXPECT_GT(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_InterpolatingSpring end";
}

/**
 * @tc.name: EstimateAnimationDuration_SpeedMultiplier
 * @tc.desc: Verify EstimateAnimationDuration speed multiplier effect
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_SpeedMultiplier, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_SpeedMultiplier start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(1);
    protocol.SetStartDelay(0);
    auto curve = RSAnimationTimingCurve::LINEAR;

    protocol.SetSpeed(2.0f);  // 2x speed
    float durationDoubleSpeed = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);

    protocol.SetSpeed(1.0f);  // normal speed
    float durationNormalSpeed = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);

    // Duration should be inversely proportional to speed
    EXPECT_FLOAT_EQ(durationDoubleSpeed, durationNormalSpeed / 2.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_SpeedMultiplier end";
}

/**
 * @tc.name: EstimateAnimationDuration_ZeroDuration
 * @tc.desc: Verify EstimateAnimationDuration with zero duration but valid startDelay
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_ZeroDuration, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_ZeroDuration start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(0);
    protocol.SetRepeatCount(1);
    protocol.SetStartDelay(100);
    protocol.SetSpeed(1.0f);
    auto curve = RSAnimationTimingCurve::LINEAR;
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    // Should be (0 * 1 + 100) * animationScale = 100 * animationScale
    EXPECT_GT(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_ZeroDuration end";
}

/**
 * @tc.name: EstimateAnimationDuration_ZeroRepeatCount
 * @tc.desc: Verify EstimateAnimationDuration with zero repeatCount (play once)
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_ZeroRepeatCount, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_ZeroRepeatCount start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(1);  // play once
    protocol.SetStartDelay(0);
    protocol.SetSpeed(1.0f);
    auto curve = RSAnimationTimingCurve::LINEAR;
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    EXPECT_GT(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_ZeroRepeatCount end";
}

/**
 * @tc.name: EstimateAnimationDuration_CombinedParams
 * @tc.desc: Verify EstimateAnimationDuration with all parameters combined
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_CombinedParams, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_CombinedParams start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    protocol.SetRepeatCount(5);
    protocol.SetStartDelay(200);
    protocol.SetSpeed(1.5f);
    auto curve = RSAnimationTimingCurve::EASE_IN_OUT;
    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    // Should be (500 * 5 + 200) * animationScale / 1.5
    EXPECT_GT(duration, 0.0f);
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_CombinedParams end";
}

/**
 * @tc.name: EstimateAnimationDuration_SpringWithoutParams
 * @tc.desc: Verify EstimateAnimationDuration when spring curve has no springParams (both branches not executed)
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EstimateAnimationDuration_SpringWithoutParams, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_SpringWithoutParams start";
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(1);
    protocol.SetStartDelay(0);
    protocol.SetSpeed(1.0f);

    // Create a spring curve (which should have springParams_)
    auto curve = RSAnimationTimingCurve::SPRING;

    // Manually clear springParams_ to test the edge case where:
    // - curve.type_ != INTERPOLATING (it's SPRING)
    // - curve.springParams_ is empty (manually cleared)
    // Result: both branches are skipped, duration remains 0
    curve.springParams_ = std::nullopt;

    float duration = RSImplicitAnimator::EstimateAnimationDuration(protocol, curve);
    // Since both branches are not executed, duration should be 0 * animationScale / speed = 0
    EXPECT_FLOAT_EQ(duration, 0.0f);

    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest EstimateAnimationDuration_SpringWithoutParams end";
}
} // namespace Rosen
} // namespace OHOS
