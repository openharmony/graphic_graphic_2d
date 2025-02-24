/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in comliance with the License.
 * You may obtian a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../../dtk_constants.h"
#include "../../dtk_test_base.h"
#include "../../dtk_test_ext.h"
#include "../../utils.h"

namespace OHOS {
namespace Rosen {

using namespace Drawing;

// Pixmap_Scene_0031
// Pixmap构造接口：Pixmap_ImageInfo_03
// colorspace构造接口：CreateRefImage_L0_0
// Pixmap操作接口：GetHeight
// cilp：ClipRoundRect(非G2)
// transform：Scale(正常值)
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateMatrixColorFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 1)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace0 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::DOT2, CMSMatrixType::DCIP3);
    ImageInfo imageinfo0(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace0); // 512 width * 512 height
    Bitmap bitmap0;
    bitmap0.Build(imageinfo0);
    bitmap0.ClearWithColor(0xABCDEF78);
    Image image;
    image.BuildFromBitmap(bitmap0);
    auto colorspace = Drawing::ColorSpace::CreateRefImage(image);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    auto height1 = pixmap.GetHeight();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 4.设置视效效果，将效果添加到笔刷
    Drawing::ColorMatrix matrix;
    matrix.SetArray(ARR);
    auto cf = Drawing::ColorFilter::CreateMatrixColorFilter(matrix);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateColorFilterImageFilter(*cf, nullptr));
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f); // ratio

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawRect(Rect(0, 0, 500, height1)); // 500 width * height1
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);      // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);       // rect region (100, 100, 200, 200)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0032
// Pixmap构造接口：Pixmap_ImageInfo_03
// colorspace构造接口：CreateRefImage_L0_0
// Pixmap操作接口：GetColorType
// cilp：ClipRoundRect(非G2)
// transform：null
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateBlendImageFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 2)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace0 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::DOT2, CMSMatrixType::DCIP3);
    ImageInfo imageinfo0(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace0); // 512 width * 512 height
    Bitmap bitmap0;
    bitmap0.Build(imageinfo0);
    bitmap0.ClearWithColor(0xABCDEF78);
    Image image;
    image.BuildFromBitmap(bitmap0);
    auto colorspace = Drawing::ColorSpace::CreateRefImage(image);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    int rectPos = 0;
    brush.SetColor(0x4CB21933);
    for (auto blendMode : blendModes) {
        auto background = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto foreground = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto filter = Drawing::Filter();
        // 3.设置视效效果，将效果添加到笔刷
        filter.SetImageFilter(Drawing::ImageFilter::CreateBlendImageFilter(blendMode, background, foreground));
        brush.SetFilter(filter);
        // 4.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->Save();
        playbackCanvas_->AttachBrush(brush);
        // 5.Pixmap操作接口
        if (pixmap.GetColorType() == COLORTYPE_UNKNOWN) {
            playbackCanvas_->DrawRect(Drawing::Rect(800 + (rectPos % VARIATION) * LEFT,
                800 + (rectPos / VARIATION) * LEFT, 1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT)); // rect region(800, 800, 1000, 1000)
        } else {
            playbackCanvas_->DrawRect(Drawing::Rect(1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT, 1200 + (rectPos % VARIATION) * LEFT,
                1200 + (rectPos / VARIATION) * LEFT)); // rect region(1000, 1000, 1200, 1200)
        }
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        playbackCanvas_->Restore();
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectnotG2(true);
}

// Pixmap_Scene_0033
// Pixmap构造接口：Pixmap_ImageInfo_03
// colorspace构造接口：CreateRefImage_L0_0
// Pixmap操作接口：GetColorType
// cilp：null
// transform：Scale(极大值)
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateSrgbGammaToLinear
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 3)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace0 = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::DOT2, CMSMatrixType::DCIP3);
    ImageInfo imageinfo0(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL, colorspace0); // 512 width * 512 height
    Bitmap bitmap0;
    bitmap0.Build(imageinfo0);
    bitmap0.ClearWithColor(0xABCDEF78);
    Image image;
    image.BuildFromBitmap(bitmap0);
    auto colorspace = Drawing::ColorSpace::CreateRefImage(image);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 3.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);

    // 4.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f); // ratio

    // 5.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    // 6.Pixmap操作接口
    if (pixmap.GetColorType() == COLORTYPE_UNKNOWN) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();
}

