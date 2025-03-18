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

#include "rosen/test/dtk/dtk_constants.h"
#include "../../dtk_test_base.h"
#include "rosen/test/dtk/dtk_test_ext.h"
#include "rosen/test/dtk/utils.h"

namespace OHOS {
namespace Rosen {
// 测试类:Pixmap
// 测试接口:Pixmap构造接口、Pixmap操作接口、clip接口、transform接口、抗锯齿、透明度、视效接口
// 测试内容:对Pixmap和图元操作接口、视效接口等进行组合

using namespace Drawing;

// Pixmap_Scene_0061
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L1_0
// Pixmap操作接口：GetRowBytes
// cilp：ClipRoundRect(G2_capsule)
// transform：Scale(正常值）
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateLumaColorFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 1)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    ImageInfo imageInfo1(1, 1, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL);
    uint32_t pixel[] = { 0xFF123456 };
    Pixmap pixmap1(imageInfo1, pixel, sizeof(pixel));
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildWithCopy(pixmap1.GetAddr(), pixmap1.GetRowBytes() * pixmap1.GetHeight());
    auto image = Image::MakeRasterData(imageInfo1, data, pixmap1.GetRowBytes());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo1);
    bitmap1.ClearWithColor(0xFF789645); // 不透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f); // ratio

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel1 = pixmap1.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel1));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawImageNine(image.get(), Drawing::RectI(0, 0, 1, 1), Rect(100, 100, 500, 500),
        FilterMode::NEAREST);                       // Rect(100, 100, 500, 500) & RectI(0, 0, 1, 1)
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2capsule(false);
}

// Pixmap_Scene_0062
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L1_0
// Pixmap操作接口：GetAddr
// cilp：ClipRoundRect(非G2)
// transform：Translate
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateLinearGradient
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 2)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));

    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x00789645); // 透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Translate(200, 200); // 200 distance

    // 5.设置视效效果，将效果添加到笔刷
    std::vector<Drawing::ColorQuad> colors = { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN,
        Drawing::Color::COLOR_BLUE };
    std::vector<Drawing::scalar> pos = { 0.00f, 0.50f, 1.00f };
    auto linearGradient = Drawing::ShaderEffect::CreateLinearGradient(
        { 0, 0 }, { 1000, 1000 }, colors, pos, Drawing::TileMode::CLAMP); // 0 start pos & 1000 end pos
    brush.SetShaderEffect(linearGradient);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap1, 300, 300); // 300，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectnotG2(true);
}

// Pixmap_Scene_0063
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L1_0
// Pixmap操作接口：GetColor
// cilp：ClipRoundRect(非G2)
// transform：ConcatMatrix
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateColorShader
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 3)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Pen pen;
    pen.SetWidth(8.5f);
    pen.SetColor(pixmap.GetColor(imageInfo.GetWidth() / 2, imageInfo.GetHeight() / 2));

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    Drawing::Matrix matrix1;
    matrix1.Rotate(15, 10, 10); // 15 angle 10 position
    playbackCanvas_->ConcatMatrix(matrix1);

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);

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
    TestBase::ClipRoundRectnotG2(true);
}

// Pixmap_Scene_0064
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L1_0
// Pixmap操作接口：GetColor
// cilp：null
// transform：ConcatMatrix
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateLinearToSrgbGamma
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 4)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Pen pen;
    pen.SetWidth(8.5f);
    pen.SetColor(pixmap.GetColor(imageInfo.GetWidth() / 2, imageInfo.GetHeight() / 2));

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    Drawing::Matrix matrix1;
    matrix1.Rotate(15, 10, 10); // 15 angle 10 position
    playbackCanvas_->ConcatMatrix(matrix1);

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

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
    // null
}

// Pixmap_Scene_0065
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L1_0
// Pixmap操作接口：ScalePixels
// cilp：ClipPath
// transform：Scale(极小值)
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateBlendImageFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 5)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    ImageInfo imageinfo2 = ImageInfo::MakeN32Premul(4, 4); // 4 width & height
    uint32_t data[16];
    Pixmap dst(imageinfo2, data, sizeof(uint32_t) * imageinfo2.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageinfo2);
    bitmap1.ClearWithColor(0xFF789645); // 不透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f); // ratio

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0x4CB21933);
    int rectPos = 0;
    for (auto blendMode : blendModes) {
        auto background = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto foreground = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto filter = Drawing::Filter();

        filter.SetImageFilter(Drawing::ImageFilter::CreateBlendImageFilter(blendMode, background, foreground));
        brush.SetFilter(filter);

        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->Save();
        playbackCanvas_->AttachBrush(brush);
        if (pixmap.ScalePixels(dst, SamplingOptions())) {
            playbackCanvas_->DrawRect(Drawing::Rect(800 + (rectPos % VARIATION) * LEFT,
                800 + (rectPos / VARIATION) * LEFT, 1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT)); // rect region (800, 800, 1000, 1000)
        } else {
            playbackCanvas_->DrawRect(Drawing::Rect(1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT, 1200 + (rectPos / VARIATION) * LEFT,
                1200 + (rectPos / VARIATION) * LEFT)); // rect region (1000, 1000, 1200, 1200)
        }
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        playbackCanvas_->Restore();
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipPath(true);
}

// Pixmap_Scene_0066
// Pixmap构造接口：Pixmap_ImageInfo_05
// colorspace构造接口：CreateCustomRGB_L1_0
// Pixmap操作接口：ScalePixels
// cilp：null
// transform：Rotate
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateSrgbGammaToLinear
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 6)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateCustomRGB(PQ, SRGBMatrix);
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    ImageInfo imageinfo2 = ImageInfo::MakeN32Premul(4, 4); // 4 width & height
    uint32_t data[16];
    Pixmap dst(imageinfo2, data, sizeof(uint32_t) * imageinfo2.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageinfo2);
    bitmap1.ClearWithColor(0xFF789645); // 不透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10); // 30 angle 10 position

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    if (pixmap.ScalePixels(dst, SamplingOptions())) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region (800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region (1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap1 position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    // null
}

// Pixmap_Scene_0067
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetWidth
// cilp：ClipRect
// transform：ConcatMatrix
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateBlendImageFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 7)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    auto width = pixmap.GetWidth();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0xFF789645); // 不透明效果，灰绿色

    // 4.组合transform函数
    Drawing::Matrix matrix;
    matrix.Rotate(15, 10, 10); // 15 angle 10 position
    playbackCanvas_->ConcatMatrix(matrix);

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0x4CB21933);
    int rectPos = 0;
    for (auto blendMode : blendModes) {
        auto background = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto foreground = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto filter = Drawing::Filter();

        filter.SetImageFilter(Drawing::ImageFilter::CreateBlendImageFilter(blendMode, background, foreground));
        brush.SetFilter(filter);

        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->Save();
        playbackCanvas_->DrawRect(Rect(0 + (rectPos % VARIATION) * LEFT, 0 + (rectPos / VARIATION) * LEFT,
            width + (rectPos % VARIATION) * LEFT, 500 + (rectPos / VARIATION) * LEFT)); // 500 width * height
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap1 position
        playbackCanvas_->Restore();
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRect(false);
}

// Pixmap_Scene_0068
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetHeight
// cilp：ClipRoundRect(G2)
// transform：Rotate
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateLinearGradient
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 8)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    auto height = pixmap.GetHeight();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x00789645); // 透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10); // 30 angle 10 position

    // 5.设置视效效果，将效果添加到笔刷
    std::vector<Drawing::ColorQuad> colors = { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN,
        Drawing::Color::COLOR_BLUE };
    std::vector<Drawing::scalar> pos = { 0.00f, 0.50f, 1.00f };
    auto linearGradient = Drawing::ShaderEffect::CreateLinearGradient(
        { 0, 0 }, { 1000, 1000 }, colors, pos, Drawing::TileMode::CLAMP); // 0 start pos & 1000 end pos
    brush.SetShaderEffect(linearGradient);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawRect(Rect(0, 0, 500, height)); // 500 width * height
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);     // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2(true);
}

