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
class RSRenderAnimationDebugTraceUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t NODE_ID = 454554;
    static constexpr uint64_t ANIMATION_ID = 36522;
    static constexpr uint64_t PROPERTY_ID = 853212;
    const std::string NODE_NAME = "UnitTest";
};

void RSRenderAnimationDebugTraceUnitTest::SetUpTestCase() {}
void RSRenderAnimationDebugTraceUnitTest::TearDownTestCase() {}
void RSRenderAnimationDebugTraceUnitTest::SetUp() {}
void RSRenderAnimationDebugTraceUnitTest::TearDown() {}

/**
 * @tc.name: AnimationDebugTraceUnit001
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceUnitTest, AnimationDebugTraceUnit001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTraceUnit001 start";

    RSAnimationTraceUtils::GetInstance().addAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, false);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(101.0f);
    RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(
        NODE_ID, NODE_NAME, PROPERTY_ID, ANIMATION_ID, 0, 0, startValue, endValue, 0, 0, 1);
    RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
        NODE_ID, NODE_NAME, ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, 1);
    RSRenderNode renderNode(NODE_ID);
    RSAnimationTraceUtils::GetInstance().addRenderNodeTrace(renderNode);
    RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);

    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTraceUnit001 end";
}

/**
 * @tc.name: AnimationDebugTraceUnit002
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceUnitTest, AnimationDebugTraceUnit002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTraceUnit002 start";

    system("param set persist.rosen.animationtrace.enabled 1");
    RSAnimationTraceUtils::GetInstance().addAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, true);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(101.0f);
    RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(
        NODE_ID, NODE_NAME, PROPERTY_ID, ANIMATION_ID, 0, 0, startValue, endValue, 0, 0, 1);
    RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
        NODE_ID, NODE_NAME, ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, 1);
    RSRenderNode renderNode(NODE_ID);
    RSAnimationTraceUtils::GetInstance().addRenderNodeTrace(renderNode);
    RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);
    system("param set persist.rosen.animationtrace.enabled 0");

    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTraceUnit002 end";
}

/**
 * @tc.name: AnimationDebugTraceUnit003
 * @tc.desc: Verify the AnimationDebugTrace
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceUnitTest, AnimationDebugTraceUnit003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTraceUnit003 start";

    RSAnimationTraceUtils::GetInstance().isDebugOpen_ = true;
    RSAnimationTraceUtils::GetInstance().addAnimationNameTrace(NODE_NAME);
    RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(NODE_NAME, NODE_ID, ANIMATION_ID, true);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(101.0f);
    RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(
        NODE_ID, NODE_NAME, PROPERTY_ID, ANIMATION_ID, 0, 0, startValue, endValue, 0, 0, 1);
    RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
        NODE_ID, NODE_NAME, ANIMATION_ID, PROPERTY_ID, 0, startValue, 0, 0, 1);
    RSRenderNode renderNode(NODE_ID);
    RSAnimationTraceUtils::GetInstance().addRenderNodeTrace(renderNode);
    RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(PROPERTY_ID, ANIMATION_ID, startValue, endValue);

    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AnimationDebugTraceUnit003 end";
}

/**
 * @tc.name: ParseRenderPropertyVauleUnit001
 * @tc.desc: Verify the ParseRenderPropertyVaule
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationDebugTraceUnitTest, ParseRenderPropertyVauleUnit001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest ParseRenderPropertyVauleUnit001 start";

    float val = 1.0f;
    auto floatPropertyUt = std::make_shared<RSRenderAnimatableProperty<float>>(val);
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(floatPropertyUt,
        RSRenderPropertyType::PROPERTY_FLOAT), "float:" + std::to_string(val));
    auto red = RgbPalette::Red();
    auto colorProperty = std::make_shared<RSRenderAnimatableProperty<Color>>(red);
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(colorProperty,
        RSRenderPropertyType::PROPERTY_COLOR), "Color:4278190335");
    auto matrixProperty = std::make_shared<RSRenderAnimatableProperty<Matrix3f>>();
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(matrixProperty,
        RSRenderPropertyType::PROPERTY_MATRIX3F), "Matrix3f");
    auto quaternionProperty = std::make_shared<RSRenderAnimatableProperty<Quaternion>>(
        Quaternion(1.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_NE(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(quaternionProperty,
        RSRenderPropertyType::PROPERTY_QUATERNION), "None");
    auto filterProperty = std::make_shared<RSRenderAnimatableProperty<std::shared_ptr<RSFilter>>>(nullptr);
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(filterProperty,
        RSRenderPropertyType::PROPERTY_FILTER), "FILTER");
    auto vector2fProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(Vector2f(1.0f, 2.0f));
    EXPECT_NE(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(vector2fProperty,
        RSRenderPropertyType::PROPERTY_VECTOR2F), "None");
    auto vector4fProperty = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_NE(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(vector4fProperty,
        RSRenderPropertyType::PROPERTY_VECTOR4F), "None");
    auto vector4ColorProperty = std::make_shared<RSRenderAnimatableProperty<Vector4<Color>>>(Vector4<Color>(red));
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(vector4ColorProperty,
        RSRenderPropertyType::PROPERTY_VECTOR4_COLOR),
        "Vector4<Color>:x:4278190335,y:4278190335,z:4278190335,w:4278190335");

    RRect rect;
    auto invalidPropertyUt = std::make_shared<RSRenderAnimatableProperty<RRect>>(rect);
    EXPECT_EQ(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(invalidPropertyUt,
        RSRenderPropertyType::INVALID), "None");

    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest ParseRenderPropertyVauleUnit001 end";
}
} // namespace Rosen
} // namespace OHOS