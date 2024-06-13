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

void NativeDrawingShaderEffectTest::SetUpTestCase() {}
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
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS