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
#include "dtk_test_ext.h"
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

// 用例 ColorSpace_Scene_0243
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 243)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::XYZ);
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
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10);

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

// 用例 ColorSpace_Scene_0244
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 244)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::XYZ);
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
    playbackCanvas_->Shear(10.0f, 10.0f);

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

// 用例 ColorSpace_Scene_0245
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 245)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::XYZ);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::XYZ);
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

// 用例 ColorSpace_Scene_0246
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 246)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::XYZ);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::XYZ);
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
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);

    // 5.组合transform函数
    playbackCanvas_->Translate(200, 200);

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

// 用例 ColorSpace_Scene_0247
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 247)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::XYZ);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::XYZ);
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
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);

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

// 用例 ColorSpace_Scene_0248
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 248)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::XYZ);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::XYZ);
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
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
}

// 用例 ColorSpace_Scene_0249
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 249)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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
    Drawing::Matrix matrix0;
    matrix0.Rotate(15, 10, 10);
    playbackCanvas_->ConcatMatrix(matrix0);

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

// 用例 ColorSpace_Scene_0250
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 250)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10);

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

// 用例 ColorSpace_Scene_0251
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 251)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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

// 用例 ColorSpace_Scene_0252
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 252)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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

    // 5.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f);

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

// 用例 ColorSpace_Scene_0253
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 253)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
}

// 用例 ColorSpace_Scene_0254
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 254)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto blendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, blendMode);
        brush.SetShaderEffect(blendShader);
    }

    // 5.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f);

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

// 用例 ColorSpace_Scene_0255
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 255)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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
    playbackCanvas_->Rotate(30, 10, 10);

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

// 用例 ColorSpace_Scene_0256
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 256)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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
    auto rect = Drawing::Rect(100, 100, 250, 200);
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0257
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 257)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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

// 用例 ColorSpace_Scene_0258
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 258)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
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

// 用例 ColorSpace_Scene_0259
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 259)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f);

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

// 用例 ColorSpace_Scene_0260
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 260)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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
    std::vector<Drawing::BlurType> blurTypes = {Drawing::BlurType::NORMAL, Drawing::BlurType::SOLID,
        Drawing::BlurType::OUTER, Drawing::BlurType::INNER};
    for (auto & blurType : blurTypes) {
        auto filter = Drawing::Filter();
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);
    }

    // 5.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f);

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

// 用例 ColorSpace_Scene_0261
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 261)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::DCIP3);
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
    std::vector<Drawing::ColorQuad> colors =
        {Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE};
    std::vector<Drawing::scalar> pos = {0.00f, 0.50f, 1.00f};
    auto linearGradient =
        Drawing::ShaderEffect::CreateLinearGradient({0, 0}, {1000, 1000}, colors, pos, Drawing::TileMode::CLAMP);
    brush.SetShaderEffect(linearGradient);

    // 5.组合transform函数

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
}

// 用例 ColorSpace_Scene_0262
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 262)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f);

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

// 用例 ColorSpace_Scene_0263
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 263)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto blendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, blendMode);
        brush.SetShaderEffect(blendShader);
    }

    // 5.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f);

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

// 用例 ColorSpace_Scene_0264
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 264)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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
    Drawing::Path path;
    path.AddRect({200, 300, 700, 800});
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// 用例 ColorSpace_Scene_0265
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 265)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlurType> blurTypes = {Drawing::BlurType::NORMAL, Drawing::BlurType::SOLID,
        Drawing::BlurType::OUTER, Drawing::BlurType::INNER};
    for (auto & blurType : blurTypes) {
        auto filter = Drawing::Filter();
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);
    }

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

// 用例 ColorSpace_Scene_0266
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 266)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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
    std::vector<Drawing::ColorQuad> colors =
        {Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE};
    std::vector<Drawing::scalar> pos = {0.00f, 0.50f, 1.00f};
    auto linearGradient =
        Drawing::ShaderEffect::CreateLinearGradient({0, 0}, {1000, 1000}, colors, pos, Drawing::TileMode::CLAMP);
    brush.SetShaderEffect(linearGradient);

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

// 用例 ColorSpace_Scene_0267
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 267)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 5.组合transform函数

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

// 用例 ColorSpace_Scene_0268
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 268)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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

// 用例 ColorSpace_Scene_0269
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 269)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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

// 用例 ColorSpace_Scene_0270
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 270)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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
    Drawing::Matrix matrix0;
    matrix0.Rotate(15, 10, 10);
    playbackCanvas_->ConcatMatrix(matrix0);

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

// 用例 ColorSpace_Scene_0271
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 271)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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

// 用例 ColorSpace_Scene_0272
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 272)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);

    // 5.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
}

// 用例 ColorSpace_Scene_0273
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 273)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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
    playbackCanvas_->Scale(2.0f, 2.0f);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
}

// 用例 ColorSpace_Scene_0274
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 274)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.ColorSpace构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
    auto imageInfo = ImageInfo(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace); // 512 width & height
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合ColorSpace操作接口
    auto colorspace2 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::REC2020, CMSMatrixType::SRGB);
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
    playbackCanvas_->Translate(200, 200);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
}

// 用例 ColorSpace_Scene_0275
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 275)
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
    playbackCanvas_->Scale(10100.0f, 1.0f);

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

// 用例 ColorSpace_Scene_0276
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 276)
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
    auto data = colorspace->Serialize();
    std::shared_ptr<ColorSpace> colorspace1 = std::make_shared<ColorSpace>();
    colorspace1->Deserialize(data);
    imageInfo.SetColorSpace(colorspace1);
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA);

    // 4.设置视效效果，将效果添加到笔刷
    std::vector<Drawing::ColorQuad> colors =
        {Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE};
    std::vector<Drawing::scalar> pos = {0.00f, 0.50f, 1.00f};
    auto linearGradient =
        Drawing::ShaderEffect::CreateLinearGradient({0, 0}, {1000, 1000}, colors, pos, Drawing::TileMode::CLAMP);
    brush.SetShaderEffect(linearGradient);

    // 5.组合transform函数

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

// 用例 ColorSpace_Scene_0277
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 277)
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
    auto data = colorspace->Serialize();
    std::shared_ptr<ColorSpace> colorspace1 = std::make_shared<ColorSpace>();
    colorspace1->Deserialize(data);
    imageInfo.SetColorSpace(colorspace1);
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA);

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10);

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

// 用例 ColorSpace_Scene_0278
DEF_DTK(ColorSpace_Scene_8, TestLevel::L2, 278)
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
    auto data = colorspace->Serialize();
    std::shared_ptr<ColorSpace> colorspace1 = std::make_shared<ColorSpace>();
    colorspace1->Deserialize(data);
    imageInfo.SetColorSpace(colorspace1);
    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA);

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
    playbackCanvas_->Rotate(30, 10, 10);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 100, 200);
    playbackCanvas_->DrawBitmap(bitmap1, 1000, 1200);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
}

}
}
