/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
class RSRenderCurveAnimationUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 46465;
    static constexpr uint64_t PROPERTY_ID = 9878897;
    static constexpr uint64_t PROPERTY_ID_2 = 121215;
};

void RSRenderCurveAnimationUnitTest::SetUpTestCase() {}
void RSRenderCurveAnimationUnitTest::TearDownTestCase() {}
void RSRenderCurveAnimationUnitTest::SetUp() {}
void RSRenderCurveAnimationUnitTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest Marshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.1f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcelUt;
    renderCurveAnimation->Marshalling(parcelUt);
    EXPECT_TRUE(renderCurveAnimation != nullptr);
    renderCurveAnimation->Attach(renderNode.get());
    renderCurveAnimation->Start();
    EXPECT_TRUE(renderCurveAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest Marshalling001 end";
}

/**
 * @tc.name: Marshalling002
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, Marshalling002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    ASSERT_TRUE(property != nullptr);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    ASSERT_TRUE(property1 != nullptr);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    ASSERT_TRUE(property2 != nullptr);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcelUt;
    auto interpolatorUt = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->UpdateFractionAfterContinue();
    renderCurveAnimation->SetInterpolator(interpolatorUt);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto result = renderCurveAnimation->Marshalling(parcelUt);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Marshalling003
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, Marshalling003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    ASSERT_TRUE(property != nullptr);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    ASSERT_TRUE(property1 != nullptr);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.6f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    ASSERT_TRUE(property2 != nullptr);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcelUt;
    renderCurveAnimation->interpolator_ = nullptr;
    auto result = renderCurveAnimation->Marshalling(parcelUt);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Marshalling004
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, Marshalling004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_ID, RSRenderPropertyType::INVALID);
    ASSERT_TRUE(property != nullptr);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f,
        PROPERTY_ID, RSRenderPropertyType::INVALID);
    ASSERT_TRUE(property1 != nullptr);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.6f,
        PROPERTY_ID, RSRenderPropertyType::INVALID);
    ASSERT_TRUE(property2 != nullptr);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcelUt;
    auto result = renderCurveAnimation->Marshalling(parcelUt);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc:Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest Unmarshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.5f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcelUt;
    renderCurveAnimation->Marshalling(parcelUt);
    std::shared_ptr<RSRenderAnimation>(RSRenderCurveAnimation::Unmarshalling(parcelUt));
    EXPECT_TRUE(renderCurveAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest Unmarshalling001 end";
}

/**
 * @tc.name: SetInterpolator001
 * @tc.desc: Verify the SetInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, SetInterpolator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest SetInterpolator001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.6f);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    EXPECT_TRUE(renderCurveAnimation != nullptr);
    auto interpolatorUt = std::make_shared<RSStepsInterpolator>(0);
    renderCurveAnimation->UpdateFractionAfterContinue();
    renderCurveAnimation->SetInterpolator(interpolatorUt);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    renderCurveAnimation->Attach(renderNode.get());
    renderCurveAnimation->Start();
    EXPECT_TRUE(renderCurveAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest SetInterpolator001 end";
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc:Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, Unmarshalling002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest Unmarshalling002 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.7f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    Parcel parcelUt;
    auto animation = RSRenderCurveAnimation::Unmarshalling(parcelUt);
    EXPECT_TRUE(animation == nullptr);
    renderCurveAnimation->Marshalling(parcelUt);
    animation = RSRenderCurveAnimation::Unmarshalling(parcelUt);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest Unmarshalling002 end";
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc:Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, Unmarshalling003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest Unmarshalling003 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    Parcel parcelUt;
    bool result = parcelUt.WriteUint64(ANIMATION_ID) &&
        parcelUt.WriteInt32(0) &&
        parcelUt.WriteInt32(0) &&
        parcelUt.WriteFloat(1.0f) &&
        parcelUt.WriteInt32(1) &&
        parcelUt.WriteBool(false) &&
        parcelUt.WriteBool(false) &&
        parcelUt.WriteInt32(1) &&
        parcelUt.WriteBool(false) &&
        parcelUt.WriteInt32(0) &&
        parcelUt.WriteInt32(1) &&
        parcelUt.WriteInt32(1) &&
        parcelUt.WriteUint64(PROPERTY_ID) &&
        RSMarshallingHelper::Marshalling(parcelUt, false) &&
        RSRenderPropertyBase::Marshalling(parcelUt, property) &&
        RSRenderPropertyBase::Marshalling(parcelUt, property) &&
        RSRenderPropertyBase::Marshalling(parcelUt, property);
    EXPECT_TRUE(result);
    auto animation = RSRenderCurveAnimation::Unmarshalling(parcelUt);
    EXPECT_TRUE(animation == nullptr);
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest Unmarshalling003 end";
}

/**
 * @tc.name: InitValueEstimatorTest001
 * @tc.desc: Test valueEstimator_
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, InitValueEstimatorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest InitValueEstimatorTest001 start";

    RSRenderCurveAnimation renderCurveAnimation;
    auto propertyUt = std::make_shared<RSRenderPropertyBase>();
    renderCurveAnimation.property_ = propertyUt;
    renderCurveAnimation.InitValueEstimator();
    EXPECT_TRUE(renderCurveAnimation.valueEstimator_ == nullptr);

    GTEST_LOG_(INFO) << "RSRenderCurveAnimationUnitTest InitValueEstimatorTest001 end";
}

/**
 * @tc.name: InitValueEstimatorTest002
 * @tc.desc: Test valueEstimator_
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, InitValueEstimatorTest002, TestSize.Level1)
{
    RSRenderCurveAnimation renderCurveAnimation;
    auto propertyUt = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    renderCurveAnimation.AttachRenderProperty(propertyUt);
    renderCurveAnimation.InitValueEstimator();
    EXPECT_TRUE(renderCurveAnimation.valueEstimator_ != nullptr);
}

/**
 * @tc.name: DumpAnimationInfo001
 * @tc.desc: Test case for property is not null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, DumpAnimationInfo001, TestSize.Level1)
{
    RSRenderCurveAnimation animation;
    auto propertyUt = std::make_shared<RSRenderPropertyBase>();
    animation.property_ = propertyUt;
    std::string out;
    animation.DumpAnimationInfo(out);
    EXPECT_EQ(out, "Type:RSRenderCurveAnimation, ModifierType: 0, StartValue: , EndValue: ");
}

/**
 * @tc.name: DumpAnimationInfo002
 * @tc.desc: Test case for property is null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, DumpAnimationInfo002, TestSize.Level1)
{
    RSRenderCurveAnimation animationUt;
    animationUt.property_ = nullptr;
    std::string out;
    animationUt.DumpAnimationInfo(out);
    EXPECT_EQ(out, "Type:RSRenderCurveAnimation, ModifierType: INVALID, StartValue: , EndValue: ");
}

/**
 * @tc.name: ParseParam001
 * @tc.desc: Verify the ParseParam
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, ParseParam001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.7f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    Parcel parcelUt;
    auto result = renderCurveAnimation->Marshalling(parcelUt);
    EXPECT_TRUE(result);
    auto renderCurveAnimation1 = std::make_shared<RSRenderCurveAnimation>();
    result = renderCurveAnimation1->ParseParam(parcelUt);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ParseParam002
 * @tc.desc: Verify the ParseParam
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, ParseParam002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_ID, RSRenderPropertyType::INVALID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_ID, RSRenderPropertyType::INVALID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.5f,
        PROPERTY_ID, RSRenderPropertyType::INVALID);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    Parcel parcelUt;
    auto result = renderCurveAnimation->Marshalling(parcelUt);
    EXPECT_FALSE(result);
    auto renderCurveAnimation1 = std::make_shared<RSRenderCurveAnimation>();
    result = renderCurveAnimation1->ParseParam(parcelUt);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: OnSetFraction001
 * @tc.desc: Verify the OnSetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, OnSetFraction001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.7f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    float fractionUt = 0.1;
    renderCurveAnimation->valueEstimator_ = nullptr;
    renderCurveAnimation->OnSetFraction(fractionUt);
    EXPECT_FALSE(renderCurveAnimation->fractionChangeInfo_.first);
}

/**
 * @tc.name: OnSetFraction002
 * @tc.desc: Verify the OnSetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, OnSetFraction002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.6f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.3f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolator = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderCurveAnimation->GetInterpolator() != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    renderCurveAnimation->AttachRenderProperty(property);
    renderCurveAnimation->InitValueEstimator();
    float fractionUt = 0.1;
    renderCurveAnimation->OnSetFraction(fractionUt);
    EXPECT_TRUE(renderCurveAnimation->fractionChangeInfo_.first);
}

/**
 * @tc.name: UpdateFractionAfterContinue001
 * @tc.desc: Verify the UpdateFractionAfterContinue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, UpdateFractionAfterContinue001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.7f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolatorUt = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolatorUt);
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
HWTEST_F(RSRenderCurveAnimationUnitTest, UpdateFractionAfterContinue002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.6f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolatorUt = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolatorUt);
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
HWTEST_F(RSRenderCurveAnimationUnitTest, UpdateFractionAfterContinue003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.6f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolatorUt = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolatorUt);
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
HWTEST_F(RSRenderCurveAnimationUnitTest, OnAnimate001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.6f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.5f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolatorUt = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolatorUt);
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
HWTEST_F(RSRenderCurveAnimationUnitTest, OnAnimate002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.45f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.03f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.34f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->interpolator_ = nullptr;
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    renderCurveAnimation->AttachRenderProperty(property);
    renderCurveAnimation->InitValueEstimator();
    float fractionUt = 0.1f;
    renderCurveAnimation->OnAnimate(fractionUt);
    EXPECT_NE(renderCurveAnimation->lastValueFraction_, 0.1f);
}

/**
 * @tc.name: OnAnimate003
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, OnAnimate003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolatorUt = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolatorUt);
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
HWTEST_F(RSRenderCurveAnimationUnitTest, OnAnimateInner001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.54f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.3f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolatorUt = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolatorUt);
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
HWTEST_F(RSRenderCurveAnimationUnitTest, OnAnimateInner002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.6f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->interpolator_ = nullptr;
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderCurveAnimation->Attach(renderNode.get());

    renderCurveAnimation->AttachRenderProperty(property);
    renderCurveAnimation->InitValueEstimator();
    float fractionUt = 0.1f;
    renderCurveAnimation->OnAnimateInner(fractionUt, renderCurveAnimation->interpolator_);
    EXPECT_NE(renderCurveAnimation->lastValueFraction_, 0.1f);
}

/**
 * @tc.name: OnAnimateInner003
 * @tc.desc: Verify the OnAnimateInner
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderCurveAnimationUnitTest, OnAnimateInner003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.8f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto interpolatorUt = std::make_shared<LinearInterpolator>();
    renderCurveAnimation->SetInterpolator(interpolatorUt);
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