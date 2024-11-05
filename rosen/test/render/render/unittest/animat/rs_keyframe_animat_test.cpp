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
class RSRenderKeyframeAnimatExtTest : public testing::ExtTest {
public:
    static void SetUpExtTestCase();
    static void TearDownExtTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderKeyframeAnimatExtTest::SetUpExtTestCase() {}
void RSRenderKeyframeAnimatExtTest::TearDownExtTestCase() {}
void RSRenderKeyframeAnimatExtTest::SetUp() {}
void RSRenderKeyframeAnimatExtTest::TearDown() {}

/**
 * @tc.name: AddKeyframeExtExtTest001
 * @tc.desc: test results of AddKeyframe
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimatExtExtTest, AddKeyframeExtExtTest001, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimat rsRenderKeyframeAnimat(0, 0, originValue);
    auto value = std::make_shared<RSRenderPropertyBase>();
    auto interpolator = std::shared_ptr<RSInterpolator>();
    rsRenderKeyframeAnimat.AddKeyframe(1.0f, value, interpolator);
    EXPECT_NE(value, nullptr);
}

/**
 * @tc.name: DumpAnimatTypeExtExtTest
 * @tc.desc: test results of DumpAnimatType
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimatExtExtTest, DumpAnimatTypeExtExtTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimat rsRenderKeyframeAnimat(0, 0, originValue);
    std::string out = "out";
    rsRenderKeyframeAnimat.DumpAnimatType(out);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: AddKeyframeExtExtTest002
 * @tc.desc: test results of AddKeyframe
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimatExtExtTest, AddKeyframeExtExtTest002, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimat rsRenderKeyframeAnimat(0, 0, originValue);
    auto value = std::make_shared<RSRenderPropertyBase>();
    auto interpolator = std::shared_ptr<RSInterpolator>();
    int start = 1;
    int end = 0;
    rsRenderKeyframeAnimat.AddKeyframe(start, end, value, interpolator);
    EXPECT_NE(value, nullptr);

    start = 0;
    end = 1;
    rsRenderKeyframeAnimat.AddKeyframe(start, end, value, interpolator);
    EXPECT_NE(value, nullptr);

    rsRenderKeyframeAnimat.Start();
    rsRenderKeyframeAnimat.AddKeyframe(start, end, value, interpolator);
    EXPECT_NE(value, nullptr);

    int val = -1;
    rsRenderKeyframeAnimat.SetDuration(val);
    rsRenderKeyframeAnimat.AddKeyframe(start, end, value, interpolator);
    EXPECT_NE(value, nullptr);
}

/**
 * @tc.name: AddKeyframesExtExtTest
 * @tc.desc: test results of AddKeyframes
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimatExtExtTest, AddKeyframesExtExtTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    auto keyframes = std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>,
    std::shared_ptr<RSInterpolator>>>();
    RSRenderKeyframeAnimat rsRenderKeyframeAnimat(0, 0, originValue);
    rsRenderKeyframeAnimat.AddKeyframes(keyframes);
    EXPECT_NE(originValue, nullptr);

    rsRenderKeyframeAnimat.Start();
    rsRenderKeyframeAnimat.AddKeyframes(keyframes);
    EXPECT_NE(originValue, nullptr);
}

/**
 * @tc.name: MarshallingExtTest
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimatExtTest, MarshallingExtTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimat rsRenderKeyframeAnimat(0, 0, originValue);
    Parcel parcel;
    bool res = rsRenderKeyframeAnimat.Marshalling(parcel);
    ASSERT_NE(res, false);
}

/**
 * @tc.name: SetDurationKeyframeExtExtTest
 * @tc.desc: test results of SetDurationKeyframe
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimatExtTest, SetDurationKeyframeExtTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimat rsRenderKeyframeAnimat(0, 0, originValue);
    rsRenderKeyframeAnimat.SetDurationKeyframe(false);
    EXPECT_EQ(rsRenderKeyframeAnimat.isDurationKeyframe_, false);
}

/**
 * @tc.name: ParseParamExtTest
 * @tc.desc: test results of ParseParam
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimatExtTest, ParseParamExtTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimat rsRenderKeyframeAnimat(0, 0, originValue);
    Parcel parcel;
    bool res = rsRenderKeyframeAnimat.ParseParam(parcel);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: UnmarshallingExtTest
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimatExtTest, UnmarshallingExtTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimat rsRenderKeyframeAnimat(0, 0, originValue);
    Parcel parcel;
    RSRenderKeyframeAnimat* renderKeyframeAnimat = rsRenderKeyframeAnimat.Unmarshalling(parcel);
    ASSERT_EQ(renderKeyframeAnimat, renderKeyframeAnimat);
    renderKeyframeAnimat = nullptr;
}

/**
 * @tc.name: OnAnimateExtTest
 * @tc.desc: test results of OnAnimate
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimatExtTest, OnAnimateExtTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimat rsRenderKeyframeAnimat(0, 0, originValue);
    float fraction = 0.0f;
    rsRenderKeyframeAnimat.OnAnimate(fraction);
    EXPECT_EQ(fraction, 0.0f);
}

/**
 * @tc.name: ParseDurationKeyframesParamExtTest
 * @tc.desc: test results of ParseDurationKeyframesParam
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderKeyframeAnimatExtTest, ParseDurationKeyframesParamExtTest, Level1)
{
    auto originValue = std::make_shared<RSRenderPropertyBase>();
    RSRenderKeyframeAnimat rsRenderKeyframeAnimat(0, 0, originValue);
    Parcel parcel;
    int keyframeSize = 2;
    bool res = rsRenderKeyframeAnimat.ParseDurationKeyframesParam(parcel, keyframeSize);
    EXPECT_EQ(res, false);

    keyframeSize = 0;
    res = rsRenderKeyframeAnimat.ParseDurationKeyframesParam(parcel, keyframeSize);
    EXPECT_EQ(res, true);
}
}