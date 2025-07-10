/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "drawing_brush.h"
#include "drawing_color.h"
#include "drawing_error_code.h"
#include "drawing_filter.h"
#include "drawing_helper.h"
#include "drawing_mask_filter.h"
#include "drawing_rect.h"
#include "drawing_shadow_layer.h"
#include "drawing_color_filter.h"
#include "effect/color_filter.h"
#include "effect/filter.h"
#include "native_color_space_manager.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingBrushTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingBrushTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingBrushTest::TearDownTestCase() {}
void NativeDrawingBrushTest::SetUp() {}
void NativeDrawingBrushTest::TearDown() {}

static Filter* CastToFilter(OH_Drawing_Filter* cFilter)
{
    return reinterpret_cast<Filter*>(cFilter);
}

/*
 * @tc.name: NativeDrawingBrushTest_brushCreate001
 * @tc.desc: test for create brush and destroy brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brushCreate001, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_EQ(brush == nullptr, false);
    OH_Drawing_BrushDestroy(brush);
}

/*
 * @tc.name: NativeDrawingBrushTest_brushSetColor002
 * @tc.desc: test for the set methods of brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brushSetColor002, TestSize.Level1)
{
    OH_Drawing_Brush* brush1 = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush1, false);
    EXPECT_EQ(OH_Drawing_BrushIsAntiAlias(brush1), false);
    OH_Drawing_BrushSetAntiAlias(nullptr, false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushIsAntiAlias(nullptr), false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BrushSetColor(brush1, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_BrushGetColor(brush1), 0xFFFF0000);
    OH_Drawing_BrushSetColor(nullptr, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetColor(nullptr), 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    constexpr uint8_t alpha = 128;
    OH_Drawing_BrushSetAlpha(brush1, alpha);
    EXPECT_EQ(OH_Drawing_BrushGetAlpha(brush1), alpha);
    OH_Drawing_BrushSetAlpha(nullptr, alpha);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetAlpha(nullptr), 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingBrushTest_brushSetBlendMode003
 * @tc.desc: test for SetBlendMode.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brushSetBlendMode003, TestSize.Level1)
{
    OH_Drawing_BrushSetBlendMode(nullptr, OH_Drawing_BlendMode::BLEND_MODE_CLEAR);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    OH_Drawing_BrushSetBlendMode(brush, OH_Drawing_BlendMode::BLEND_MODE_CLEAR);
}

/*
 * @tc.name: NativeDrawingBrushTest_brushReset004
 * @tc.desc: test for the reset methods of brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brushReset004, TestSize.Level1)
{
    OH_Drawing_Brush* brush1 = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush1, true);
    OH_Drawing_BrushSetColor(brush1, OH_Drawing_ColorSetArgb(0x00, 0xFF, 0x00, 0xFF));
    constexpr uint8_t alpha = 128;
    OH_Drawing_BrushSetAlpha(brush1, alpha);

    OH_Drawing_BrushReset(brush1);
    EXPECT_EQ(OH_Drawing_BrushIsAntiAlias(brush1), false);
    EXPECT_EQ(OH_Drawing_BrushGetColor(brush1), 0xFF000000);
    EXPECT_EQ(OH_Drawing_BrushGetAlpha(brush1), 0xFF);
    OH_Drawing_BrushReset(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_BrushDestroy(brush1);
}

/*
 * @tc.name: NativeDrawingBrushTest_brushGetFilter005
 * @tc.desc: gets the filter from a brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brushGetFilter005, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    OH_Drawing_Filter* cFilter_ = OH_Drawing_FilterCreate();
    EXPECT_NE(cFilter_, nullptr);
    OH_Drawing_Filter* tmpFilter_ = OH_Drawing_FilterCreate();
    EXPECT_NE(tmpFilter_, nullptr);

    OH_Drawing_ColorFilter* colorFilterTmp = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();

    OH_Drawing_FilterSetColorFilter(cFilter_, nullptr);
    OH_Drawing_FilterGetColorFilter(cFilter_, colorFilterTmp);
    NativeHandle<ColorFilter>* colorFilterHandle = Helper::CastTo<OH_Drawing_ColorFilter*,
        NativeHandle<ColorFilter>*>(colorFilterTmp);
    EXPECT_NE(colorFilterHandle, nullptr);
    EXPECT_NE(colorFilterHandle->value, nullptr);
    EXPECT_EQ(colorFilterHandle->value->GetType(), ColorFilter::FilterType::NO_TYPE);

    OH_Drawing_ColorFilter* cColorFilter_ = OH_Drawing_ColorFilterCreateBlendMode(0xFF0000FF, BLEND_MODE_COLOR);
    OH_Drawing_FilterSetColorFilter(cFilter_, cColorFilter_);
    OH_Drawing_BrushSetFilter(brush, cFilter_);
    OH_Drawing_BrushGetFilter(brush, tmpFilter_);

    EXPECT_NE(CastToFilter(tmpFilter_)->GetColorFilter(), nullptr);
    EXPECT_EQ(CastToFilter(tmpFilter_)->GetColorFilter()->GetType(), ColorFilter::FilterType::BLEND_MODE);

    OH_Drawing_BrushSetFilter(nullptr, cFilter_);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BrushGetFilter(nullptr, tmpFilter_);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BrushGetFilter(brush, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_FilterDestroy(cFilter_);
    OH_Drawing_FilterDestroy(tmpFilter_);
    OH_Drawing_ColorFilterDestroy(cColorFilter_);
    OH_Drawing_ColorFilterDestroy(colorFilterTmp);
    OH_Drawing_BrushDestroy(brush);
}

/*
 * @tc.name: NativeDrawingBrushTest_brushSetShadowLayer006
 * @tc.desc: test for the set methods of brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brushSetShadowLayer006, TestSize.Level1)
{
    // blurRadius:-3.f, offset:(-3.f, 3.f), shadowColor:green
    OH_Drawing_ShadowLayer* shadow = OH_Drawing_ShadowLayerCreate(-3.f, -3.f, 3.f, 0xFF00FF00);
    EXPECT_EQ(shadow, nullptr);
    OH_Drawing_ShadowLayerDestroy(nullptr);
    // blurRadius:3.f, offset:(-3.f, 3.f), shadowColor:green
    OH_Drawing_ShadowLayer* shadowLayer = OH_Drawing_ShadowLayerCreate(3.f, -3.f, 3.f, 0xFF00FF00);
    EXPECT_NE(shadowLayer, nullptr);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    OH_Drawing_BrushSetShadowLayer(nullptr, shadowLayer);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BrushSetShadowLayer(brush, nullptr);
    OH_Drawing_BrushSetShadowLayer(brush, shadowLayer);
    OH_Drawing_ShadowLayerDestroy(shadowLayer);
    OH_Drawing_BrushDestroy(brush);
}

/*
 @tc.name: NativeDrawingBrushTest_brushCopy007
 * @tc.desc: test for creates an copy object.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brushCopy007, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_EQ(brush == nullptr, false);
    OH_Drawing_Brush* brush_copy = OH_Drawing_BrushCopy(brush);
    EXPECT_EQ(brush_copy == nullptr, false);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_BrushDestroy(brush_copy);
}

/*
 * @tc.name: NativeDrawingBrushTest_BrushSetColor4f
 * @tc.desc: test for OH_Drawing_BrushSetColor4f.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_BrushSetColor4f, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    OH_NativeColorSpaceManager* colorSpaceManager =
        OH_NativeColorSpaceManager_CreateFromName(ColorSpaceName::ADOBE_RGB);
    EXPECT_EQ(OH_Drawing_BrushSetColor4f(nullptr, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.3f, 0.4f, 0.1f, nullptr), OH_DRAWING_SUCCESS);
    OH_Drawing_BrushDestroy(brush);
    OH_NativeColorSpaceManager_Destroy(colorSpaceManager);
}

/*
 * @tc.name: NativeDrawingBrushTest_BrushSetColor4f001
 * @tc.desc: test for OH_Drawing_BrushSetColor4f.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_BrushSetColor4f001, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    OH_NativeColorSpaceManager* colorSpaceManager =
        OH_NativeColorSpaceManager_CreateFromName(ColorSpaceName::ADOBE_RGB);
    EXPECT_EQ(OH_Drawing_BrushSetColor4f(brush, 1.01f, 1.1f, 1.5f, 1.4f, colorSpaceManager),
        OH_DRAWING_SUCCESS);
    float a = 0;
    float r = 0;
    float g = 0;
    float b = 0;
    EXPECT_EQ(OH_Drawing_BrushGetAlphaFloat(brush, &a), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_BrushGetRedFloat(brush, &r), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_BrushGetGreenFloat(brush, &g), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_BrushGetBlueFloat(brush, &b), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(a - 1.0f) < 0.01);
    EXPECT_TRUE(std::abs(r - 1.0f) < 0.01);
    EXPECT_TRUE(std::abs(g - 1.0f) < 0.01);
    EXPECT_TRUE(std::abs(b - 1.0f) < 0.01);
    EXPECT_EQ(OH_Drawing_BrushSetColor4f(brush, -0.1f, -1.1f, -1.5f, -1.4f, colorSpaceManager),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_BrushGetAlphaFloat(brush, &a), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_BrushGetRedFloat(brush, &r), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_BrushGetGreenFloat(brush, &g), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_BrushGetBlueFloat(brush, &b), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(a) < 0.01);
    EXPECT_TRUE(std::abs(r) < 0.01);
    EXPECT_TRUE(std::abs(g) < 0.01);
    EXPECT_TRUE(std::abs(b) < 0.01);
    OH_Drawing_BrushDestroy(brush);
    OH_NativeColorSpaceManager_Destroy(colorSpaceManager);
}

/*
 @tc.name: NativeDrawingBrushTest_BrushGetAlphaFloat
 * @tc.desc: test for OH_Drawing_BrushGetAlphaFloat.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_BrushGetAlphaFloat, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    float a = 0.0f;
    OH_NativeColorSpaceManager* colorSpaceManager = nullptr;
    OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager);
    EXPECT_EQ(OH_Drawing_BrushGetAlphaFloat(nullptr, &a), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetAlphaFloat(brush, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetAlphaFloat(nullptr, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetAlphaFloat(brush, &a), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(a - 0.2f) < 0.01);
    OH_Drawing_BrushDestroy(brush);
}

/*
 @tc.name: NativeDrawingBrushTest_BrushGetRedFloat
 * @tc.desc: test for OH_Drawing_BrushGetRedFloat.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_BrushGetRedFloat, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    float r = 0.0f;
    OH_NativeColorSpaceManager* colorSpaceManager = nullptr;
    OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager);
    EXPECT_EQ(OH_Drawing_BrushGetRedFloat(nullptr, &r), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetRedFloat(brush, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetRedFloat(nullptr, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetRedFloat(brush, &r), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(r - 0.3f) < 0.01);
    OH_Drawing_BrushDestroy(brush);
}

/*
 @tc.name: NativeDrawingBrushTest_BrushGetGreenFloat
 * @tc.desc: test for OH_Drawing_BrushGetGreenFloat.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_BrushGetGreenFloat, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    float g = 0.0f;
    OH_NativeColorSpaceManager* colorSpaceManager = nullptr;
    OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager);
    EXPECT_EQ(OH_Drawing_BrushGetGreenFloat(nullptr, &g), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetGreenFloat(brush, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetGreenFloat(nullptr, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetGreenFloat(brush, &g), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(g - 0.4f) < 0.01);
    OH_Drawing_BrushDestroy(brush);
}

/*
 @tc.name: NativeDrawingBrushTest_BrushGetBlueFloat
 * @tc.desc: test for OH_Drawing_BrushGetBlueFloat.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_BrushGetBlueFloat, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    float b = 0.2f;
    OH_NativeColorSpaceManager* colorSpaceManager = nullptr;
    OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager);
    EXPECT_EQ(OH_Drawing_BrushGetBlueFloat(nullptr, &b), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetBlueFloat(brush, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetBlueFloat(nullptr, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_BrushGetBlueFloat(brush, &b), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(b - 0.1f) < 0.01);
    OH_Drawing_BrushDestroy(brush);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS