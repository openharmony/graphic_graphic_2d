/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "drawing_error_code.h"
#include "drawing_image.h"
#include "drawing_point.h"
#include "drawing_sampling_options.h"
#include "drawing_shader_effect.h"
#include "gtest/gtest.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingShaderEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

constexpr uint32_t POINT_NUM = 3;

void NativeDrawingShaderEffectTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingShaderEffectTest::TearDownTestCase() {}
void NativeDrawingShaderEffectTest::SetUp() {}
void NativeDrawingShaderEffectTest::TearDown() {}

constexpr uint32_t NUM_20 = 20;
constexpr uint32_t NUM_300 = 300;

/*
 * @tc.name: OH_Drawing_ShaderEffectCreateLinearGradient
 * @tc.desc: Test OH_Drawing_ShaderEffectCreateLinearGradient
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingShaderEffectTest, OH_Drawing_ShaderEffectCreateLinearGradient, TestSize.Level1)
{
    OH_Drawing_Point* startPt = OH_Drawing_PointCreate(NUM_20, NUM_20);
    OH_Drawing_Point* endPt = OH_Drawing_PointCreate(NUM_300, NUM_300);
    uint32_t colors[] = { 0xFFFFFFFF };
    float pos[] = { 0.0f, 1.0f };
    OH_Drawing_ShaderEffect* effect =
        OH_Drawing_ShaderEffectCreateLinearGradient(nullptr, nullptr, nullptr, nullptr, 1, OH_Drawing_TileMode::CLAMP);
    ASSERT_TRUE(effect == nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ShaderEffectCreateLinearGradient(nullptr, endPt, colors, pos, 1, OH_Drawing_TileMode::CLAMP);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ShaderEffectCreateLinearGradient(startPt, nullptr, colors, pos, 1, OH_Drawing_TileMode::CLAMP);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, nullptr, pos, 1, OH_Drawing_TileMode::CLAMP);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ShaderEffect* effect2 = OH_Drawing_ShaderEffectCreateLinearGradient(startPt,
        endPt, nullptr, pos, 1, static_cast<OH_Drawing_TileMode>(-1));
    EXPECT_EQ(effect2, nullptr);
    OH_Drawing_ShaderEffect* effect3 = OH_Drawing_ShaderEffectCreateLinearGradient(startPt,
        endPt, nullptr, pos, 1, static_cast<OH_Drawing_TileMode>(99));
    EXPECT_EQ(effect3, nullptr);
    OH_Drawing_ShaderEffect* effect4 = OH_Drawing_ShaderEffectCreateLinearGradient(startPt,
        endPt, colors, nullptr, 1, OH_Drawing_TileMode::CLAMP);
    EXPECT_NE(effect4, nullptr);
    OH_Drawing_ShaderEffectDestroy(effect4);
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
}

/*
 * @tc.name: OH_Drawing_ShaderEffectCreateRadialGradient
 * @tc.desc: Test OH_Drawing_ShaderEffectCreateRadialGradient
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingShaderEffectTest, OH_Drawing_ShaderEffectCreateRadialGradient001, TestSize.Level1)
{
    OH_Drawing_ShaderEffect* effect =
        OH_Drawing_ShaderEffectCreateRadialGradient(nullptr, 1, nullptr, nullptr, 1, OH_Drawing_TileMode::CLAMP);
    ASSERT_TRUE(effect == nullptr);
    uint32_t gColors[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000 };
    float_t gPos[] = { 0.25f, 0.75f };
    float radius = 200;
    float x = 200;
    float y = 200;
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(x, y);
    OH_Drawing_ShaderEffect* effect1 =
        OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, radius, gColors, gPos, 1, OH_Drawing_TileMode::REPEAT);
    ASSERT_TRUE(effect1 != nullptr);
    OH_Drawing_ShaderEffectCreateRadialGradient(nullptr, radius, gColors, gPos, 1, OH_Drawing_TileMode::REPEAT);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, radius, nullptr, gPos, 1, OH_Drawing_TileMode::REPEAT);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, radius, gColors, gPos, 1,
        static_cast<OH_Drawing_TileMode>(-1)), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, radius, gColors, gPos, 1,
        static_cast<OH_Drawing_TileMode>(99)), nullptr);
    OH_Drawing_ShaderEffect* effect2 = OH_Drawing_ShaderEffectCreateRadialGradient(centerPt,
        radius, gColors, nullptr, 1, OH_Drawing_TileMode::REPEAT);
    EXPECT_NE(effect2, nullptr);
    OH_Drawing_ShaderEffectDestroy(effect1);
    OH_Drawing_ShaderEffectDestroy(effect2);
}

/*
 * @tc.name: OH_Drawing_ShaderEffectCreateImageShader
 * @tc.desc: Test OH_Drawing_ShaderEffectCreateImageShader
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingShaderEffectTest, OH_Drawing_ShaderEffectCreateImageShader, TestSize.Level1)
{
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    ASSERT_TRUE(image != nullptr);
    OH_Drawing_SamplingOptions* options = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_LINEAR, MIPMAP_MODE_LINEAR);
    ASSERT_TRUE(options != nullptr);
    OH_Drawing_Matrix* matrix = nullptr;
    OH_Drawing_ShaderEffect* effect = OH_Drawing_ShaderEffectCreateImageShader(image, CLAMP, CLAMP, options, matrix);
    ASSERT_TRUE(effect != nullptr);
    OH_Drawing_ShaderEffectCreateImageShader(nullptr, CLAMP, CLAMP, options, matrix);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ShaderEffectCreateImageShader(image, CLAMP, CLAMP, nullptr, matrix);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateImageShader(image, static_cast<OH_Drawing_TileMode>(-1),
        CLAMP, options, matrix), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateImageShader(image, static_cast<OH_Drawing_TileMode>(99),
        CLAMP, options, matrix), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateImageShader(image, CLAMP,
        static_cast<OH_Drawing_TileMode>(-1), options, matrix), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateImageShader(image, CLAMP,
        static_cast<OH_Drawing_TileMode>(99), options, matrix), nullptr);
}

/*
 * @tc.name: NativeDrawingShaderEffectTest_ShaderEffectCreateLinearGradientWithLocalMatrix001
 * @tc.desc: test for creates an <b>OH_Drawing_ShaderEffect</b> that generates a linear gradient between
 * the two specified points.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingShaderEffectTest,
    NativeDrawingShaderEffectTest_ShaderEffectCreateLinearGradientWithLocalMatrix001, TestSize.Level1)
{
    float pos[POINT_NUM] = {0.0f, 0.5f, 1.0f}; // 0.5f: gradient color points, 1.0f: gradient color points
    OH_Drawing_Point2D start = {0, 0};
    OH_Drawing_Point2D end = {100.f, 0}; // 100.f: end point's x
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
        &start, &end, nullptr, pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);

    uint32_t colors[POINT_NUM] = {0xFF00FFFF, 0xFFFF00FF, 0xFFFFFF00};
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
        &start, nullptr, colors, pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
        nullptr, &end, colors, pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
        &start, &end, colors, pos, POINT_NUM, static_cast<OH_Drawing_TileMode>(-1), nullptr), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
        &start, &end, colors, pos, POINT_NUM, static_cast<OH_Drawing_TileMode>(99), nullptr), nullptr);
    OH_Drawing_ShaderEffect* effect1 = OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
        &start, &end, colors, nullptr, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr);
    EXPECT_NE(effect1, nullptr);
    OH_Drawing_ShaderEffect* effect2 = OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
        &start, &end, colors, pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr);
    EXPECT_NE(effect2, nullptr);
    OH_Drawing_ShaderEffectDestroy(effect1);
    OH_Drawing_ShaderEffectDestroy(effect2);
}

/*
 * @tc.name: NativeDrawingShaderEffectTest_ShaderEffectCreateRadialGradientWithLocalMatrix002
 * @tc.desc: test for creates an <b>OH_Drawing_ShaderEffect</b> that generates a linear gradient between
 * the two specified points.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingShaderEffectTest,
    NativeDrawingShaderEffectTest_ShaderEffectCreateRadialGradientWithLocalMatrix002, TestSize.Level1)
{
    float pos[POINT_NUM] = {0.0f, 0.5f, 1.0f}; // 0.5f: gradient color points, 1.0f: gradient color points
    float radius = 5.0; // 5.0f: gradient color radius
    OH_Drawing_Point2D start = {100.f, 0};  // 100.f: start point's x
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(
        &start, radius, nullptr, pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);

    uint32_t colors[POINT_NUM] = {0xFF00FFFF, 0xFFFF00FF, 0xFFFFFF00};
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(
        nullptr, radius, colors, pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(
        &start, radius, colors, pos, POINT_NUM, static_cast<OH_Drawing_TileMode>(-1), nullptr), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(
        &start, radius, colors, pos, POINT_NUM, static_cast<OH_Drawing_TileMode>(99), nullptr), nullptr);
    EXPECT_NE(OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(
        &start, radius, colors, pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);
    EXPECT_NE(OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(
        &start, radius, colors, nullptr, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);
}

/*
 * @tc.name: NativeDrawingShaderEffectTest_ShaderEffectCreateTwoPointConicalGradient003
 * @tc.desc: test for greates an <b>OH_Drawing_ShaderEffect</b> that generates a conical gradient given two circles.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingShaderEffectTest, NativeDrawingShaderEffectTest_ShaderEffectCreateTwoPointConicalGradient003,
    TestSize.Level1)
{
    OH_Drawing_Point2D startPt = {0, 0};
    OH_Drawing_Point2D endPt = {100.f, 0}; // 100.f: end point's x
    uint32_t colors[POINT_NUM] = {0xFF00FFFF, 0xFFFF00FF, 0xFFFFFF00};
    float pos[POINT_NUM] = {0.0f, 0.5f, 1.0f}; // 0.5f: gradient color points, 1.0f: gradient color points
    float startRadius = 5.0f, endRadius = 10.0f; // 5.0f: gradient color radius, 10.0f: gradient color radius
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(&startPt, startRadius, nullptr, endRadius, colors,
        pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(nullptr, startRadius, &endPt, endRadius, colors,
        pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(&startPt, startRadius, &endPt, endRadius, nullptr,
        pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(&startPt, startRadius, &endPt, endRadius, colors,
        pos, POINT_NUM, static_cast<OH_Drawing_TileMode>(-1), nullptr), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(&startPt, startRadius, &endPt, endRadius, colors,
        pos, POINT_NUM, static_cast<OH_Drawing_TileMode>(99), nullptr), nullptr);
    EXPECT_NE(OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(&startPt, startRadius, &endPt, endRadius, colors,
        pos, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);
    EXPECT_NE(OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(&startPt, startRadius, &endPt, endRadius, colors,
        nullptr, POINT_NUM, OH_Drawing_TileMode::CLAMP, nullptr), nullptr);
}

/*
 * @tc.name: NativeDrawingShaderEffectTest_ShaderEffectCreateSweepGradient001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingShaderEffectTest, NativeDrawingShaderEffectTest_ShaderEffectCreateSweepGradient001,
    TestSize.Level1)
{
    OH_Drawing_Point* startPt = OH_Drawing_PointCreate(0, 0);
    uint32_t colors[POINT_NUM] = {0xFF00FFFF, 0xFFFF00FF, 0xFFFFFF00};
    float pos[POINT_NUM] = {0.0f, 0.5f, 1.0f}; // 0.5f: gradient color points, 1.0f: gradient color points
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateSweepGradient(nullptr, colors, pos, POINT_NUM,
        OH_Drawing_TileMode::CLAMP), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateSweepGradient(nullptr, nullptr, pos, POINT_NUM,
        OH_Drawing_TileMode::CLAMP), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateSweepGradient(startPt, nullptr, pos, POINT_NUM,
        OH_Drawing_TileMode::CLAMP), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateSweepGradient(startPt, colors, pos, POINT_NUM,
        static_cast<OH_Drawing_TileMode>(-1)), nullptr);
    EXPECT_EQ(OH_Drawing_ShaderEffectCreateSweepGradient(startPt, colors, pos, POINT_NUM,
        static_cast<OH_Drawing_TileMode>(99)), nullptr);
    EXPECT_NE(OH_Drawing_ShaderEffectCreateSweepGradient(startPt, colors, nullptr, POINT_NUM,
        OH_Drawing_TileMode::CLAMP), nullptr);
}

/*
 * @tc.name: NativeDrawingShaderEffectTest_ShaderEffectCreateColorShader004
 * @tc.desc: test for creates an <b>OH_Drawing_ShaderEffect</b> that generates a shader with single color.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingShaderEffectTest, NativeDrawingShaderEffectTest_ShaderEffectCreateColorShader004,
    TestSize.Level1)
{
    const uint32_t color = 0xFFFF0000;
    OH_Drawing_ShaderEffect * colorShaderEffect = OH_Drawing_ShaderEffectCreateColorShader(color);
    EXPECT_NE(colorShaderEffect, nullptr);
    OH_Drawing_ShaderEffectDestroy(colorShaderEffect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS