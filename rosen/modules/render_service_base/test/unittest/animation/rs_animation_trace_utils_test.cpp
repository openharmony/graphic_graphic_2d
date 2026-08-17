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

} // namespace Rosen
} // namespace OHOS
