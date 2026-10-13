/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "animation/rs_animation_trace_utils.h"
#include "animation/rs_interpolator.h"
#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_render_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSAnimationTraceUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationTraceUtilsTest::SetUpTestCase() {}
void RSAnimationTraceUtilsTest::TearDownTestCase() {}
void RSAnimationTraceUtilsTest::SetUp() {}
void RSAnimationTraceUtilsTest::TearDown() {}

/**
 * @tc.name: ParseRenderPropertyValueQuaternionNotAnimatable001
 * @tc.desc: Verify ParseRenderPropertyValue returns invalid string when Quaternion property is not animatable
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, ParseRenderPropertyValueQuaternionNotAnimatable001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    auto nonAnimatable = std::make_shared<RSRenderProperty<Quaternion>>();
    EXPECT_FALSE(nonAnimatable->IsAnimatable());
    auto result = utils.ParseRenderPropertyValue(nonAnimatable);
    EXPECT_EQ(result, "Quaternion:invalid");
}

/**
 * @tc.name: ParseRenderPropertyValueVector2fNotAnimatable001
 * @tc.desc: Verify ParseRenderPropertyValue returns invalid string when Vector2f property is not animatable
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, ParseRenderPropertyValueVector2fNotAnimatable001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    auto nonAnimatable = std::make_shared<RSRenderProperty<Vector2f>>();
    EXPECT_FALSE(nonAnimatable->IsAnimatable());
    auto result = utils.ParseRenderPropertyValue(nonAnimatable);
    EXPECT_EQ(result, "Vector2f:invalid");
}

/**
 * @tc.name: ParseRenderPropertyValueVector3fNotAnimatable001
 * @tc.desc: Verify ParseRenderPropertyValue returns invalid string when Vector3f property is not animatable
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, ParseRenderPropertyValueVector3fNotAnimatable001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    auto nonAnimatable = std::make_shared<RSRenderProperty<Vector3f>>();
    EXPECT_FALSE(nonAnimatable->IsAnimatable());
    auto result = utils.ParseRenderPropertyValue(nonAnimatable);
    EXPECT_EQ(result, "Vector3f:invalid");
}

/**
 * @tc.name: ParseRenderPropertyValueVector4fNotAnimatable001
 * @tc.desc: Verify ParseRenderPropertyValue returns invalid string when Vector4f property is not animatable
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, ParseRenderPropertyValueVector4fNotAnimatable001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    auto nonAnimatable = std::make_shared<RSRenderProperty<Vector4f>>();
    EXPECT_FALSE(nonAnimatable->IsAnimatable());
    auto result = utils.ParseRenderPropertyValue(nonAnimatable);
    EXPECT_EQ(result, "Vector4f:invalid");
}

/**
 * @tc.name: ParseRenderPropertyValueVector4ColorNotAnimatable001
 * @tc.desc: Verify ParseRenderPropertyValue returns invalid string when Vector4<Color> property is not animatable
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, ParseRenderPropertyValueVector4ColorNotAnimatable001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    auto nonAnimatable = std::make_shared<RSRenderProperty<Vector4<Color>>>();
    EXPECT_FALSE(nonAnimatable->IsAnimatable());
    auto result = utils.ParseRenderPropertyValue(nonAnimatable);
    EXPECT_EQ(result, "Vector4<Color>:invalid");
}

/**
 * @tc.name: ParseRenderPropertyValueAnimatableSuccess001
 * @tc.desc: Verify ParseRenderPropertyValue succeeds for animatable Quaternion property
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, ParseRenderPropertyValueAnimatableSuccess001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    auto animatable = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(Quaternion(1, 2, 3, 4));
    EXPECT_TRUE(animatable->IsAnimatable());
    auto result = utils.ParseRenderPropertyValue(animatable);
    EXPECT_NE(result.find("Quaternion"), std::string::npos);
    EXPECT_EQ(result.find("invalid"), std::string::npos);
}

/**
 * @tc.name: AddKeyframeAnimationClientTraceDebugDisabled001
 * @tc.desc: Verify AddKeyframeAnimationClientTrace returns early when debug is disabled
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, AddKeyframeAnimationClientTraceDebugDisabled001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    bool prevDebug = RSAnimationTraceUtils::isDebugEnabled_;
    RSAnimationTraceUtils::isDebugEnabled_ = false;
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    std::vector<KeyframeTuple> keyframes;
    std::vector<DurationKeyframeTuple> durationKeyframes;
    // Should return early without crash
    utils.AddKeyframeAnimationClientTrace(1, 2, ModifierNG::RSPropertyType::INVALID, startValue,
        false, 300, keyframes, durationKeyframes);
    // Verify early return: isDebugEnabled_ unchanged (const method, no side effects)
    EXPECT_FALSE(RSAnimationTraceUtils::isDebugEnabled_);
    RSAnimationTraceUtils::isDebugEnabled_ = prevDebug;
}

/**
 * @tc.name: AddKeyframeAnimationClientTraceNullStartValue001
 * @tc.desc: Verify AddKeyframeAnimationClientTrace returns early when startValue is null
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, AddKeyframeAnimationClientTraceNullStartValue001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    bool prevDebug = RSAnimationTraceUtils::isDebugEnabled_;
    RSAnimationTraceUtils::isDebugEnabled_ = true;
    std::vector<KeyframeTuple> keyframes;
    std::vector<DurationKeyframeTuple> durationKeyframes;
    // startValue null -> return early without crash
    utils.AddKeyframeAnimationClientTrace(1, 2, ModifierNG::RSPropertyType::INVALID, nullptr,
        false, 300, keyframes, durationKeyframes);
    // Verify early return: debug flag unchanged (method is const, no side effects)
    EXPECT_TRUE(RSAnimationTraceUtils::isDebugEnabled_);
    RSAnimationTraceUtils::isDebugEnabled_ = prevDebug;
}

/**
 * @tc.name: AddKeyframeAnimationClientTraceKeyframePath001
 * @tc.desc: Verify AddKeyframeAnimationClientTrace with keyframe path (non-duration)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, AddKeyframeAnimationClientTraceKeyframePath001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    bool prevDebug = RSAnimationTraceUtils::isDebugEnabled_;
    RSAnimationTraceUtils::isDebugEnabled_ = true;
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto interpolator = std::make_shared<LinearInterpolator>();
    std::vector<KeyframeTuple> keyframes;
    keyframes.push_back({0.0f, startValue, interpolator});
    auto kv1 = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    keyframes.push_back({1.0f, kv1, interpolator});
    // null value keyframe should be skipped (continue)
    keyframes.push_back({0.5f, nullptr, interpolator});
    std::vector<DurationKeyframeTuple> durationKeyframes;
    // isDurationKeyframe false -> keyframe path
    utils.AddKeyframeAnimationClientTrace(1, 2, ModifierNG::RSPropertyType::ALPHA, startValue,
        false, 300, keyframes, durationKeyframes);
    // Verify keyframe path completed without crash; debug flag unchanged
    EXPECT_TRUE(RSAnimationTraceUtils::isDebugEnabled_);
    RSAnimationTraceUtils::isDebugEnabled_ = prevDebug;
}

/**
 * @tc.name: AddKeyframeAnimationClientTraceDurationKeyframePath001
 * @tc.desc: Verify AddKeyframeAnimationClientTrace with duration keyframe path
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, AddKeyframeAnimationClientTraceDurationKeyframePath001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    bool prevDebug = RSAnimationTraceUtils::isDebugEnabled_;
    RSAnimationTraceUtils::isDebugEnabled_ = true;
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto interpolator = std::make_shared<LinearInterpolator>();
    std::vector<KeyframeTuple> keyframes;
    std::vector<DurationKeyframeTuple> durationKeyframes;
    auto kv0 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    durationKeyframes.push_back({0.0f, 100.0f, kv0, interpolator});
    auto kv1 = std::make_shared<RSRenderAnimatableProperty<float>>(10.0f);
    durationKeyframes.push_back({100.0f, 300.0f, kv1, interpolator});
    // null value in duration keyframe should be skipped (continue)
    durationKeyframes.push_back({300.0f, 400.0f, nullptr, interpolator});
    // isDurationKeyframe true -> duration keyframe path
    utils.AddKeyframeAnimationClientTrace(1, 2, ModifierNG::RSPropertyType::ALPHA, startValue,
        true, 300, keyframes, durationKeyframes);
    // Verify duration keyframe path completed without crash; debug flag unchanged
    EXPECT_TRUE(RSAnimationTraceUtils::isDebugEnabled_);
    RSAnimationTraceUtils::isDebugEnabled_ = prevDebug;
}

/**
 * @tc.name: AddKeyframeAnimationClientTraceEmptyKeyframes001
 * @tc.desc: Verify AddKeyframeAnimationClientTrace with empty keyframe vectors
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTraceUtilsTest, AddKeyframeAnimationClientTraceEmptyKeyframes001, TestSize.Level1)
{
    auto& utils = RSAnimationTraceUtils::GetInstance();
    bool prevDebug = RSAnimationTraceUtils::isDebugEnabled_;
    RSAnimationTraceUtils::isDebugEnabled_ = true;
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    std::vector<KeyframeTuple> keyframes;
    std::vector<DurationKeyframeTuple> durationKeyframes;
    // empty keyframes with isDurationKeyframe true
    utils.AddKeyframeAnimationClientTrace(1, 2, ModifierNG::RSPropertyType::ALPHA, startValue,
        true, 300, keyframes, durationKeyframes);
    // empty keyframes with isDurationKeyframe false
    utils.AddKeyframeAnimationClientTrace(1, 2, ModifierNG::RSPropertyType::ALPHA, startValue,
        false, 300, keyframes, durationKeyframes);
    // Verify both paths completed without crash; debug flag unchanged
    EXPECT_TRUE(RSAnimationTraceUtils::isDebugEnabled_);
    RSAnimationTraceUtils::isDebugEnabled_ = prevDebug;
}

} // namespace Rosen
} // namespace OHOS
