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
#include "animation/rs_render_keyframe_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderKeyframeAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderKeyframeAnimationTest::SetUpTestCase() {}
void RSRenderKeyframeAnimationTest::TearDownTestCase() {}
void RSRenderKeyframeAnimationTest::SetUp() {}
void RSRenderKeyframeAnimationTest::TearDown() {}

/**
 * @tc.name: DumpAnimationTypeTest
 * @tc.desc: test results of DumpAnimationType
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, DumpAnimationTypeTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    std::string out = "out";
    rsRenderKeyframeAnimation.DumpAnimationType(out);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: AddKeyframeTest001
 * @tc.desc: test results of AddKeyframe
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, AddKeyframeTest001, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    auto value = std::make_shared<RSRenderPropertyBase>();
    auto interpolator = std::shared_ptr<RSInterpolator>();
    rsRenderKeyframeAnimation.AddKeyframe(1.0f, value, interpolator);
    EXPECT_NE(value, nullptr);
}

/**
 * @tc.name: AddKeyframesTest
 * @tc.desc: test results of AddKeyframes
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, AddKeyframesTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    auto keyframes = std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>,
    std::shared_ptr<RSInterpolator>>>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    rsRenderKeyframeAnimation.AddKeyframes(keyframes);
    EXPECT_NE(originValue, nullptr);

    rsRenderKeyframeAnimation.Start();
    rsRenderKeyframeAnimation.AddKeyframes(keyframes);
    EXPECT_NE(originValue, nullptr);
}

/**
 * @tc.name: AddKeyframeTest002
 * @tc.desc: test results of AddKeyframe
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, AddKeyframeTest002, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    auto value = std::make_shared<RSRenderPropertyBase>();
    auto interpolator = std::shared_ptr<RSInterpolator>();
    int start = 1;
    int end = 0;
    rsRenderKeyframeAnimation.AddKeyframe(start, end, value, interpolator);
    EXPECT_NE(value, nullptr);

    start = 0;
    end = 1;
    rsRenderKeyframeAnimation.AddKeyframe(start, end, value, interpolator);
    EXPECT_NE(value, nullptr);

    rsRenderKeyframeAnimation.Start();
    rsRenderKeyframeAnimation.AddKeyframe(start, end, value, interpolator);
    EXPECT_NE(value, nullptr);

    int val = -1;
    rsRenderKeyframeAnimation.SetDuration(val);
    rsRenderKeyframeAnimation.AddKeyframe(start, end, value, interpolator);
    EXPECT_NE(value, nullptr);
}

/**
 * @tc.name: SetDurationKeyframeTest
 * @tc.desc: test results of SetDurationKeyframe
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, SetDurationKeyframeTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    rsRenderKeyframeAnimation.SetDurationKeyframe(false);
    EXPECT_EQ(rsRenderKeyframeAnimation.isDurationKeyframe_, false);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, MarshallingTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    Parcel parcel;
    bool res = rsRenderKeyframeAnimation.Marshalling(parcel);
    ASSERT_NE(res, false);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, UnmarshallingTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    Parcel parcel;
    RSRenderKeyframeAnimation* renderKeyframeAnimation = rsRenderKeyframeAnimation.Unmarshalling(parcel);
    ASSERT_EQ(renderKeyframeAnimation, renderKeyframeAnimation);
    renderKeyframeAnimation = nullptr;
}

/**
 * @tc.name: ParseParamTest
 * @tc.desc: test results of ParseParam
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, ParseParamTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    Parcel parcel;
    bool res = rsRenderKeyframeAnimation.ParseParam(parcel);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: ParseDurationKeyframesParamTest
 * @tc.desc: test results of ParseDurationKeyframesParam
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, ParseDurationKeyframesParamTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    Parcel parcel;
    int keyframeSize = 2;
    bool res = rsRenderKeyframeAnimation.ParseDurationKeyframesParam(parcel, keyframeSize);
    EXPECT_EQ(res, false);

    keyframeSize = 0;
    res = rsRenderKeyframeAnimation.ParseDurationKeyframesParam(parcel, keyframeSize);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: OnAnimateTest
 * @tc.desc: test results of OnAnimate
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, OnAnimateTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimation rsRenderKeyframeAnimation(0, 0, originValue);
    float fraction = 0.0f;
    rsRenderKeyframeAnimation.OnAnimate(fraction);
    EXPECT_EQ(fraction, 0.0f);
}
}