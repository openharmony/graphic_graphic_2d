/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <sys/types.h>
#include <unistd.h>

#include "gtest/gtest.h"

#include "animation/rs_animation_callback.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_keyframe_animation.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_render_animation.h"
#include "animation/rs_spring_animation.h"
#include "animation/rs_transition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSupplementAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSupplementAnimationTest::SetUpTestCase() {}
void RSSupplementAnimationTest::TearDownTestCase() {}
void RSSupplementAnimationTest::SetUp() {}
void RSSupplementAnimationTest::TearDown() {}

/**
 * @tc.name: AnimationSupplementTest
 * @tc.desc: Verify the CloseImplicitCancelAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init
     */
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    auto ret = implicitAnimator->CloseImplicitCancelAnimation();
    EXPECT_EQ(ret, CancelAnimationStatus::NO_OPEN_CLOSURE);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    implicitAnimator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    ret = implicitAnimator->CloseImplicitCancelAnimation();
    EXPECT_EQ(ret, CancelAnimationStatus::INCORRECT_PARAM_TYPE);

    protocol.SetDuration(0);
    implicitAnimator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    ret = implicitAnimator->CloseImplicitCancelAnimation();
    EXPECT_EQ(ret, CancelAnimationStatus::EMPTY_PENDING_SYNC_LIST);
}

#ifdef MODIFIER_NG_TEST
class MyFilterMock : public RSFilter {
public:
    MyFilterMock() : RSFilter() {}
    virtual ~MyFilterMock() = default;
};
class MyNewData : public RSArithmetic<MyNewData> {
public:
    MyNewData() : data(0.f) {}
    explicit MyNewData(const float num) : data(num) {}
    virtual ~MyNewData() = default;
    bool IsEqual(const MyNewData& value) const override
    {
        return data == value.data;
    }

    float data;
};
class RSSpringAnimationMock : public RSSpringAnimation {
public:
    RSSpringAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& byValue)
        : RSSpringAnimation(property, byValue)
    {}

    RSSpringAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue)
        : RSSpringAnimation(property, startValue, endValue)
    {}
    ~RSSpringAnimationMock() = default;

    void OnStart() override
    {
        RSSpringAnimation::OnStart();
    }
};
class RSTransitionTest : public RSTransition {
public:
    RSTransitionTest(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
        : RSTransition(effect, isTransitionIn)
    {}
    virtual ~RSTransitionTest() = default;
};

class RSTransitionMock : public RSTransition {
public:
    RSTransitionMock(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
        : RSTransition(effect, isTransitionIn)
    {}
    virtual ~RSTransitionMock() = default;

    void OnStart()
    {
        RSTransition::OnStart();
    }
};
class RSPathAnimationMock : public RSPathAnimation {
public:
    RSPathAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPath>& animationPath)
        : RSPathAnimation(property, animationPath)
    {}
    RSPathAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::string& path,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue)
        : RSPathAnimation(property, path, startValue, endValue)
    {}
    ~RSPathAnimationMock() = default;

    void OnStart()
    {
        RSPathAnimation::OnStart();
    }

    void InitInterpolationValue()
    {
        RSPathAnimation::InitInterpolationValue();
    }

    void OnUpdateStagingValue(bool isFirstStart)
    {
        RSPathAnimation::OnUpdateStagingValue(isFirstStart);
    }

    void InitRotationId(const std::shared_ptr<RSNode>& node)
    {
        RSPathAnimation::InitRotationId(node);
    }

    PropertyId GetRotationPropertyId(const std::shared_ptr<RSNode>& node)
    {
        return RSPathAnimation::GetRotationPropertyId(node);
    }

    void SetRotation(const std::shared_ptr<RSNode>& node, const float rotation)
    {
        RSPathAnimation::SetRotation(node, rotation);
    }

    void OnCallFinishCallback()
    {
        RSPathAnimation::OnCallFinishCallback();
    }
};
class MockRSRenderAnimation : public RSRenderAnimation {
public:
    MockRSRenderAnimation(AnimationId id) : RSRenderAnimation(id) {}
    ~MockRSRenderAnimation() = default;

