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

#include "animation/rs_interpolator.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_property_animation.h"
#include "animation/rs_value_estimator.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_simple_draw_cmd_list.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderPropertyAnimationMock : public RSRenderPropertyAnimation {
public:
    RSRenderPropertyAnimationMock(
        AnimationId id, const PropertyId& propertyId,
        const std::shared_ptr<RSRenderPropertyBase>& originValue)
        : RSRenderPropertyAnimation(id, propertyId, originValue)
    {}
    void RebuildPropertyValue(float fraction) override {}
};

class RSRenderPropertyAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    static constexpr uint64_t PROPERTY_ID_2 = 54322;
    static constexpr uint64_t PROPERTY_ID_3 = 0;
};

void RSRenderPropertyAnimationTest::SetUpTestCase() {}
void RSRenderPropertyAnimationTest::TearDownTestCase() {}
void RSRenderPropertyAnimationTest::SetUp() {}
void RSRenderPropertyAnimationTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, Marshalling001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    Parcel parcel;
    renderPropertyAnimation->Marshalling(parcel);
    renderPropertyAnimation->Attach(renderNode.get());
    renderPropertyAnimation->Start();
    EXPECT_TRUE(renderPropertyAnimation->IsRunning());
}

/**
 * @tc.name: SetPropertyValue001
 * @tc.desc: Verify the SetPropertyValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, SetPropertyValue001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    auto animationValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    renderPropertyAnimation->SetPropertyValue(animationValue);
    renderPropertyAnimation->Attach(renderNode.get());
    renderPropertyAnimation->Start();
    EXPECT_TRUE(renderPropertyAnimation->IsRunning());
}

/**
 * @tc.name: GetPropertyValue001
 * @tc.desc: Verify the GetPropertyValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, GetPropertyValue001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto animationValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    renderPropertyAnimation->SetPropertyValue(animationValue);
    auto propertyValue = renderPropertyAnimation->GetPropertyValue();
    auto lastValue = renderPropertyAnimation->GetLastValue();
    EXPECT_NE(propertyValue, nullptr);
    EXPECT_NE(lastValue, nullptr);
    renderPropertyAnimation->property_ = nullptr;
    propertyValue = renderPropertyAnimation->GetPropertyValue();
    EXPECT_NE(propertyValue, nullptr);
    renderPropertyAnimation->lastValue_ = nullptr;
    propertyValue = renderPropertyAnimation->GetPropertyValue();
    EXPECT_EQ(propertyValue, nullptr);
    propertyValue = propertyValue + property;
    EXPECT_NE(property, nullptr);
    EXPECT_EQ(propertyValue, nullptr);
    propertyValue += property;
    EXPECT_EQ(propertyValue, nullptr);
}

/**
 * @tc.name: GetAnimationValue001
 * @tc.desc: Verify the GetAnimationValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, GetAnimationValue001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto animationValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    auto newAnimationValue = renderPropertyAnimation->GetAnimationValue(animationValue);
    EXPECT_NE(newAnimationValue, nullptr);
    animationValue = nullptr;
    newAnimationValue = renderPropertyAnimation->GetAnimationValue(animationValue);
    EXPECT_EQ(newAnimationValue, nullptr);
}

/**
 * @tc.name: ProcessAnimateVelocityUnderAngleRotation001
 * @tc.desc: Verify the ProcessAnimateVelocityUnderAngleRotation
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, ProcessAnimateVelocityUnderAngleRotation001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto renderPropertyAnimation2 = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto animationValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    float frameInterval = 17.0f;
    float frameInterval2 = 0.0f;

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    renderPropertyAnimation->AttachRenderProperty(animationValue);
    renderPropertyAnimation->RecordLastAnimateValue();
    renderPropertyAnimation->ProcessAnimateVelocityUnderAngleRotation(frameInterval);
    renderPropertyAnimation->Start();
    EXPECT_TRUE(renderPropertyAnimation->IsRunning());

    EXPECT_TRUE(renderPropertyAnimation2 != nullptr);
    renderPropertyAnimation2->AttachRenderProperty(animationValue);
    renderPropertyAnimation2->RecordLastAnimateValue();
    renderPropertyAnimation2->ProcessAnimateVelocityUnderAngleRotation(frameInterval2);
    renderPropertyAnimation2->Start();
    EXPECT_TRUE(renderPropertyAnimation2->IsRunning());
}

/**
 * @tc.name: RSRenderPropertyAnimation_Constructor001
 * @tc.desc: Verify the RSRenderPropertyAnimation_Constructor originValue is not null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, RSRenderPropertyAnimation_Constructor001, TestSize.Level1)
{
    auto originValue = std::make_shared<RSRenderAnimatableProperty<float>>(0);
    RSRenderPropertyAnimationMock animation(ANIMATION_ID, PROPERTY_ID, originValue);
    EXPECT_NE(animation.GetOriginValue(), nullptr);
    EXPECT_NE(animation.GetLastValue(), nullptr);
}

/**
 * @tc.name: RSRenderPropertyAnimation_Constructor002
 * @tc.desc: Verify the RSRenderPropertyAnimation_Constructor originValue is null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, RSRenderPropertyAnimation_Constructor002, TestSize.Level1)
{
    RSRenderPropertyAnimationMock animation(ANIMATION_ID, PROPERTY_ID, nullptr);
    EXPECT_NE(animation.GetOriginValue(), nullptr);
    EXPECT_NE(animation.GetLastValue(), nullptr);
}

/**
 * @tc.name: RSRenderPropertyAnimation_DumpAnimationInfo001
 * @tc.desc: Verify the RSRenderPropertyAnimation_DumpAnimationInfo001
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, RSRenderPropertyAnimation_DumpAnimationInfo001, TestSize.Level1)
{
    RSRenderPropertyAnimationMock animation(ANIMATION_ID, PROPERTY_ID, nullptr);
    std::string out1;
    animation.DumpAnimationInfo(out1);
    EXPECT_EQ(out1, "Type:RSRenderPropertyAnimation, ModifierType: INVALID");
    auto prop = std::make_shared<RSRenderProperty<float>>();
    animation.property_ = prop;
    std::string out2;
    animation.DumpAnimationInfo(out2);
    EXPECT_EQ(out2, "Type:RSRenderPropertyAnimation, ModifierType: INVALID");
}

/**
 * @tc.name: GetType001
 * @tc.desc: Verify GetType returns PROPERTY_ANIMATION for RSRenderPropertyAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, GetType001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property);
    EXPECT_EQ(renderPropertyAnimation->GetType(), RSRenderAnimationType::PROPERTY_ANIMATION);
}

/**
 * @tc.name: OnRestart001
 * @tc.desc: Verify OnRestart with null originValue and null valueEstimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, OnRestart001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest OnRestart001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto animation = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property);
    // Explicitly null out to test the null-skip branches of OnRestart
    animation->originValue_ = nullptr;
    animation->valueEstimator_ = nullptr;
    animation->lastValue_ = nullptr;
    animation->OnRestart();
    // Both branches skipped: originValue_ null -> no clone; estimator null -> no ResetLastValue
    EXPECT_EQ(animation->originValue_, nullptr);
    EXPECT_EQ(animation->valueEstimator_, nullptr);
    EXPECT_EQ(animation->lastValue_, nullptr);
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest OnRestart001 end";
}

/**
 * @tc.name: OnRestart002
 * @tc.desc: Verify OnRestart with valid originValue and null valueEstimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, OnRestart002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest OnRestart002 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto originValue = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    auto animation = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, originValue);
    animation->lastValue_ = std::make_shared<RSRenderAnimatableProperty<float>>(10.0f);
    animation->valueEstimator_ = nullptr;
    // originValue_ non-null -> lastValue_ = originValue_->Clone()
    animation->OnRestart();
    EXPECT_NE(animation->lastValue_, nullptr);
    EXPECT_NE(animation->lastValue_, originValue); // should be a clone, not same pointer
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest OnRestart002 end";
}

/**
 * @tc.name: OnRestart003
 * @tc.desc: Verify OnRestart with null originValue and valid valueEstimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, OnRestart003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest OnRestart003 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto animation = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property);
    animation->originValue_ = nullptr;
    auto estimator = std::make_shared<RSCurveValueEstimator<float>>();
    animation->valueEstimator_ = estimator;
    // originValue_ null -> skip clone; valueEstimator_ non-null -> call ResetLastValue
    animation->OnRestart();
    EXPECT_EQ(animation->originValue_, nullptr);
    EXPECT_NE(animation->valueEstimator_, nullptr);
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest OnRestart003 end";
}

/**
 * @tc.name: OnRestart004
 * @tc.desc: Verify OnRestart with valid originValue and valid valueEstimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, OnRestart004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest OnRestart004 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto originValue = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    auto animation = std::make_shared<RSRenderPropertyAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, originValue);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(2.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(8.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(8.0f);
    auto estimator = std::make_shared<RSCurveValueEstimator<float>>();
    estimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    animation->valueEstimator_ = estimator;
    animation->SetAdditive(true);
    // Both non-null -> clone lastValue_ and call ResetLastValue
    animation->OnRestart();
    EXPECT_NE(animation->lastValue_, nullptr);
    EXPECT_NE(animation->lastValue_, originValue);
    // ResetLastValue should have set lastValue_ back to startValue_ (2.0f)
    EXPECT_FLOAT_EQ(estimator->lastValue_, 2.0f);
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest OnRestart004 end";
}

} // namespace Rosen
} // namespace OHOS