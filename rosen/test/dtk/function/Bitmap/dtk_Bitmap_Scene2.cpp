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
#include "../../dtk_test_ext.h"
#include "../../utils.h"

namespace OHOS {
namespace Rosen {
// Bitmap_Scene_0031
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：SetInfo
// cilp：ClipRect
// transform：Scale(正常值）
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateBlendModeColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 1)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { COLORTYPE_ALPHA_8, ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.SetInfo(bitmap.GetImageInfo());
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 4.组合transform函数，Scale(正常值）
    playbackCanvas_->Scale(2.0f, 2.0f); // ratio

    // 5.设置视效效果
    brush.SetColor(0xFFFF0000);
    for (auto blendMode : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode);
        auto filter = Drawing::Filter();
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);

        // 6.绘制结果
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
        playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
        playbackCanvas_->DetachBrush();
    }

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(250, 300, 500, 500); // rect region (250, 300, 500, 500)
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0032
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：ComputeByteSize
// cilp：ClipRoundRect(G2)
// transform：Translate
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateBlendShader
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 2)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { COLORTYPE_ALPHA_8, ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色
    size_t size = bitmap.GetRowBytes() * bitmap.GetHeight();
    if (bitmap1.ComputeByteSize() == size) {
        playbackCanvas_->DrawBackground(0xFF675800); // 深棕榈
    }

    // 4.组合transform函数，Translate
    playbackCanvas_->Translate(200, 200); // 200 distance

    // 5.设置视效效果
    brush.SetColor(0xFFFF0000);
    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto BlendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, BlendMode);
        brush.SetShaderEffect(blendShader);

        // 6.绘制结果
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
        playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数
    auto rect = Drawing::Rect(250, 250, 400, 400);       // rect region (250, 250, 400, 400)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0033
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：SetPixel/GetPixel
// cilp：ClipRoundRect(G2_capsule)
// transform：Shear
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateLumaColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 3)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { COLORTYPE_ALPHA_8, ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap.ClearWithColor(0x89789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色
    bitmap1.SetPixels(bitmap.GetPixels());

    // 4.组合transform函数，Shear
    playbackCanvas_->Shear(10.0f, 10.0f); // slope

    // 5.设置视效效果
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数
    auto rect = Drawing::Rect(250, 300, 400, 500);       // rect region (250, 300, 400, 500)
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f); // 50.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0034
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：IsEmpty
// cilp：ClipRoundRect(非G2)
// transform：Translate
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateComposeColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 4)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { COLORTYPE_ALPHA_8, ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色
    if (bitmap1.IsEmpty()) {
        playbackCanvas_->DrawBackground(0xFF675800); // 深棕榈
    }

    // 4.组合transform函数，Translate
    playbackCanvas_->Translate(200, 200); // 200 distance

    // 5.设置视效效果
    brush.SetColor(0xFFFF0000);
    for (auto blendMode1 : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode1);
        for (auto blendMode2 : blendModes) {
            std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
                Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, blendMode2);
            std::shared_ptr<Drawing::ColorFilter> colorFilter3 =
                Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
            auto filter = Drawing::Filter();
            filter.SetColorFilter(colorFilter3);
            brush.SetFilter(filter);

            // 6.绘制结果
            playbackCanvas_->AttachBrush(brush);
            playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
            playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
        }
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数
    auto rect = Drawing::Rect(300, 300, 400, 400);       // rect region (300, 300, 400, 400)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0035
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：IsValid
// cilp：null
// transform：Shear
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateBlurImageFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 5)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { COLORTYPE_ALPHA_8, ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色
    if (bitmap1.IsValid()) {
        playbackCanvas_->DrawBackground(0xFF675800); // 深棕榈
    }

    // 4.组合transform函数，Shear
    playbackCanvas_->Shear(10.0f, 10.0f); // slope

    // 5.设置视效效果
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数,null
}

