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
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
class MockCmdListProperty : public RSAnimatableProperty<float> {
public:
    explicit MockCmdListProperty(const float& value) : RSAnimatableProperty<float>(value) {}
    ~MockCmdListProperty() = default;

protected:
    RSPropertyType GetPropertyType() const
    {
        return RSPropertyType::DRAW_CMD_LIST;
    }
};
} // namespace
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

#ifndef MODIFIER_NG
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
    auto rsUIContext = std::make_shared<RSUIContext>();

    auto ret = animationParam->SyncProperties(rsUIContext);
    EXPECT_TRUE(animationParam != nullptr);
    EXPECT_EQ(ret, CancelAnimationStatus::EMPTY_PENDING_SYNC_LIST);

    std::shared_ptr<RSCanvasNode> node = RSCanvasNode::Create();
    auto property = std::make_shared<RSAnimatableProperty<float>>(100.f);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    modifier->AttachToNode(node);
    animationParam->AddPropertyToPendingSyncList(property);
    ret = animationParam->SyncProperties(rsUIContext);
    EXPECT_TRUE(animationParam != nullptr);
    EXPECT_NE(ret, CancelAnimationStatus::EMPTY_PENDING_SYNC_LIST);

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
    auto rsUIContext = std::make_shared<RSUIContext>();

    std::shared_ptr<RSCanvasNode> node1 = RSCanvasNode::Create(true, false);
    auto property1 = std::make_shared<RSAnimatableProperty<float>>(100.f);
    auto startProperty1 = std::make_shared<RSAnimatableProperty<float>>(200.f);
    auto endProperty1 = std::make_shared<RSAnimatableProperty<float>>(500.f);
    auto modifier1 = std::make_shared<RSBoundsModifier>(property1);
    node1->AddModifier(modifier1);

    auto animation1 = std::make_shared<RSCurveAnimation>(property1, startProperty1, endProperty1);
    animation1->SetDuration(1000);
    animation1->SetTimingCurve(curve);
    animation1->SetFinishCallback([weakNode1 = std::weak_ptr<RSCanvasNode>(node1)]() {
        auto node1_lock = weakNode1.lock();
        if (node1_lock) {
            node1_lock->SetBoundsWidth(200);
        }
    });
    node1->AddAnimation(animation1);
    animation1->Start(node1);

    animationParam->AddPropertyToPendingSyncList(property1);
    auto ret = animationParam->SyncProperties(rsUIContext);
    EXPECT_TRUE(animationParam != nullptr);
    EXPECT_NE(ret, CancelAnimationStatus::EMPTY_PENDING_SYNC_LIST);

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
    ret = animationParam->SyncProperties(rsUIContext);
    EXPECT_TRUE(animationParam != nullptr);
    EXPECT_NE(ret, CancelAnimationStatus::EMPTY_PENDING_SYNC_LIST);

    GTEST_LOG_(INFO) << "RSImplicitAnimationParamTest SyncProperties002 end";
}
#endif

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