// Pixmap_Scene_0034
// Pixmap构造接口：Pixmap_ImageInfo_03
// colorspace构造接口：CreateRefImage_L1_0
// Pixmap操作接口：GetAlphaType
// cilp：ClipPath
// transform：ConcatMatrix
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateBlurMaskFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 4)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    ImageInfo imageinfo0(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL); // 512 width * 512 height
    Bitmap bitmap0;
    bitmap0.Build(imageinfo0);
    bitmap0.ClearWithColor(0xABCDEF78);
    Image image;
    image.BuildFromBitmap(bitmap0);
    auto colorspace = Drawing::ColorSpace::CreateRefImage(image);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 3.组合transform函数
    Drawing::Matrix matrix;
    matrix.Rotate(15, 10, 10); // 15 angle 10 position
    playbackCanvas_->ConcatMatrix(matrix);

    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    for (auto& blurType : blurTypes) {
        auto filter = Drawing::Filter();
        // 4.设置视效效果，将效果添加到笔刷
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);
        // 5.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
        // 6.Pixmap操作接口
        if (pixmap.GetAlphaType() == ALPHATYPE_UNKNOWN) {
            playbackCanvas_->DrawRect(Drawing::Rect(800 + (rectPos % VARIATION) * LEFT,
                800 + (rectPos / VARIATION) * LEFT, 1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT)); // rect region(800, 800, 1000, 1000)
        } else {
            playbackCanvas_->DrawRect(Drawing::Rect(1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT, 1200 + (rectPos % VARIATION) * LEFT,
                1200 + (rectPos / VARIATION) * LEFT)); // rect region(1000, 1000, 1200, 1200)
        }
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        rectPos += 200;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 }); // rect region (200, 300, 700, 800)
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0035
// Pixmap构造接口：Pixmap_ImageInfo_03
// colorspace构造接口：CreateRefImage_L1_0
// Pixmap操作接口：GetColorSpace
// cilp：ClipRect
// transform：Rotate
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateBlurImageFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 5)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    ImageInfo imageinfo0(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL); // 512 width * 512 height
    Bitmap bitmap0;
    bitmap0.Build(imageinfo0);
    bitmap0.ClearWithColor(0xABCDEF78);
    Image image;
    image.BuildFromBitmap(bitmap0);
    auto colorspace = Drawing::ColorSpace::CreateRefImage(image);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 3.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);

    // 4.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10); // 30 angle 10 position

    // 5.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    // 6.Pixmap操作接口
    if (pixmap.GetColorSpace() == colorspace) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200); // rect region (100, 100, 200, 200)
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0036
// Pixmap构造接口：Pixmap_ImageInfo_03
// colorspace构造接口：CreateRefImage_L1_0
// Pixmap操作接口：GetRowBytes
// cilp：null
// transform：Scale(极大值)
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateLinearGradient
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 6)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    ImageInfo imageinfo0(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL); // 512 width * 512 height
    Bitmap bitmap0;
    bitmap0.Build(imageinfo0);
    bitmap0.ClearWithColor(0xABCDEF78);
    Image image;
    image.BuildFromBitmap(bitmap0);
    auto colorspace = Drawing::ColorSpace::CreateRefImage(image);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    ImageInfo imageInfo1(1, 1, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL);
    uint32_t pixel[] = { 0xFF123456 };
    Pixmap pixmap1(imageInfo1, pixel, sizeof(pixel));
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildWithCopy(pixmap1.GetAddr(), pixmap1.GetRowBytes() * pixmap1.GetHeight());
    auto image1 = Image::MakeRasterData(imageInfo1, data, pixmap1.GetRowBytes());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 4.设置视效效果，将效果添加到笔刷
    std::vector<Drawing::ColorQuad> colors = { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN,
        Drawing::Color::COLOR_BLUE };
    std::vector<Drawing::scalar> pos = { 0.00f, 0.50f, 1.00f };
    auto linearGradient = Drawing::ShaderEffect::CreateLinearGradient(
        { 0, 0 }, { 1000, 1000 }, colors, pos, Drawing::TileMode::CLAMP); // 0 start pos & 1000 end pos
    brush.SetShaderEffect(linearGradient);

    // 5.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f); // ratio

    // 6.绘制结果
    const void* pixel1 = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel1));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawImageNine(image1.get(), Drawing::RectI(0, 0, 1, 1),
        Rect(100, 100, 500, 500), // Rect(100, 100, 500, 500) & RectI(0, 0, 1, 1)
        FilterMode::NEAREST);
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();
}

