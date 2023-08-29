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
 * @tc.name: SetInterpolatorTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetInterpolatorTest, Level1)
{
    auto originPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto startPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto endPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto animationPath = std::shared_ptr<RSPath>();
    float t = 1.0f;
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    rsRenderPathAnimation.SetInterpolator(nullptr);
    ASSERT_NE(t, 0);
}

/**
 * @tc.name: SetRotationModeTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetRotationModeTest, Level1)
{
    auto originPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto startPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto endPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto animationPath = std::shared_ptr<RSPath>();
    float t = 1.0f;
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    RotationMode rotationMode = RotationMode::ROTATE_NONE;
    rsRenderPathAnimation.SetRotationMode(rotationMode);
    ASSERT_NE(t, 0);
}

/**
 * @tc.name: SetBeginFractionTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetBeginFractionTest, Level1)
{
    auto originPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto startPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto endPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto animationPath = std::shared_ptr<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float fraction = 1.0f;
    rsRenderPathAnimation.SetBeginFraction(fraction);
    ASSERT_NE(fraction, 0);
}

/**
 * @tc.name: GetBeginFractionTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, GetBeginFractionTest, Level1)
{
    auto originPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto startPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto endPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto animationPath = std::shared_ptr<RSPath>();
    float t = 1.0f;
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float res = rsRenderPathAnimation.GetBeginFraction();
    ASSERT_NE(t, 0);
}

/**
 * @tc.name: SetEndFractionTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetEndFractionTest, Level1)
{
    auto originPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto startPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto endPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto animationPath = std::shared_ptr<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float fraction = 1.0f;
    rsRenderPathAnimation.SetEndFraction(fraction);
    ASSERT_NE(fraction, 0);
}

/**
 * @tc.name: GetEndFractionTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, GetEndFractionTest, Level1)
{
    auto originPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto startPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto endPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto animationPath = std::shared_ptr<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    float res = rsRenderPathAnimation.GetEndFraction();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: SetIsNeedPathTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetIsNeedPathTest, Level1)
{
    auto originPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto startPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto endPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto animationPath = std::shared_ptr<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    bool isNeedPath = true;
    rsRenderPathAnimation.SetIsNeedPath(isNeedPath);
    ASSERT_NE(isNeedPath, false);
    
}

/**
 * @tc.name: SetPathNeedAddOriginTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetPathNeedAddOriginTest, Level1)
{
    auto originPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto startPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto endPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto animationPath = std::shared_ptr<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    bool needAddOrigin = true;
    rsRenderPathAnimation.SetPathNeedAddOrigin(needAddOrigin);
    ASSERT_NE(needAddOrigin, false);
}

/**
 * @tc.name: SetRotationIdTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetRotationIdTest, Level1)
{
    auto originPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto startPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto endPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto animationPath = std::shared_ptr<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    PropertyId rotationId_ = 0;
    rsRenderPathAnimation.SetRotationId(rotationId_);
    ASSERT_NE(rotationId_, 1);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, MarshallingTest, Level1)
{
    auto originPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto startPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto endPosition = std::shared_ptr<RSRenderPropertyBase>();
    auto animationPath = std::shared_ptr<RSPath>();
    RSRenderPathAnimation rsRenderPathAnimation(
        0, 0, originPosition, startPosition, endPosition, 0.f, animationPath);
    Parcel parcel;
    bool res = rsRenderPathAnimation.Marshalling(parcel);
    ASSERT_NE(res, false);
}
}
