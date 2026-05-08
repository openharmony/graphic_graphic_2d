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
    static constexpr uint64_t ANIMATION_ID_2 = 2223;
    static constexpr uint64_t PROPERTY_ID = 54321;
    static constexpr uint64_t PROPERTY_ID_2 = 54322;
    static constexpr uint64_t NODE_ID = 111;
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
    EXPECT_EQ(out, "Type:RSRenderCurveAnimation, ModifierType: INVALID, StartValue: , EndValue: ");
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
    EXPECT_TRUE(result);
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

class MockCmdListProperty : public RSRenderAnimatableProperty<float> {
public:
    explicit MockCmdListProperty(const float& value, const PropertyId& id)
        : RSRenderAnimatableProperty<float>(value, id)
    {}
    ~MockCmdListProperty() = default;

    RSPropertyType typeTest_ = RSPropertyType::DRAW_CMD_LIST;

protected:
    RSPropertyType GetPropertyType() const override
    {
        return typeTest_;
    }
};

/**
 * @tc.name: RSRenderCurveAnimationTest_OnAttach
 * @tc.desc: Verify the OnAttach function.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAttach001, TestSize.Level1)
{
    auto property = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockCmdListProperty>(1.0f, PROPERTY_ID);

    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ASSERT_NE(renderCurveAnimation, nullptr);
    renderCurveAnimation->AttachRenderProperty(property);
    auto target = renderCurveAnimation->GetTarget();
    EXPECT_EQ(target, nullptr);
    renderCurveAnimation->OnAttach();
}

/**
 * @tc.name: RSRenderCurveAnimationTest_OnAttach
 * @tc.desc: Verify the OnAttach function.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAttach002, TestSize.Level1)
{
    auto property = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockCmdListProperty>(1.0f, PROPERTY_ID);
    // test property type not DRAW_CMD_LIST
    property->typeTest_ = RSPropertyType::RS_COLOR;

    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ASSERT_NE(renderCurveAnimation, nullptr);
    renderCurveAnimation->AttachRenderProperty(property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    std::string nodeName = "1"; // for test
    renderNode->SetNodeName(nodeName);
    renderCurveAnimation->Attach(renderNode.get());
    auto target = renderCurveAnimation->GetTarget();
    EXPECT_NE(target, nullptr);
    renderCurveAnimation->OnAttach();
}

/**
 * @tc.name: RSRenderCurveAnimationTest_OnAttach
 * @tc.desc: Verify the OnAttach function.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAttach003, TestSize.Level1)
{
    auto property = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockCmdListProperty>(1.0f, PROPERTY_ID);

    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ASSERT_NE(renderCurveAnimation, nullptr);
    renderCurveAnimation->AttachRenderProperty(property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    std::string nodeName = "1"; // for test
    renderNode->SetNodeName(nodeName);
    renderCurveAnimation->Attach(renderNode.get());
    auto target = renderCurveAnimation->GetTarget();
    EXPECT_NE(target, nullptr);
    // test property type is DRAW_CMD_LIST
    // test preDrawCmdListAnimationId_ is 0
    auto animationManager = target->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    animationManager->preDrawCmdListAnimationId_ = 0;
    renderCurveAnimation->OnAttach();
    // test preDrawCmdListAnimationId_ > 0
    animationManager->preDrawCmdListAnimationId_ = ANIMATION_ID;
    // test animationManager->GetAnimation(ANIMATION_ID) is null
    EXPECT_EQ(animationManager->GetAnimation(ANIMATION_ID), nullptr);
    renderCurveAnimation->OnAttach();
}

/**
 * @tc.name: RSRenderCurveAnimationTest_OnAttach
 * @tc.desc: Verify the OnAttach function.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAttach004, TestSize.Level1)
{
    auto property = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockCmdListProperty>(1.0f, PROPERTY_ID);

    auto renderCurveAnimation2 =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID_2, PROPERTY_ID_2, property, property1, property2);
    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ASSERT_NE(renderCurveAnimation, nullptr);
    renderCurveAnimation->AttachRenderProperty(property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    std::string nodeName = "1"; // for test
    renderNode->SetNodeName(nodeName);
    renderCurveAnimation->Attach(renderNode.get());
    auto target = renderCurveAnimation->GetTarget();
    EXPECT_NE(target, nullptr);
    // test property type is DRAW_CMD_LIST
    // test preDrawCmdListAnimationId_ > 0
    auto animationManager = target->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    animationManager->preDrawCmdListAnimationId_ = ANIMATION_ID_2;
    animationManager->AddAnimation(renderCurveAnimation2);
    auto preAnimation = animationManager->GetAnimation(ANIMATION_ID_2);
    ASSERT_NE(preAnimation, nullptr);
    // test preAnimaton propertyId is not equal this propertyId
    EXPECT_NE(preAnimation->GetPropertyId(), renderCurveAnimation->property_->GetId());
    renderCurveAnimation->OnAttach();
}

/**
 * @tc.name: RSRenderCurveAnimationTest_OnAttach
 * @tc.desc: Verify the OnAttach function.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAttach005, TestSize.Level1)
{
    auto property = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockCmdListProperty>(1.0f, PROPERTY_ID);

    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ASSERT_NE(renderCurveAnimation, nullptr);
    renderCurveAnimation->AttachRenderProperty(property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    std::string nodeName = "1"; // for test
    renderNode->SetNodeName(nodeName);
    renderCurveAnimation->Attach(renderNode.get());
    auto target = renderCurveAnimation->GetTarget();
    EXPECT_NE(target, nullptr);
    // test property type is DRAW_CMD_LIST
    // test preAnimaton is not nullptr
    auto animationManager = target->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    animationManager->preDrawCmdListAnimationId_ = ANIMATION_ID;
    animationManager->AddAnimation(renderCurveAnimation);
    auto preAnimation = animationManager->GetAnimation(ANIMATION_ID);
    ASSERT_NE(preAnimation, nullptr);
    // test preAnimaton propertyId is equal this propertyId
    EXPECT_EQ(preAnimation->GetPropertyId(), renderCurveAnimation->property_->GetId());
    renderCurveAnimation->OnAttach();
}

/**
 * @tc.name: RSRenderCurveAnimationTest_OnAttach
 * @tc.desc: Verify the OnAttach function.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAttach006, TestSize.Level1)
{
    auto property = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockCmdListProperty>(1.0f, PROPERTY_ID);
    // test property type not DRAW_CMD_LIST
    property->typeTest_ = RSPropertyType::RS_COLOR;

    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ASSERT_NE(renderCurveAnimation, nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    std::string nodeName = "1"; // for test
    renderNode->SetNodeName(nodeName);
    renderCurveAnimation->Attach(renderNode.get());
    auto target = renderCurveAnimation->GetTarget();
    EXPECT_NE(target, nullptr);
    // test property_ is nullptr
    EXPECT_EQ(renderCurveAnimation->property_, nullptr);
    renderCurveAnimation->OnAttach();
}

/**
 * @tc.name: RSRenderCurveAnimationTest_OnAttach
 * @tc.desc: Verify OnAttach with non-null animationManager and DRAW_CMD_LIST property
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAttach007, TestSize.Level1)
{
    auto property = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockCmdListProperty>(1.0f, PROPERTY_ID);

    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ASSERT_NE(renderCurveAnimation, nullptr);
    renderCurveAnimation->AttachRenderProperty(property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    renderCurveAnimation->Attach(renderNode.get());
    // Make animationManager_ non-null by adding an animation to the node
    renderNode->AddAnimation(renderCurveAnimation);
    auto target = renderCurveAnimation->GetTarget();
    EXPECT_NE(target, nullptr);
    // animationManager is now non-null, OnAttach should proceed past null check
    renderCurveAnimation->OnAttach();
}

/**
 * @tc.name: RSRenderCurveAnimationTest_OnAttach
 * @tc.desc: Verify OnAttach with non-null animationManager, existing preDrawCmdListAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAttach008, TestSize.Level1)
{
    auto property = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockCmdListProperty>(1.0f, PROPERTY_ID);

    auto renderCurveAnimation2 =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID_2, PROPERTY_ID_2, property, property1, property2);
    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ASSERT_NE(renderCurveAnimation, nullptr);
    renderCurveAnimation->AttachRenderProperty(property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    renderCurveAnimation->Attach(renderNode.get());
    // Make animationManager_ non-null by adding animations to the node
    renderNode->AddAnimation(renderCurveAnimation);
    renderNode->AddAnimation(renderCurveAnimation2);
    auto animationManager = renderNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    // Set preDrawCmdListAnimationId_ to an existing animation
    animationManager->preDrawCmdListAnimationId_ = ANIMATION_ID_2;
    auto preAnimation = animationManager->GetAnimation(ANIMATION_ID_2);
    ASSERT_NE(preAnimation, nullptr);
    // preAnimation has different propertyId, should not FinishOnCurrentPosition
    EXPECT_NE(preAnimation->GetPropertyId(), renderCurveAnimation->property_->GetId());
    renderCurveAnimation->OnAttach();
}

/**
 * @tc.name: RSRenderCurveAnimationTest_OnAttach
 * @tc.desc: Verify OnAttach with non-null animationManager, same propertyId preDrawCmdListAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderCurveAnimationTest, OnAttach009, TestSize.Level1)
{
    auto property = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockCmdListProperty>(1.0f, PROPERTY_ID);

    auto renderCurveAnimation =
        std::make_shared<RSRenderCurveAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ASSERT_NE(renderCurveAnimation, nullptr);
    renderCurveAnimation->AttachRenderProperty(property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    renderCurveAnimation->Attach(renderNode.get());
    // Make animationManager_ non-null by adding the animation to the node
    renderNode->AddAnimation(renderCurveAnimation);
    auto animationManager = renderNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    // Set preDrawCmdListAnimationId_ to the same animation (same propertyId)
    animationManager->preDrawCmdListAnimationId_ = ANIMATION_ID;
    auto preAnimation = animationManager->GetAnimation(ANIMATION_ID);
    ASSERT_NE(preAnimation, nullptr);
    // preAnimation has same propertyId, should call FinishOnCurrentPosition
    EXPECT_EQ(preAnimation->GetPropertyId(), renderCurveAnimation->property_->GetId());
    renderCurveAnimation->OnAttach();
}

} // namespace Rosen
} // namespace OHOS