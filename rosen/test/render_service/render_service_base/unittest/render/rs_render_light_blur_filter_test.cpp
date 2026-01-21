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
#include <memory>

#include "gtest/gtest.h"

#include "draw/brush.h"
#include "draw/surface.h"
#include "image/image.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "render/rs_render_light_blur_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockCanvasLightBlur : public Drawing::Canvas {
public:
    explicit MockCanvasLightBlur(Drawing::Surface* surface);
    Drawing::Surface* GetSurface() const override;
private:
    Drawing::Surface* surface_ = nullptr;
};

MockCanvasLightBlur::MockCanvasLightBlur(Drawing::Surface* surface)
    : surface_(surface)
{
}
Drawing::Surface* MockCanvasLightBlur::GetSurface() const
{
    return surface_;
}

class RSLightBlurShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLightBlurShaderFilterTest::SetUpTestCase() {}
void RSLightBlurShaderFilterTest::TearDownTestCase() {}
void RSLightBlurShaderFilterTest::SetUp() {}
void RSLightBlurShaderFilterTest::TearDown() {}

/**
 * @tc.name: RSLightBlurShaderFilterTest001
 * @tc.desc: Verify function GetRadius and GetType
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLightBlurShaderFilterTest, RSLightBlurShaderFilterTest001, TestSize.Level1)
{
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    EXPECT_EQ(filter->GetType(), RSUIFilterType::LIGHT_BLUR);
    EXPECT_EQ(filter->GetRadius(), 1);
}

/**
 * @tc.name: RSLightBlurShaderFilterTest002
 * @tc.desc: Verify function ApplyLightBlur
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLightBlurShaderFilterTest, RSLightBlurShaderFilterTest002, TestSize.Level1)
{
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Rect src(0, 0, 10, 10);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);
}

/**
 * @tc.name: RSLightBlurShaderFilterTest003
 * @tc.desc: Verify function ApplyLightBlur
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSLightBlurShaderFilterTest, RSLightBlurShaderFilterTest003, TestSize.Level1)
{
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Rect src(12, 12, 10, 10);
    Drawing::Rect dst(8, 8, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);
}

/**
 * @tc.name: RSLightBlurShaderFilterTest004
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, RSLightBlurShaderFilterTest004, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_UNKNOWN_ALPHATYPE_OPAQUE
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_UNKNOWN_ALPHATYPE_OPAQUE, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_UNKNOWN, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_ALPHA_8_ALPHATYPE_OPAQUE
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_ALPHA_8_ALPHATYPE_OPAQUE, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_ALPHA_8, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGB_565_ALPHATYPE_OPAQUE
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGB_565_ALPHATYPE_OPAQUE, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGB_565, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_ARGB_4444_ALPHATYPE_OPAQUE
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_ARGB_4444_ALPHATYPE_OPAQUE, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_ARGB_4444, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_BGRA_8888_ALPHATYPE_OPAQUE
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_BGRA_8888_ALPHATYPE_OPAQUE, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_BGRA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGBA_F16_ALPHATYPE_OPAQUE
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGBA_F16_ALPHATYPE_OPAQUE, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_N32_ALPHATYPE_OPAQUE
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_N32_ALPHATYPE_OPAQUE, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatGBA_1010102_ALPHATYPE_OPAQUE
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatGBA_1010102_ALPHATYPE_OPAQUE, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_1010102, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_GRAY_8_ALPHATYPE_OPAQUE
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_GRAY_8_ALPHATYPE_OPAQUE, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_GRAY_8, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGB_888X_ALPHATYPE_OPAQUE
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGB_888X_ALPHATYPE_OPAQUE, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGB_888X, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: RSLightBlurShaderFilterTest005
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, RSLightBlurShaderFilterTest005, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_UNKNOWN_ALPHATYPE_UNKNOWN
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_UNKNOWN_ALPHATYPE_UNKNOWN, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_UNKNOWN, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_ALPHA_8_ALPHATYPE_UNKNOWN
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_ALPHA_8_ALPHATYPE_UNKNOWN, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_ALPHA_8, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGB_565_ALPHATYPE_UNKNOWN
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGB_565_ALPHATYPE_UNKNOWN, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGB_565, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_ARGB_4444_ALPHATYPE_UNKNOWN
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_ARGB_4444_ALPHATYPE_UNKNOWN, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_ARGB_4444, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_BGRA_8888_ALPHATYPE_UNKNOWN
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_BGRA_8888_ALPHATYPE_UNKNOWN, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_BGRA_8888, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGBA_F16_ALPHATYPE_UNKNOWN
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGBA_F16_ALPHATYPE_UNKNOWN, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_N32_ALPHATYPE_UNKNOWN
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_N32_ALPHATYPE_UNKNOWN, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatGBA_1010102_ALPHATYPE_UNKNOWN
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatGBA_1010102_ALPHATYPE_UNKNOWN, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_1010102, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_GRAY_8_ALPHATYPE_UNKNOWN
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_GRAY_8_ALPHATYPE_UNKNOWN, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_GRAY_8, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGB_888X_ALPHATYPE_UNKNOWN
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGB_888X_ALPHATYPE_UNKNOWN, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGB_888X, Drawing::ALPHATYPE_UNKNOWN };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: RSLightBlurShaderFilterTest006
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, RSLightBlurShaderFilterTest006, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_UNKNOWN_ALPHATYPE_PREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_UNKNOWN_ALPHATYPE_PREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_UNKNOWN, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_ALPHA_8_ALPHATYPE_PREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_ALPHA_8_ALPHATYPE_PREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_ALPHA_8, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGB_565_ALPHATYPE_PREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGB_565_ALPHATYPE_PREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGB_565, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_ARGB_4444_ALPHATYPE_PREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_ARGB_4444_ALPHATYPE_PREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_ARGB_4444, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_BGRA_8888_ALPHATYPE_PREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_BGRA_8888_ALPHATYPE_PREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_BGRA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGBA_F16_ALPHATYPE_PREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGBA_F16_ALPHATYPE_PREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_N32_ALPHATYPE_PREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_N32_ALPHATYPE_PREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatGBA_1010102_ALPHATYPE_PREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatGBA_1010102_ALPHATYPE_PREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_1010102, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_GRAY_8_ALPHATYPE_PREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_GRAY_8_ALPHATYPE_PREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_GRAY_8, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGB_888X_ALPHATYPE_PREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGB_888X_ALPHATYPE_PREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGB_888X, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: RSLightBlurShaderFilterTest007
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, RSLightBlurShaderFilterTest007, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_UNKNOWN_ALPHATYPE_UNPREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_UNKNOWN_ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_UNKNOWN, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_FALSE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_ALPHA_8_ALPHATYPE_UNPREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_ALPHA_8_ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_ALPHA_8, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGB_565_ALPHATYPE_UNPREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGB_565_ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGB_565, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_ARGB_4444_ALPHATYPE_UNPREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_ARGB_4444_ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_ARGB_4444, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_BGRA_8888_ALPHATYPE_UNPREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_BGRA_8888ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_BGRA_8888, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGBA_F16_ALPHATYPE_UNPREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGBA_F16_ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_N32_ALPHATYPE_UNPREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_N32_ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatGBA_1010102_ALPHATYPE_UNPREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatGBA_1010102_ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_1010102, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_GRAY_8_ALPHATYPE_UNPREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_GRAY_8_ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_GRAY_8, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: TestFormatCOLORTYPE_RGB_888X_ALPHATYPE_UNPREMUL
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, TestFormatCOLORTYPE_RGB_888X_ALPHATYPE_UNPREMUL, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGB_888X, Drawing::ALPHATYPE_UNPREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = 1;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_EQ(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: RSLightBlurShaderFilterTest008
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, RSLightBlurShaderFilterTest008, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = INT32_MAX;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_NE(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: RSLightBlurShaderFilterTest009
 * @tc.desc: Verify function ApplyLightBlur and ClearLightBlurResultCache
 * @tc.type:FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, RSLightBlurShaderFilterTest009, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap.Build(10, 10, format);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image->BuildFromBitmap(bitmap));
    int radius = INT32_MIN;
    auto filter = std::make_shared<RSLightBlurShaderFilter>(radius);
    ASSERT_NE(filter, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect src(0, 0, 5, 5);
    Drawing::Rect dst(0, 0, 5, 5);
    LightBlurParameter para { src, dst, Drawing::Brush() };
    EXPECT_NE(filter->GetRadius(), 1);
    filter->ApplyLightBlur(canvas, image, para);

    filter->lightBlurResultCache_[0] = std::make_shared<Drawing::Image>();
    filter->lightBlurResultCache_[1] = std::make_shared<Drawing::Image>();
    filter->ClearLightBlurResultCache();
    EXPECT_EQ(filter->lightBlurResultCache_[0], nullptr);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: InitDownSample4xAndMixShader001
 * @tc.desc: Verify function InitDownSample4xAndMixShader
 * @tc.type: FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, InitDownSample4xAndMixShader001, TestSize.Level1)
{
    bool ret = RSLightBlurShaderFilter::InitDownSample4xAndMixShader();
    ASSERT_TRUE(ret);
    EXPECT_NE(RSLightBlurShaderFilter::downSample4xAndMixShader_, nullptr);

    ret = RSLightBlurShaderFilter::InitDownSample4xAndMixShader();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: InitDownSample4xShader001
 * @tc.desc: Verify function InitDownSample4xShader
 * @tc.type: FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, InitDownSample4xShader001, TestSize.Level1)
{
    bool ret = RSLightBlurShaderFilter::InitDownSample4xShader();
    ASSERT_TRUE(ret);
    EXPECT_NE(RSLightBlurShaderFilter::downSample4xShader_, nullptr);

    ret = RSLightBlurShaderFilter::InitDownSample4xShader();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: GetDownSampleImage001
 * @tc.desc: Verify function GetDownSampleImage
 * @tc.type: FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, GetDownSampleImage001, TestSize.Level1)
{
    auto filter = std::make_shared<RSLightBlurShaderFilter>(1.0);
    auto surface = std::make_unique<Drawing::Surface>();
    std::shared_ptr<MockCanvasLightBlur> canvas = std::make_shared<MockCanvasLightBlur>(surface.get());
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::RectI srcRect(0, 0, 10, 10);
    Drawing::RectI dstRect(0, 0, 10, 10);
    auto imageRet = filter->GetDownSampleImage(*image, *surface, srcRect, dstRect);
    EXPECT_EQ(imageRet, nullptr);

    dstRect.SetRight(0);
    dstRect.SetBottom(10);
    imageRet = filter->GetDownSampleImage(*image, *surface, srcRect, dstRect);
    EXPECT_EQ(imageRet, nullptr);

    dstRect.SetRight(10);
    dstRect.SetBottom(0);
    imageRet = filter->GetDownSampleImage(*image, *surface, srcRect, dstRect);
    EXPECT_EQ(imageRet, nullptr);
}

/**
 * @tc.name: GetDownSampleImage4x001
 * @tc.desc: Verify function GetDownSampleImage4x
 * @tc.type: FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, GetDownSampleImage4x001, TestSize.Level1)
{
    auto filter = std::make_shared<RSLightBlurShaderFilter>(1.0);
    auto surface = std::make_unique<Drawing::Surface>();
    MockCanvasLightBlur canvas(surface.get());
    std::shared_ptr<Drawing::Image> image = nullptr;
    Drawing::RectI srcRect(0, 0, 10, 10);
    Drawing::RectI dstRect(0, 0, 10, 10);
    auto imageRet = filter->GetDownSampleImage4x(image, canvas, srcRect, dstRect);
    EXPECT_EQ(imageRet, nullptr);

    RSLightBlurShaderFilter::downSample4xShader_ = nullptr;
    image = std::make_shared<Drawing::Image>();
    imageRet = filter->GetDownSampleImage4x(image, canvas, srcRect, dstRect);
    EXPECT_EQ(imageRet, nullptr);

    bool ret = RSLightBlurShaderFilter::InitDownSample4xShader();
    ASSERT_TRUE(ret);
    imageRet = filter->GetDownSampleImage4x(image, canvas, srcRect, dstRect);
    EXPECT_EQ(imageRet, nullptr);

    dstRect.SetRight(0);
    dstRect.SetBottom(10);
    imageRet = filter->GetDownSampleImage4x(image, canvas, srcRect, dstRect);
    EXPECT_EQ(imageRet, nullptr);

    dstRect.SetRight(10);
    dstRect.SetBottom(0);
    imageRet = filter->GetDownSampleImage4x(image, canvas, srcRect, dstRect);
    EXPECT_EQ(imageRet, nullptr);
}

/**
 * @tc.name: GetDownSample4xAndMixImage001
 * @tc.desc: Verify function GetDownSample4xAndMixImage
 * @tc.type: FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, GetDownSample4xAndMixImage001, TestSize.Level1)
{
    auto filter = std::make_shared<RSLightBlurShaderFilter>(1.0);
    auto surface = std::make_unique<Drawing::Surface>();
    MockCanvasLightBlur canvas(surface.get());
    std::shared_ptr<Drawing::Image> image = nullptr;
    RSLightBlurShaderFilter::downSample4xAndMixShader_ = nullptr;
    auto imageRet = filter->GetDownSample4xAndMixImage(canvas, image);
    EXPECT_EQ(imageRet, nullptr);

    bool ret = RSLightBlurShaderFilter::InitDownSample4xAndMixShader();
    ASSERT_TRUE(ret);
    imageRet = filter->GetDownSample4xAndMixImage(canvas, image);
    EXPECT_EQ(imageRet, nullptr);

    image = std::make_shared<Drawing::Image>();
    imageRet = filter->GetDownSample4xAndMixImage(canvas, image);
    EXPECT_EQ(imageRet, nullptr);

    filter->lightBlurResultCache_[0] = nullptr;
    filter->lightBlurResultCache_[1] = image;
    imageRet = filter->GetDownSample4xAndMixImage(canvas, image);
    EXPECT_EQ(imageRet, nullptr);

    filter->lightBlurResultCache_[0] = image;
    filter->lightBlurResultCache_[1] = nullptr;
    imageRet = filter->GetDownSample4xAndMixImage(canvas, image);
    EXPECT_EQ(imageRet, nullptr);

    filter->lightBlurResultCache_[1] = image;
    imageRet = filter->GetDownSample4xAndMixImage(canvas, image);
    EXPECT_EQ(imageRet, nullptr);
}

/**
 * @tc.name: ApplyLightBlur001
 * @tc.desc: Verify function ApplyLightBlur for image object
 * @tc.type: FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, ApplyLightBlur001, TestSize.Level1)
{
    auto filter = std::make_shared<RSLightBlurShaderFilter>(1.0);
    Drawing::Canvas canvas;
    LightBlurParameter para;
    std::shared_ptr<Drawing::Image> image = nullptr;
    filter->lightBlurResultCache_[1] = nullptr;
    filter->ApplyLightBlur(canvas, image, para);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);

    image = std::make_shared<Drawing::Image>();
    filter->ApplyLightBlur(canvas, image, para);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);

    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, format);
    ASSERT_TRUE(image->BuildFromBitmap(bitmap));
    filter->ApplyLightBlur(canvas, image, para);
    EXPECT_EQ(filter->lightBlurResultCache_[1], nullptr);
}

/**
 * @tc.name: ApplyLightBlur002
 * @tc.desc: Verify function ApplyLightBlur for specal image size
 * @tc.type: FUNC
 * @tc.require: issuesIBZ4YD
 */
