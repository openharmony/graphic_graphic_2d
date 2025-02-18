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
#include "../../dtk_test_ext.h"
#include "text/font_types.h"
#include "text/text_blob_builder.h"
#include "text/font_mgr.h"
#include "text/font.h"
#include "recording/mem_allocator.h"
#include "text/font_style_set.h"
#include "text/rs_xform.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "text/typeface.h"
#include "text/font_style.h"
#include <sstream>

namespace OHOS {
namespace Rosen {

/* 测试类：ColorSpace
   测试接口:ColorSpace构造接口、ColorSpace操作接口、clip接口、transform接口、抗锯齿、透明度、视效接口
   测试内容：对ColorSpace和图元操作接口、视效接口等进行组合 */

using namespace Drawing;

static void MakeCmsMatrix(Drawing::CMSMatrix3x3& matrix)
{
    float srgb[3][3] = {
        {0.436065674f, 0.385147095f, 0.143066406f},
        {0.222488403f, 0.716873169f, 0.060607910f},
        {0.013916016f, 0.097076416f, 0.714096069f}
    };
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            matrix.vals[i][j] = srgb[i][j];
        }
    }
}

static std::shared_ptr<Drawing::TextBlob> MakeTextBlob()
{
    Drawing::TextBlobBuilder builder;
    auto buffer = builder.AllocRunPos(font1, 20, nullptr);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font1.UnicharToGlyph(0x9088);
        buffer.pos[i * 2] = 50.f * i;
        buffer.pos[i * 2 + 1] = 0;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = builder.Make();
    return textBlob;
}

static std::vector<Drawing::BlendMode> MakeBlendModes()
{
    std::vector<Drawing::BlendMode> blendModes = {Drawing::BlendMode::CLEAR, Drawing::BlendMode::SRC,
        Drawing::BlendMode::DST, Drawing::BlendMode::SRC_OVER, Drawing::BlendMode::DST_OVER,
        Drawing::BlendMode::SRC_IN, Drawing::BlendMode::DST_IN, Drawing::BlendMode::SRC_OUT,
        Drawing::BlendMode::DST_OUT, Drawing::BlendMode::SRC_ATOP, Drawing::BlendMode::DST_ATOP,
        Drawing::BlendMode::XOR, Drawing::BlendMode::PLUS, Drawing::BlendMode::MODULATE,
        Drawing::BlendMode::SCREEN, Drawing::BlendMode::OVERLAY, Drawing::BlendMode::DARKEN,
        Drawing::BlendMode::LIGHTEN, Drawing::BlendMode::COLOR_DODGE, Drawing::BlendMode::COLOR_BURN,
        Drawing::BlendMode::HARD_LIGHT, Drawing::BlendMode::SOFT_LIGHT, Drawing::BlendMode::DIFFERENCE,
        Drawing::BlendMode::EXCLUSION, Drawing::BlendMode::MULTIPLY, Drawing::BlendMode::HUE,
        Drawing::BlendMode::STATURATION, Drawing::BlendMode::COLOR_MODE, Drawing::BlendMode::LUMINOSITY};

        return blendModes;
}

// 用例 ColorSpace_Scene_0279
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 279)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    if (colorspace->IsSRGB()) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA);

    // 4.设置视效效果，将效果添加到笔刷
    Drawing::ColorMatrix matrixc;
    float arr[] = {
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };
    matrixc.SetArray(arr);
    auto cf = Drawing::ColorFilter::CreateMatrixColorFilter(matrixc);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateColorFilterImageFilter(*cf, nullptr));
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    Drawing::Path path;
    path.AddRect({200, 300, 700, 800});
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0280
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 280)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    if (colorspace->IsSRGB()) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Translate(200, 200);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0281
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 281)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    if (colorspace->IsSRGB()) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA);

    // 4.设置视效效果，将效果添加到笔刷

    // 5.组合transform函数
    playbackCanvas_->Translate(200, 200);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0282
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 282)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    if (colorspace->IsSRGB()) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);

    // 5.组合transform函数
    Drawing::Matrix matrix0;
    matrix0.Rotate(15, 10, 10);
    playbackCanvas_->ConcatMatrix(matrix0);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 200);
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0283
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 283)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    if (colorspace->IsSRGB()) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0x4CB21933);
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    for (auto blendMode : blendModes) {
        auto background = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto foreground = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto filter = Drawing::Filter();
        filter.SetImageFilter(Drawing::ImageFilter::CreateBlendImageFilter(blendMode, background, foreground));
        brush.SetFilter(filter);
    }

    // 5.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0284
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 284)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    if (colorspace->Equals(colorspace2)) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, Drawing::BlendMode::SRC_IN);
    std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, Drawing::BlendMode::MODULATE);
    std::shared_ptr<Drawing::ColorFilter> colorFilter3
        = Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(colorFilter3);
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0285
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 285)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    if (colorspace->Equals(colorspace2)) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 250);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0286
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 286)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    if (colorspace->Equals(colorspace2)) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0287
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 287)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::ADOBE_RGB);
    if (colorspace->Equals(colorspace2)) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto blendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, blendMode);
        brush.SetShaderEffect(blendShader);
    }

    // 5.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
}

