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
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

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
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
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
        RSRenderPropertyBase::Marshalling(parcel, property) &&
        RSRenderPropertyBase::Marshalling(parcel, property) &&
        RSRenderPropertyBase::Marshalling(parcel, property);
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
    auto property = std::make_shared<RSRenderPropertyBase>();
    renderCurveAnimation.property_ = property;
    renderCurveAnimation.InitValueEstimator();
    EXPECT_TRUE(renderCurveAnimation.valueEstimator_ == nullptr);

    GTEST_LOG_(INFO) << "RSRenderCurveAnimationTest InitValueEstimatorTest001 end";
}
} // namespace Rosen
} // namespace OHOS