/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <parameter.h>
#include <parameters.h>
#include <unistd.h>
#include "param/sys_param.h"
#include "hpae_base/rs_hpae_fusion_operator.h"
#include "hpae_base/rs_hpae_base_types.h"
#include "hpae_base/rs_hpae_base_data.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_render_maskcolor_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHpaeFusionOperatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::string hpaeSwitch;
    static inline std::string hpaeAaeSwitch;

    static constexpr int32_t CANVAS_SIZE = 100;
    static constexpr float EPSILON = 1e-6;
    static bool FloatEqual(float a, float b);
};

void RSHpaeFusionOperatorTest::SetUpTestCase()
{
    hpaeSwitch = OHOS::system::GetParameter("debug.graphic.hpae.blur.enabled", "0");
    hpaeAaeSwitch = OHOS::system::GetParameter("rosen.graphic.hpae.blur.aae.enabled", "0");
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", "1");
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", "1");
}
void RSHpaeFusionOperatorTest::TearDownTestCase()
{
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", hpaeSwitch);
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", hpaeAaeSwitch);
}
void RSHpaeFusionOperatorTest::SetUp() {}
void RSHpaeFusionOperatorTest::TearDown() {}

bool RSHpaeFusionOperatorTest::FloatEqual(float a, float b)
{
    return std::fabs(a - b) < EPSILON;
}