// 用例 ColorSpace_Scene_0288
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 288)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::REC2020);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto data = colorspace->Serialize();
    std::shared_ptr<ColorSpace> colorspace1 = std::make_shared<ColorSpace>();
    colorspace1->Deserialize(data);
    imageInfo.SetColorSpace(colorspace1);
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlurType> blurTypes = {Drawing::BlurType::NORMAL, Drawing::BlurType::SOLID,
        Drawing::BlurType::OUTER, Drawing::BlurType::INNER};
    for (auto & blurType : blurTypes) {
        auto filter = Drawing::Filter();
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);
    }

    // 5.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    Drawing::Path path;
    path.AddRect({200, 300, 700, 800});
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0289
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 289)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::REC2020);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto data = colorspace->Serialize();
    std::shared_ptr<ColorSpace> colorspace1 = std::make_shared<ColorSpace>();
    colorspace1->Deserialize(data);
    imageInfo.SetColorSpace(colorspace1);
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA);

    // 4.设置视效效果，将效果添加到笔刷
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    brush.SetColor(0xFFFF0000);
    for (auto blendMode : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter
            = Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode);
        auto filter = Drawing::Filter();
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);
    }

    // 5.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    Drawing::Path path;
    path.AddRect({200, 300, 700, 800});
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0290
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 290)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::REC2020);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto data = colorspace->Serialize();
    std::shared_ptr<ColorSpace> colorspace1 = std::make_shared<ColorSpace>();
    colorspace1->Deserialize(data);
    imageInfo.SetColorSpace(colorspace1);
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA);

    // 4.设置视效效果，将效果添加到笔刷

    // 5.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 250);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0291
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 291)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::REC2020);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto data = colorspace->Serialize();
    std::shared_ptr<ColorSpace> colorspace1 = std::make_shared<ColorSpace>();
    colorspace1->Deserialize(data);
    imageInfo.SetColorSpace(colorspace1);
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 5.组合transform函数
    Drawing::Matrix matrix0;
    matrix0.Rotate(15, 10, 10);
    playbackCanvas_->ConcatMatrix(matrix0);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 200);
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0292
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 292)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::REC2020);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto data = colorspace->Serialize();
    std::shared_ptr<ColorSpace> colorspace1 = std::make_shared<ColorSpace>();
    colorspace1->Deserialize(data);
    imageInfo.SetColorSpace(colorspace1);
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);

    // 5.组合transform函数

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 200);
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0293
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 293)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::REC2020);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    if (colorspace->IsSRGB()) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA);

    // 4.设置视效效果，将效果添加到笔刷
    Drawing::ColorMatrix matrixc;
    float arr[] = {
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };
    matrixc.SetArray(arr);
    auto cf = Drawing::ColorFilter::CreateMatrixColorFilter(matrixc);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateColorFilterImageFilter(*cf, nullptr));
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0294
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 294)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::REC2020);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    if (colorspace->IsSRGB()) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, Drawing::BlendMode::SRC_IN);
    std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, Drawing::BlendMode::MODULATE);
    std::shared_ptr<Drawing::ColorFilter> colorFilter3
        = Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(colorFilter3);
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Translate(200, 200);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 200);
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0295
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 295)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::REC2020);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    if (colorspace->IsSRGB()) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);

    // 5.组合transform函数
    Drawing::Matrix matrix0;
    matrix0.Rotate(15, 10, 10);
    playbackCanvas_->ConcatMatrix(matrix0);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0296
DEF_DTK(ColorSpace_Scene_17, TestLevel::L2, 296)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::REC2020);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    if (colorspace->IsSRGB()) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000));
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200));
    }
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto blendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, blendMode);
        brush.SetShaderEffect(blendShader);
    }

    // 5.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

}
}
