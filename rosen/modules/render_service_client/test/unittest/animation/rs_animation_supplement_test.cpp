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

#include "rs_animation_base_test.h"
#include "rs_animation_test_utils.h"

#include "animation/rs_animation_callback.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_spring_animation.h"
#include "animation/rs_transition.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_render_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "render/rs_path.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSAnimationTest : public RSAnimationBaseTest {
};

class RSAnimationMock : public RSAnimation {
public:
    RSAnimationMock() : RSAnimation() {}
    ~RSAnimationMock() = default;
    void StartInner(const std::shared_ptr<RSNode>& target)
    {
        RSAnimation::StartInner(target);
    }

    void OnStart() override
    {
        RSAnimation::OnStart();
    }

    void OnReverse() override
    {
        RSAnimation::OnReverse();
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

    void OnSetFraction(float fraction) override
    {
        RSAnimation::OnSetFraction(fraction);
    }

    void OnUpdateStagingValue(bool isFirstStart) override
    {
        RSAnimation::OnUpdateStagingValue(isFirstStart);
    }

    PropertyId GetPropertyId() const override
    {
        return RSAnimation::GetPropertyId();
    }
};

class RSRenderAnimationMock : public RSRenderAnimation {
public:
    RSRenderAnimationMock(AnimationId id) : RSRenderAnimation(id) {}
    ~RSRenderAnimationMock() = default;
};

/**
 * @tc.name: AnimationSupplementTest001
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest001 start";
    /**
     * @tc.steps: step1. init
     */
    auto animation = std::make_shared<RSAnimationMock>();
    std::function<void()> callback = nullptr;
    animation->SetFinishCallback(callback);
    callback = []() {
        std::cout << "RSAnimationTest AnimationSupplementTest001 callback" << std::endl;
    };
    animation->SetFinishCallback(callback);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest001 end";
}

/**
 * @tc.name: AnimationSupplementTest002
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest002 start";
    /**
     * @tc.steps: step1. init
     */
    auto animation = std::make_shared<RSAnimationMock>();
    std::shared_ptr<RSNode> node = nullptr;
    animation->StartInner(node);
    animation->Pause();
    animation->OnPause();
    animation->Resume();
    animation->OnResume();
    animation->Finish();
    animation->OnFinish();
    animation->Reverse();
    animation->OnReverse();
    node = RSCanvasNode::Create();
    node->AddAnimation(animation);
    animation->Pause();
    animation->OnPause();
    animation->Resume();
    animation->OnResume();
    animation->Finish();
    animation->OnFinish();
    animation->Reverse();
    animation->OnReverse();
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest002 end";
}

/**
 * @tc.name: AnimationSupplementTest003
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest003 start";
    /**
     * @tc.steps: step1. init
     */
    auto animation = std::make_shared<RSAnimationMock>();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    node->AddAnimation(animation);
    animation->SetFraction(-1.0f);
    animation->SetFraction(0.5f);
    animation->Pause();
    animation->SetFraction(0.5f);
    animation->OnSetFraction(0.5f);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest003 end";
}

/**
 * @tc.name: AnimationSupplementTest004
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest004 start";
    /**
     * @tc.steps: step1. init
     */
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    auto animations = implicitAnimator->CloseImplicitAnimation();
    EXPECT_TRUE(animations.empty());
    implicitAnimator->BeginImplicitKeyFrameAnimation(0.1f, RSAnimationTimingCurve::LINEAR);
    implicitAnimator->BeginImplicitKeyFrameAnimation(0.2f);
    std::shared_ptr<RSMotionPathOption> motionPathOption;
    implicitAnimator->BeginImplicitPathAnimation(motionPathOption);
    implicitAnimator->EndImplicitPathAnimation();
    std::shared_ptr<const RSTransitionEffect> effect;
    implicitAnimator->BeginImplicitTransition(effect);
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
    implicitAnimator->CloseImplicitAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest004 end";
}

class RSPathAnimationMock : public RSPathAnimation {
public:
    RSPathAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPath>& animationPath)
        : RSPathAnimation(property, animationPath)
    {}
    RSPathAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::string& path,
        const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue)
        : RSPathAnimation(property, path, startValue, endValue) {}
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

