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

#include "animationMock/rs_animation_callback.h"
#include "animationMock/rs_animation_group.h"
#include "animationMock/rs_animation_timing_protocol.h"
#include "animationMock/rs_curve_animation.h"
#include "animationMock/rs_keyframe_animation.h"
#include "animationMock/rs_path_animation.h"
#include "animationMock/rs_spring_animation.h"
#include "animationMock/rs_transition.h"
#include "animationMock/rs_implicit_animation_param.h"
#include "animationMock/rs_render_animation.h"
#include "animationMock/rs_keyframe_animation.h"
#include "modifier/rs_modifier_manager.h"
#include "transaction/rs_interfaces.h"
#include "render/rs_blur_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_path.h"
#include "render/rs_light_up_effect_filter.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_proxy_node.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSAnimationUnitTest : public RSAnimationBaseTest {
};

class RSAnimationTestMock : public RSAnimation {
public:
    RSAnimationTestMock() : RSAnimation() {}
    ~RSAnimationTestMock() = default;
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

    bool IsReversed() const
    {
        return RSAnimation::IsReversed();
    }

    void UpdateParamToRenderAnimation(const std::shared_ptr<RSRenderAnimation>& animationMock)
    {
        RSAnimation::UpdateParamToRenderAnimation(animationMock);
    }

    void StartCustomAnimation(const std::shared_ptr<RSRenderAnimation>& animationMock)
    {
        RSAnimation::StartCustomAnimation(animationMock);
    }

    std::string DumpAnimation() const
    {
        return RSAnimation::DumpAnimation();
    }

    void DumpAnimationInfo(std::string& dumpInfo) const override
    {
        RSAnimation::DumpAnimationInfo(dumpInfo);
    }
};

class RSRenderAnimationTestMock : public RSRenderAnimation {
public:
    explicit RSRenderAnimationTestMock(AnimationId id) : RSRenderAnimation(id) {}
    ~RSRenderAnimationTestMock() = default;
};

class RSAnimationGroupTestMock : public RSAnimationGroup {
public:
    RSAnimationGroupTestMock() : RSAnimationGroup() {}
    ~RSAnimationGroupTestMock() = default;

    void OnStart() override
    {
        RSAnimationGroup::OnStart();
    }

    void OnPause() override
    {
        RSAnimationGroup::OnPause();
    }

    void OnResume() override
    {
        RSAnimationGroup::OnResume();
    }

    void OnFinish() override
    {
        RSAnimationGroup::OnFinish();
    }

    void OnReverse() override
    {
        RSAnimationGroup::OnReverse();
    }

    void OnSetFraction(float fraction) override
    {
        RSAnimationGroup::OnSetFraction(fraction);
    }

    PropertyId GetPropertyId() const override
    {
        return RSAnimationGroup::GetPropertyId();
    }

    void StartInner(const std::shared_ptr<RSNode>& target)
    {
        RSAnimation::StartInner(target);
    }
};

class RSKeyframeAnimationTestMock : public RSKeyframeAnimation {
public:
    explicit RSKeyframeAnimationTestMock(std::shared_ptr<RSPropertyBase> property) : RSKeyframeAnimation(property) {}
    ~RSKeyframeAnimationTestMock() = default;

    void OnStart() override
    {
        RSKeyframeAnimation::OnStart();
    }

    void InitInterpolationValue() override
    {
        RSKeyframeAnimation::InitInterpolationValue();
    }
};

/**
 * @tc.name: AnimationSupplementTest001
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest001 start";
    /**
     * @tc.steps: step1. init
     */
    auto animationMock = std::make_shared<RSAnimationTestMock>();
    std::function<void()> finishCallback = nullptr;
    animationMock->SetFinishCallback(finishCallback);
    finishCallback = []() {
        std::cout << "RSAnimationUnitTest AnimationSupplementTest001 finishCallback" << std::endl;
    };
    animationMock->SetFinishCallback(finishCallback);
    EXPECT_TRUE(animationMock != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest001 end";
}

