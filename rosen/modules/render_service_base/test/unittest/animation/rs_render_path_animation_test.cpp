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

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);

    Parcel parcel;
    renderPathAnimation->Marshalling(parcel);
    EXPECT_TRUE(renderPathAnimation != nullptr);
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

    auto property = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(ANIMATION_ID, PROPERTY_ID,
        property, property1, property2, 1.0f, path);

    Parcel parcel;
    renderPathAnimation->Marshalling(parcel);
    RSRenderPathAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(renderPathAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderPathAnimationTest Unmarshalling001 end";
}
} // namespace Rosen
} // namespace OHOS