HWTEST_F(RSLightBlurShaderFilterTest, ApplyLightBlur002, TestSize.Level1)
{
    auto filter = std::make_shared<RSLightBlurShaderFilter>(1.0);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(1, 1, format);
    ASSERT_TRUE(image->BuildFromBitmap(bitmap));

    filter->lightBlurResultCache_[1] = nullptr;
    auto surface = std::make_unique<Drawing::Surface>();
    MockCanvasLightBlur canvas(surface.get());
    LightBlurParameter para;
    filter->ApplyLightBlur(canvas, image, para);
    EXPECT_NE(filter->lightBlurResultCache_[1], nullptr);

    bitmap.Build(10, 1, format);
    ASSERT_TRUE(image->BuildFromBitmap(bitmap));
    filter->lightBlurResultCache_[1] = nullptr;
    filter->ApplyLightBlur(canvas, image, para);
    EXPECT_NE(filter->lightBlurResultCache_[1], nullptr);

    bitmap.Build(1, 10, format);
    ASSERT_TRUE(image->BuildFromBitmap(bitmap));
    filter->lightBlurResultCache_[1] = nullptr;
    filter->ApplyLightBlur(canvas, image, para);
    EXPECT_NE(filter->lightBlurResultCache_[1], nullptr);
}
} // namespace Rosen
} // namespace OHOS