// Pixmap_Scene_0037
// Pixmap构造接口：Pixmap_ImageInfo_03
// colorspace构造接口：CreateRefImage_L1_0
// Pixmap操作接口：GetAddr
// cilp：ClipRoundRect(G2_capsule)
// transform：Scale(极小值)
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateLinearToSrgbGamma
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 7)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    ImageInfo imageinfo0(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL); // 512 width * 512 height
    Bitmap bitmap0;
    bitmap0.Build(imageinfo0);
    bitmap0.ClearWithColor(0xABCDEF78);
    Image image;
    image.BuildFromBitmap(bitmap0);
    auto colorspace = Drawing::ColorSpace::CreateRefImage(image);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    Drawing::Bitmap bitmap1;
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));

    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f); // ratio

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap1, 300, 300); // 300，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 200);       // rect region (100, 100, 250, 200)
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f); // 50.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0038
// Pixmap构造接口：Pixmap_ImageInfo_03
// colorspace构造接口：CreateRefImage_L1_0
// Pixmap操作接口：GetColor
// cilp：ClipRoundRect(非G2)
// transform：Translate
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateBlurMaskFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 8)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    ImageInfo imageinfo0(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL); // 512 width * 512 height
    Bitmap bitmap0;
    bitmap0.Build(imageinfo0);
    bitmap0.ClearWithColor(0xABCDEF78);
    Image image;
    image.BuildFromBitmap(bitmap0);
    auto colorspace = Drawing::ColorSpace::CreateRefImage(image);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    Drawing::Pen pen;
    pen.SetWidth(8.5f);
    pen.SetColor(pixmap.GetColor(imageInfo.GetWidth() / 2, imageInfo.GetHeight() / 2));

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 4.组合transform函数
    playbackCanvas_->Translate(200, 200); // 200 distance

    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    for (auto& blurType : blurTypes) {
        auto filter = Drawing::Filter();
        // 5.设置视效效果，将效果添加到笔刷
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);
        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->AttachPen(pen);
        auto p1 = Drawing::Point(
            800 + (rectPos % VARIATION) * LEFT, 800 + (rectPos / VARIATION) * RIGHT); // point position(800, 800)
        auto p2 = Drawing::Point(
            1000 + (rectPos % VARIATION) * LEFT, 1000 + (rectPos / VARIATION) * RIGHT); // point position(1000, 1000)
        playbackCanvas_->DrawLine(p1, p2);
        playbackCanvas_->DetachPen();
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        rectPos += 200;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);       // rect region (100, 100, 200, 200)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0039
// Pixmap构造接口：Pixmap_ImageInfo_03
// colorspace构造接口：CreateRefImage_L1_0
// Pixmap操作接口：ScalePixels
// cilp：ClipRoundRect(G2)
// transform：ConcatMatrix
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateBlendModeColorFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 9)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    ImageInfo imageinfo0(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL); // 512 width * 512 height
    Bitmap bitmap0;
    bitmap0.Build(imageinfo0);
    bitmap0.ClearWithColor(0xABCDEF78);
    Image image;
    image.BuildFromBitmap(bitmap0);
    auto colorspace = Drawing::ColorSpace::CreateRefImage(image);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    ImageInfo imageinfo2 = ImageInfo::MakeN32Premul(4, 4); // pixel row and col(4, 4)
    uint32_t data[16];
    Pixmap dst(imageinfo2, data, sizeof(uint32_t) * imageinfo2.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.组合transform函数
    Drawing::Matrix matrix;
    matrix.Rotate(15, 10, 10); // 15 angle 10 position
    playbackCanvas_->ConcatMatrix(matrix);

    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    for (auto blendMode : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode);
        auto filter = Drawing::Filter();
        // 5.设置视效效果，将效果添加到笔刷
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);
        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
        if (pixmap.ScalePixels(dst, SamplingOptions())) {
            playbackCanvas_->DrawRect(Drawing::Rect(800 + (rectPos % VARIATION) * LEFT,
                800 + (rectPos / VARIATION) * LEFT, 1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT)); // rect region(800, 800, 1000, 1000)
        } else {
            playbackCanvas_->DrawRect(Drawing::Rect(1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT, 1200 + (rectPos % VARIATION) * LEFT,
                1200 + (rectPos / VARIATION) * LEFT)); // rect region(1000, 1000, 1200, 1200)
        }
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2(true);
}

