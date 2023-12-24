/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "render/rs_skia_filter.h"
#ifdef USE_ROSEN_DRAWING
#include <memory>
#include "draw/blend_mode.h"
#endif

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
RSSkiaFilter::RSSkiaFilter(sk_sp<SkImageFilter> imageFilter) : RSFilter(), imageFilter_(imageFilter) {}

RSSkiaFilter::~RSSkiaFilter() {}
#else
RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter)
    : RSFilter(), imageFilter_(imageFilter) {}

RSDrawingFilter::~RSDrawingFilter() {}
#endif

#ifndef USE_ROSEN_DRAWING
SkPaint RSSkiaFilter::GetPaint() const
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setImageFilter(imageFilter_);
    return paint;
}
#else
Drawing::Brush RSDrawingFilter::GetBrush() const
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetImageFilter(imageFilter_);
    brush.SetFilter(filter);
    return brush;
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImageFilter> RSSkiaFilter::GetImageFilter() const
#else
std::shared_ptr<Drawing::ImageFilter> RSDrawingFilter::GetImageFilter() const
#endif
{
    return imageFilter_;
}

#ifndef USE_ROSEN_DRAWING
void RSSkiaFilter::DrawImageRect(
    SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const
#else
void RSDrawingFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
#endif
{
#ifndef USE_ROSEN_DRAWING
    auto paint = GetPaint();
#ifdef NEW_SKIA
    canvas.drawImageRect(image.get(), src, dst, SkSamplingOptions(), &paint, SkCanvas::kStrict_SrcRectConstraint);
#else
    canvas.drawImageRect(image.get(), src, dst, &paint);
#endif
#else
    auto brush = GetBrush();
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*image, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
#endif
}
} // namespace Rosen
} // namespace OHOS