/**
 * @tc.name: AddKeyframe002
 * @tc.desc: Verify the RSImplicitKeyframeAnimationParam AddKeyframe
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSImplicitAnimationParamTest, AddKeyframe002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AddKeyframe002 start";

    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    float fraction = 0.1f;
    int duration = 1000;
    auto animationParam = std::make_shared<RSImplicitKeyframeAnimationParam>(
        timingProtocol, timingCurve, fraction, duration);

    int startDuration = INT32_MAX;
    auto property = std::make_shared<RSAnimatableProperty<float>>(100.f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(100.f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(200.f);

    std::shared_ptr<RSAnimation> animation = std::make_shared<RSKeyframeAnimation>(property);
    animationParam->AddKeyframe(animation, startDuration, startValue, endValue);
    EXPECT_TRUE(animationParam != nullptr);

    GTEST_LOG_(INFO) << "RSImplicitAnimationParamTest AddKeyframe002 end";
}

/**
 * @tc.name: CreateAnimation001
 * @tc.desc: Verify the RSImplicitKeyframeAnimationParam CreateAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSImplicitAnimationParamTest, CreateAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest CreateAnimation001 start";

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

    animationParam->CreateAnimation(property, true, startDuration, startValue, endValue);
    EXPECT_TRUE(animationParam != nullptr);

    GTEST_LOG_(INFO) << "RSImplicitAnimationParamTest CreateAnimation001 end";
}

/**
 * @tc.name: CreateAnimation002
 * @tc.desc: Verify the RSImplicitKeyframeAnimationParam CreateAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSImplicitAnimationParamTest, CreateAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest CreateAnimation002 start";

    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    float fraction = 0.1f;
    int duration = 1000;
    auto animationParam = std::make_shared<RSImplicitKeyframeAnimationParam>(
        timingProtocol, timingCurve, fraction, duration);

    int startDuration = INT32_MAX;
    auto property = std::make_shared<RSAnimatableProperty<float>>(100.f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(100.f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(200.f);

    animationParam->CreateAnimation(property, true, startDuration, startValue, endValue);
    EXPECT_TRUE(animationParam != nullptr);

    GTEST_LOG_(INFO) << "RSImplicitAnimationParamTest CreateAnimation002 end";
}

/**
 * @tc.name: CreateAnimation003
 * @tc.desc: Verify the RSImplicitCurveAnimationParam CreateAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSImplicitAnimationParamTest, CreateAnimation003, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSImplicitCurveAnimationParam param(timingProtocol, timingCurve);
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.f);
    ASSERT_TRUE(param.CreateAnimation(property, startValue, endValue));
}

/**
 * @tc.name: CreateAnimation004
 * @tc.desc: Verify the RSImplicitCurveAnimationParam CreateAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSImplicitAnimationParamTest, CreateAnimation004, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSImplicitCurveAnimationParam param(timingProtocol, timingCurve);
    auto property = std::make_shared<MockCmdListProperty>(1.f);
    auto startValue = std::make_shared<MockCmdListProperty>(0.f);
    auto endValue = std::make_shared<MockCmdListProperty>(1.f);
    ASSERT_TRUE(param.CreateAnimation(property, startValue, endValue));
}

/**
 * @tc.name: AddPropertyToPendingSyncListTest
 * @tc.desc: Verify the AddPropertyToPendingSyncList
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimationParamTest, AddPropertyToPendingSyncListTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AddPropertyToPendingSyncListTest start";
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSImplicitCancelAnimationParam animationParam(protocol);
    ASSERT_TRUE(animationParam.pendingSyncList_.empty());

    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    animationParam.AddPropertyToPendingSyncList(property);
    ASSERT_FALSE(animationParam.pendingSyncList_.empty());
    GTEST_LOG_(INFO) << "RSAnimationTest AddPropertyToPendingSyncListTest end";
}

/**
 * @tc.name: SyncPropertiesTest
 * @tc.desc: Verify the SyncProperties
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimationParamTest, SyncPropertiesTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest SyncPropertiesTest start";
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSImplicitCancelAnimationParam animationParam(protocol);

    // case1: pendingSyncList_ empty
    ASSERT_EQ(animationParam.SyncProperties(nullptr), CancelAnimationStatus::EMPTY_PENDING_SYNC_LIST);

    // case2: target_ is null
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    property->target_ = {};
    animationParam.AddPropertyToPendingSyncList(property);
    animationParam.SyncProperties(nullptr);
    ASSERT_TRUE(animationParam.pendingSyncList_.empty());

    // case3: node not has property animation
    auto node = RSCanvasNode::Create();
    property->target_ = node;
    ASSERT_FALSE(node->HasPropertyAnimation(property->GetId()));
    animationParam.AddPropertyToPendingSyncList(property);
    animationParam.SyncProperties(nullptr);
    ASSERT_TRUE(animationParam.pendingSyncList_.empty());

    // case4: node has property animation and property is custom
    node->animatingPropertyNum_[property->GetId()] = 1;
    property->SetIsCustom(true);
    ASSERT_TRUE(node->HasPropertyAnimation(property->GetId()));
    ASSERT_TRUE(property->GetIsCustom());
    animationParam.AddPropertyToPendingSyncList(property);
    animationParam.SyncProperties(nullptr);
    ASSERT_TRUE(animationParam.pendingSyncList_.empty());

    // case5: node has property animation and property not custom and render service node
    node->isRenderServiceNode_ = true;
    node->isTextureExportNode_ = false;
    property->SetIsCustom(false);
    ASSERT_TRUE(node->HasPropertyAnimation(property->GetId()));
    ASSERT_TRUE(node->IsRenderServiceNode());
    ASSERT_FALSE(property->GetIsCustom());
    animationParam.AddPropertyToPendingSyncList(property);
    animationParam.SyncProperties(nullptr);
    ASSERT_TRUE(animationParam.pendingSyncList_.empty());

    // case6: node has property animation and property not custom and not render service node
    node->isTextureExportNode_ = true;
    ASSERT_TRUE(node->HasPropertyAnimation(property->GetId()));
    ASSERT_FALSE(node->IsRenderServiceNode());
    ASSERT_FALSE(property->GetIsCustom());
    animationParam.AddPropertyToPendingSyncList(property);
    animationParam.SyncProperties(nullptr);
    ASSERT_TRUE(animationParam.pendingSyncList_.empty());

    GTEST_LOG_(INFO) << "RSAnimationTest SyncPropertiesTest end";
}

/**
 * @tc.name: ExecuteSyncPropertiesTaskTest
 * @tc.desc: Verify the ExecuteSyncPropertiesTask
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimationParamTest, ExecuteSyncPropertiesTaskTest, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSImplicitCancelAnimationParam animationParam(protocol);

    // case1: rsUIContext is null
    std::shared_ptr<RSUIContext> rsUIContext = nullptr;
    {
        RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap propertiesMap;
        ASSERT_FALSE(animationParam.ExecuteSyncPropertiesTask(std::move(propertiesMap), false, rsUIContext));
    }

    // case2: rsUIContext not null
    rsUIContext = std::make_shared<RSUIContext>();
    {
        RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap propertiesMap;
        ASSERT_FALSE(animationParam.ExecuteSyncPropertiesTask(std::move(propertiesMap), false, rsUIContext));
    }

    auto node = RSCanvasNode::Create();
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto renderProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.f);
    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap propertiesMapValue;
    propertiesMapValue.emplace(std::make_pair<NodeId, PropertyId>(node->GetId(), property->GetId()),
        std::make_pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>(
            renderProperty, node->GetAnimationByPropertyId(property->GetId())));
    propertiesMapValue.emplace(std::make_pair<NodeId, PropertyId>(node->GetId(), property->GetId()),
        std::make_pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>(
            nullptr, node->GetAnimationByPropertyId(property->GetId())));

    {
        RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap propertiesMap = propertiesMapValue;
        ASSERT_FALSE(animationParam.ExecuteSyncPropertiesTask(std::move(propertiesMap), false, rsUIContext));
    }

    // case3: in node map
    auto& nodeMap = rsUIContext->GetMutableNodeMap();
    nodeMap.RegisterNode(node);
    {
        RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap propertiesMap = propertiesMapValue;
        ASSERT_FALSE(animationParam.ExecuteSyncPropertiesTask(std::move(propertiesMap), false, rsUIContext));
    }

    // cas4: node has property
    node->properties_[property->GetId()] = property;
    {
        RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap propertiesMap = propertiesMapValue;
        ASSERT_FALSE(animationParam.ExecuteSyncPropertiesTask(std::move(propertiesMap), false, rsUIContext));
    }
}

/**
 * @tc.name: CreateEmptyAnimationTest
 * @tc.desc: Verify the CreateEmptyAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimationParamTest, CreateEmptyAnimationTest, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSImplicitCancelAnimationParam animationParam(protocol);

    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.f);

    ASSERT_TRUE(animationParam.CreateEmptyAnimation(property, startValue, endValue));
}

/**
 * @tc.name: CreateAnimationTest
 * @tc.desc: Verify the CreateAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimationParamTest, CreateAnimationTest, TestSize.Level1)
{
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    std::string path = "L150 0 L50 50";
    auto option = std::make_shared<RSMotionPathOption>(path);
    RSImplicitPathAnimationParam animationParam(protocol, curve, option);

    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.f);
    ASSERT_TRUE(animationParam.CreateAnimation(property, startValue, endValue));
}
} // namespace OHOS::Rosen
