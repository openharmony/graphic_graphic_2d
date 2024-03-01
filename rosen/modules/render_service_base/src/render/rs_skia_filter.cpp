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
#include <memory>
#include "draw/blend_mode.h"

namespace OHOS {
namespace Rosen {
RSDrawingFilter::RSDrawingFilter(std::shared_ptr<Drawing::ImageFilter> imageFilter)
    : RSFilter(), imageFilter_(imageFilter) {}

RSDrawingFilter::~RSDrawingFilter() {}

Drawing::Brush RSDrawingFilter::GetBrush() const
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetImageFilter(imageFilter_);
    brush.SetFilter(filter);
    return brush;
}

std::shared_ptr<Drawing::ImageFilter> RSDrawingFilter::GetImageFilter() const
{
    return imageFilter_;
}

void RSDrawingFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    auto brush = GetBrush();
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*image, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
}
} // namespace Rosen
} // namespace OHOS
