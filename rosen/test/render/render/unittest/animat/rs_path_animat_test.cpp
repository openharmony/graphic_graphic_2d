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
class RSRenderPathAnimatUniTest : public testing::UniTest {
public:
    static void SetUpUniTestCase();
    static void TearDownUniTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderPathAnimatUniTest::SetUpUniTestCase() {}
void RSRenderPathAnimatUniTest::TearDownUniTestCase() {}
void RSRenderPathAnimatUniTest::SetUp() {}
void RSRenderPathAnimatUniTest::TearDown() {}

/**
 * @tc.name: DumpAnimatTypeUniTest
 * @tc.desc: test results of DumpAnimatType
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, DumpAnimatTypeUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    std::string out = "Out";
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimat.DumpAnimatType(out);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetInterpolatorUniTest
 * @tc.desc: test results of GetInterpolator
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, GetInterpolatorUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    std::shared_ptr<RSInterpolator> res;
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    res = rsRenderPathAnimat.GetInterpolator();
    ASSERT_EQ(res, RSInterpolator::DEFAULT);
}

/**
 * @tc.name: SetInterpolatorUniTest
 * @tc.desc: test results of SetInterpolator
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetInterpolatorUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    float t = 1.0f;
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimat.SetInterpolator(nullptr);
    ASSERT_NE(t, 0);
}

/**
 * @tc.name: GetRotationModeUniTest
 * @tc.desc: test results of GetRotationMode
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, GetRotationModeUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RotationMode rotation;
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rotation = rsRenderPathAnimat.GetRotationMode();
    ASSERT_EQ(rotation, RotationMode::ROTATE_NONE);
}

/**
 * @tc.name: SetRotationModeUniTest
 * @tc.desc: test results of SetRotationMode
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetRotationModeUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    float t = 1.0f;
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    RotationMode rotationMode = RotationMode::ROTATE_NONE;
    rsRenderPathAnimat.SetRotationMode(rotationMode);
    ASSERT_NE(t, 0);
}

/**
 * @tc.name: GetBeginFractionUniTest
 * @tc.desc: test results of GetBeginFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, GetBeginFractionUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float res = rsRenderPathAnimat.GetBeginFraction();
    ASSERT_EQ(res, res);
}

/**
 * @tc.name: SetBeginFractionUniTest
 * @tc.desc: test results of SetBeginFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetBeginFractionUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float fraction = 1.0f;
    rsRenderPathAnimat.SetBeginFraction(fraction);
    ASSERT_NE(fraction, 0);
}

/**
 * @tc.name: GetEndFractionUniTest
 * @tc.desc: test results of GetEndFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, GetEndFractionUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float res = rsRenderPathAnimat.GetEndFraction();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: SetEndFractionUniTest
 * @tc.desc: test results of SetEndFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetEndFractionUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float fraction = 1.0f;
    rsRenderPathAnimat.SetEndFraction(fraction);
    ASSERT_NE(fraction, 0);
}

/**
 * @tc.name: SetPathNeedAddOriginUniTest
 * @tc.desc: test results of SetPathNeedAddOrigin
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetPathNeedAddOriginUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    bool needAddOrigin = true;
    rsRenderPathAnimat.SetPathNeedAddOrigin(needAddOrigin);
    ASSERT_NE(needAddOrigin, false);
}

/**
 * @tc.name: SetIsNeedPathUniTest
 * @tc.desc: test results of SetIsNeedPath
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetIsNeedPathUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    bool isNeedPath = true;
    rsRenderPathAnimat.SetIsNeedPath(isNeedPath);
    ASSERT_NE(isNeedPath, false);
}

/**
 * @tc.name: MarshallingUniTest
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, MarshallingUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Parcel parcel;
    bool res = rsRenderPathAnimat.Marshalling(parcel);
    ASSERT_NE(res, true);
}

/**
 * @tc.name: SetRotationIdUniTest
 * @tc.desc: test results of SetRotationId
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetRotationIdUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    PropertyId rotationId_ = 0;
    rsRenderPathAnimat.SetRotationId(rotationId_);
    ASSERT_NE(rotationId_, 1);
}

/**
 * @tc.name: ParseParamUniTest
 * @tc.desc: test results of ParseParam
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, ParseParamUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Parcel parcel;
    bool res = rsRenderPathAnimat.ParseParam(parcel);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: UnmarshallingUniTest
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, UnmarshallingUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Parcel parcel;
    RSRenderPathAnimat* render;
    render = rsRenderPathAnimat.Unmarshalling(parcel);
    ASSERT_EQ(render, nullptr);
}

/**
 * @tc.name: OnAttachUniTest
 * @tc.desc: test results of OnAttach
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, OnAttachUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimat.OnAttach();
    EXPECT_EQ(rsRenderPathAnimat.isNeedPath_, true);
}

/**
 * @tc.name: OnRemoveOnCompletionUniTest
 * @tc.desc: test results of OnRemoveOnCompletion
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, OnRemoveOnCompletionUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimat.OnRemoveOnCompletion();
    EXPECT_EQ(rsRenderPathAnimat.isNeedPath_, true);
}

/**
 * @tc.name: SetPathValueUniTest001
 * @tc.desc: test results of SetPathValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetPathValueUniTest001, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Vector2f value;
    float tangent = 0.f;
    rsRenderPathAnimat.SetPathValue(value, tangent);
    EXPECT_EQ(tangent, 0.f);
}

/**
 * @tc.name: OnDetachUniTest
 * @tc.desc: test results of OnDetach
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, OnDetachUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimat.OnDetach();
    EXPECT_EQ(rsRenderPathAnimat.isNeedPath_, true);
}

/**
 * @tc.name: SetRotationValueUniTest
 * @tc.desc: test results of SetRotationValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetRotationValueUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat animation(
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
 * @tc.name: SetPathValueUniTest002
 * @tc.desc: test results of SetPathValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetPathValueUniTest002, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Vector4f value;
    float tangent = 0.f;
    rsRenderPathAnimat.SetPathValue(value, tangent);
    EXPECT_EQ(tangent, 0.f);
}

/**
 * @tc.name: GetPosTanValueUniTest
 * @tc.desc: test results of GetPosTanValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, GetPosTanValueUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float fraction = 0.f;
    Vector2f position;
    float tangent = 0.f;
    rsRenderPathAnimat.GetPosTanValue(fraction, position, tangent);
    EXPECT_EQ(tangent, 0.f);
}

/**
 * @tc.name: SetRotationUniTest
 * @tc.desc: test results of SetRotation
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, SetRotationUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float tangent = 0.f;
    rsRenderPathAnimat.SetRotation(tangent);
    EXPECT_EQ(tangent, 0.f);
}

/**
 * @tc.name: UpdateVector4fPathValueUniTest
 * @tc.desc: test results of UpdateVector4fPathValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, UpdateVector4fPathValueUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Vector4f value;
    Vector2f position;
    rsRenderPathAnimat.SetPathNeedAddOrigin(true);
    rsRenderPathAnimat.UpdateVector4fPathValue(value, position);
    EXPECT_EQ(rsRenderPathAnimat.needAddOrigin_, true);
}

/**
 * @tc.name: UpdateVector2fPathValueUniTest
 * @tc.desc: test results of UpdateVector2fPathValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimatUniTest, UpdateVector2fPathValueUniTest, Level1)
{
    auto originPosition = std::make_shared<RSRenderPropertyBase>();
    auto startPosition = std::make_shared<RSRenderPropertyBase>();
    auto endPosition = std::make_shared<RSRenderPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSRenderPathAnimat rsRenderPathAnimat(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Vector2f value;
    rsRenderPathAnimat.SetPathNeedAddOrigin(true);
    rsRenderPathAnimat.UpdateVector2fPathValue(value);
    EXPECT_EQ(rsRenderPathAnimat.needAddOrigin_, true);
}
}