// Pixmap_Scene_0040
// Pixmap构造接口：Pixmap_ImageInfo_03
// colorspace构造接口：CreateRefImage_L1_0
// Pixmap操作接口：ScalePixels
// cilp：null
// transform：Shear
// 抗锯齿：AA
// 透明度：不透明
// 视效：null
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 10)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    ImageInfo imageinfo0(512, 512, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL); // 512 width * 512 height
    Bitmap bitmap0;
    bitmap0.Build(imageinfo0);
    bitmap0.ClearWithColor(0xABCDEF78);
    Image image;
    image.BuildFromBitmap(bitmap0);
    auto colorspace = Drawing::ColorSpace::CreateRefImage(image);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    ImageInfo imageinfo2 = ImageInfo::MakeN32Premul(4, 4); // pixel row and col(4, 4)
    uint32_t data[16];
    Pixmap dst(imageinfo2, data, sizeof(uint32_t) * imageinfo2.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f); // slope

    // 5.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    if (pixmap.ScalePixels(dst, SamplingOptions())) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();
}

// Pixmap_Scene_0041
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L0_0
// Pixmap操作接口：GetWidth
// cilp：ClipPath
// transform：null
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateLumaColorFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 11)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::DOT2, CMSMatrixType::DCIP3);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    auto width = pixmap.GetWidth();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 5.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawRect(Rect(0, 0, width, 500)); // width * 500 height
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);    // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 6.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 }); // rect region (200, 300, 700, 800)
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0042
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_0
// Pixmap操作接口：GetHeight
// cilp：ClipRect
// transform：ConcatMatrix
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateMatrixColorFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 12)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::DOT2, CMSMatrixType::SRGB);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    auto height = pixmap.GetHeight();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 4.设置视效效果，将效果添加到笔刷
    Drawing::ColorMatrix matrix;
    matrix.SetArray(ARR);
    auto cf = Drawing::ColorFilter::CreateMatrixColorFilter(matrix);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateColorFilterImageFilter(*cf, nullptr));
    brush.SetFilter(filter);

    // 5.组合transform函数
    Drawing::Matrix matrix1;
    matrix1.Rotate(15, 10, 10); // 15 angle 10 position
    playbackCanvas_->ConcatMatrix(matrix1);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawRect(Rect(0, 0, 500, height)); // 500 width * height
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);     // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200); // rect region (100, 100, 200, 200)
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0043
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_1
// Pixmap操作接口：GetColorType
// cilp：ClipRoundRect(G2_capsule)
// transform：Translate
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateLinearToSrgbGamma
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 13)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::DOT2, CMSMatrixType::ADOBE_RGB);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 3.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

    // 4.组合transform函数
    playbackCanvas_->Translate(200, 200); // 200 distance

    // 5.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    // 6.Pixmap操作接口
    if (pixmap.GetColorType() == COLORTYPE_UNKNOWN) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 200);       // rect region (100, 100, 250, 200)
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f); // 50.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0044
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_2
// Pixmap操作接口：GetAlphaType
// cilp：ClipPath
// transform：Scale(极小值)
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateBlendModeColorFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 14)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::DOT2, CMSMatrixType::REC2020);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 3.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f); // ratio

    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    for (auto blendMode : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode);
        auto filter = Drawing::Filter();
        // 4.设置视效效果，将效果添加到笔刷
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);
        // 5.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
        // 6.Pixmap操作接口
        if (pixmap.GetAlphaType() == ALPHATYPE_UNKNOWN) {
            playbackCanvas_->DrawRect(Drawing::Rect(800 + (rectPos % VARIATION) * LEFT,
                800 + (rectPos / VARIATION) * LEFT, 1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT)); // rect region(800, 800, 1000, 1000)
        } else {
            playbackCanvas_->DrawRect(Drawing::Rect(1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT, 1200 + (rectPos % VARIATION) * LEFT,
                1200 + (rectPos / VARIATION) * LEFT)); // rect region(1000, 1000, 1200, 1200)
        }
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 }); // rect region (200, 300, 700, 800)
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0045
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_3
// Pixmap操作接口：GetAlphaType
// cilp：ClipRoundRect(非G2)
// transform：Scale(极大值)
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateSrgbGammaToLinear
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 15)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::DOT2, CMSMatrixType::XYZ);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 3.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);

    // 4.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f); // ratio

    // 5.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    // 6.Pixmap操作接口
    if (pixmap.GetAlphaType() == ALPHATYPE_UNKNOWN) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);       // rect region (100, 100, 200, 200)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0046
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_4
// Pixmap操作接口：GetColorSpace
// cilp：ClipRect
// transform：Translate
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateComposeColorFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 16)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::SRGB, CMSMatrixType::DCIP3);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 3.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, Drawing::BlendMode::SRC_IN);
    std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, Drawing::BlendMode::MODULATE);
    std::shared_ptr<Drawing::ColorFilter> colorFilter3 =
        Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(colorFilter3);
    brush.SetFilter(filter);

    // 4.组合transform函数
    playbackCanvas_->Translate(200, 200); // 200 distance

    // 5.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    // 6.Pixmap操作接口
    if (pixmap.GetColorSpace() == colorspace) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200); // rect region (100, 100, 200, 200)
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0047
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_5
// Pixmap操作接口：GetColorSpace
// cilp：null
// transform：Rotate
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateColorShader
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 17)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::SRGB, CMSMatrixType::SRGB);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 3.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);

    // 4.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10); // 30 angle 10 position

    // 5.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    // 6.Pixmap操作接口
    if (pixmap.GetColorSpace() == colorspace) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();
}