    void DumpAnimation(std::string& out)
    {
        RSRenderAnimation::DumpAnimation(out);
    }
};
class MockRSKeyframeAnimation : public RSKeyframeAnimation {
public:
    explicit MockRSKeyframeAnimation(std::shared_ptr<RSPropertyBase> property) : RSKeyframeAnimation(property) {}
    ~MockRSKeyframeAnimation() = default;

    void OnStart() override
    {
        RSKeyframeAnimation::OnStart();
    }

    void InitInterpolationValue() override
    {
        RSKeyframeAnimation::InitInterpolationValue();
    }
};
class MockRSAnimation : public RSAnimation {
public:
    MockRSAnimation() : RSAnimation() {}
    ~MockRSAnimation() = default;

    void OnReverse() override
    {
        RSAnimation::OnReverse();
    }

    void StartInner(const std::shared_ptr<RSNode>& target)
    {
        RSAnimation::StartInner(target);
    }

    void OnStart() override
    {
        RSAnimation::OnStart();
    }
    void OnSetFraction(float fraction) override
    {
        RSAnimation::OnSetFraction(fraction);
    }

    void OnPause() override
    {
        RSAnimation::OnPause();
    }

    void OnResume() override
    {
        RSAnimation::OnResume();
    }

    void OnFinish() override
    {
        RSAnimation::OnFinish();
    }

    bool IsReversed() const
    {
        return RSAnimation::IsReversed();
    }
    void OnUpdateStagingValue(bool isFirstStart) override
    {
        RSAnimation::OnUpdateStagingValue(isFirstStart);
    }

    PropertyId GetPropertyId() const override
    {
        return RSAnimation::GetPropertyId();
    }
    void StartCustomAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
    {
        RSAnimation::StartCustomAnimation(animation);
    }

    void UpdateParamToRenderAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
    {
        RSAnimation::UpdateParamToRenderAnimation(animation);
    }

    void DumpAnimationInfo(std::string& dumpInfo) const override
    {
        RSAnimation::DumpAnimationInfo(dumpInfo);
    }

    std::string DumpAnimation() const
    {
        return RSAnimation::DumpAnimation();
    }
};
class MyData : public RSAnimatableArithmetic<MyData> {
public:
    MyData() : data(0.f) {}
    explicit MyData(const float num) : data(num) {}
    virtual ~MyData() = default;

    MyData Add(const MyData& value) const override
    {
        float res = data + value.data;
        return MyData(res);
    }
    MyData Minus(const MyData& value) const override
    {
        float res = data - value.data;
        return MyData(res);
    }
    MyData Multiply(const float scale) const override
    {
        float res = data * scale;
        return MyData(res);
    }
    bool IsEqual(const MyData& value) const override
    {
        return data == value.data;
    }

    float data;
};

/**
 * @tc.name: AnimationSupplementTest001
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest001 start";
    /**
     * @tc.steps: step1. init
     */
    auto mockAnimation = std::make_shared<MockRSAnimation>();
    std::function<void()> callback = nullptr;
    mockAnimation->SetFinishCallback(callback);
    callback = []() { std::cout << "RSSupplementAnimationTest AnimationSupplementTest001 callback" << std::endl; };
    mockAnimation->SetFinishCallback(callback);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest001 end";
}

/**
 * @tc.name: AnimationSupplementTest002
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest002 start";
    /**
     * @tc.steps: step1. init
     */
    auto mockAnimation = std::make_shared<MockRSAnimation>();
    EXPECT_TRUE(!mockAnimation->IsStarted());
    EXPECT_TRUE(!mockAnimation->IsRunning());
    EXPECT_TRUE(!mockAnimation->IsPaused());
    EXPECT_TRUE(!mockAnimation->IsFinished());
    EXPECT_TRUE(!mockAnimation->IsReversed());
    std::shared_ptr<RSNode> node = nullptr;
    mockAnimation->Reverse();
    mockAnimation->Finish();
    mockAnimation->Pause();
    mockAnimation->Start(node);
    mockAnimation->StartInner(node);
    mockAnimation->Resume();
    mockAnimation->Pause();
    mockAnimation->OnPause();
    mockAnimation->Resume();
    mockAnimation->OnResume();
    mockAnimation->Finish();
    mockAnimation->OnFinish();
    mockAnimation->Reverse();
    mockAnimation->OnReverse();
    node = RSCanvasNode::Create();
    node->AddAnimation(animation);
    mockAnimation->Pause();
    mockAnimation->OnPause();
    mockAnimation->Resume();
    mockAnimation->OnResume();
    mockAnimation->Finish();
    mockAnimation->OnFinish();
    mockAnimation->Reverse();
    mockAnimation->OnReverse();
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest002 end";
}

