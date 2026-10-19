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
 
#include "common/rs_rect_shape.h"
#include "common/rs_vector4.h"
#include "modifier_ng/geometry/rs_bounds_clip_shape_modifier.h"
 
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
 
class RSBoundsClipShapeModifierTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};
 
/**
 * @tc.name: GetClipShapeDefault001
 * @tc.desc: test results of GetClipShape with default value
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSBoundsClipShapeModifierTest, GetClipShapeDefault001, TestSize.Level1)
{
    auto modifier = std::make_shared<RSBoundsClipShapeModifier>();
    RectShape shape = modifier->GetClipShape();
    EXPECT_EQ(shape.GetWidth(), 0.0f);
    EXPECT_EQ(shape.GetHeight(), 0.0f);
    EXPECT_EQ(shape.GetCornerRadius(), Vector4f(0.0f));
}
 
/**
 * @tc.name: SetAndGetClipShape001
 * @tc.desc: test results of SetClipShape then GetClipShape returns the set value
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSBoundsClipShapeModifierTest, SetAndGetClipShape001, TestSize.Level1)
{
    auto modifier = std::make_shared<RSBoundsClipShapeModifier>();
    Vector4f radius(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB); // for test
    RectShape shape(CLIP_WIDTH, CLIP_HEIGHT, radius);
    modifier->SetClipShape(shape);
    RectShape result = modifier->GetClipShape();
    EXPECT_EQ(result.GetWidth(), CLIP_WIDTH);
    EXPECT_EQ(result.GetHeight(), CLIP_HEIGHT);
    EXPECT_EQ(result.GetCornerRadius(), radius);
}
 
/**
 * @tc.name: SetAndGetClipShape002
 * @tc.desc: test results of SetClipShape overwriting default then get returns new value
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSBoundsClipShapeModifierTest, SetAndGetClipShape002, TestSize.Level1)
{
    auto modifier = std::make_shared<RSBoundsClipShapeModifier>();
    RectShape defaultShape = modifier->GetClipShape();
    EXPECT_EQ(defaultShape.GetWidth(), 0.0f);
    Vector4f radius(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB); // for test
    RectShape shape(CLIP_WIDTH, CLIP_HEIGHT, radius);
    modifier->SetClipShape(shape);
    RectShape result = modifier->GetClipShape();
    EXPECT_EQ(result.GetWidth(), CLIP_WIDTH);
    EXPECT_EQ(result.GetHeight(), CLIP_HEIGHT);
    EXPECT_EQ(result.GetCornerRadius(), radius);
    EXPECT_TRUE(result == shape);
}
} // namespace OHOS::Rosen