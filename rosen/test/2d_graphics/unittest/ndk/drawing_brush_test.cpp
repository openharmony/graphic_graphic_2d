/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "drawing_filter.h"
#include "drawing_mask_filter.h"
#include "drawing_rect.h"
#include "drawing_color_filter.h"
#include "effect/color_filter.h"
#include "effect/filter.h"

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

void NativeDrawingBrushTest::SetUpTestCase() {}
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
    OH_Drawing_BrushSetColor(brush1, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_BrushGetColor(brush1), 0xFFFF0000);
    constexpr uint8_t alpha = 128;
    OH_Drawing_BrushSetAlpha(brush1, alpha);
    EXPECT_EQ(OH_Drawing_BrushGetAlpha(brush1), alpha);
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
    EXPECT_EQ((reinterpret_cast<ColorFilter*>(colorFilterTmp))->GetType(),
        ColorFilter::FilterType::NO_TYPE);

    OH_Drawing_ColorFilter* cColorFilter_ = OH_Drawing_ColorFilterCreateBlendMode(0xFF0000FF, BLEND_MODE_COLOR);
    OH_Drawing_FilterSetColorFilter(cFilter_, cColorFilter_);
    OH_Drawing_BrushSetFilter(brush, cFilter_);
    OH_Drawing_BrushGetFilter(brush, tmpFilter_);

    EXPECT_NE(CastToFilter(tmpFilter_)->GetColorFilter(), nullptr);
    EXPECT_EQ(CastToFilter(tmpFilter_)->GetColorFilter()->GetType(), ColorFilter::FilterType::BLEND_MODE);
    OH_Drawing_FilterDestroy(cFilter_);
    OH_Drawing_FilterDestroy(tmpFilter_);
    OH_Drawing_ColorFilterDestroy(cColorFilter_);
    OH_Drawing_ColorFilterDestroy(colorFilterTmp);
    OH_Drawing_BrushDestroy(brush);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS