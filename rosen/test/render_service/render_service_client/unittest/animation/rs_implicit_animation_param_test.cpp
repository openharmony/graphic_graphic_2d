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

#include "gtest/gtest.h"
#include "animation/rs_animation.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_motion_path_option.h"
#include "modifier/rs_property.h"
#include "ui/rs_node.h"
#include "ui/rs_canvas_node.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "modifier/rs_property_modifier.h"
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImplicitAnimationParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImplicitAnimationParamTest::SetUpTestCase() {}
void RSImplicitAnimationParamTest::TearDownTestCase() {}
void RSImplicitAnimationParamTest::SetUp() {}
void RSImplicitAnimationParamTest::TearDown() {}

/**
 * @tc.name: ApplyTimingProtocolTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimationParamTest, ApplyTimingProtocolTest, Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSImplicitAnimationParam rsImplicitAnimationParam(timingProtocol, ImplicitAnimationParamType::CURVE);
    auto animation = std::make_shared<RSAnimation>();
    rsImplicitAnimationParam.ApplyTimingProtocol(animation);
    ASSERT_NE(animation, nullptr);
}

/**
 * @tc.name: SyncProperties001
 * @tc.desc: Verify the RSImplicitCancelAnimationParam SyncProperties
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSImplicitAnimationParamTest, SyncProperties001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest SyncProperties001 start";

    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animationParam = std::make_shared<RSImplicitCancelAnimationParam>(protocol);

    animationParam->SyncProperties();
    EXPECT_TRUE(animationParam != nullptr);

    std::shared_ptr<RSCanvasNode> node = RSCanvasNode::Create();
    auto property = std::make_shared<RSAnimatableProperty<float>>(100.f);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    modifier->AttachToNode(node);
    animationParam->AddPropertyToPendingSyncList(property);
    animationParam->SyncProperties();
    EXPECT_TRUE(animationParam != nullptr);

    GTEST_LOG_(INFO) << "RSAnimationTest SyncProperties001 end";
}

/**
 * @tc.name: SyncProperties002
 * @tc.desc: Verify the RSImplicitCancelAnimationParam SyncProperties
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSImplicitAnimationParamTest, SyncProperties002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimationParamTest SyncProperties002 start";

    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animationParam = std::make_shared<RSImplicitCancelAnimationParam>(protocol);

    std::shared_ptr<RSCanvasNode> node1 = RSCanvasNode::Create(true, false);
    auto property1 = std::make_shared<RSAnimatableProperty<float>>(100.f);
    auto startProperty1 = std::make_shared<RSAnimatableProperty<float>>(200.f);
    auto endProperty1 = std::make_shared<RSAnimatableProperty<float>>(500.f);
    auto modifier1 = std::make_shared<RSBoundsModifier>(property1);
    node1->AddModifier(modifier1);

    auto animation1 = std::make_shared<RSCurveAnimation>(property1, startProperty1, endProperty1);
    animation1->SetDuration(1000);
    animation1->SetTimingCurve(curve);
    animation1->SetFinishCallback([&]() { node1->SetBoundsWidth(200); });
    node1->AddAnimation(animation1);
    animation1->Start(node1);

    animationParam->AddPropertyToPendingSyncList(property1);
    animationParam->SyncProperties();
    EXPECT_TRUE(animationParam != nullptr);

    std::shared_ptr<RSCanvasNode> node2 = RSCanvasNode::Create(false, true);
    auto property2 = std::make_shared<RSAnimatableProperty<float>>(0.5f);
    auto startProperty2 = std::make_shared<RSAnimatableProperty<float>>(0.5f);
    auto endProperty2 = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto modifier2 = std::make_shared<RSAlphaModifier>(property2);
    node2->AddModifier(modifier2);

    auto animation2 = std::make_shared<RSCurveAnimation>(property2, startProperty2, endProperty2);
    animation2->SetDuration(1000);
    animation2->SetTimingCurve(curve);
    std::shared_ptr<AnimationFinishCallback> finishCallback = nullptr;
    animation2->SetFinishCallback(finishCallback);
    node2->AddAnimation(animation2);
    animation2->Start(node2);

    animationParam->AddPropertyToPendingSyncList(property2);
    animationParam->SyncProperties();
    EXPECT_TRUE(animationParam != nullptr);

    GTEST_LOG_(INFO) << "RSImplicitAnimationParamTest SyncProperties002 end";
}

/**
 * @tc.name: AddKeyframe001
 * @tc.desc: Verify the RSImplicitKeyframeAnimationParam AddKeyframe
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSImplicitAnimationParamTest, AddKeyframe001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AddKeyframe001 start";

    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    float fraction = 0.1f;
    int duration = 1000;
    auto animationParam = std::make_shared<RSImplicitKeyframeAnimationParam>(
        timingProtocol, timingCurve, fraction, duration);

    int startDuration = 500;
    auto property = std::make_shared<RSAnimatableProperty<float>>(100.f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(100.f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(200.f);
    std::shared_ptr<RSAnimation> animation = nullptr;
    animationParam->AddKeyframe(animation, startDuration, startValue, endValue);
    EXPECT_TRUE(animationParam != nullptr);

    std::shared_ptr<RSAnimation> animation1 = std::make_shared<RSKeyframeAnimation>(property);
    animationParam->AddKeyframe(animation1, startDuration, startValue, endValue);
    EXPECT_TRUE(animationParam != nullptr);

    GTEST_LOG_(INFO) << "RSImplicitAnimationParamTest AddKeyframe001 end";
}
} // namespace OHOS::Rosen