/**
 * @tc.name: AnimationSupplementTest002
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest002 start";
    /**
     * @tc.steps: step1. init
     */
    auto animationMock = std::make_shared<RSAnimationTestMock>();
    EXPECT_TRUE(!animationMock->IsStarted());
    EXPECT_TRUE(!animationMock->IsRunning());
    EXPECT_TRUE(!animationMock->IsPaused());
    EXPECT_TRUE(!animationMock->IsFinished());
    EXPECT_TRUE(!animationMock->IsReversed());
    std::shared_ptr<RSNode> node = nullptr;
    animationMock->Reverse();
    animationMock->Finish();
    animationMock->Pause();
    animationMock->Start(node);
    animationMock->StartInner(node);
    animationMock->Resume();
    animationMock->Pause();
    animationMock->OnPause();
    animationMock->Resume();
    animationMock->OnResume();
    animationMock->Finish();
    animationMock->OnFinish();
    animationMock->Reverse();
    animationMock->OnReverse();
    node = RSCanvasNode::Create();
    node->AddAnimation(animationMock);
    animationMock->Pause();
    animationMock->OnPause();
    animationMock->Resume();
    animationMock->OnResume();
    animationMock->Finish();
    animationMock->OnFinish();
    animationMock->Reverse();
    animationMock->OnReverse();
    EXPECT_TRUE(animationMock != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest002 end";
}

/**
 * @tc.name: AnimationSupplementTest003
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest003 start";
    /**
     * @tc.steps: step1. init
     */
    auto animationMock = std::make_shared<RSAnimationTestMock>();
    auto renderAnimation = std::make_shared<RSRenderAnimationTestMock>(animationMock->GetId());
    animationMock->SetFraction(-1.0f);
    animationMock->SetFraction(0.5f);
    animationMock->Pause();
    animationMock->SetFraction(0.5f);
    animationMock->OnSetFraction(0.5f);
    animationMock->StartCustomAnimation(renderAnimation);
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    node->AddAnimation(animationMock);
    animationMock->SetFraction(-1.0f);
    animationMock->SetFraction(0.5f);
    animationMock->Pause();
    animationMock->SetFraction(0.5f);
    animationMock->OnSetFraction(0.5f);
    animationMock->StartCustomAnimation(renderAnimation);
    animationMock->DumpAnimation();
    std::string dumpInfo = "";
    animationMock->DumpAnimationInfo(dumpInfo);
    EXPECT_TRUE(animationMock != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest003 end";
}

/**
 * @tc.name: AnimationSupplementTest004
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest004 start";
    /**
     * @tc.steps: step1. init
     */
    auto animator = std::make_shared<RSImplicitAnimator>();
    auto animations = animator->CloseImplicitAnimation();
    EXPECT_TRUE(animations.empty());
    animator->BeginImplicitKeyFrameAnimation(0.1f, RSAnimationTimingCurve::LINEAR);
    animator->BeginImplicitKeyFrameAnimation(0.2f);
    animator->EndImplicitKeyFrameAnimation();
    std::shared_ptr<RSMotionPathOption> motionPathOption;
    animator->BeginImplicitPathAnimation(motionPathOption);
    animator->EndImplicitPathAnimation();
    std::shared_ptr<const RSTransitionEffect> effect;
    animator->BeginImplicitTransition(effect, true);
    animator->EndImplicitTransition();
    animator->BeginImplicitTransition(effect, false);
    animator->EndImplicitTransition();
    std::shared_ptr<RSNode> node;
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<RSPropertyBase> startValue = std::make_shared<RSAnimatableProperty<float>>(2.f);
    std::shared_ptr<RSPropertyBase> endValue = std::make_shared<RSAnimatableProperty<float>>(10.0f);
    animator->CreateImplicitAnimation(node, property, startValue, endValue);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    animator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    animator->CreateImplicitAnimation(node, property, startValue, endValue);
    animator->CreateImplicitTransition(*node);
    animator->CloseImplicitAnimation();
    animator->EndImplicitKeyFrameAnimation();
    animator->EndImplicitTransition();
    animator->EndImplicitPathAnimation();

    node = RSCanvasNode::Create();
    animator->CreateImplicitAnimation(node, property, startValue, endValue);
    animator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    animator->CreateImplicitAnimation(node, property, startValue, endValue);
    animator->CreateImplicitTransition(*node);
    animator->CloseImplicitAnimation();
    animator->EndImplicitKeyFrameAnimation();
    animator->EndImplicitTransition();
    animator->EndImplicitPathAnimation();
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest004 end";
}

class RSPathAnimationTestMock : public RSPathAnimation {
public:
    RSPathAnimationTestMock(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPath>& animationPath)
        : RSPathAnimation(property, animationPath)
    {}
    RSPathAnimationTestMock(std::shared_ptr<RSPropertyBase> property, const std::string& path,
        const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue)
        : RSPathAnimation(property, path, startValue, endValue) {}
    ~RSPathAnimationTestMock() = default;

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
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest005 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    std::shared_ptr<RSPath> path;
    auto animationMock = std::make_shared<RSPathAnimationTestMock>(property, path);
    EXPECT_TRUE(animationMock != nullptr);
    animationMock->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    auto curve = animationMock->GetTimingCurve();
    animationMock->SetTimingCurve(curve);
    animationMock->SetRotationMode(RotationMode::ROTATE_AUTO);
    auto begin = animationMock->GetBeginFraction();
    auto end = animationMock->GetEndFraction();
    animationMock->SetBeginFraction(begin);
    animationMock->SetEndFraction(end);
    auto needAddOrigin = animationMock->GetPathNeedAddOrigin();
    animationMock->SetPathNeedAddOrigin(needAddOrigin);
    animationMock->OnStart();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animationMock->Start(node);
    animationMock->SetRotationMode(RotationMode::ROTATE_AUTO);
    animationMock->SetEndFraction(1.f);
    animationMock->SetBeginFraction(0.1f);
    animationMock->SetPathNeedAddOrigin(true);
    animationMock->InitInterpolationValue();
    animationMock->OnStart();

    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(5.f, 6.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    auto animationMock2 = std::make_shared<RSPathAnimationTestMock>(property, "abc", startValue, endValue);
    auto id = animationMock2->GetRotationPropertyId(node);
    animationMock2->InitRotationId(node);
    animationMock2->SetRotation(node, 1.f);
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(10.f);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    node->AddModifier(rotationModifier);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    auto alphaModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    node->AddModifier(alphaModifier);
    id = animationMock2->GetRotationPropertyId(node);
    EXPECT_TRUE(id != rotationModifier->GetPropertyId());
    animationMock2->SetRotation(node, 1.f);
    animationMock2->InitRotationId(node);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest005 end";
}

/**
 * @tc.name: AnimationSupplementTest006
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest006 start";
    /**
     * @tc.steps: step1. init
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(10.f);
    auto modifier = std::make_shared<RSAlphaModifier>(property);
    auto manager = std::make_shared<RSModifierManager>();
    manager->AddModifier(modifier);
    auto animationMock = std::make_shared<RSAnimationTestMock>();
    auto renderAnimation = std::make_shared<RSRenderAnimationTestMock>(animationMock->GetId());
    EXPECT_TRUE(animationMock != nullptr);
    manager->RemoveAnimation(animationMock->GetId());
    manager->AddAnimation(renderAnimation);
    manager->Animate(0);
    manager->Draw();
    manager->RemoveAnimation(animationMock->GetId());
    manager->AddAnimation(renderAnimation);
    manager->AddAnimation(renderAnimation);
    auto uiAnimation2 = std::make_shared<RSAnimationTestMock>();
    auto animationMock2 = std::make_shared<RSRenderAnimationTestMock>(uiAnimation2->GetId());
    manager->RemoveAnimation(uiAnimation2->GetId());
    manager->RemoveAnimation(animationMock->GetId());
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest006 end";
}

/**
 * @tc.name: AnimationSupplementTest007
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest007 start";
    /**
     * @tc.steps: step1. init
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto keyframeParam = std::make_shared<RSImplicitKeyframeAnimationParam>(
        protocol, RSAnimationTimingCurve::LINEAR, 0.1f, 0);
    EXPECT_TRUE(keyframeParam != nullptr);
    std::shared_ptr<RSAnimation> animationMock;
    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(5.f, 6.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    keyframeParam->AddKeyframe(animationMock, startValue, endValue);
    animationMock = std::make_shared<RSKeyframeAnimation>(startValue);
    keyframeParam->AddKeyframe(animationMock, startValue, endValue);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest007 end";
}

/**
 * @tc.name: AnimationSupplementTest008
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest008 start";
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
    auto animationMock = pathParam->CreateAnimation(property, startValue, endValue);

    option = std::make_shared<RSMotionPathOption>("abc");
    auto pathParam2 = std::make_shared<RSImplicitPathAnimationParam>(
        protocol, RSAnimationTimingCurve::LINEAR, option);
    auto animationMock2 = pathParam->CreateAnimation(property, startValue, endValue);

    auto springParam = std::make_shared<RSImplicitSpringAnimationParam>(protocol, RSAnimationTimingCurve::LINEAR);
    auto animationMock3 = springParam->CreateAnimation(property, startValue, endValue);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest008 end";
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
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest009 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<const RSTransitionEffect> effect;
    auto animationMock = std::make_shared<RSTransitionMock>(effect, true);
    EXPECT_TRUE(animationMock != nullptr);
    animationMock->OnStart();
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest009 end";
}

/**
 * @tc.name: AnimationSupplementTest010
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest010 start";
    /**
     * @tc.steps: step1. init
     */
    std::function<void()> finishCallback;
    AnimationFinishCallback* finishCallback1 = new AnimationFinishCallback(finishCallback);
    delete finishCallback1;
    finishCallback = []() {
        std::cout << "RSAnimationUnitTest AnimationSupplementTest010" << std::endl;
    };
    EXPECT_TRUE(finishCallback != nullptr);
    AnimationFinishCallback* finishCallback2 = new AnimationFinishCallback(finishCallback);
    delete finishCallback2;
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest010 end";
}

/**
 * @tc.name: AnimationSupplementTest011
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest011 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSAnimationTestMock> animationMock;
    std::shared_ptr<RSAnimationGroupTestMock> animationGrp = std::make_shared<RSAnimationGroupTestMock>();
    animationGrp->AddAnimation(animationMock);
    animationGrp->RemoveAnimation(animationMock);
    animationMock = std::make_shared<RSAnimationTestMock>();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animationGrp->StartInner(node);
    animationGrp->AddAnimation(animationMock);
    animationGrp->RemoveAnimation(animationMock);
    animationGrp->Pause();
    animationMock->StartInner(node);
    animationGrp->AddAnimation(animationMock);
    animationGrp->RemoveAnimation(animationMock);
    animationMock->Pause();
    animationGrp->AddAnimation(animationMock);
    animationGrp->AddAnimation(animationMock);
    animationGrp->RemoveAnimation(animationMock);
    animationGrp->RemoveAnimation(animationMock);
    auto id = animationGrp->GetPropertyId();
    EXPECT_TRUE(id == 0);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest011 end";
}

/**
 * @tc.name: AnimationSupplementTest012
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest012 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSAnimationGroupTestMock> animationGrp = std::make_shared<RSAnimationGroupTestMock>();
    animationGrp->OnStart();
    animationGrp->OnPause();
    animationGrp->OnResume();
    animationGrp->OnFinish();
    animationGrp->OnReverse();
    animationGrp->OnSetFraction(0.1f);
    std::shared_ptr<RSAnimationTestMock> animationMock = std::make_shared<RSAnimationTestMock>();
    animationGrp->AddAnimation(animationMock);
    animationGrp->OnStart();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animationGrp->StartInner(node);
    animationGrp->OnStart();
    animationGrp->OnPause();
    animationGrp->OnResume();
    animationGrp->OnFinish();
    animationGrp->OnReverse();
    animationGrp->OnSetFraction(0.1f);
    EXPECT_TRUE(animationGrp != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest012 end";
}

/**
 * @tc.name: AnimationSupplementTest013
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest013 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<RSKeyframeAnimationTestMock> animationMock =
        std::make_shared<RSKeyframeAnimationTestMock>(property);
    std::shared_ptr<RSPropertyBase> value1 = std::make_shared<RSAnimatableProperty<float>>(0.2f);
    std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>> keyframes;
    keyframes.push_back({ 0.5f, value1, RSAnimationTimingCurve::LINEAR });
    animationMock->AddKeyFrame(-0.1f, value1, RSAnimationTimingCurve::LINEAR);
    animationMock->AddKeyFrame(0.2f, value1, RSAnimationTimingCurve::LINEAR);
    animationMock->AddKeyFrames(keyframes);
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animationMock->Start(node);
    animationMock->AddKeyFrame(0.3f, value1, RSAnimationTimingCurve::LINEAR);
    animationMock->AddKeyFrames(keyframes);
    EXPECT_TRUE(animationMock != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest013 end";
}

/**
 * @tc.name: AnimationSupplementTest014
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest014 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<RSKeyframeAnimationTestMock> animationMock =
        std::make_shared<RSKeyframeAnimationTestMock>(property);
    animationMock->InitInterpolationValue();
    animationMock->OnStart();
    std::shared_ptr<RSPropertyBase> value1 = std::make_shared<RSAnimatableProperty<float>>(0.2f);
    animationMock->AddKeyFrame(0.2f, value1, RSAnimationTimingCurve::LINEAR);
    animationMock->InitInterpolationValue();
    animationMock->AddKeyFrame(0.f, value1, RSAnimationTimingCurve::LINEAR);
    animationMock->InitInterpolationValue();
    animationMock->OnStart();
    animationMock->SetIsCustom(true);
    animationMock->OnStart();
    animationMock->SetIsCustom(false);
    std::shared_ptr<RSNode> node1 = RSCanvasNode::Create();
    struct RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "test"};
    std::shared_ptr<RSNode> node2 = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> node3 = RSSurfaceNode::Create(surfaceNodeConfig, true);
    animationMock->Start(node1);
    animationMock->OnStart();
    EXPECT_TRUE(animationMock != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest014 end";
}

/**
 * @tc.name: AnimationSupplementTest015
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest015 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<RSKeyframeAnimationTestMock> animationMock1 =
        std::make_shared<RSKeyframeAnimationTestMock>(property);
    std::shared_ptr<RSKeyframeAnimationTestMock> animationMock2 =
        std::make_shared<RSKeyframeAnimationTestMock>(property);
    std::shared_ptr<RSKeyframeAnimationTestMock> animationMock3 =
        std::make_shared<RSKeyframeAnimationTestMock>(property);
    std::shared_ptr<RSNode> node1 = RSCanvasNode::Create();
    struct RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "test"};
    std::shared_ptr<RSNode> node2 = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> node3 = RSSurfaceNode::Create(surfaceNodeConfig, true);
    animationMock1->Start(node1);
    animationMock1->OnStart();
    animationMock1->SetIsCustom(true);
    animationMock1->OnStart();
    animationMock2->Start(node1);
    animationMock2->OnStart();
    animationMock2->SetIsCustom(true);
    animationMock2->OnStart();
    animationMock3->Start(node1);
    animationMock3->OnStart();
    animationMock3->SetIsCustom(true);
    animationMock3->OnStart();
    EXPECT_TRUE(animationMock1 != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest015 end";
}

/**
 * @tc.name: AnimationSupplementTest016
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest016 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(0.f, 1.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    auto animationMock = std::make_shared<RSPathAnimationTestMock>(property, "abc", startValue, endValue);
    animationMock->InitInterpolationValue();
    animationMock->OnUpdateStagingValue(true);
    animationMock->SetDirection(false);
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animationMock->Start(node);
    animationMock->OnStart();
    animationMock->OnUpdateStagingValue(false);
    animationMock->SetAutoReverse(true);
    animationMock->SetRepeatCount(2);
    animationMock->OnUpdateStagingValue(false);
    animationMock->InitInterpolationValue();
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(10.f);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    node->AddModifier(rotationModifier);
    animationMock->GetRotationPropertyId(node);
    animationMock->SetRotation(node, 1.f);
    animationMock->InitInterpolationValue();
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest016 end";
}

/**
 * @tc.name: AnimationSupplementTest017
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest017 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    std::shared_ptr<RSPath> path = std::make_shared<RSPath>();
    auto animationMock1 = std::make_shared<RSPathAnimationTestMock>(property, path);
    animationMock1->OnCallFinishCallback();
    property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    Vector2f startData(0.f, 1.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector2f>>(startData);
    Vector2f endData(5.f, 6.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector2f>>(endData);
    auto animationMock = std::make_shared<RSPathAnimationTestMock>(property, "abc", startValue, endValue);
    EXPECT_TRUE(animationMock != nullptr);
    animationMock->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    auto curve = animationMock->GetTimingCurve();
    animationMock->SetTimingCurve(curve);
    animationMock->SetRotationMode(RotationMode::ROTATE_AUTO);
    animationMock->SetBeginFraction(-0.1f);
    animationMock->SetEndFraction(-0.1f);
    animationMock->SetEndFraction(1.0f);
    animationMock->SetBeginFraction(0.1f);
    animationMock->SetPathNeedAddOrigin(true);
    animationMock->OnStart();
    animationMock->InitInterpolationValue();
    animationMock->OnCallFinishCallback();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animationMock->Start(node);
    animationMock->SetRotationMode(RotationMode::ROTATE_AUTO);
    animationMock->SetEndFraction(1.f);
    animationMock->SetBeginFraction(0.1f);
    animationMock->SetPathNeedAddOrigin(true);
    animationMock->InitInterpolationValue();
    animationMock->OnStart();
    animationMock->SetAutoReverse(true);
    animationMock->SetRepeatCount(2);
    animationMock->OnUpdateStagingValue(true);
    animationMock->OnUpdateStagingValue(false);
    animationMock->SetAutoReverse(false);
    animationMock->SetRepeatCount(1);
    animationMock->OnUpdateStagingValue(true);
    animationMock->OnUpdateStagingValue(false);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest017 end";
}

/**
 * @tc.name: AnimationSupplementTest018
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest018 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    Vector2f startData(0.f, 1.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector2f>>(startData);
    Vector2f endData(5.f, 6.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector2f>>(endData);
    auto animationMock = std::make_shared<RSPathAnimationTestMock>(property, "abc", startValue, endValue);
    animationMock->InitInterpolationValue();

    std::shared_ptr<RSPathAnimationTestMock> animationMock1 =
        std::make_shared<RSPathAnimationTestMock>(property, "abc", startValue, endValue);
    std::shared_ptr<RSPathAnimationTestMock> animationMock2 =
        std::make_shared<RSPathAnimationTestMock>(property, "abc", startValue, endValue);
    struct RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "test"};
    std::shared_ptr<RSNode> node1 = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> node2 = RSSurfaceNode::Create(surfaceNodeConfig, true);
    animationMock1->Start(node1);
    animationMock1->OnStart();
    animationMock2->Start(node1);
    animationMock2->OnStart();
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest018 end";
}

class RSSpringAnimationMock : public RSSpringAnimation {
public:
    RSSpringAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& byValue)
        : RSSpringAnimation(property, byValue) {}

    RSSpringAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue)
        : RSSpringAnimation(property, startValue, endValue) {}
    ~RSSpringAnimationMock() = default;

    void OnStart() override
    {
        RSSpringAnimation::OnStart();
    }
};

/**
 * @tc.name: AnimationSupplementTest019
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest019 start";
    /**
     * @tc.steps: step1. init
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto value1 = std::make_shared<RSAnimatableProperty<float>>(2.f);
    auto value2 = std::make_shared<RSAnimatableProperty<float>>(3.f);
    std::shared_ptr<RSSpringAnimationMock> animationMock1 = std::make_shared<RSSpringAnimationMock>(property, value1);
    std::shared_ptr<RSSpringAnimationMock> animationMock2 =
        std::make_shared<RSSpringAnimationMock>(property, value1, value2);
    animationMock1->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    auto curve = RSAnimationTimingCurve::CreateSpringCurve(1.f, 2.f, 3.f, 4.f);
    animationMock1->SetTimingCurve(curve);
    animationMock1->SetIsCustom(true);
    animationMock1->OnStart();
    animationMock1->SetIsCustom(false);
    animationMock1->OnStart();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animationMock1->Start(node);
    animationMock1->SetIsCustom(true);
    animationMock1->OnStart();
    animationMock1->SetIsCustom(false);
    animationMock1->OnStart();

    std::shared_ptr<RSSpringAnimationMock> animationMock3 =
        std::make_shared<RSSpringAnimationMock>(property, value1, value2);
    struct RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "test"};
    std::shared_ptr<RSNode> node1 = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> node2 = RSSurfaceNode::Create(surfaceNodeConfig, true);
    animationMock2->Start(node1);
    animationMock2->OnStart();
    animationMock3->Start(node1);
    animationMock3->OnStart();
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest019 end";
}

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
 * @tc.name: AnimationSupplementTest020
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest020 start";
    /**
     * @tc.steps: step1. init
     */
    auto data1 = MyData();
    auto data2 = MyData();
    [[maybe_unused]] bool ret = data1 == data2;

    RSFilter filter1;
    filter1.IsNearEqual(nullptr, ANIMATION_DEFAULT_VALUE);
    filter1.IsNearZero(ANIMATION_DEFAULT_VALUE);
    filter1.IsEqual(nullptr);
    filter1.IsEqualZero();

    RSImageBase imageBase;
    Parcel parcel;
    imageBase.Marshalling(parcel);

    RRectT<float> rect1;
    RRectT<float> rect2;
    auto tmp = rect1 * ANIMATION_END_VALUE;
    tmp *= ANIMATION_END_VALUE;
    [[maybe_unused]] bool ret1 = tmp != rect2;
    rect1.IsNearEqual(rect2, ANIMATION_DEFAULT_VALUE);

    RectT<float> rect3;
    RectT<float> rect4;
    rect3.Intersect(rect4);

    RSDisplayNodeConfig config = { 0, false, 0 };
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    displayNode->Marshalling(parcel);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.EnableCacheForRotation();
    rsInterfaces.DisableCacheForRotation();

    auto mask = RSMask::Unmarshalling(parcel);
    delete mask;

    RSLightUpEffectFilter filter3(ANIMATION_DEFAULT_VALUE);
    filter3.IsNearEqual(nullptr, ANIMATION_DEFAULT_VALUE);
    filter3.IsNearZero(ANIMATION_DEFAULT_VALUE);
    filter3.IsEqual(nullptr);
    filter3.IsEqualZero();

    NodeId id = 1;
    RSModifierExtractor extractor(id);
    extractor.GetCameraDistance();
    extractor.GetShadowMask();
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest020 end";
}