/**
 * @tc.name: ProcessGreyAndStretchTest
 * @tc.desc: Verify function ProcessGreyAndStretch
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeFusionOperatorTest, ProcessGreyAndStretchTest, TestSize.Level1)
{
    HpaeBufferInfo targetBuffer;
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(CANVAS_SIZE, CANVAS_SIZE);
    targetBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());

    auto image = std::make_shared<Drawing::Image>();
    Drawing::RectI clipBounds(0, 0, 50, 50);
    Drawing::RectI src(0, 0, 50, 50);
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    RSHpaeBaseData::GetInstance().hpaeStatus_.greyCoef = Vector2f(1.0f, 1.0f);

    int ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.greyCoef = Vector2f(0, 1.0f);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.greyCoef = Vector2f(1.0f, 0);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.greyCoef = Vector2f(0, 0);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: ProcessGreyAndStretchTest01
 * @tc.desc: Verify function ProcessGreyAndStretch
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeFusionOperatorTest, ProcessGreyAndStretchTest01, TestSize.Level1)
{
    Drawing::RectI clipBounds(0, 0, 50, 50);
    Drawing::RectI src(0, 0, 50, 50);
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());

    HpaeBufferInfo targetBuffer;
    auto image = std::make_shared<Drawing::Image>();

    int ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, -1);

    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, nullptr, targetBuffer, filter, src);
    EXPECT_EQ(ret, -1);

    auto drawingCanvas = std::make_unique<Drawing::Canvas>(CANVAS_SIZE, CANVAS_SIZE);
    targetBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());

    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, nullptr, targetBuffer, filter, src);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: ProcessGreyAndStretchTest02
 * @tc.desc: Verify function ProcessGreyAndStretch
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeFusionOperatorTest, ProcessGreyAndStretchTest02, TestSize.Level1)
{
    HpaeBufferInfo targetBuffer;
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(CANVAS_SIZE, CANVAS_SIZE);
    targetBuffer.canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());

    auto image = std::make_shared<Drawing::Image>();
    Drawing::RectI clipBounds(0, 0, 50, 50);
    Drawing::RectI src(0, 0, 50, 50);
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(0, 0, 0, 0);
    int ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(1.0f, 0, 0, 0);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(0, 1.0f, 0, 0);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(0, 0, 1.0f, 0);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(0, 0, 0, 1.0f);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(1.0f, 1.0f, 0, 0);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(1.0f, 0, 1.0f, 0);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(1.0f, 0, 0, 1.0f);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(0, 1.0f, 1.0f, 0);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(0, 1.0f, 0, 1.0f);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(0, 0, 1.0f, 1.0f);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(1.0f, 1.0f, 1.0f, 0);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(1.0f, 1.0f, 0, 1.0f);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(1.0f, 0, 1.0f, 1.0f);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);

    RSHpaeBaseData::GetInstance().hpaeStatus_.pixelStretch = Vector4f(0, 1.0f, 1.0f, 1.0f);
    ret = RSHpaeFusionOperator::ProcessGreyAndStretch(clipBounds, image, targetBuffer, filter, src);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: GetColorMatrixCoefTest
 * @tc.desc: Verify function GetColorMatrixCoef
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeFusionOperatorTest, GetColorMatrixCoefTest, TestSize.Level1)
{
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    HaeBlurEffectAttr effectInfo;

    RSHpaeBaseData::GetInstance().hpaeStatus_.brightness = 2.0f;
    RSHpaeBaseData::GetInstance().hpaeStatus_.saturation = 3.0f;

    RSHpaeFusionOperator::GetColorMatrixCoef(filter, effectInfo.colorMatrixCoef);

    EXPECT_TRUE(FloatEqual(effectInfo.colorMatrixCoef[Drawing::ColorMatrix::TRANS_FOR_R], 1.0f));
    EXPECT_TRUE(FloatEqual(effectInfo.colorMatrixCoef[Drawing::ColorMatrix::TRANS_FOR_G], 1.0f));
    EXPECT_TRUE(FloatEqual(effectInfo.colorMatrixCoef[Drawing::ColorMatrix::TRANS_FOR_B], 1.0f));
}

/**
 * @tc.name: GetHaePixelTest
 * @tc.desc: Verify function GetHaePixel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeFusionOperatorTest, GetHaePixelTest, TestSize.Level1)
{
    RSColor maskColors(4, 3, 2, 1);
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSMaskColorShaderFilter>(0, maskColors));
    HaePixel haePixel = RSHpaeFusionOperator::GetHaePixel(filter);

    EXPECT_EQ(haePixel.r, 16);
    EXPECT_EQ(haePixel.g, 12);
    EXPECT_EQ(haePixel.b, 8);
    EXPECT_EQ(haePixel.a, 4);

    filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    haePixel = RSHpaeFusionOperator::GetHaePixel(filter);

    EXPECT_EQ(haePixel.r, 0);
    EXPECT_EQ(haePixel.g, 0);
    EXPECT_EQ(haePixel.b, 0);
    EXPECT_EQ(haePixel.a, 0);

    filter = nullptr;
    haePixel = RSHpaeFusionOperator::GetHaePixel(filter);

    EXPECT_EQ(haePixel.r, 0);
    EXPECT_EQ(haePixel.g, 0);
    EXPECT_EQ(haePixel.b, 0);
    EXPECT_EQ(haePixel.a, 0);
}

/**
 * @tc.name: GetShaderTransformTest
 * @tc.desc: Verify function GetShaderTransform
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeFusionOperatorTest, GetShaderTransformTest, TestSize.Level1)
{
    Drawing::Rect blurRect(10, 10, 50, 50);
    auto matrix = RSHpaeFusionOperator::GetShaderTransform(blurRect, 1.0f, 1.0f);
    Drawing::Matrix::Buffer buffer;
    matrix.GetAll(buffer);

    EXPECT_TRUE(FloatEqual(buffer[Drawing::Matrix::SCALE_X], 1.0f));
    EXPECT_TRUE(FloatEqual(buffer[Drawing::Matrix::TRANS_X], 10.f));
    EXPECT_TRUE(FloatEqual(buffer[Drawing::Matrix::SCALE_Y], 1.0f));
    EXPECT_TRUE(FloatEqual(buffer[Drawing::Matrix::TRANS_Y], 10.f));
}

} // namespace Rosen
} // namespace OHOS