// Bitmap_Scene_0036
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：GetColor
// cilp：ClipPath
// transform：null
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateBlurMaskFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 6)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap(构造)
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { COLORTYPE_ALPHA_8, ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format); // 500 weidth & 500 height
    bitmap1.ClearWithColor(bitmap.GetColor(1, 1));

    // 4.组合transform函数,null

    // 5.设置视效效果
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlurType> blurTypes = { Drawing::BlurType::NORMAL, Drawing::BlurType::SOLID,
        Drawing::BlurType::OUTER, Drawing::BlurType::INNER };

    for (auto& blurType : blurTypes) {
        auto filter = Drawing::Filter();
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);

        // 6.绘制结果
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
        playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 }); // rect region (200, 300, 700, 800)
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0037
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：SetFormat/GetFormat
// cilp：ClipRect
// transform：ConcatMatrix
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateBlurMaskFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 7)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { COLORTYPE_ALPHA_8, ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1 = BuildRandomBitmap();
    bitmap1.SetFormat(bitmap.GetFormat());
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.组合transform函数,ConcatMatrix
    Drawing::Matrix matrix;
    matrix.Rotate(15, 10, 10); // 15 angle 10 position
    playbackCanvas_->ConcatMatrix(matrix);

    // 5.设置视效效果
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlurType> blurTypes = { Drawing::BlurType::NORMAL, Drawing::BlurType::SOLID,
        Drawing::BlurType::OUTER, Drawing::BlurType::INNER };

    for (auto& blurType : blurTypes) {
        auto filter = Drawing::Filter();
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);

        // 6.绘制结果
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
        playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数
    auto rect = Drawing::Rect(250, 300, 500, 500); // rect region (250, 300, 500, 500)
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0038
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：GetPixmap
// cilp：ClipRoundRect(G2)
// transform：Rotate
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateMatrixColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 8)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为非抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { COLORTYPE_ALPHA_8, ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色
    auto image = Drawing::Image::MakeFromRaster(bitmap1.GetPixmap(), nullptr, nullptr);

    // 4.组合transform函数,Rotate
    playbackCanvas_->Rotate(30, 10, 10); // 30 angle 10 position

    // 5.设置视效效果
    Drawing::ColorMatrix matrix;
    float arr[] = {
        1.0f,
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
    };
    matrix.SetArray(arr);
    auto cf = Drawing::ColorFilter::CreateMatrixColorFilter(matrix);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateColorFilterImageFilter(*cf, nullptr));
    brush.SetFilter(filter);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawImage(*image, 250, 300, SamplingOptions());
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数
    auto rect = Drawing::Rect(250, 250, 400, 400);       // rect region (250, 250, 400, 400)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0039
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：MakeImage
// cilp：ClipRoundRect(G2_capsule)
// transform：Scale(正常值）
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateColorShader
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 9)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { COLORTYPE_ALPHA_8, ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色
    std::shared_ptr<Image> image = bitmap1.MakeImage();

    // 4.组合transform函数,Scale(正常值）
    playbackCanvas_->Scale(2.0f, 2.0f); // ratio

    // 5.设置视效效果
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawImage(*image, 250, 300, SamplingOptions());
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数
    auto rect = Drawing::Rect(250, 300, 400, 500);       // rect region (250, 300, 400, 500)
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f); // 50.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0040
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：SetImmutable/IsImmutable
// cilp：ClipRoundRect(非G2)
// transform：Scale(极大值)
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateColorShader
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 10)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { COLORTYPE_ALPHA_8, ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // 500 weidth & 500 height
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色
    bitmap1.SetImmutable();
    if (bitmap1.IsImmutable()) {
        playbackCanvas_->DrawBackground(0xFF675800); // 深棕榈
    }

    // 4.组合transform函数,Scale(极大值)
    playbackCanvas_->Scale(1.0f, 10100.0f); // ratio

    // 5.设置视效效果
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数
    auto rect = Drawing::Rect(300, 300, 400, 400);       // rect region (300, 300, 400, 400)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0041
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：ClearWithColor
// cilp：null
// transform：Scale(极小值)
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateLinearGradient
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 11)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为不抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f);

    // 5.设置视效效果，将效果添加到笔刷
    std::vector<Drawing::ColorQuad> colors = { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN,
        Drawing::Color::COLOR_BLUE };
    std::vector<Drawing::scalar> pos = { 0.00f, 0.50f, 1.00f };
    auto linearGradient =
        Drawing::ShaderEffect::CreateLinearGradient({ 0, 0 }, { 500, 500 }, colors, pos, Drawing::TileMode::CLAMP);
    brush.SetShaderEffect(linearGradient);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致，此样例clip为null
}