/**
 * @tc.name: AnimationSupplementTest005
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest005 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    std::shared_ptr<RSPath> path;
    auto animation = std::make_shared<RSPathAnimationMock>(property, path);
    EXPECT_TRUE(animation != nullptr);
    animation->SetBeginFraction(-0.1f);
    animation->SetEndFraction(-0.1f);
    animation->SetPathNeedAddOrigin(true);
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animation->Start(node);
    animation->SetPathNeedAddOrigin(true);
    animation->InitInterpolationValue();

    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(5.f, 6.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    auto animation2 = std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    animation2->InitInterpolationValue();
    animation2->OnUpdateStagingValue(true);
    animation2->SetDirection(false);
    animation2->Start(node);
    animation2->OnUpdateStagingValue(false);
    animation2->SetAutoReverse(true);
    animation2->SetRepeatCount(2);
    animation2->OnUpdateStagingValue(false);

    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(10.f);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    node->AddModifier(rotationModifier);
    animation2->GetRotationPropertyId(node);
    animation2->SetRotation(node, 1.f);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest005 end";
}

/**
 * @tc.name: AnimationSupplementTest006
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest006 start";
    /**
     * @tc.steps: step1. init
     */
    auto animationManager = std::make_shared<RSUIAnimationManager>();
    auto animation = std::make_shared<RSAnimationMock>();
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>(animation->GetId());
    EXPECT_TRUE(animation != nullptr);
    animationManager->RemoveAnimation(animation->GetId());
    animationManager->AddAnimation(renderAnimation, animation);
    animationManager->RemoveAnimation(animation->GetId());
    animationManager->AddAnimation(renderAnimation, animation);
    animationManager->AddAnimation(renderAnimation, animation);
    auto animation2 = std::make_shared<RSAnimationMock>();
    animationManager->RemoveAnimation(animation2->GetId());
    animationManager->RemoveAnimation(animation->GetId());
    auto uiProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto renderProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.f, uiProperty->GetId());
    animationManager->AddAnimatableProp(uiProperty->GetId(), uiProperty, renderProperty);
    auto propertyGet = animationManager->GetRenderProperty(uiProperty->GetId());
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest006 end";
}

/**
 * @tc.name: AnimationSupplementTest007
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest007 start";
    /**
     * @tc.steps: step1. init
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto keyframeParam = std::make_shared<RSImplicitKeyframeAnimationParam>(
        protocol, RSAnimationTimingCurve::LINEAR, 0.1f);
    EXPECT_TRUE(keyframeParam != nullptr);
    std::shared_ptr<RSAnimation> animation;
    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(5.f, 6.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    keyframeParam->AddKeyframe(animation, startValue, endValue);
    animation = std::make_shared<RSKeyframeAnimation>(startValue);
    keyframeParam->AddKeyframe(animation, startValue, endValue);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest007 end";
}

/**
 * @tc.name: AnimationSupplementTest008
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest008 start";
    /**
     * @tc.steps: step1. init
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    std::shared_ptr<RSMotionPathOption> option;
    auto pathParam = std::make_shared<RSImplicitPathAnimationParam>(
        protocol, RSAnimationTimingCurve::LINEAR, option);
    EXPECT_TRUE(pathParam != nullptr);
    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(5.f, 6.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    auto animation = pathParam->CreateAnimation(property, startValue, endValue);

    option = std::make_shared<RSMotionPathOption>("abc");
    auto pathParam2 = std::make_shared<RSImplicitPathAnimationParam>(
        protocol, RSAnimationTimingCurve::LINEAR, option);
    auto animation2 = pathParam->CreateAnimation(property, startValue, endValue);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest008 end";
}

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

/**
 * @tc.name: AnimationSupplementTest009
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest009 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<const RSTransitionEffect> effect;
    auto animation = std::make_shared<RSTransitionMock>(effect, true);
    EXPECT_TRUE(animation != nullptr);
    animation->OnStart();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest009 end";
}

/**
 * @tc.name: AnimationSupplementTest010
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest010 start";
    /**
     * @tc.steps: step1. init
     */
    std::function<void()> callback;
    AnimationFinishCallback* finishCallback1 = new AnimationFinishCallback(callback);
    delete finishCallback1;
    callback = []() {
        std::cout << "RSAnimationTest AnimationSupplementTest010" << std::endl;
    };
    AnimationFinishCallback* finishCallback2 = new AnimationFinishCallback(callback);
    delete finishCallback2;
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest010 end";
}

} // namespace Rosen
} // namespace OHOS