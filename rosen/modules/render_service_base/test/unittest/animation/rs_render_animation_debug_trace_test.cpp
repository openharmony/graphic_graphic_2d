/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderAnimationDebugTraceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t NODE_ID = 123456;
    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    const std::string NODE_NAME = "test";
};

void RSRenderAnimationDebugTraceTest::SetUpTestCase() {}
void RSRenderAnimationDebugTraceTest::TearDownTestCase() {}
void RSRenderAnimationDebugTraceTest::SetUp() {}
void RSRenderAnimationDebugTraceTest::TearDown() {}

/**
 * @tc.name: AnimationDebugTrace001
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AnimationDebugTrace001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace001 start";

    RSAnimationTraceUtils::GetInstance().addAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, false);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(
        NODE_ID, NODE_NAME, PROPERTY_ID, ANIMATION_ID, 0, 0, startValue, endValue, 0, 0, 1);
    RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
        NODE_ID, NODE_NAME, ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, 1);
    RSRenderNode renderNode(NODE_ID);
    RSAnimationTraceUtils::GetInstance().addRenderNodeTrace(renderNode);
    RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);
    EXPECT_TRUE(renderNode.GetNodeName().empty());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace001 end";
}

/**
 * @tc.name: AnimationDebugTrace002
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AnimationDebugTrace002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace002 start";

    system("param set persist.rosen.animationtrace.enabled 1");
    RSAnimationTraceUtils::GetInstance().addAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, true);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(
        NODE_ID, NODE_NAME, PROPERTY_ID, ANIMATION_ID, 0, 0, startValue, endValue, 0, 0, 1);
    RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
        NODE_ID, NODE_NAME, ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, 1);
    RSRenderNode renderNode(NODE_ID);
    RSAnimationTraceUtils::GetInstance().addRenderNodeTrace(renderNode);
    RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);
    system("param set persist.rosen.animationtrace.enabled 0");
    EXPECT_TRUE(renderNode.GetNodeName().empty());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace002 end";
}

/**
 * @tc.name: AnimationDebugTrace003
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, AnimationDebugTrace003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace003 start";

    RSAnimationTraceUtils::GetInstance().isDebugOpen_ = true;
    RSAnimationTraceUtils::GetInstance().addAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, true);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(
        NODE_ID, NODE_NAME, PROPERTY_ID, ANIMATION_ID, 0, 0, startValue, endValue, 0, 0, 1);
    RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
        NODE_ID, NODE_NAME, ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, 1);
    RSRenderNode renderNode(NODE_ID);
    RSAnimationTraceUtils::GetInstance().addRenderNodeTrace(renderNode);
    RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);
    EXPECT_TRUE(renderNode.GetNodeName().empty());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTrace003 end";
}

/**
 * @tc.name: ParseRenderPropertyVaule001
 * @tc.desc: Verify the ParseRenderPropertyVaule
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceTest, ParseRenderPropertyVaule001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest ParseRenderPropertyVaule001 start";

    float val = 1.0f;
    auto floatProperty = std::make_shared<RSRenderAnimatableProperty<float>>(val);
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(floatProperty,
        RSPropertyType::FLOAT), "float:" + std::to_string(val));
    auto red = RgbPalette::Red();
    auto colorProperty = std::make_shared<RSRenderAnimatableProperty<Color>>(red);
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(colorProperty,
        RSPropertyType::RS_COLOR), "Color:4278190335");
    auto matrixProperty = std::make_shared<RSRenderAnimatableProperty<Matrix3f>>();
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(matrixProperty,
        RSPropertyType::MATRIX3F), "Matrix3f");
    auto quaternionProperty = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(
        Quaternion(1.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_NE(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(quaternionProperty,
        RSPropertyType::QUATERNION), "None");
    auto vector2fProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(1.0f, 2.0f));
    EXPECT_NE(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(vector2fProperty,
        RSPropertyType::VECTOR2F), "None");
    auto vector4fProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_NE(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(vector4fProperty,
        RSPropertyType::VECTOR4F), "None");
    auto vector4ColorProperty = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(Vector4<Color>(red));
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(vector4ColorProperty,
        RSPropertyType::VECTOR4_COLOR),
        "Vector4<Color>:x:4278190335,y:4278190335,z:4278190335,w:4278190335");

    RRect rect;
    auto invalidProperty = std::make_shared<RSRenderAnimatableProperty<RRect>>(rect);
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(invalidProperty,
        RSPropertyType::INVALID), "None");

    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest ParseRenderPropertyVaule001 end";
}
} // namespace Rosen
} // namespace OHOS