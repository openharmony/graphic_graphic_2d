/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {

RSPaintFilterCanvas::RSPaintFilterCanvas(SkCanvas* canvas, float alpha)
    : SkPaintFilterCanvas(canvas), alpha_(std::clamp(alpha, 0.f, 1.f))
{}

RSPaintFilterCanvas::RSPaintFilterCanvas(SkSurface* skSurface, float alpha)
    : SkPaintFilterCanvas(skSurface ? skSurface->getCanvas() : nullptr),
      alpha_(std::clamp(alpha, 0.f, 1.f)),
      skSurface_(skSurface)
{}

SkSurface* RSPaintFilterCanvas::GetSurface() const
{
    return skSurface_;
}

bool RSPaintFilterCanvas::onFilter(SkPaint& paint) const
{
    if (alpha_ >= 1.f) {
        return true;
    } else if (alpha_ <= 0.f) {
        return false;
    }

    paint.setAlphaf(paint.getAlphaf() * alpha_);
    return true;
}

void RSPaintFilterCanvas::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint)
{
    SkPaint filteredPaint(paint ? *paint : SkPaint());
    if (this->onFilter(filteredPaint)) {
        this->SkCanvas::onDrawPicture(picture, matrix, &filteredPaint);
    }
}

void RSPaintFilterCanvas::MultiplyAlpha(float alpha)
{
    alpha_ = alpha_ * std::clamp(alpha, 0.f, 1.f);
}

int RSPaintFilterCanvas::SaveAlpha()
{
    alphaStack_.push(alpha_);
    return alphaStack_.size() - 1;
}

void RSPaintFilterCanvas::RestoreAlpha()
{
    if (alphaStack_.empty()) {
        return;
    }
    alpha_ = alphaStack_.top();
    alphaStack_.pop();
}

void RSPaintFilterCanvas::RestoreAlphaToCount(int count)
{
    while (alphaStack_.size() > count) {
        alpha_ = alphaStack_.top();
        alphaStack_.pop();
    }
}

std::pair<int, int> RSPaintFilterCanvas::SaveCanvasAndAlpha()
{
    return { save(), SaveAlpha() };
}

void RSPaintFilterCanvas::RestoreCanvasAndAlpha(std::pair<int, int>& count)
{
    restoreToCount(count.first);
    RestoreAlphaToCount(count.second);
}
} // namespace Rosen
} // namespace OHOS