// Pixmap_Scene_0069
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetColorType
// cilp：ClipRoundRect(G2)
// transform：Scale(极小值)
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateBlurImageFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 9)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetColorType只有绘制部分，放在绘制结果里

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0xFF789645); // 不透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f); // ratio

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    if (pixmap.GetColorType() == COLORTYPE_UNKNOWN) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2(true);
}

// Pixmap_Scene_0070
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetColorType
// cilp：null
// transform：Scale(极大值)
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateBlendShader
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 10)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetColorType只有绘制部分，放在绘制结果里

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f); // ratio

    // 5.设置视效效果，将效果添加到笔刷
    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto BlendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, BlendMode);
        brush.SetShaderEffect(blendShader);

        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->AttachBrush(brush);
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
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap position
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    // null
}

// Pixmap_Scene_0071
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetAlphaType
// cilp：ClipRoundRect(G2_capsule)
// transform：Scale(正常值）
// 抗锯齿：AA
// 透明度：透明
// 视效：null
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 11)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetAlphaType只有绘制部分，放在绘制结果里

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x00789645); // 透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f); // ratio

    // 5.设置视效效果，将效果添加到笔刷
    // null

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    if (pixmap.GetAlphaType() == ALPHATYPE_OPAQUE) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2capsule(true);
}

// Pixmap_Scene_0072
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetAlphaType
// cilp：ClipRoundRect(G2_capsule)
// transform：Translate
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateLinearToSrgbGamma
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 12)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetAlphaType只有绘制部分，放在绘制结果里

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x00789645); // 透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Translate(200, 200); // 200 distance

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    if (pixmap.GetAlphaType() == ALPHATYPE_OPAQUE) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2capsule(false);
}

// Pixmap_Scene_0073
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetColorSpace
// cilp：ClipRoundRect(非G2)
// transform：Scale(极小值)
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateLumaColorFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 13)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetColorSpace只有绘制部分，放在绘制结果里

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f); // ratio

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    if (pixmap.GetColorSpace() == colorspace) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectnotG2(false);
}

// Pixmap_Scene_0074
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetColorSpace
// cilp：ClipRoundRect(非G2)
// transform：Shear
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateSrgbGammaToLinear
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 14)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetColorSpace只有绘制部分，放在绘制结果里

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f); // slope

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    if (pixmap.GetColorSpace() == colorspace) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectnotG2(true);
}

// Pixmap_Scene_0075
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetRowBytes
// cilp：ClipPath
// transform：Translate
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateColorShader
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 15)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    ImageInfo imageInfo1(1, 1, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL);
    uint32_t pixel[] = { 0xFF123456 };
    Pixmap pixmap1(imageInfo1, pixel, sizeof(pixel));
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildWithCopy(pixmap1.GetAddr(), pixmap1.GetRowBytes() * pixmap1.GetHeight());
    auto image = Image::MakeRasterData(imageInfo1, data, pixmap1.GetRowBytes());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo1);
    bitmap1.ClearWithColor(0xFF789645); // 不透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Translate(200, 200); // 200 distance

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);

    // 6.绘制结果
    const void* pixel1 = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel1));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawImageNine(image.get(), Drawing::RectI(0, 0, 1, 1), Rect(100, 100, 500, 500),
        FilterMode::NEAREST);                       // Rect(100, 100, 500, 500) & RectI(0, 0, 1, 1)
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipPath(true);
}

// Pixmap_Scene_0076
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetRowBytes
// cilp：null
// transform：null
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateBlurMaskFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 16)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    ImageInfo imageInfo1(1, 1, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL);
    uint32_t pixel[] = { 0xFF123456 };
    Pixmap pixmap1(imageInfo1, pixel, sizeof(pixel));
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildWithCopy(pixmap1.GetAddr(), pixmap1.GetRowBytes() * pixmap1.GetHeight());
    auto image = Image::MakeRasterData(imageInfo1, data, pixmap1.GetRowBytes());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo1);
    bitmap1.ClearWithColor(0xFF789645); // 不透明效果，灰绿色

    // 4.组合transform函数
    // null

    // 5.设置视效效果，将效果添加到笔刷
    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    for (auto& blurType : blurTypes) {
        auto filter = Drawing::Filter();
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);

        // 6.绘制结果
        const void* pixel1 = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel1));
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawImageNine(image.get(),
            Drawing::RectI(0 + (rectPos % VARIATION) * LEFT, 0 + (rectPos / VARIATION) * LEFT,
                1 + (rectPos % VARIATION) * LEFT, 1 + (rectPos / VARIATION) * LEFT),
            Rect(100 + (rectPos % VARIATION) * LEFT, 100 + (rectPos / VARIATION) * LEFT,
                500 + (rectPos % VARIATION) * LEFT, 500 + (rectPos / VARIATION) * LEFT),
            FilterMode::NEAREST); // Rect(100, 100, 500, 500) & RectI(0, 0, 1, 1)
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap position
        rectPos += 200;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    // null
}

// Pixmap_Scene_0077
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetAddr
// cilp：ClipRect
// transform：null
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateBlendModeColorFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 17)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));

    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x00789645); // 透明效果，灰绿色

    // 4.组合transform函数
    // null

    // 5.设置视效效果，将效果添加到笔刷
    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    for (auto blendMode : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode);
        auto filter = Drawing::Filter();
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);

        // 6.绘制结果
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawBitmap(bitmap1, 300 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 300，300 is bitmap position
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRect(false);
}

// Pixmap_Scene_0078
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：GetColor
// cilp：ClipRoundRect(G2)
// transform：Shear
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateComposeColorFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 18)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Pen pen;
    pen.SetWidth(8.5f);
    pen.SetColor(pixmap.GetColor(imageInfo.GetWidth() / 2, imageInfo.GetHeight() / 2));

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f); // slope

    // 5.设置视效效果，将效果添加到笔刷
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

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->AttachPen(pen);
    auto p1 = Drawing::Point(800, 800);   // point position(800, 800)
    auto p2 = Drawing::Point(1000, 1000); // point position(1000, 1000)
    playbackCanvas_->DrawLine(p1, p2);
    playbackCanvas_->DetachPen();
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2(true);
}

// Pixmap_Scene_0079
// Pixmap构造接口：Pixmap_ImageInfo_06
// Pixmap操作接口：ScalePixels
// cilp：ClipRoundRect(G2_capsule)
// transform：null
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateMatrixColorFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 19)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_OPAQUE,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    ImageInfo imageinfo2 = ImageInfo::MakeN32Premul(4, 4); // pixel row and col(4, 4)
    uint32_t data[16];
    Pixmap dst(imageinfo2, data, sizeof(uint32_t) * imageinfo2.GetWidth());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageinfo2);
    bitmap1.ClearWithColor(0xFF789645); // 不透明效果，灰绿色

    // 4.组合transform函数
    // null

    // 5.设置视效效果，将效果添加到笔刷
    Drawing::ColorMatrix matrix;
    matrix.SetArray(ARR);
    auto cf = Drawing::ColorFilter::CreateMatrixColorFilter(matrix);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateColorFilterImageFilter(*cf, nullptr));
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    if (pixmap.ScalePixels(dst, SamplingOptions())) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2capsule(false);
}

// Pixmap_Scene_0080
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetWidth
// cilp：ClipRoundRect(G2)
// transform：Rotate
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateLinearToSrgbGamma
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 20)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    auto width = pixmap.GetWidth();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0xFF789645); // 不透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10); // 30 angle 10 position

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawRect(Rect(0, 0, width, 500)); // width * 500 height
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);    // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2(true);
}