// Pixmap_Scene_0048
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_6
// Pixmap操作接口：GetRowBytes
// cilp：ClipRoundRect(G2)
// transform：Scale(极小值)
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateBlendImageFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 18)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::SRGB, CMSMatrixType::ADOBE_RGB);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    ImageInfo imageInfo1(1, 1, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL);
    uint32_t pixel[] = { 0xFF123456 };
    Pixmap pixmap1(imageInfo1, pixel, sizeof(pixel));
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildWithCopy(pixmap1.GetAddr(), pixmap1.GetRowBytes() * pixmap1.GetHeight());
    auto image = Image::MakeRasterData(imageInfo1, data, pixmap1.GetRowBytes());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 4.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f); // ratio

    int rectPos = 0;
    brush.SetColor(0x4CB21933);
    for (auto blendMode : blendModes) {
        auto background = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto foreground = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto filter = Drawing::Filter();
        // 5.设置视效效果，将效果添加到笔刷
        filter.SetImageFilter(Drawing::ImageFilter::CreateBlendImageFilter(blendMode, background, foreground));
        brush.SetFilter(filter);
        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->Save();
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawImageNine(image.get(),
            Drawing::RectI(0 + (rectPos % VARIATION) * LEFT, 0 + (rectPos / VARIATION) * LEFT,
                1 + (rectPos % VARIATION) * LEFT, 1 + (rectPos / VARIATION) * LEFT),
            Rect(100 + (rectPos % VARIATION) * LEFT, 100 + (rectPos / VARIATION) * LEFT,
                500 + (rectPos % VARIATION) * LEFT,
                500 + (rectPos / VARIATION) * LEFT), // Rect(100, 100, 500, 500) & RectI(0, 0, 1, 1)
            FilterMode::NEAREST);
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        playbackCanvas_->Restore();
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2(true);
}