// Bitmap_Scene_0042
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：ExtractSubset
// cilp：ClipPath
// transform：Rotate
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateBlendModeColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 12)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色
    if (bitmap1.ExtractSubset(bitmap, Rect(0, 1, 2, 2))) {
        playbackCanvas_->DrawBackground(0x00675800); // 设置背景为透明
    }
    brush.SetColor(0xFFFF0000);

    // 4.组合transform函数
    playbackCanvas_->Rotate(30, 250, 250);

    for (auto blendMode : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode);
        auto filter = Drawing::Filter();

        // 5.设置视效效果，将效果添加到笔刷
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);

        // 6.绘制结果
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
        playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
        playbackCanvas_->DetachBrush();
    }

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致，此样例clip为ClipPath
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 });
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0043
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：PeekPixels
// cilp：ClipRect
// transform：Shear
// 抗锯齿：非AA
// 透明度：半透明
// 视效：CreateComposeColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 13)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为不抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色
    Drawing::Pixmap pixmap;
    if (bitmap1.PeekPixels(pixmap)) {
        auto image = Image::MakeFromRaster(pixmap, nullptr, nullptr);
    }

    // 4.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f);
    brush.SetColor(0xFFFF0000);

    for (auto blendMode1 : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode1);
        for (auto blendMode2 : blendModes) {
            std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
                Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, blendMode2);
            std::shared_ptr<Drawing::ColorFilter> colorFilter3 =
                Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
            auto filter = Drawing::Filter();

            // 5.设置视效效果，将效果添加到笔刷
            filter.SetColorFilter(colorFilter3);
            brush.SetFilter(filter);

            // 6.绘制结果
            playbackCanvas_->AttachBrush(brush);
            playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
            playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
            playbackCanvas_->DetachBrush();
        }
    }

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致，此样例clip为ClipRect
    auto rect = Drawing::Rect(250, 300, 500, 500);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0044
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：ReadPixels
// cilp：ClipRoundRect(G2)
// transform：null
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateLinearToSrgbGamma
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 14)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色
    auto imageInfo =
        Drawing::ImageInfo(200, 200, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    bitmap1.ReadPixels(imageInfo, bitmap.GetPixels(), bitmap.GetRowBytes(), 250, 250);

    // 4.组合transform函数,此样例为null

    // 5.设置视效效果，将效果添加到笔刷, CreateLinearToSrgbGamma
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致，此样例clip为ClipRoundRect(G2)
    auto rect = Drawing::Rect(250, 300, 500, 500);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0045
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：CopyPixels
// cilp：ClipRoundRect(G2_capsule)
// transform：ConcatMatrix
// 抗锯齿：非AA
// 透明度：透明
// 视效：null
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 15)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为不抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色
    bitmap1.CopyPixels(bitmap, 50, 50);

    // 4.组合transform函数, ConcatMatrix
    Drawing::Matrix matrix;
    matrix.Rotate(15, 10, 10);
    playbackCanvas_->ConcatMatrix(matrix);

    // 5.设置视效效果，将效果添加到笔刷，此样例视效为null

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致，此样例clip为ClipRoundRect(G2_capsule)
    auto rect = Drawing::Rect(250, 300, 500, 500);
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0046
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：InstallPixels
// cilp：ClipRoundRect(非G2)
// transform：Rotate
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateLinearGradient
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 16)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // InstallPixels
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色
    auto imageInfo =
        Drawing::ImageInfo(200, 200, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN);
    uint32_t pixels[][5] = { { 0xBFA12345, 0xCB9745D2, 0xD0ACB2F4, 0xD34A1234, 0xCDE7AB9F },
        { 0xD98289C1, 0xC56A2F1A, 0xF3A807CC, 0xBD31C9A2, 0xF05A5378 },
        { 0xCF19A7E4, 0xF29074BD, 0xC0C35F1A, 0xE73812A7, 0xD98E2B43 },
        { 0xB6F10D34, 0xD98289C1, 0xC9A4527A, 0xC03D15A2, 0xF4B2C380 },
        { 0xBF1D68A3, 0xE5A3D1C4, 0xCBF722ED, 0xD7A9BC64, 0xB4C5F219 } };
    if (bitmap1.InstallPixels(imageInfo, pixels, bitmap.GetRowBytes())) {
        Drawing::Image image;
        image.BuildFromBitmap(bitmap1);
    }

    // 4.组合transform函数, Rotate
    playbackCanvas_->Rotate(30, 250, 250);

    // 5.设置视效效果，将效果添加到笔刷，CreateLinearGradient
    std::vector<Drawing::ColorQuad> colors = { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN,
        Drawing::Color::COLOR_BLUE };
    std::vector<Drawing::scalar> pos = { 0.00f, 0.50f, 1.00f };
    auto linearGradient =
        Drawing::ShaderEffect::CreateLinearGradient({ 0, 0 }, { 500, 500 }, colors, pos, Drawing::TileMode::CLAMP);
    brush.SetShaderEffect(linearGradient);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致，此样例clip为ClipRoundRect(非G2)
    auto rect = Drawing::Rect(250, 300, 500, 500);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0047
