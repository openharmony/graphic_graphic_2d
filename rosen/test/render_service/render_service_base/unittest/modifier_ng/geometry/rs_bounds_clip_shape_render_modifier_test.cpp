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
 
#include <memory>
 
#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
 
#include "common/rs_rect_shape.h"
#include "common/rs_vector4.h"
#include "modifier_ng/geometry/rs_bounds_clip_shape_render_modifier.h"
#include "property/rs_properties.h"
 
using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;
 
namespace OHOS::Rosen {
namespace {
constexpr float CLIP_WIDTH = 100.0f;
constexpr float CLIP_HEIGHT = 200.0f;
constexpr float CORNER_LT = 10.0f;
constexpr float CORNER_RT = 20.0f;
constexpr float CORNER_RB = 30.0f;
constexpr float CORNER_LB = 40.0f;
} // namespace
 
class RSBoundsClipShapeRenderModifierTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};
 
/**
 * @tc.name: RSBoundsClipShapeRenderModifierGetTypeTest
 * @tc.desc: Verify GetType returns CLIP_SHAPE
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSBoundsClipShapeRenderModifierTest, RSBoundsClipShapeRenderModifierGetTypeTest, TestSize.Level1)
{
    RSBoundsClipShapeRenderModifier modifier;
    EXPECT_EQ(modifier.GetType(), RSModifierType::CLIP_SHAPE);
}
 
/**
 * @tc.name: RSBoundsClipShapeRenderModifierResetTest
 * @tc.desc: Verify ResetProperties clears clipShape_ to nullptr
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSBoundsClipShapeRenderModifierTest, RSBoundsClipShapeRenderModifierResetTest, TestSize.Level1)
{
    RSProperties properties;
    Vector4f radius(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB);
    RectShape shape(CLIP_WIDTH, CLIP_HEIGHT, radius);
    properties.SetClipShape(shape);
    ASSERT_TRUE(properties.GetClipToShape());
 
    RSBoundsClipShapeRenderModifier modifier;
    modifier.ResetProperties(properties);
    EXPECT_FALSE(properties.GetClipToShape());
    EXPECT_EQ(properties.GetClipShape(), nullptr);
}
 
/**
 * @tc.name: RSBoundsClipShapeRenderModifierResetTwiceTest
 * @tc.desc: Verify ResetProperties on already-cleared properties does not crash
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSBoundsClipShapeRenderModifierTest, RSBoundsClipShapeRenderModifierResetTwiceTest, TestSize.Level1)
{
    RSProperties properties;
    RSBoundsClipShapeRenderModifier modifier;
    modifier.ResetProperties(properties);
    EXPECT_FALSE(properties.GetClipToShape());
 
    modifier.ResetProperties(properties);
    EXPECT_FALSE(properties.GetClipToShape());
    EXPECT_EQ(properties.GetClipShape(), nullptr);
}
} // namespace OHOS::Rosen