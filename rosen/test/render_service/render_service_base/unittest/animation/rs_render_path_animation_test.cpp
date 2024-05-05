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
#include "animation/rs_render_path_animation.h"
#include "modifier/rs_render_property.h"
#include "render/rs_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderPathAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderPathAnimationTest::SetUpTestCase() {}
void RSRenderPathAnimationTest::TearDownTestCase() {}
void RSRenderPathAnimationTest::SetUp() {}
void RSRenderPathAnimationTest::TearDown() {}

/**
 * @tc.name: DumpAnimationTypeTest
 * @tc.desc: test results of DumpAnimationType
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, DumpAnimationTypeTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    std::string out = "Out";
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimation.DumpAnimationType(out);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetInterpolatorTest
 * @tc.desc: test results of SetInterpolator
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetInterpolatorTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    float t = 1.0f;
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimation.SetInterpolator(nullptr);
    ASSERT_NE(t, 0);
}

/**
 * @tc.name: GetInterpolatorTest
 * @tc.desc: test results of GetInterpolator
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, GetInterpolatorTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    std::shared_ptr<RSInterpolator> res;
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    res = rsRenderPathAnimation.GetInterpolator();
    ASSERT_EQ(res, RSInterpolator::DEFAULT);
}

/**
 * @tc.name: SetRotationModeTest
 * @tc.desc: test results of SetRotationMode
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetRotationModeTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    float t = 1.0f;
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    RotationMode rotationMode = RotationMode::ROTATE_NONE;
    rsRenderPathAnimation.SetRotationMode(rotationMode);
    ASSERT_NE(t, 0);
}

/**
 * @tc.name: GetRotationModeTest
 * @tc.desc: test results of GetRotationMode
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, GetRotationModeTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RotationMode rotation;
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rotation = rsRenderPathAnimation.GetRotationMode();
    ASSERT_EQ(rotation, RotationMode::ROTATE_NONE);
}

/**
 * @tc.name: SetBeginFractionTest
 * @tc.desc: test results of SetBeginFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetBeginFractionTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float fraction = 1.0f;
    rsRenderPathAnimation.SetBeginFraction(fraction);
    ASSERT_NE(fraction, 0);
}

/**
 * @tc.name: GetBeginFractionTest
 * @tc.desc: test results of GetBeginFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, GetBeginFractionTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float res = rsRenderPathAnimation.GetBeginFraction();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: SetEndFractionTest
 * @tc.desc: test results of SetEndFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetEndFractionTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float fraction = 1.0f;
    rsRenderPathAnimation.SetEndFraction(fraction);
    ASSERT_NE(fraction, 0);
}

/**
 * @tc.name: GetEndFractionTest
 * @tc.desc: test results of GetEndFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, GetEndFractionTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float res = rsRenderPathAnimation.GetEndFraction();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: SetIsNeedPathTest
 * @tc.desc: test results of SetIsNeedPath
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetIsNeedPathTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    bool isNeedPath = true;
    rsRenderPathAnimation.SetIsNeedPath(isNeedPath);
    ASSERT_NE(isNeedPath, false);
    
}

/**
 * @tc.name: SetPathNeedAddOriginTest
 * @tc.desc: test results of SetPathNeedAddOrigin
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetPathNeedAddOriginTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    bool needAddOrigin = true;
    rsRenderPathAnimation.SetPathNeedAddOrigin(needAddOrigin);
    ASSERT_NE(needAddOrigin, false);
}

/**
 * @tc.name: SetRotationIdTest
 * @tc.desc: test results of SetRotationId
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetRotationIdTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    PropertyId rotationId_ = 0;
    rsRenderPathAnimation.SetRotationId(rotationId_);
    ASSERT_NE(rotationId_, 1);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, MarshallingTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Parcel parcel;
    bool res = rsRenderPathAnimation.Marshalling(parcel);
    ASSERT_NE(res, true);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, UnmarshallingTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Parcel parcel;
    RSRenderPathAnimation* render;
    render = rsRenderPathAnimation.Unmarshalling(parcel);
    ASSERT_EQ(render, nullptr);
}

/**
 * @tc.name: ParseParamTest
 * @tc.desc: test results of ParseParam
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, ParseParamTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Parcel parcel;
    bool res = rsRenderPathAnimation.ParseParam(parcel);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: OnRemoveOnCompletionTest
 * @tc.desc: test results of OnRemoveOnCompletion
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnRemoveOnCompletionTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimation.OnRemoveOnCompletion();
    EXPECT_EQ(rsRenderPathAnimation.isNeedPath_, true);
}

/**
 * @tc.name: OnAttachTest
 * @tc.desc: test results of OnAttach
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnAttachTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimation.OnAttach();
    EXPECT_EQ(rsRenderPathAnimation.isNeedPath_, true);
}

/**
 * @tc.name: OnDetachTest
 * @tc.desc: test results of OnDetach
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, OnDetachTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimation.OnDetach();
    EXPECT_EQ(rsRenderPathAnimation.isNeedPath_, true);
}

/**
 * @tc.name: SetPathValueTest001
 * @tc.desc: test results of SetPathValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetPathValueTest001, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Vector2f value;
    float tangent = 0.f;
    rsRenderPathAnimation.SetPathValue(value, tangent);
    EXPECT_EQ(tangent, 0.f);
}

/**
 * @tc.name: SetPathValueTest002
 * @tc.desc: test results of SetPathValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetPathValueTest002, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Vector4f value;
    float tangent = 0.f;
    rsRenderPathAnimation.SetPathValue(value, tangent);
    EXPECT_EQ(tangent, 0.f);
}

/**
 * @tc.name: SetRotationValueTest
 * @tc.desc: test results of SetRotationValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetRotationValueTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation animation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float tangent = 0.f;
    animation.SetRotationValue(tangent);

    animation.SetRotationMode(RotationMode::ROTATE_AUTO);
    animation.SetRotationValue(tangent);
    EXPECT_EQ(animation.GetRotationMode(), RotationMode::ROTATE_AUTO);
	
    animation.SetRotationMode(RotationMode::ROTATE_AUTO_REVERSE);
    animation.SetRotationValue(tangent);
    EXPECT_EQ(animation.GetRotationMode(), RotationMode::ROTATE_AUTO_REVERSE);

    animation.SetRotationMode(RotationMode::ROTATE_NONE);
    animation.SetRotationValue(tangent);
    EXPECT_EQ(animation.GetRotationMode(), RotationMode::ROTATE_NONE);
}

/**
 * @tc.name: SetRotationTest
 * @tc.desc: test results of SetRotation
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetRotationTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float tangent = 0.f;
    rsRenderPathAnimation.SetRotation(tangent);
    EXPECT_EQ(tangent, 0.f);
}

/**
 * @tc.name: GetPosTanValueTest
 * @tc.desc: test results of GetPosTanValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, GetPosTanValueTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float fraction = 0.f;
    Vector2f position;
    float tangent = 0.f;
    rsRenderPathAnimation.GetPosTanValue(fraction, position, tangent);
    EXPECT_EQ(tangent, 0.f);
}

/**
 * @tc.name: UpdateVector2fPathValueTest
 * @tc.desc: test results of UpdateVector2fPathValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, UpdateVector2fPathValueTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Vector2f value;
    rsRenderPathAnimation.SetPathNeedAddOrigin(true);
    rsRenderPathAnimation.UpdateVector2fPathValue(value);
    EXPECT_EQ(rsRenderPathAnimation.needAddOrigin_, true);
}

/**
 * @tc.name: UpdateVector4fPathValueTest
 * @tc.desc: test results of UpdateVector4fPathValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, UpdateVector4fPathValueTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Vector4f value;
    Vector2f position;
    rsRenderPathAnimation.SetPathNeedAddOrigin(true);
    rsRenderPathAnimation.UpdateVector4fPathValue(value, position);
    EXPECT_EQ(rsRenderPathAnimation.needAddOrigin_, true);
}
}
