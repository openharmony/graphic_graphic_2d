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

#include "animation/rs_render_path_animation.h"
#include "animation/rs_steps_interpolator.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"
#include "render/rs_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderPathAnimationMock : public RSRenderPathAnimation {
public:
    explicit RSRenderPathAnimationMock(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originPosition,
    const std::shared_ptr<RSRenderPropertyBase>& startPosition,
    const std::shared_ptr<RSRenderPropertyBase>& endPosition, float originRotation,
    const std::shared_ptr<RSPath>& animationPath) : RSRenderPathAnimation(id,
        propertyId, originPosition, startPosition, endPosition, originRotation, animationPath) {}
    ~RSRenderPathAnimationMock() = default;

    void OnAnimate(float fraction) override
    {
        RSRenderPathAnimation::OnAnimate(fraction);
    }

    void OnRemoveOnCompletion() override
    {
        RSRenderPathAnimation::OnRemoveOnCompletion();
    }

    void InitValueEstimator() override
    {
        RSRenderPathAnimation::InitValueEstimator();
    }

    void OnAttach() override
    {
        RSRenderPathAnimation::OnAttach();
    }

    void OnDetach() override
    {
        RSRenderPathAnimation::OnDetach();
    }
};

class RSRenderPathAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    const Vector2f PATH_ANIMATION_DEFAULT_VALUE = Vector2f(0.f, 0.f);
    const Vector2f PATH_ANIMATION_START_VALUE = Vector2f(0.f, 0.f);
    const Vector2f PATH_ANIMATION_END_VALUE = Vector2f(500.f, 500.f);
    const Vector4f PATH_ANIMATION_DEFAULT_4F_VALUE = Vector4f(0.f, 0.f, 0.f, 0.f);
    const Vector4f PATH_ANIMATION_START_4F_VALUE = Vector4f(0.f, 0.f, 0.f, 0.f);
    const Vector4f PATH_ANIMATION_END_4F_VALUE = Vector4f(500.f, 500.f, 500.f, 500.f);
    const std::string ANIMATION_PATH = "L350 0 L150 100";
    const int errorRotationMode = 10;
};

void RSRenderPathAnimationTest::SetUpTestCase() {}
void RSRenderPathAnimationTest::TearDownTestCase() {}
void RSRenderPathAnimationTest::SetUp() {}
void RSRenderPathAnimationTest::TearDown() {}