/**
 * @tc.name: AnimationSupplementTest003
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest003 start";
    /**
     * @tc.steps: step1. init
     */
    auto mockAnimation = std::make_shared<MockRSAnimation>();
    auto rendermockAnimation = std::make_shared<MockRSRenderAnimation>(mockAnimation->GetId());
    mockAnimation->SetFraction(-1.0f);
    mockAnimation->SetFraction(0.5f);
    mockAnimation->Pause();
    mockAnimation->SetFraction(0.5f);
    mockAnimation->OnSetFraction(0.5f);
    mockAnimation->StartCustomAnimation(renderAnimation);
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    node->AddAnimation(animation);
    mockAnimation->SetFraction(-1.0f);
    mockAnimation->SetFraction(0.5f);
    mockAnimation->Pause();
    mockAnimation->SetFraction(0.5f);
    mockAnimation->OnSetFraction(0.5f);
    mockAnimation->StartCustomAnimation(renderAnimation);
    mockAnimation->DumpAnimation();
    std::string dumpInfo = "";
    mockAnimation->DumpAnimationInfo(dumpInfo);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest003 end";
}

/**
 * @tc.name: AnimationSupplementTest004
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest004 start";
    /**
     * @tc.steps: step1. init
     */
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    auto animations = implicitAnimator->CloseImplicitAnimation();
    EXPECT_TRUE(animations.empty());
    implicitAnimator->BeginImplicitKeyFrameAnimation(0.1f, RSAnimationTimingCurve::LINEAR);
    implicitAnimator->BeginImplicitKeyFrameAnimation(0.2f);
    implicitAnimator->EndImplicitKeyFrameAnimation();
    std::shared_ptr<RSMotionPathOption> motionPathOption;
    implicitAnimator->BeginImplicitPathAnimation(motionPathOption);
    implicitAnimator->EndImplicitPathAnimation();
    std::shared_ptr<const RSTransitionEffect> effect;
    implicitAnimator->BeginImplicitTransition(effect, true);
    implicitAnimator->EndImplicitTransition();
    implicitAnimator->BeginImplicitTransition(effect, false);
    implicitAnimator->EndImplicitTransition();
    std::shared_ptr<RSNode> node;
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<RSPropertyBase> startValue = std::make_shared<RSAnimatableProperty<float>>(2.f);
    std::shared_ptr<RSPropertyBase> endValue = std::make_shared<RSAnimatableProperty<float>>(10.0f);
    implicitAnimator->CreateImplicitAnimation(node, property, startValue, endValue);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    implicitAnimator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    implicitAnimator->CreateImplicitAnimation(node, property, startValue, endValue);
    implicitAnimator->CreateImplicitTransition(*node);
    implicitAnimator->CloseImplicitAnimation();
    implicitAnimator->EndImplicitKeyFrameAnimation();
    implicitAnimator->EndImplicitTransition();
    implicitAnimator->EndImplicitPathAnimation();

    node = RSCanvasNode::Create();
    implicitAnimator->CreateImplicitAnimation(node, property, startValue, endValue);
    implicitAnimator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    implicitAnimator->CreateImplicitAnimation(node, property, startValue, endValue);
    implicitAnimator->CreateImplicitTransition(*node);
    implicitAnimator->CloseImplicitAnimation();
    implicitAnimator->EndImplicitKeyFrameAnimation();
    implicitAnimator->EndImplicitTransition();
    implicitAnimator->EndImplicitPathAnimation();
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest004 end";
}