// Pixmap_Scene_0081
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetHeight
// cilp：ClipRect
// transform：Scale(极小值)
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateLinearGradient
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 21)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    auto height = pixmap.GetHeight();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f); // ratio

    // 5.设置视效效果，将效果添加到笔刷
    std::vector<Drawing::ColorQuad> colors = { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN,
        Drawing::Color::COLOR_BLUE };
    std::vector<Drawing::scalar> pos = { 0.00f, 0.50f, 1.00f };
    auto linearGradient = Drawing::ShaderEffect::CreateLinearGradient(
        { 0, 0 }, { 1000, 1000 }, colors, pos, Drawing::TileMode::CLAMP); // 0 start pos & 1000 end pos
    brush.SetShaderEffect(linearGradient);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawRect(Rect(0, 0, 500, height)); // 500 width * height
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);     // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRect(false);
}

// Pixmap_Scene_0082
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetHeight
// cilp：ClipRoundRect(G2_capsule)
// transform：Scale(正常值）
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateSrgbGammaToLinear
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 22)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    auto height = pixmap.GetHeight();

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x00789645); // 透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f); // ratio

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawRect(Rect(0, 0, 500, height)); // 500 width * height
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);     // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectG2capsule(false);
}

// Pixmap_Scene_0083
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetColorType
// cilp：ClipRoundRect(非G2)
// transform：ConcatMatrix
// 抗锯齿：AA
// 透明度：半透明
// 视效：null
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 23)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetColorType只有绘制部分，放在绘制结果里

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    Drawing::Matrix matrix;
    matrix.Rotate(15, 10, 10); // 15 angle 10 position
    playbackCanvas_->ConcatMatrix(matrix);

    // 5.设置视效效果，将效果添加到笔刷
    // null

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    if (pixmap.GetColorType() == COLORTYPE_UNKNOWN) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRoundRectnotG2(true);
}

// Pixmap_Scene_0084
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetAlphaType
// cilp：null
// transform：Shear
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateLumaColorFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 24)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetAlphaType只有绘制部分，放在绘制结果里

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x00789645); // 透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f); // slope

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    if (pixmap.GetAlphaType() == ALPHATYPE_PREMUL) {
        playbackCanvas_->DrawRect(Drawing::Rect(800, 800, 1000, 1000)); // rect region(800, 800, 1000, 1000)
    } else {
        playbackCanvas_->DrawRect(Drawing::Rect(1000, 1000, 1200, 1200)); // rect region(1000, 1000, 1200, 1200)
    }
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    // null
}

// Pixmap_Scene_0085
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetColorSpace
// cilp：ClipPath
// transform：Translate
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateBlendImageFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 25)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetColorSpace只有绘制部分，放在绘制结果里

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Translate(200, 200); // 200 distance

    // 5.设置视效效果，将效果添加到笔刷
    int rectPos = 0;
    brush.SetColor(0x4CB21933);
    for (auto blendMode : blendModes) {
        auto background = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto foreground = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto filter = Drawing::Filter();
        filter.SetImageFilter(Drawing::ImageFilter::CreateBlendImageFilter(blendMode, background, foreground));
        brush.SetFilter(filter);

        // 6.绘制结果
        const void* pixel = pixmap.GetAddr();
        bitmap1.SetPixels(const_cast<void*>(pixel));
        playbackCanvas_->Save();
        playbackCanvas_->AttachBrush(brush);
        if (pixmap.GetColorSpace() == colorspace) {
            playbackCanvas_->DrawRect(Drawing::Rect(800 + (rectPos % VARIATION) * LEFT,
                800 + (rectPos / VARIATION) * LEFT, 1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT)); // rect region(800, 800, 1000, 1000)
        } else {
            playbackCanvas_->DrawRect(Drawing::Rect(1000 + (rectPos % VARIATION) * LEFT,
                1000 + (rectPos / VARIATION) * LEFT, 1200 + (rectPos % VARIATION) * LEFT,
                1200 + (rectPos / VARIATION) * LEFT)); // rect region(1000, 1000, 1200, 1200)
        }
        playbackCanvas_->DrawBitmap(bitmap1, 250 + (rectPos % VARIATION) * LEFT,
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap position
        playbackCanvas_->Restore();
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipPath(true);
}

// Pixmap_Scene_0086
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetRowBytes
// cilp：ClipRect
// transform：Translate
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateBlendShader
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 26)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    ImageInfo imageInfo1(1, 1, COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL);
    uint32_t pixel[] = { 0xFF123456 };
    Pixmap pixmap1(imageInfo1, pixel, sizeof(pixel));
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildWithCopy(pixmap1.GetAddr(), pixmap1.GetRowBytes() * pixmap1.GetHeight());
    auto image = Image::MakeRasterData(imageInfo1, data, pixmap1.GetRowBytes());

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0xFF789645); // 不透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Translate(200, 200); // 200 distance

    // 5.设置视效效果，将效果添加到笔刷
    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto BlendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, BlendMode);
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
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap position
        rectPos += 1;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRect(false);
}