// Bitmap构造接口：Bitmap_Build_L1_11
// Bitmap操作接口：Serialize/Deserialize
// cilp：null
// transform：Scale(正常值）
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateLumaColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 17)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.创建Bitmap
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // Serialize/Deserialize
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    auto data = bitmap1.Serialize();
    if (bitmap1.Deserialize(data)) {
        playbackCanvas_->DrawBackground(0xFF675800);
    }

    // 4.组合transform函数, Scale(正常值)
    playbackCanvas_->Scale(2.0f, 2.0f);

    // 5.设置视效效果，将效果添加到笔刷，  CreateLumaColorFilter
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致，此样例clip为null
}

// Bitmap_Scene_0048
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：GetWidth
// cilp：ClipPath
// transform：ConcatMatrix
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateBlendImageFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 18)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
    // 2.创建Bitmap  Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetWidth
    Drawing::Bitmap bitmap1;
    bitmap1.Build(bitmap.GetWidth(), 500, format);
    bitmap1.ClearWithColor(0x894567FA); // 半透明效果，粉蓝色

    // 4.组合transform函数,ConcatMatrix
    Drawing::Matrix matrix;
    matrix.Rotate(15, 10, 10);
    playbackCanvas_->ConcatMatrix(matrix);

    brush.SetColor(0x4CB21933);
    for (auto blendMode : blendModes) {
        auto background = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto foreground = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto filter = Drawing::Filter();

        // 5.设置视效效果，将效果添加到笔刷，  CreateLumaColorFilter
        filter.SetImageFilter(Drawing::ImageFilter::CreateBlendImageFilter(blendMode, background, foreground));
        brush.SetFilter(filter);

        // 6.绘制结果
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
        playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致，ClipPath
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 });
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0049
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：GetHeight
// cilp：ClipRect
// transform：Rotate
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateBlendShader
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 19)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为不抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
                               // 2.创建Bitmap  Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetWidth
    Drawing::Bitmap bitmap1;
    bitmap1.Build(500, bitmap.GetHeight(), format);
    bitmap1.ClearWithColor(0xFF4567FA); // 不透明效果，粉蓝色

    // 4.组合transform函数,Rotate
    playbackCanvas_->Rotate(30, 250, 250);

    brush.SetColor(0xFFFF0000);
    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto BlendMode : blendModes) {
        // 5.设置视效效果，将效果添加到笔刷，  CreateBlendShader
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, BlendMode);
        brush.SetShaderEffect(blendShader);

        // 6.绘制结果
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
        playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致，ClipRect
    auto rect = Drawing::Rect(250, 300, 500, 500);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0050
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：GetColorType
// cilp：ClipRoundRect(非G2)
// transform：Scale(极大值)
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateMatrixColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 20)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）
                              // 2.创建Bitmap  Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetWidth
    Drawing::Bitmap bitmap1;
    Drawing::BitmapFormat format1 { bitmap.GetColorType(), Drawing::AlphaType::ALPHATYPE_UNKNOWN };
    bitmap1.Build(500, 500, format1);
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    // 4.组合transform函数, Scale(极大值)
    playbackCanvas_->Scale(1.0f, 10100.0f);

    // 5.设置视效效果，将效果添加到笔刷，  CreateMatrixColorFilter
    Drawing::ColorMatrix matrix;
    float arr[] = {
        1.0f,
        1.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
    };
    matrix.SetArray(arr);
    auto cf = Drawing::ColorFilter::CreateMatrixColorFilter(matrix);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateColorFilterImageFilter(*cf, nullptr));
    brush.SetFilter(filter);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致，ClipRoundRect(非G2)
    auto rect = Drawing::Rect(250, 300, 500, 500);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0051
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：GetAlphaType
// cilp：ClipRoundRect(G2_capsule)
// transform：Scale(极小值)
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateBlurMaskFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 21)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    // Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetAlphaType, 创建Bitmap1涉及的ColorType参数和Bitmap保持一致
    Drawing::Bitmap bitmap1;
    Drawing::BitmapFormat format1 { Drawing::ColorType::COLORTYPE_RGB_565, bitmap.GetAlphaType() };
    bitmap1.Build(500, 500, format1);   // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0x004567FA); // 透明效果，粉蓝色

    std::vector<Drawing::BlurType> blurTypes = { Drawing::BlurType::NORMAL, Drawing::BlurType::SOLID,
        Drawing::BlurType::OUTER, Drawing::BlurType::INNER };
    brush.SetColor(0xFFFF0000);

    // 4.组合transform函数
    playbackCanvas_->Scale(10100.0f, 1.0f); // Scale(极小值)

    for (auto& blurType : blurTypes) {
        auto filter = Drawing::Filter();
        // 5.设置视效效果，将效果添加到笔刷
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);

        // 6.绘制结果
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
        playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 200);
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0052
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：GetRowBytes
// cilp：ClipRoundRect(G2)
// transform：Translate
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateBlendModeColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 22)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    // Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // GetAlphaType, 创建Bitmap1涉及的ColorType参数和Bitmap保持一致
    Drawing::Bitmap bitmap1;
    Drawing::BitmapFormat format1 { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap1.Build(500, 500, format1);   // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // GetRowBytes, 创建Bitmap1的方式和Bitmap保持一致, 此处不展示
    if (bitmap1.GetRowBytes() == sizeof(uint32_t) * bitmap1.GetWidth()) {
        playbackCanvas_->DrawBackground(0x89675800); // 89为半透明
    }
    brush.SetColor(0xFFFF0000);

    // 4.组合transform函数
    playbackCanvas_->Translate(200, 200); // Translate

    for (auto blendMode : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode);
        auto filter = Drawing::Filter();

        // 5.设置视效效果，将效果添加到笔刷
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);
        // 6.绘制结果
        playbackCanvas_->AttachBrush(brush);
        playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
        playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
        playbackCanvas_->DetachBrush();
    }

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 250);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0053
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：GetImageInfo
// cilp：null
// transform：Shear
// 抗锯齿：非AA
// 透明度：不透明
// 视效：null
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 23)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    // Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明

    // GetImageInfo, 固定bitmap1创建方式
    Drawing::Bitmap bitmap1;
    bitmap1.TryAllocPixels(bitmap.GetImageInfo());
    bitmap1.ClearWithColor(0xFF4567FA); // FF为不透明

    // 4.组合transform函数
    playbackCanvas_->Shear(10.0f, 10.0f); // Shear

    // 5.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();
}