/**
 * @tc.name: AnimationSupplementTest006
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest006 start";
    /**
     * @tc.steps: step1. init
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(10.f);
    auto modifier = std::make_shared<RSAlphaModifier>(property);
    auto modifierManager = std::make_shared<RSModifierManager>();
    modifierManager->AddModifier(modifier);
    auto mockAnimation = std::make_shared<MockRSAnimation>();
    auto rendermockAnimation = std::make_shared<MockRSRenderAnimation>(mockAnimation->GetId());
    EXPECT_TRUE(animation != nullptr);
    modifierManager->RemoveAnimation(mockAnimation->GetId());
    modifierManager->AddAnimation(renderAnimation);
    modifierManager->Animate(0);
    modifierManager->Draw();
    modifierManager->RemoveAnimation(mockAnimation->GetId());
    modifierManager->AddAnimation(renderAnimation);
    modifierManager->AddAnimation(renderAnimation);
    auto uisecondAnimation = std::make_shared<MockRSAnimation>();
    auto secondAnimation = std::make_shared<MockRSRenderAnimation>(uisecondAnimation->GetId());
    modifierManager->RemoveAnimation(uisecondAnimation->GetId());
    modifierManager->RemoveAnimation(mockAnimation->GetId());
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest006 end";
}

/**
 * @tc.name: AnimationSupplementTest007
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest007 start";
    /**
     * @tc.steps: step1. init
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto keyframeParam =
        std::make_shared<RSImplicitKeyframeAnimationParam>(protocol, RSAnimationTimingCurve::LINEAR, 0.1f, 0);
    EXPECT_TRUE(keyframeParam != nullptr);
    std::shared_ptr<RSAnimation> animation;
    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(5.f, 6.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    keyframeParam->AddKeyframe(animation, startValue, endValue);
    mockAnimation = std::make_shared<RSKeyframeAnimation>(startValue);
    keyframeParam->AddKeyframe(animation, startValue, endValue);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest007 end";
}

/**
 * @tc.name: AnimationSupplementTest008
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest008 start";
    /**
     * @tc.steps: step1. init
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    std::shared_ptr<RSMotionPathOption> option;
    auto pathParam = std::make_shared<RSImplicitPathAnimationParam>(protocol, RSAnimationTimingCurve::LINEAR, option);
    EXPECT_TRUE(pathParam != nullptr);
    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(5.f, 6.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    auto mockAnimation = pathParam->CreateAnimation(property, startValue, endValue);

    option = std::make_shared<RSMotionPathOption>("abc");
    auto secondPathParam =
        std::make_shared<RSImplicitPathAnimationParam>(protocol, RSAnimationTimingCurve::LINEAR, option);
    auto secondAnimation = pathParam->CreateAnimation(property, startValue, endValue);

    auto springParam = std::make_shared<RSImplicitSpringAnimationParam>(protocol, RSAnimationTimingCurve::LINEAR);
    auto thirdAnimation = springParam->CreateAnimation(property, startValue, endValue);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest008 end";
}

/**
 * @tc.name: RSTransitionTest001
 * @tc.desc: Verify the RSTransition
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, RSTransitionTest001, TestSize.Level1)
{
    std::shared_ptr<const RSTransitionEffect> effect;
    auto mockAnimation = std::make_shared<RSTransitionMock>(effect, true);
    ASSERT_NE(animation, nullptr);
    mockAnimation->OnStart();
}

/**
 * @tc.name: AnimationSupplementTest010
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest010 start";
    /**
     * @tc.steps: step1. init
     */
    std::function<void()> callback;
    AnimationFinishCallback* firstFinishCallback = new AnimationFinishCallback(callback);
    delete firstFinishCallback;
    callback = []() { std::cout << "RSSupplementAnimationTest AnimationSupplementTest010" << std::endl; };
    EXPECT_TRUE(callback != nullptr);
    AnimationFinishCallback* secondFinishCallback = new AnimationFinishCallback(callback);
    delete secondFinishCallback;
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest010 end";
}