/**
 * @tc.name: GetInterpolator001
 * @tc.desc: Verify the GetInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, GetInterpolator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest GetInterpolator001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
 
    EXPECT_TRUE(renderPathAnimation != nullptr);
    auto interpolator = std::make_shared<RSStepsInterpolator>(0);
    renderPathAnimation->SetInterpolator(interpolator);
    EXPECT_TRUE(renderPathAnimation->GetInterpolator() != nullptr);

    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest GetInterpolator001 end";
}

/**
 * @tc.name: GetRotationMode001
 * @tc.desc: Verify the GetRotationMode
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, GetRotationMode001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest GetInterpolator001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
 
    EXPECT_TRUE(renderPathAnimation != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderPathAnimation->Attach(renderNode.get());
    renderPathAnimation->Start();
    EXPECT_TRUE(renderPathAnimation->IsRunning());
    renderPathAnimation->SetRotationMode(RotationMode::ROTATE_AUTO);
    EXPECT_TRUE(renderPathAnimation->GetRotationMode() == RotationMode::ROTATE_NONE);

    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest GetRotationMode001 end";
}

/**
 * @tc.name: GetRotationMode002
 * @tc.desc: Verify the GetRotationMode
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, GetRotationMode002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest GetInterpolator001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(PATH_ANIMATION_DEFAULT_4F_VALUE);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(PATH_ANIMATION_START_4F_VALUE);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(PATH_ANIMATION_END_4F_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    renderPathAnimation->SetRotationMode((RotationMode)errorRotationMode);
 
    EXPECT_TRUE(renderPathAnimation != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderPathAnimation->Attach(renderNode.get());
    renderPathAnimation->Start();
    EXPECT_TRUE(renderPathAnimation->IsRunning());
    EXPECT_TRUE(renderPathAnimation->GetRotationMode() == (RotationMode)errorRotationMode);

    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest GetRotationMode002 end";
}

/**
 * @tc.name: SetBeginFraction001
 * @tc.desc: Verify the SetBeginFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetBeginFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest SetBeginFraction001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
 
    EXPECT_TRUE(renderPathAnimation != nullptr);
    renderPathAnimation->SetBeginFraction(-1.0f);
    EXPECT_TRUE(renderPathAnimation->GetBeginFraction() != -1.0f);
    renderPathAnimation->SetBeginFraction(2.0f);
    EXPECT_TRUE(renderPathAnimation->GetBeginFraction() != 2.0f);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderPathAnimation->Attach(renderNode.get());
    renderPathAnimation->Start();
    EXPECT_TRUE(renderPathAnimation->IsRunning());
    renderPathAnimation->SetBeginFraction(1.0f);
    EXPECT_TRUE(renderPathAnimation->GetBeginFraction() != 1.0f);

    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest SetBeginFraction001 end";
}

/**
 * @tc.name: SetEndFraction001
 * @tc.desc: Verify the SetEndFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetEndFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest SetEndFraction001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
 
    EXPECT_TRUE(renderPathAnimation != nullptr);
    renderPathAnimation->SetEndFraction(-1.0f);
    EXPECT_TRUE(renderPathAnimation->GetEndFraction() != -1.0f);
    renderPathAnimation->SetEndFraction(2.0f);
    EXPECT_TRUE(renderPathAnimation->GetEndFraction() != 2.0f);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderPathAnimation->Attach(renderNode.get());
    renderPathAnimation->Start();
    EXPECT_TRUE(renderPathAnimation->IsRunning());
    renderPathAnimation->SetEndFraction(0.0f);
    EXPECT_TRUE(renderPathAnimation->GetEndFraction() != 0.0f);

    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest SetEndFraction001 end";
}

/**
 * @tc.name: SetPathNeedAddOrigin001
 * @tc.desc: Verify the SetPathNeedAddOrigin
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetPathNeedAddOrigin001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest SetPathNeedAddOrigin001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
 
    EXPECT_TRUE(renderPathAnimation != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderPathAnimation->Attach(renderNode.get());
    renderPathAnimation->Start();
    EXPECT_TRUE(renderPathAnimation->IsRunning());
    renderPathAnimation->SetPathNeedAddOrigin(true);

    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest SetPathNeedAddOrigin001 end";
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest Marshalling001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE,
        PROPERTY_ID);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    Parcel parcel;
    auto result = renderPathAnimation->Marshalling(parcel);
    EXPECT_TRUE(result == true);
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest Marshalling001 end";
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest Unmarshalling001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE,
        PROPERTY_ID);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    Parcel parcel;
    auto renderAnimation = RSRenderPathAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(renderAnimation == nullptr);
    renderPathAnimation->Marshalling(parcel);
    renderAnimation = RSRenderPathAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(renderAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest Unmarshalling001 end";
}

/**
 * @tc.name: OnAnimate001
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnAnimate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE,
        PROPERTY_ID);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimationMock>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    renderPathAnimation->OnAnimate(1.0f);
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate001 end";
}

/**
 * @tc.name: OnAnimate002
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnAnimate002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate002 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(PATH_ANIMATION_DEFAULT_4F_VALUE,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(PATH_ANIMATION_START_4F_VALUE,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(PATH_ANIMATION_END_4F_VALUE,
        PROPERTY_ID);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimationMock>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    renderPathAnimation->SetIsNeedPath(false);
    renderPathAnimation->AttachRenderProperty(property);
    renderPathAnimation->OnAnimate(1.0f);
    renderPathAnimation->InitValueEstimator();
    renderPathAnimation->OnAnimate(1.0f);
    renderPathAnimation->SetIsNeedPath(true);
    renderPathAnimation->OnAnimate(1.0f);
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate002 end";
}

/**
 * @tc.name: OnAnimate003
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnAnimate003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate003 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(PATH_ANIMATION_DEFAULT_4F_VALUE,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(PATH_ANIMATION_START_4F_VALUE,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(PATH_ANIMATION_END_4F_VALUE,
        PROPERTY_ID);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimationMock>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    renderPathAnimation->AttachRenderProperty(property);
    renderPathAnimation->InitValueEstimator();
    renderPathAnimation->SetIsNeedPath(true);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderPathAnimation->Attach(renderNode.get());
    renderPathAnimation->SetRotationMode(RotationMode::ROTATE_AUTO);
    renderPathAnimation->OnAnimate(1.0f);
    renderPathAnimation->SetRotationMode(RotationMode::ROTATE_AUTO_REVERSE);
    renderPathAnimation->OnAnimate(1.0f);

    renderPathAnimation->valueEstimator_ = nullptr;
    renderPathAnimation->property_ = std::make_shared<RSRenderProperty<bool>>();
    renderPathAnimation->property_->CreateRSValueEstimator(RSValueEstimatorType::CURVE_VALUE_ESTIMATOR);
    EXPECT_FALSE(renderPathAnimation->valueEstimator_);
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate003 end";
}

/**
 * @tc.name: OnAnimate004
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnAnimate004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate004 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE,
        PROPERTY_ID);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimationMock>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    renderPathAnimation->SetRotationMode(RotationMode::ROTATE_NONE);
    renderPathAnimation->OnAnimate(1.0f);
    renderPathAnimation->SetRotationMode(RotationMode::ROTATE_AUTO_REVERSE);
    renderPathAnimation->OnAnimate(1.0f);
    renderPathAnimation->SetRotationMode((RotationMode)errorRotationMode);
    renderPathAnimation->OnAnimate(1.0f);
    auto path1 = nullptr;
    auto renderPathAnimation1 = std::make_shared<RSRenderPathAnimationMock>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path1);
    EXPECT_TRUE(renderPathAnimation1 != nullptr);
    renderPathAnimation1->OnAnimate(1.0f);
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate004 end";
}

/**
 * @tc.name: OnAnimate005
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnAnimate005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate005 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimationMock>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderPathAnimation->Attach(renderNode.get());
    renderPathAnimation->SetRotationMode(RotationMode::ROTATE_AUTO);
    renderPathAnimation->OnAnimate(1.0f);
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate005 end";
}

/**
 * @tc.name: OnAnimate006
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnAnimate006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate006 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimationMock>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    renderPathAnimation->originValue_.reset();
    EXPECT_TRUE(renderPathAnimation->GetOriginValue() == nullptr);
    renderPathAnimation->OnAnimate(1.0f);
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnAnimate006 end";
}

/**
 * @tc.name: OnDetach001
 * @tc.desc: Verify the OnDetach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnDetach001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnDetach001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE,
        PROPERTY_ID);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimationMock>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    renderPathAnimation->OnDetach();
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderPathAnimation->Attach(renderNode.get());
    renderPathAnimation->OnDetach();
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnDetach001 end";
}

/**
 * @tc.name: OnRemoveOnCompletion001
 * @tc.desc: Verify the OnRemoveOnCompletion
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnRemoveOnCompletion001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnRemoveOnCompletion001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE,
        PROPERTY_ID);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimationMock>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    renderPathAnimation->OnRemoveOnCompletion();
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderPathAnimation->Attach(renderNode.get());
    renderPathAnimation->OnRemoveOnCompletion();
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest OnRemoveOnCompletion001 end";
}
} // namespace Rosen
} // namespace OHOS