// Bitmap_Scene_0054
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：SetInfo
// cilp：ClipPath
// transform：null
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateBlurImageFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 24)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    // Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // SetInfo, 固定bitmap1创建方式
    Drawing::Bitmap bitmap1;
    bitmap1.SetInfo(bitmap.GetImageInfo());
    bitmap1.ClearWithColor(0xFF4567FA); // FF为不透明

    brush.SetColor(0xFFFF0000);
    // 4.设置视效效果，将效果添加到笔刷
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);

    // 5.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 6.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    // ClipPath
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 });
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0055
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：ComputeByteSize
// cilp：ClipRect
// transform：Scale(正常值）
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateComposeColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 25)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    // Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    Drawing::BitmapFormat format1 { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap1.Build(500, 500, format1);   // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // ComputeBytesSize,创建Bitmap1的方式和Bitmap保持一致, 此处不展示
    size_t size = bitmap.GetRowBytes() * bitmap.GetHeight();
    if (bitmap1.ComputeByteSize() == size) {
        playbackCanvas_->DrawBackground(0x00675800); // 00为透明效果
    }
    brush.SetColor(0xFFFF0000);

    // 4.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f); // Scale(正常值)

    for (auto blendMode1 : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode1);
        for (auto blendMode2 : blendModes) {
            std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
                Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, blendMode2);
            std::shared_ptr<Drawing::ColorFilter> colorFilter3 =
                Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
            auto filter = Drawing::Filter();
            // 5.设置视效效果，将效果添加到笔刷
            filter.SetColorFilter(colorFilter3);
            brush.SetFilter(filter);

            // 6.绘制结果
            playbackCanvas_->AttachBrush(brush);
            playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
            playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
        }
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    // ClipRect
    auto rect = Drawing::Rect(100, 100, 200, 200);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, false);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0056
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：SetPixel/GetPixel
// cilp：ClipRoundRect(G2)
// transform：ConcatMatrix
// 抗锯齿：AA
// 透明度：不透明
// 视效：CreateComposeColorFilter