/**
 * @tc.name: AnimationSupplementTest013
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest013 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<MockRSKeyframeAnimation> mockAnimation = std::make_shared<MockRSKeyframeAnimation>(property);
    std::shared_ptr<RSPropertyBase> firstValue = std::make_shared<RSAnimatableProperty<float>>(0.2f);
    std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>> keyframes;
    keyframes.push_back({ 0.5f, firstValue, RSAnimationTimingCurve::LINEAR });
    mockAnimation->AddKeyFrame(-0.1f, firstValue, RSAnimationTimingCurve::LINEAR);
    mockAnimation->AddKeyFrame(0.2f, firstValue, RSAnimationTimingCurve::LINEAR);
    mockAnimation->AddKeyFrames(keyframes);
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    mockAnimation->Start(node);
    mockAnimation->AddKeyFrame(0.3f, firstValue, RSAnimationTimingCurve::LINEAR);
    mockAnimation->AddKeyFrames(keyframes);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest013 end";
}

/**
 * @tc.name: AnimationSupplementTest014
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest014 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<MockRSKeyframeAnimation> mockAnimation = std::make_shared<MockRSKeyframeAnimation>(property);
    mockAnimation->InitInterpolationValue();
    mockAnimation->OnStart();
    std::shared_ptr<RSPropertyBase> firstValue = std::make_shared<RSAnimatableProperty<float>>(0.2f);
    mockAnimation->AddKeyFrame(0.2f, firstValue, RSAnimationTimingCurve::LINEAR);
    mockAnimation->InitInterpolationValue();
    mockAnimation->AddKeyFrame(0.f, firstValue, RSAnimationTimingCurve::LINEAR);
    mockAnimation->InitInterpolationValue();
    mockAnimation->OnStart();
    mockAnimation->SetIsCustom(true);
    mockAnimation->OnStart();
    mockAnimation->SetIsCustom(false);
    std::shared_ptr<RSNode> firstNode = RSCanvasNode::Create();
    struct RSSurfaceNodeConfig surfaceNodeConfig = { .SurfaceNodeName = "test" };
    std::shared_ptr<RSNode> secondNode = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> thirdNode = RSSurfaceNode::Create(surfaceNodeConfig, true);
    mockAnimation->Start(firstNode);
    mockAnimation->OnStart();
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest014 end";
}

/**
 * @tc.name: AnimationSupplementTest015
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest015 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<MockRSKeyframeAnimation> firstAnimation = std::make_shared<MockRSKeyframeAnimation>(property);
    std::shared_ptr<MockRSKeyframeAnimation> secondAnimation = std::make_shared<MockRSKeyframeAnimation>(property);
    std::shared_ptr<MockRSKeyframeAnimation> thirdAnimation = std::make_shared<MockRSKeyframeAnimation>(property);
    std::shared_ptr<RSNode> firstNode = RSCanvasNode::Create();
    struct RSSurfaceNodeConfig surfaceNodeConfig = { .SurfaceNodeName = "test" };
    std::shared_ptr<RSNode> secondNode = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> thirdNode = RSSurfaceNode::Create(surfaceNodeConfig, true);
    firstAnimation->Start(firstNode);
    firstAnimation->OnStart();
    firstAnimation->SetIsCustom(true);
    firstAnimation->OnStart();
    secondAnimation->Start(firstNode);
    secondAnimation->OnStart();
    secondAnimation->SetIsCustom(true);
    secondAnimation->OnStart();
    thirdAnimation->Start(firstNode);
    thirdAnimation->OnStart();
    thirdAnimation->SetIsCustom(true);
    thirdAnimation->OnStart();
    EXPECT_TRUE(animation1 != nullptr);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest015 end";
}

/**
 * @tc.name: AnimationSupplementTest016
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest016 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(0.f, 1.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    auto mockAnimation = std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    mockAnimation->InitInterpolationValue();
    mockAnimation->OnUpdateStagingValue(true);
    mockAnimation->SetDirection(false);
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    mockAnimation->Start(node);
    mockAnimation->OnStart();
    mockAnimation->OnUpdateStagingValue(false);
    mockAnimation->SetAutoReverse(true);
    mockAnimation->SetRepeatCount(2);
    mockAnimation->OnUpdateStagingValue(false);
    mockAnimation->InitInterpolationValue();
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(10.f);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    node->AddModifier(rotationModifier);
    mockAnimation->GetRotationPropertyId(node);
    mockAnimation->SetRotation(node, 1.f);
    mockAnimation->InitInterpolationValue();
    EXPECT_TRUE(mockAnimation->GetAutoReverse());
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest016 end";
}

/**
 * @tc.name: AnimationSupplementTest017
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest017 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    std::shared_ptr<RSPath> path = std::make_shared<RSPath>();
    auto firstAnimation = std::make_shared<RSPathAnimationMock>(property, path);
    firstAnimation->OnCallFinishCallback();
    property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    Vector2f startData(0.f, 1.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector2f>>(startData);
    Vector2f endData(5.f, 6.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector2f>>(endData);
    auto mockAnimation = std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    EXPECT_TRUE(animation != nullptr);
    mockAnimation->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    auto curve = mockAnimation->GetTimingCurve();
    mockAnimation->SetTimingCurve(curve);
    mockAnimation->SetRotationMode(RotationMode::ROTATE_AUTO);
    mockAnimation->SetBeginFraction(-0.1f);
    mockAnimation->SetEndFraction(-0.1f);
    mockAnimation->SetEndFraction(1.0f);
    mockAnimation->SetBeginFraction(0.1f);
    mockAnimation->SetPathNeedAddOrigin(true);
    mockAnimation->OnStart();
    mockAnimation->InitInterpolationValue();
    mockAnimation->OnCallFinishCallback();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    mockAnimation->Start(node);
    mockAnimation->SetRotationMode(RotationMode::ROTATE_AUTO);
    mockAnimation->SetEndFraction(1.f);
    mockAnimation->SetBeginFraction(0.1f);
    mockAnimation->SetPathNeedAddOrigin(true);
    mockAnimation->InitInterpolationValue();
    mockAnimation->OnStart();
    mockAnimation->SetAutoReverse(true);
    mockAnimation->SetRepeatCount(2);
    mockAnimation->OnUpdateStagingValue(true);
    mockAnimation->OnUpdateStagingValue(false);
    mockAnimation->SetAutoReverse(false);
    mockAnimation->SetRepeatCount(1);
    mockAnimation->OnUpdateStagingValue(true);
    mockAnimation->OnUpdateStagingValue(false);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest017 end";
}

/**
 * @tc.name: AnimationSupplementTest018
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest018 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    Vector2f startData(0.f, 1.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector2f>>(startData);
    Vector2f endData(5.f, 6.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector2f>>(endData);
    auto mockAnimation = std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    mockAnimation->InitInterpolationValue();

    std::shared_ptr<RSPathAnimationMock> firstAnimation =
        std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    std::shared_ptr<RSPathAnimationMock> secondAnimation =
        std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    struct RSSurfaceNodeConfig surfaceNodeConfig = { .SurfaceNodeName = "test" };
    std::shared_ptr<RSNode> firstNode = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> secondNode = RSSurfaceNode::Create(surfaceNodeConfig, true);
    firstAnimation->Start(firstNode);
    firstAnimation->OnStart();
    secondAnimation->Start(firstNode);
    secondAnimation->OnStart();
    EXPECT_FALSE(firstNode->HasPropertyAnimation(0));
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest018 end";
}

/**
 * @tc.name: AnimationSupplementTest019
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest019 start";
    /**
     * @tc.steps: step1. init
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto firstValue = std::make_shared<RSAnimatableProperty<float>>(2.f);
    auto value2 = std::make_shared<RSAnimatableProperty<float>>(3.f);
    std::shared_ptr<RSSpringAnimationMock> firstAnimation =
        std::make_shared<RSSpringAnimationMock>(property, firstValue);
    std::shared_ptr<RSSpringAnimationMock> secondAnimation =
        std::make_shared<RSSpringAnimationMock>(property, firstValue, value2);
    firstAnimation->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    auto curve = RSAnimationTimingCurve::CreateSpringCurve(1.f, 2.f, 3.f, 4.f);
    firstAnimation->SetTimingCurve(curve);
    firstAnimation->SetIsCustom(true);
    firstAnimation->OnStart();
    firstAnimation->SetIsCustom(false);
    firstAnimation->OnStart();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    firstAnimation->Start(node);
    firstAnimation->SetIsCustom(true);
    firstAnimation->OnStart();
    firstAnimation->SetIsCustom(false);
    firstAnimation->OnStart();

    std::shared_ptr<RSSpringAnimationMock> thirdAnimation =
        std::make_shared<RSSpringAnimationMock>(property, firstValue, value2);
    struct RSSurfaceNodeConfig surfaceNodeConfig = { .SurfaceNodeName = "test" };
    std::shared_ptr<RSNode> firstNode = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> secondNode = RSSurfaceNode::Create(surfaceNodeConfig, true);
    secondAnimation->Start(firstNode);
    secondAnimation->OnStart();
    thirdAnimation->Start(firstNode);
    thirdAnimation->OnStart();
    EXPECT_FALSE(firstNode->HasPropertyAnimation(0));
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest019 end";
}

/**
 * @tc.name: AnimationSupplementTest020
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest020 start";
    /**
     * @tc.steps: step1. init
     */
    auto firstData = MyData();
    auto secondData = MyData();
    [[maybe_unused]] bool ret = firstData == secondData;

    RSImageBase imageBase;
    Parcel parcel;
    imageBase.Marshalling(parcel);

    RRectT<float> firstRect;
    RRectT<float> secondRect;
    auto tmp = firstRect * ANIMATION_END_VALUE;
    tmp *= ANIMATION_END_VALUE;
    [[maybe_unused]] bool ret1 = tmp != secondRect;
    firstRect.IsNearEqual(secondRect, ANIMATION_DEFAULT_VALUE);

    RectT<float> thirdRect;
    RectT<float> fourthRect;
    thirdRect.Intersect(fourthRect);

    RSDisplayNodeConfig config = { 0, false, 0 };
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    displayNode->Marshalling(parcel);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.EnableCacheForRotation();
    rsInterfaces.DisableCacheForRotation();

    auto mask = RSMask::Unmarshalling(parcel);
    delete mask;

    NodeId id = 1;
    RSModifierExtractor extractor(id);
    extractor.GetCameraDistance();
    extractor.GetShadowMask();
    EXPECT_TRUE(extractor.id_);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest020 end";
}