// Pixmap_Scene_0049
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_7
// Pixmap操作接口：GetRowBytes
// cilp：ClipRoundRect(G2)
// transform：Shear
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateBlendShader
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 19)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::SRGB, CMSMatrixType::REC2020);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    ImageInfo imageInfo1(1, 1, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL);
    uint32_t pixel[] = { 0xFF123456 };
    Pixmap pixmap1(imageInfo1, pixel, sizeof(pixel));
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildWithCopy(pixmap1.GetAddr(), pixmap1.GetRowBytes() * pixmap1.GetHeight());
    auto image = Image::MakeRasterData(imageInfo1, data, pixmap1.GetRowBytes());
    // 绘制部分要放到第6步的绘制结果里
    playbackCanvas_->DrawImageNine(image.get(), Drawing::RectI(0, 0, 1, 1),
        Rect(100, 100, 500, 500), // Rect(100, 100, 500, 500) & RectI(0, 0, 1, 1)
        FilterMode::NEAREST);

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f); // slope

    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto BlendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, BlendMode);
        // 5.设置视效效果，将效果添加到笔刷
        brush.SetShaderEffect(blendShader);
        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawImageNine(image.get(),
            Drawing::RectI(0 + (rectPos % VARIATION) * LEFT, 0 + (rectPos / VARIATION) * LEFT,
                1 + (rectPos % VARIATION) * LEFT, 1 + (rectPos / VARIATION) * LEFT),
            Rect(100 + (rectPos % VARIATION) * LEFT, 100 + (rectPos / VARIATION) * LEFT,
                500 + (rectPos % VARIATION) * LEFT,
                500 + (rectPos / VARIATION) * LEFT), // Rect(100, 100, 500, 500) & RectI(0, 0, 1, 1)
            FilterMode::NEAREST);
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 250);       // rect region (100, 100, 250, 250)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0050
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_8
// Pixmap操作接口：GetAddr
// cilp：ClipPath
// transform：Shear
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateBlurImageFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 20)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::SRGB, CMSMatrixType::XYZ);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    Drawing::Bitmap bitmap1;
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));

    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f); // slope

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap1, 300, 300); // 300，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 }); // rect region (200, 300, 700, 800)
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0051
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_9
// Pixmap操作接口：GetAddr
// cilp：ClipRoundRect(G2_capsule)
// transform：null
// 抗锯齿：非AA
// 透明度：透明
// 视效：null
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 21)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::DCIP3);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    Drawing::Bitmap bitmap1;
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));

    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 4.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap1, 300, 300); // 300，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 5.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 200);       // rect region (100, 100, 250, 200)
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f); // 50.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0052
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_10
// Pixmap操作接口：GetColor
// cilp：ClipRect
// transform：Scale(正常值)
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateLinearGradient
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 22)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::SRGB);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    Drawing::Pen pen;
    pen.SetWidth(8.5f);
    pen.SetColor(pixmap.GetColor(imageInfo.GetWidth() / 2, imageInfo.GetHeight() / 2));

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.设置视效效果，将效果添加到笔刷
    std::vector<Drawing::ColorQuad> colors = { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN,
        Drawing::Color::COLOR_BLUE };
    std::vector<Drawing::scalar> pos = { 0.00f, 0.50f, 1.00f };
    auto linearGradient = Drawing::ShaderEffect::CreateLinearGradient(
        { 0, 0 }, { 1000, 1000 }, colors, pos, Drawing::TileMode::CLAMP); // 0 start pos & 1000 end pos
    brush.SetShaderEffect(linearGradient);

    // 5.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f); // ratio

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->AttachPen(pen);
    auto p1 = Drawing::Point(800, 800);   // point position(800, 800)
    auto p2 = Drawing::Point(1000, 1000); // point position(1000, 1000)
    playbackCanvas_->DrawLine(p1, p2);
    playbackCanvas_->DetachPen();
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200); // rect region (100, 100, 200, 200)
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0053
// Pixmap构造接口：Pixmap_ImageInfo_04
// colorspace构造接口：CreateRGB_L1_11
// Pixmap操作接口：ScalePixels
// cilp：ClipRoundRect(G2)
// transform：Rotate
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateBlurMaskFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 23)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateRGB(CMSTransferFuncType::LINEAR, CMSMatrixType::ADOBE_RGB);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    ImageInfo imageinfo2 = ImageInfo::MakeN32Premul(4, 4); // pixel row and col(4, 4)
    uint32_t data[16];
    Pixmap dst(imageinfo2, data, sizeof(uint32_t) * imageinfo2.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10); // 30 angle 10 position

    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    for (auto& blurType : blurTypes) {
        auto filter = Drawing::Filter();
        // 5.设置视效效果，将效果添加到笔刷
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);
        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
        if (pixmap.ScalePixels(dst, SamplingOptions())) {
            playbackCanvas_->DrawRect(Drawing::Rect(800 + (rectPos % VARIATION) * LEFT,
                800 + (rectPos / VARIATION) * LEFT, 1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT)); // rect region(800, 800, 1000, 1000)
        } else {
            playbackCanvas_->DrawRect(Drawing::Rect(1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT, 1200 + (rectPos % VARIATION) * LEFT,
                1200 + (rectPos / VARIATION) * LEFT)); // rect region(1000, 1000, 1200, 1200)
        }
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        rectPos += 200;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 250);       // rect region (100, 100, 250, 250)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0054
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L0_0
// Pixmap操作接口：GetWidth
// cilp：ClipPath
// transform：Scale(正常值)
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateBlendShader
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 24)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ1, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    auto width = pixmap.GetWidth();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 4.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f); // ratio

    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto BlendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, BlendMode);
        // 5.设置视效效果，将效果添加到笔刷
        brush.SetShaderEffect(blendShader);
        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawRect(Rect(0 + (rectPos % VARIATION) * LEFT, 0 + (rectPos / VARIATION) * LEFT,
            width + (rectPos % VARIATION) * LEFT, 500 + (rectPos / VARIATION) * LEFT)); // width * 500 height
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 }); // rect region (200, 300, 700, 800)
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0055
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L0_0
// Pixmap操作接口：GetWidth
// cilp：ClipRoundRect(非G2)
// transform：Shear
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateMatrixColorFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 25)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ1, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    auto width = pixmap.GetWidth();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.设置视效效果，将效果添加到笔刷
    Drawing::ColorMatrix matrix1;
    matrix1.SetArray(ARR);
    auto cf = Drawing::ColorFilter::CreateMatrixColorFilter(matrix1);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateColorFilterImageFilter(*cf, nullptr));
    brush.SetFilter(filter);

    // 5.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f); // slope

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawRect(Rect(0, 0, width, 500)); // width * 500 height
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);    // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);       // rect region (100, 100, 200, 200)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0056
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L0_0
// Pixmap操作接口：GetHeight
// cilp：ClipRect
// transform：Scale(极大值)
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateBlurMaskFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 26)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ1, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    auto height = pixmap.GetHeight();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 4.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f); // ratio

    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    for (auto& blurType : blurTypes) {
        auto filter = Drawing::Filter();
        // 5.设置视效效果，将效果添加到笔刷
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);
        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawRect(Rect(0 + (rectPos % VARIATION) * LEFT, 0 + (rectPos / VARIATION) * LEFT,
            500 + (rectPos % VARIATION) * LEFT, height + (rectPos / VARIATION) * LEFT)); // 500 width * height
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        rectPos += 200;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200); // rect region (100, 100, 200, 200)
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0057
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L0_0
// Pixmap操作接口：GetHeight
// cilp：null
// transform：null
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateBlurImageFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 27)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ1, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口
    auto height = pixmap.GetHeight();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 4.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);

    // 5.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawRect(Rect(0, 0, 500, height)); // 500 width * height
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);     // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();
}

// Pixmap_Scene_0058
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L0_0
// Pixmap操作接口：GetColorType
// cilp：ClipPath
// transform：Rotate
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateBlendModeColorFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 28)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ1, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 3.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10); // 30 angle 10 position

    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    for (auto blendMode : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode);
        auto filter = Drawing::Filter();
        // 4.设置视效效果，将效果添加到笔刷
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);
        // 5.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
        // 6.Pixmap操作接口
        if (pixmap.GetColorType() == COLORTYPE_UNKNOWN) {
            playbackCanvas_->DrawRect(Drawing::Rect(800 + (rectPos % VARIATION) * LEFT,
                800 + (rectPos / VARIATION) * LEFT, 1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT)); // rect region(800, 800, 1000, 1000)
        } else {
            playbackCanvas_->DrawRect(Drawing::Rect(1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT, 1200 + (rectPos % VARIATION) * LEFT,
                1200 + (rectPos / VARIATION) * LEFT)); // rect region(1000, 1000, 1200, 1200)
        }
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipPath(true);
}

// Pixmap_Scene_0059
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L0_0
// Pixmap操作接口：GetAlphaType
// cilp：ClipRect
// transform：Scale(极小值)
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateComposeColorFilter
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 29)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ1, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 3.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, Drawing::BlendMode::SRC_IN);
    std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, Drawing::BlendMode::MODULATE);
    std::shared_ptr<Drawing::ColorFilter> colorFilter3 =
        Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(colorFilter3);
    brush.SetFilter(filter);

    // 4.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f); // ratio

    // 5.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    // 6.Pixmap操作接口
    if (pixmap.GetAlphaType() == ALPHATYPE_UNKNOWN) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200); // rect region (100, 100, 200, 200)
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Pixmap_Scene_0060
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L0_0
// Pixmap操作接口：GetColorSpace
// cilp：ClipRoundRect(G2)
// transform：Scale(极大值)
// 抗锯齿：AA
// 透明度：半透明
// 视效：null
DEF_DTK(Pixmap_Scene2, TestLevel::L2, 30)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ1, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    // 设置透明度, 由填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 3.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f); // ratio

    // 4.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    // 5.Pixmap操作接口
    if (pixmap.GetColorSpace() == colorspace) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 6.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 250);       // rect region (100, 100, 250, 250)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

} // namespace Rosen
} // namespace OHOS