DEF_DTK(Bitmap_Scene2, TestLevel::L2, 26)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    // Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    Drawing::BitmapFormat format1 { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap1.Build(500, 500, format1);   // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // SetPixel/GetPixel,创建Bitmap1的方式和Bitmap保持一致, 此处不展示
    bitmap1.SetPixels(bitmap.GetPixels());
    brush.SetColor(0xFFFF0000);

    // 4.组合transform函数
    Drawing::Matrix matrix; // ConcatMatrix
    matrix.Rotate(15, 10, 10);
    playbackCanvas_->ConcatMatrix(matrix);

    for (auto blendMode1 : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
            Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode1);
        for (auto blendMode2 : blendModes) {
            std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
                Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, blendMode2);
            std::shared_ptr<Drawing::ColorFilter> colorFilter3 =
                Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
            auto filter = Drawing::Filter();
            // 5.设置视效效果，将效果添加到笔刷
            filter.SetColorFilter(colorFilter3);
            brush.SetFilter(filter);

            // 6.绘制结果
            playbackCanvas_->AttachBrush(brush);
            playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
            playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
        }
    }
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 250);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0057
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：IsEmpty
// cilp：ClipRoundRect(G2_capsule)
// transform：Rotate
// 抗锯齿：非AA
// 透明度：透明
// 视效：CreateSrgbGammaToLinear
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 27)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    // Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    Drawing::BitmapFormat format1 { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap1.Build(500, 500, format1);   // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色
    // IsEmpty,创建Bitmap1的方式和Bitmap保持一致, 此处不展示
    if (bitmap1.IsEmpty()) {
        playbackCanvas_->DrawBackground(0x00675800); // 00为透明效果
    }

    brush.SetColor(0xFF4F7091);

    // 4.组合transform函数
    playbackCanvas_->Rotate(30, 10, 10); // Rotate

    auto filter = Drawing::Filter();

    // 5.设置视效效果，将效果添加到笔刷
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 250, 200);
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0058
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：IsValid
// cilp：ClipRoundRect(非G2)
// transform：Scale(正常值）
// 抗锯齿：AA
// 透明度：半透明
// 视效：CreateLinearToSrgbGamma
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 28)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    // Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    Drawing::BitmapFormat format1 { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap1.Build(500, 500, format1);   // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色
                                        // IsValid,创建Bitmap1的方式和Bitmap保持一致, 此处不展示
    if (bitmap1.IsValid()) {
        playbackCanvas_->DrawBackground(0x89675800); // 89为半透明
    }

    brush.SetColor(0xFF4F7091);

    // 4.组合transform函数
    playbackCanvas_->Scale(2.0f, 2.0f); // Scale(正常值)

    auto filter = Drawing::Filter();

    // 5.设置视效效果，将效果添加到笔刷
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    auto rect = Drawing::Rect(100, 100, 200, 200);
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f);
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