/**
 * @tc.name: AnimationSupplementTest021
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest021, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest021 start";
    /**
     * @tc.steps: step1. init
     */
    RSProperty<float> property;
    [[maybe_unused]] auto tmp = property.GetThreshold();
    property.SetValueFromRender(nullptr);

    RSModifierManager modifierManager;
    modifierManager.HasUIRunningAnimation();
    auto animatablProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto value = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto springmockAnimation = std::make_shared<RSSpringAnimationMock>(animatablProperty, value);
    modifierManager.RegisterSpringAnimation(springmockAnimation->GetPropertyId(), springmockAnimation->GetId());
    modifierManager.UnregisterSpringAnimation(springmockAnimation->GetPropertyId(), springmockAnimation->GetId());
    modifierManager.QuerySpringAnimation(springmockAnimation->GetPropertyId());
    modifierManager.FlushStartAnimation(0);

    std::string str;
    RSMotionPathOption option(str);
    option.GetRotationMode();
    option.GetPathNeedAddOrigin();

    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.colorMode_, mode);

    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest021 end";
}

/**
 * @tc.name: AnimationSupplementTest022
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest022, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest022 start";
    /**
     * @tc.steps: step1. init
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    property->GetStagingValue();
    auto stagingValue = property->Get();
    property->Set(stagingValue);
    property->GetShowingValueAndCancelAnimation();
    property->RequestCancelAnimation();
    auto propertyUnit_ { RSPropertyUnit::UNKNOWN };
    property->SetPropertyUnit(propertyUnit_);
    auto base = std::make_shared<RSRenderProperty<bool>>();
    base->SetModifierType(RSModifierType::BOUNDS);
    auto type = base->GetModifierType();
    EXPECT_TRUE(type == RSModifierType::BOUNDS);
    property->SetValueFromRender(base);
    property->SetUpdateCallback(nullptr);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetFinishCallbackType(FinishCallbackType::TIME_SENSITIVE);
    std::shared_ptr<RSPropertyBase> targetValue = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    property->AnimateWithInitialVelocity(timingProtocol, timingCurve, targetValue);

    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    node->SetShadowMask(true);
    node->IsImplicitAnimationOpen();
    node->GetChildByIndex(1);
    std::string nodeName = "nodeName";
    node->SetNodeName(nodeName);
    PropertyCallback callback;
    callback = []() {};
    node->AddDurationKeyFrame(100, timingCurve, callback); // 100 Set duration is 100ms
    NodeId id1 = 0;
    NodeId id2 = 1;
    node->RegisterTransitionPair(id1, id2, true);
    node->UnregisterTransitionPair(id1, id2);
    node->AnimateWithCurrentCallback(timingProtocol, timingCurve, callback);
    std::optional<Vector2f> vec(Vector2f(1.f, 1.f));
    node->SetSandBox(vec);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest022 end";
}

/**
 * @tc.name: AnimationSupplementTest023
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest023, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest023 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f vec1(1.f, 1.f, 1.f, 1.f);
    NodeId nodeId = 1;
    NodeId childNode = 2; // 2 is childNode
    auto node = RSProxyNode::Create(nodeId);
    auto child = RSProxyNode::Create(childNode);
    node->SetBounds(vec1);
    node->SetFrame(vec1);
    node->SetBounds(1.f, 1.f, 1.f, 1.f);
    node->SetBoundsWidth(1.f);
    node->SetBoundsHeight(1.f);
    node->SetFrame(1.f, 1.f, 1.f, 1.f);
    node->SetFramePositionX(1.f);
    node->SetFramePositionY(1.f);
    node->GetType();
    node->AddChild(child, 1);
    node->RemoveChild(child);
    node->ClearChildren();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->FlushAnimationStartTime(1);
    rsUiDirector->HasUIRunningAnimation();

    Vector4 vec = { 0.f, 0.f, 0.f, 0.f };
    vec.IsNearEqual(vec1, 1.f);
    EXPECT_TRUE(vec.IsZero());
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest023 end";
}

HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest024, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest024 start";
    /**
     * @tc.steps: step1. init
     */
    Vector2f data(1.f, 1.f);
    RRectT<float> firstRect;
    RRectT<float> secondRect;
    RectT<float> thirdRect;
    RectT<float> fourthRect;
    firstRect.SetValues(thirdRect, &data);
    secondRect.SetValues(fourthRect, &data);
    auto firstTemp = firstRect - secondRect;
    firstRect = firstTemp + secondRect;
    firstTemp = firstRect * (1.f);
    firstTemp = firstTemp / (1.f);
    firstTemp += firstRect;
    firstTemp -= firstRect;
    firstTemp *= (1.f);
    firstTemp = firstRect;
    [[maybe_unused]] bool ret = (firstTemp != secondRect);
    ret = (firstTemp == secondRect);

    auto firstData = MyData();
    auto secondData = MyData();
    auto secondTemp = firstData + secondData;
    secondTemp += firstData;
    secondTemp -= firstData;
    firstData = secondTemp - secondData;
    secondTemp = firstData * (1.f);
    secondTemp *= (1.f);

    auto data3 = MyNewData();
    auto data4 = MyNewData();
    ret = (data3 == data4);
    ret = (data3 != data4);
    EXPECT_TRUE(data3.IsEqual(data4));
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest024 end";
}

/**
 * @tc.name: AnimationSupplementTest025
 * @tc.desc: Verify the CloseImplicitCancelAnimation of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSSupplementAnimationTest, AnimationSupplementTest025, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest025 start";
    /**
     * @tc.steps: step1. init
     */
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    auto ret = implicitAnimator->CloseImplicitCancelAnimation();
    EXPECT_EQ(ret, CancelAnimationStatus::NO_OPEN_CLOSURE);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    implicitAnimator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    ret = implicitAnimator->CloseImplicitCancelAnimation();
    EXPECT_EQ(ret, CancelAnimationStatus::INCORRECT_PARAM_TYPE);

    protocol.SetDuration(0);
    implicitAnimator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    ret = implicitAnimator->CloseImplicitCancelAnimation();
    EXPECT_EQ(ret, CancelAnimationStatus::EMPTY_PENDING_SYNC_LIST);
    GTEST_LOG_(INFO) << "RSSupplementAnimationTest AnimationSupplementTest025 end";
}
#endif
} // namespace Rosen
} // namespace OHOS