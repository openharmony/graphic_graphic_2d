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

#include "animation/rs_render_curve_animation.h"
#include "animation/rs_steps_interpolator.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderCurveAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    static constexpr uint64_t PROPERTY_ID_2 = 54322;
};

void RSRenderCurveAnimationTest::SetUpTestCase() {}
void RSRenderCurveAnimationTest::TearDownTestCase() {}
void RSRenderCurveAnimationTest::SetUp() {}
void RSRenderCurveAnimationTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest Marshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcel;
    renderCurveAnimation->Marshalling(parcel);
    EXPECT_TRUE(renderCurveAnimation != nullptr);
    renderCurveAnimation->Attach(renderNode.get());
    renderCurveAnimation->Start();
    EXPECT_TRUE(renderCurveAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest Marshalling001 end";
}

/**
 * @tc.name: Marshalling002
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, Marshalling002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    ASSERT_TRUE(property != nullptr);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f,
        PROPERTY_ID);
    ASSERT_TRUE(property1 != nullptr);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);
    ASSERT_TRUE(property2 != nullptr);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcel;
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->UpdateFractionAfterContinue();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto result = renderCurveAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Marshalling003
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, Marshalling003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    ASSERT_TRUE(property != nullptr);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f,
        PROPERTY_ID);
    ASSERT_TRUE(property1 != nullptr);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);
    ASSERT_TRUE(property2 != nullptr);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcel;
    renderCurveAnimation->interpolator_ = nullptr;
    auto result = renderCurveAnimation->Marshalling(parcel);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Marshalling004
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, Marshalling004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    ASSERT_TRUE(property != nullptr);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f,
        PROPERTY_ID);
    ASSERT_TRUE(property1 != nullptr);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);
    ASSERT_TRUE(property2 != nullptr);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcel;
    auto result = renderCurveAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc:Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest Unmarshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcel;
    renderCurveAnimation->Marshalling(parcel);
    std::shared_ptr<RSRenderAnimation>(RSRenderCurveAnimation::Unmarshalling(parcel));
    EXPECT_TRUE(renderCurveAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest Unmarshalling001 end";
}

/**
 * @tc.name: SetInterpolator001
 * @tc.desc: Verify the SetInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, SetInterpolator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest SetInterpolator001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    EXPECT_TRUE(renderCurveAnimation != nullptr);
    auto interpolator = std::make_shared<RSStepsInterpolator>(0);
    renderCurveAnimation->UpdateFractionAfterContinue();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    renderCurveAnimation->Attach(renderNode.get());
    renderCurveAnimation->Start();
    EXPECT_TRUE(renderCurveAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest SetInterpolator001 end";
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc:Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, Unmarshalling002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest Unmarshalling002 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    Parcel parcel;
    auto animation = RSRenderCurveAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation == nullptr);
    renderCurveAnimation->Marshalling(parcel);
    animation = RSRenderCurveAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest Unmarshalling002 end";
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc:Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, Unmarshalling003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest Unmarshalling003 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    Parcel parcel;
    bool result = parcel.WriteUint64(ANIMATION_ID) &&
        parcel.WriteInt32(0) &&
        parcel.WriteInt32(0) &&
        parcel.WriteFloat(1.0f) &&
        parcel.WriteInt32(1) &&
        parcel.WriteBool(false) &&
        parcel.WriteBool(false) &&
        parcel.WriteInt32(1) &&
        parcel.WriteBool(false) &&
        parcel.WriteInt32(0) &&
        parcel.WriteInt32(1) &&
        parcel.WriteInt32(1) &&
        parcel.WriteUint64(PROPERTY_ID) &&
        RSMarshallingHelper::Marshalling(parcel, false) &&
        RSMarshallingHelper::Marshalling(parcel, property) &&
        RSMarshallingHelper::Marshalling(parcel, property) &&
        RSMarshallingHelper::Marshalling(parcel, property);
    EXPECT_TRUE(result);
    auto animation = RSRenderCurveAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation == nullptr);
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest Unmarshalling003 end";
}

/**
 * @tc.name: InitValueEstimatorTest001
 * @tc.desc: Test valueEstimator_
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, InitValueEstimatorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest InitValueEstimatorTest001 start";

    RSRenderCurveAnimation renderCurveAnimation;
    auto property = std::make_shared<RSRenderProperty<bool>>();
    renderCurveAnimation.property_ = property;
    renderCurveAnimation.InitValueEstimator();
    EXPECT_TRUE(renderCurveAnimation.valueEstimator_ == nullptr);

    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest InitValueEstimatorTest001 end";
}

/**
 * @tc.name: InitValueEstimatorTest002
 * @tc.desc: Test valueEstimator_
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, InitValueEstimatorTest002, TestSize.Level1)
{
    RSRenderCurveAnimation renderCurveAnimation;
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);

    renderCurveAnimation.AttachRenderProperty(property);
    renderCurveAnimation.InitValueEstimator();
    EXPECT_TRUE(renderCurveAnimation.valueEstimator_ != nullptr);
}

/**
 * @tc.name: DumpAnimationInfo001
 * @tc.desc: Test case for property is not null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, DumpAnimationInfo001, TestSize.Level1)
{
    RSRenderCurveAnimation animation;
    auto property = std::make_shared<RSRenderProperty<bool>>();
    animation.property_ = property;
    std::string out;
    animation.DumpAnimationInfo(out);
    EXPECT_EQ(out, "Type:RSRenderCurveAnimation, ModifierType: 0, StartValue: , EndValue: ");
}

/**
 * @tc.name: DumpAnimationInfo002
 * @tc.desc: Test case for property is null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, DumpAnimationInfo002, TestSize.Level1)
{
    RSRenderCurveAnimation animation;
    animation.property_ = nullptr;
    std::string out;
    animation.DumpAnimationInfo(out);
    EXPECT_EQ(out, "Type:RSRenderCurveAnimation, ModifierType: INVALID, StartValue: , EndValue: ");
}

/**
 * @tc.name: ParseParam001
 * @tc.desc: Verify the ParseParam
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, ParseParam001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    Parcel parcel;
    auto result = renderCurveAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
    auto renderCurveAnimation1 = std::make_shared<RSRenderCurveAnimation>();
    result = renderCurveAnimation1->ParseParam(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ParseParam002
 * @tc.desc: Verify the ParseParam
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, ParseParam002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    Parcel parcel;
    auto result = renderCurveAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
    auto renderCurveAnimation1 = std::make_shared<RSRenderCurveAnimation>();
    result = renderCurveAnimation1->ParseParam(parcel);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: OnSetFraction001
 * @tc.desc: Verify the OnSetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnSetFraction001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    float fraction = 0.1;
    renderCurveAnimation->valueEstimator_ = nullptr;
    renderCurveAnimation->OnSetFraction(fraction);
    EXPECT_FALSE(renderCurveAnimation->fractionChangeInfo_.first);
}

/**
 * @tc.name: OnSetFraction002
 * @tc.desc: Verify the OnSetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnSetFraction002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    renderCurveAnimation->AttachRenderProperty(property);
    renderCurveAnimation->InitValueEstimator();
    float fraction = 0.1;
    renderCurveAnimation->OnSetFraction(fraction);
    EXPECT_TRUE(renderCurveAnimation->fractionChangeInfo_.first);
}

/**
 * @tc.name: UpdateFractionAfterContinue001
 * @tc.desc: Verify the UpdateFractionAfterContinue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, UpdateFractionAfterContinue001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());
    renderCurveAnimation->fractionChangeInfo_ = { true, 0.1f };
    renderCurveAnimation->valueEstimator_ = nullptr;
    renderCurveAnimation->UpdateFractionAfterContinue();
    EXPECT_TRUE(renderCurveAnimation->fractionChangeInfo_.first);
}

/**
 * @tc.name: UpdateFractionAfterContinue002
 * @tc.desc: Verify the UpdateFractionAfterContinue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, UpdateFractionAfterContinue002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());
    renderCurveAnimation->fractionChangeInfo_ = { false, 0.1f };

    renderCurveAnimation->AttachRenderProperty(property);
    renderCurveAnimation->InitValueEstimator();
    renderCurveAnimation->UpdateFractionAfterContinue();
    EXPECT_TRUE(renderCurveAnimation->fractionChangeInfo_.second == 0.1f);
}

/**
 * @tc.name: UpdateFractionAfterContinue003
 * @tc.desc: Verify the UpdateFractionAfterContinue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, UpdateFractionAfterContinue003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());
    renderCurveAnimation->fractionChangeInfo_ = { true, 0.1f };

    renderCurveAnimation->AttachRenderProperty(property);
    renderCurveAnimation->InitValueEstimator();
    renderCurveAnimation->UpdateFractionAfterContinue();
    EXPECT_FALSE(renderCurveAnimation->fractionChangeInfo_.first);
}

/**
 * @tc.name: OnAnimate001
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAnimate001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    renderCurveAnimation->AttachRenderProperty(property);
    renderCurveAnimation->InitValueEstimator();
    float fraction = 0.1f;
    renderCurveAnimation->OnAnimate(fraction);
    EXPECT_EQ(renderCurveAnimation->lastValueFraction_, 0.1f);
}

/**
 * @tc.name: OnAnimate002
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAnimate002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->interpolator_ = nullptr;
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    renderCurveAnimation->AttachRenderProperty(property);
    renderCurveAnimation->InitValueEstimator();
    float fraction = 0.1f;
    renderCurveAnimation->OnAnimate(fraction);
    EXPECT_NE(renderCurveAnimation->lastValueFraction_, 0.1f);
}

/**
 * @tc.name: OnAnimate003
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAnimate003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    renderCurveAnimation->valueEstimator_ = nullptr;
    float fraction = 0.1f;
    renderCurveAnimation->OnAnimate(fraction);
    EXPECT_NE(renderCurveAnimation->lastValueFraction_, 0.1f);
}

/**
 * @tc.name: OnAnimateInner001
 * @tc.desc: Verify the OnAnimateInner
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAnimateInner001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    renderCurveAnimation->AttachRenderProperty(property);
    renderCurveAnimation->InitValueEstimator();
    float fraction = 0.1f;
    renderCurveAnimation->OnAnimateInner(fraction, renderCurveAnimation->interpolator_);
    EXPECT_EQ(renderCurveAnimation->lastValueFraction_, 0.1f);
}

/**
 * @tc.name: OnAnimateInner002
 * @tc.desc: Verify the OnAnimateInner
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAnimateInner002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->interpolator_ = nullptr;
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    renderCurveAnimation->AttachRenderProperty(property);
    renderCurveAnimation->InitValueEstimator();
    float fraction = 0.1f;
    renderCurveAnimation->OnAnimateInner(fraction, renderCurveAnimation->interpolator_);
    EXPECT_NE(renderCurveAnimation->lastValueFraction_, 0.1f);
}

/**
 * @tc.name: OnAnimateInner003
 * @tc.desc: Verify the OnAnimateInner
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAnimateInner003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    renderCurveAnimation->valueEstimator_ = nullptr;
    float fraction = 0.1f;
    renderCurveAnimation->OnAnimateInner(fraction, renderCurveAnimation->interpolator_);
    EXPECT_NE(renderCurveAnimation->lastValueFraction_, 0.1f);
}
} // namespace Rosen
} // namespace OHOS