/**
 * @tc.name: AnimationSupplementTest021
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest021, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest021 start";
    /**
     * @tc.steps: step1. init
     */
    RSPropertyBase property;
    [[maybe_unused]] auto tmp = property.GetThreshold();
    property.SetValueFromRender(nullptr);

    RSModifierManager manager;
    manager.HasUIRunningAnimation();
    auto animatablProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto value = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto springAnimation = std::make_shared<RSSpringAnimationMock>(animatablProperty, value);
    manager.RegisterSpringAnimation(springAnimation->GetPropertyId(), springAnimation->GetId());
    manager.UnregisterSpringAnimation(springAnimation->GetPropertyId(), springAnimation->GetId());

    std::string str;
    RSMotionPathOption option(str);
    option.SetRotationMode(RotationMode::ROTATE_AUTO);
    option.GetRotationMode();
    option.SetPathNeedAddOrigin(true);
    option.GetPathNeedAddOrigin();

    RSBlurFilter filter(0, 0);
    filter.IsEqual(nullptr);
    filter.IsEqualZero();

    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    rsMaterialFilter.IsEqual(nullptr);
    rsMaterialFilter.IsEqualZero();

    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest021 end";
}

/**
 * @tc.name: AnimationSupplementTest022
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest022, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest022 start";
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
    auto base = std::make_shared<RSRenderPropertyBase>();
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
    node->GetChildIdByIndex(1);
    std::string nodeName = "nodeName";
    node->SetNodeName(nodeName);
    PropertyCallback finishCallback;
    finishCallback = []() {
    };
    node->AddDurationKeyFrame(100, timingCurve, finishCallback); // 100 Set duration is 100ms
    NodeId id1 = 0;
    NodeId id2 = 1;
    node->RegisterTransitionPair(id1, id2);
    node->UnregisterTransitionPair(id1, id2);
    node->AnimateWithCurrentCallback(timingProtocol, timingCurve, finishCallback);
    std::optional<Vector2f> vec(Vector2f(1.f, 1.f));
    node->SetSandBox(vec);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest022 end";
}

/**
 * @tc.name: AnimationSupplementTest023
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest023, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest023 start";
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
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest023 end";
}

/**
 * @tc.name: AnimationSupplementTest025
 * @tc.desc: Verify the CloseImplicitCancelAnimation of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationUnitTest, AnimationSupplementTest025, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest025 start";
    /**
     * @tc.steps: step1. init
     */
    bool success = false;
    auto animator = std::make_shared<RSImplicitAnimator>();
    success = animator->CloseImplicitCancelAnimation();
    EXPECT_TRUE(!success);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    animator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    success = animator->CloseImplicitCancelAnimation();
    EXPECT_TRUE(!success);

    protocol.SetDuration(0);
    animator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    success = animator->CloseImplicitCancelAnimation();
    EXPECT_TRUE(!success);
    GTEST_LOG_(INFO) << "RSAnimationUnitTest AnimationSupplementTest025 end";
}
} // namespace Rosen
} // namespace OHOS