// Pixmap_Scene_0087
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetAddr
// cilp：ClipPath
// transform：ConcatMatrix
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateColorShader
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 27)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));

    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x00789645); // 透明效果，灰绿色

    // 4.组合transform函数
    Drawing::Matrix matrix;
    matrix.Rotate(15, 10, 10); // 15 angle 10 position
    playbackCanvas_->ConcatMatrix(matrix);

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipPath(true);
}

// Pixmap_Scene_0088
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetAddr
// cilp：ClipPath
// transform：Scale(极大值)
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateComposeColorFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 28)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));

    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x00789645); // 透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f); // ratio

    // 5.设置视效效果，将效果添加到笔刷
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

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipPath(true);
}

// Pixmap_Scene_0089
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetColor
// cilp：ClipRect
// transform：Rotate
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateBlurImageFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 29)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Pen pen;
    pen.SetWidth(8.5f);
    pen.SetColor(pixmap.GetColor(imageInfo.GetWidth() / 2, imageInfo.GetHeight() / 2));

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10); // 30 angle 10 position

    // 5.设置视效效果，将效果添加到笔刷
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);

    // 6.绘制结果
    const void* pixel = pixmap.GetAddr();
    bitmap1.SetPixels(const_cast<void*>(pixel));
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->AttachPen(pen);
    auto p1 = Drawing::Point(800, 800);   // point position(800, 800)
    auto p2 = Drawing::Point(1000, 1000); // point position(1000, 1000)
    playbackCanvas_->DrawLine(p1, p2);
    playbackCanvas_->DetachPen();
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300); // 250，300 is bitmap position
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRect(false);
}

// Pixmap_Scene_0090
// Pixmap构造接口：Pixmap_ImageInfo_07
// Pixmap操作接口：GetColor
// cilp：ClipRect
// transform：null
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateBlurMaskFilter
DEF_DTK(Pixmap_Scene3, TestLevel::L2, 30)
{
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.Pixmap构造接口
    auto colorspace = Drawing::ColorSpace::CreateSRGB();
    ImageInfo imageInfo(512, 512, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_PREMUL,
        colorspace); // 512 width * 512 height
    Pixmap pixmap(imageInfo, nullptr, imageInfo.GetBytesPerPixel() * imageInfo.GetWidth());

    // 3.Pixmap操作接口,设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Pen pen;
    pen.SetWidth(8.5f);
    pen.SetColor(pixmap.GetColor(imageInfo.GetWidth() / 2, imageInfo.GetHeight() / 2));

    Drawing::Bitmap bitmap1;
    bitmap1.Build(imageInfo);
    bitmap1.ClearWithColor(0x89789645); // 半透明效果，灰绿色

    // 4.组合transform函数
    // null

    // 5.设置视效效果，将效果添加到笔刷
    int rectPos = 0;
    brush.SetColor(0xFFFF0000);
    for (auto& blurType : blurTypes) {
        auto filter = Drawing::Filter();
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
            300 + (rectPos / VARIATION) * RIGHT); // 250，300 is bitmap position
        rectPos += 200;
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    TestBase::ClipRect(false);
}

} // namespace Rosen
} // namespace OHOS