// Bitmap_Scene_0059
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：GetColor
// cilp：null
// transform：Scale(极大值)
// 抗锯齿：非AA
// 透明度：不透明
// 视效：CreateLumaColorFilter
DEF_DTK(Bitmap_Scene2, TestLevel::L2, 29)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(false); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    // Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    Drawing::Bitmap bitmap1;
    Drawing::BitmapFormat format1 { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap1.Build(500, 500, format1);   // bitmap Pixelwidth and Height
    bitmap1.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色
    // GetColor,创建Bitmap1的方式和Bitmap保持一致, 此处不展示
    bitmap1.ClearWithColor(bitmap.GetColor(1, 1));

    brush.SetColor(0xFF0000FF);

    // 4.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f); // Scale(极大值)

    auto filter = Drawing::Filter();

    // 5.设置视效效果，将效果添加到笔刷
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();
}

// Bitmap_Scene_0060
// Bitmap构造接口：Bitmap_Build_L1_21
// Bitmap操作接口：SetFormat/GetFormat
// cilp：ClipPath
// transform：Scale(极大值)
// 抗锯齿：AA
// 透明度：透明
// 视效：CreateLinearGradient

DEF_DTK(Bitmap_Scene2, TestLevel::L2, 30)
{
    /*
     *绘制流程说明：默认绘制两个Bitmap，即2、3两步，透明度效果只在第3步设置
     */
    // 1.创建笔刷，设置笔刷是否为抗锯齿效果
    using namespace Drawing;
    Drawing::Brush brush;
    brush.SetAntiAlias(true); // 设置笔刷抗锯齿，true为AA（抗锯齿），false为非AA（不抗锯齿）

    // 2.创建Bitmap
    // Bitmap_Build_L1_21
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGB_565, Drawing::AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(500, 500, format);    // bitmap Pixelwidth and Height
    bitmap.ClearWithColor(0xFF789645); // 固定为不透明，灰绿色

    // 3.组合Bitmap操作函数，设置透明度, 由最后一步填充的颜色值的前两位控制，00为透明效果，89为半透明，FF为不透明
    // SetFormat/GetFormat, 固定bitmap1创建方式
    Drawing::Bitmap bitmap1 = BuildRandomBitmap();
    bitmap1.SetFormat(bitmap.GetFormat());
    bitmap1.ClearWithColor(0x004567FA); // 00为透明效果

    std::vector<Drawing::ColorQuad> colors = { Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN,
        Drawing::Color::COLOR_BLUE };
    std::vector<Drawing::scalar> pos = { 0.00f, 0.50f, 1.00f };

    // 4.组合transform函数
    playbackCanvas_->Scale(1.0f, 10100.0f); // Scale(极大值)

    auto linearGradient =
        Drawing::ShaderEffect::CreateLinearGradient({ 0, 0 }, { 1000, 1000 }, colors, pos, Drawing::TileMode::CLAMP);
    brush.SetShaderEffect(linearGradient);

    // 6.绘制结果
    playbackCanvas_->AttachBrush(brush);
    playbackCanvas_->DrawBitmap(bitmap, 200, 200); // 200，200 is bitmap position
    playbackCanvas_->DrawBitmap(bitmap1, 250, 300);
    playbackCanvas_->DetachBrush();

    // 7.组合Clip函数，cilp也有抗锯齿效果，默认和笔刷效果保持一致
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 });
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

} // namespace Rosen
} // namespace OHOS